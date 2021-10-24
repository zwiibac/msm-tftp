#include <array>
#include <algorithm>
#include <gmock/gmock-function-mocker.h>
#include <tuple>
#include <string>
#include <span>
#include <memory>

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/msm/front/state_machine_def.hpp>

#include <gmock/gmock.h>

#include <zwiibac/tftp/state/accepting.h>
#include <zwiibac/tftp/event/request_received.h>
#include <zwiibac/tftp/event/accepting_failed.h>

#include <zwiibac/tftp/test_utils/random_iterator.h>

namespace asio = boost::asio;

namespace {

using namespace zwiibac::tftp;

class SocketMock : private boost::noncopyable
{
public:
    using MutableBuffer = boost::asio::mutable_buffer;
    using SystemErrorCode = boost::system::error_code;
    using Endpoint = boost::asio::ip::udp::endpoint;
    using IpAddress = boost::asio::ip::address;

    MOCK_METHOD(void, bind, (const Endpoint&, SystemErrorCode&));
    MOCK_METHOD(void, async_receive_from, (const MutableBuffer& buffer, Endpoint& end_point, std::function<void(const SystemErrorCode&, size_t)>));
    MOCK_METHOD(void, cancel, ()); // should return size_t
};

struct AcceptingMachineMock : private boost::noncopyable
{
    using Endpoint = SocketMock::Endpoint;
    using ReceiveBuffer = std::array<char, 4096>;
    using IpAddress = SocketMock::IpAddress;

    MOCK_METHOD(void, process_event, (RequestReceived event));
    MOCK_METHOD(void, process_event, (AcceptingFailed event));

    MOCK_METHOD(bool, IsBlockSizeOption, (const std::string_view&));
    MOCK_METHOD(bool, IsTimeOutOption, (const std::string_view&));
    MOCK_METHOD(bool, IsTransferSizeOption, (const std::string_view&));

    std::weak_ptr<SocketMock> listener_socket_;
    Endpoint agreed_remote_endpoint_;
    Endpoint last_remote_endpoint_;
    Endpoint local_data_endpoint_;
    IpAddress local_address_ = boost::asio::ip::address_v4::from_string("127.0.0.1");

    std::chrono::milliseconds time_out_{500};
    ReceiveBuffer receive_buffer_;
    std::span<const char> received_packet_;
};

using ::testing::Matcher;
using ::testing::_;
namespace msm = boost::msm;
using none = msm::front::none;

class AcceptingTest : public testing::Test
{
protected:
    virtual void SetUp() override 
    {

        ON_CALL(*socket_mock, async_receive_from).WillByDefault(
        [&](const SocketMock::MutableBuffer&, SocketMock::Endpoint, std::function<void(const SocketMock::SystemErrorCode&, size_t)> handler)
        {
            handler(ec, received_bytes);
        });

        ON_CALL(state_machine_mock, process_event(Matcher<AcceptingFailed>(_))).WillByDefault(
        [&](AcceptingFailed event) 
        {
            EXPECT_EQ(event.ec, ec);
            EXPECT_EQ(event.bytes_received, received_bytes);
        });

        ON_CALL(*socket_mock, bind).WillByDefault(
            [](const SocketMock::Endpoint&, SocketMock::SystemErrorCode&){}
        );

        state_machine_mock.listener_socket_ = socket_mock;
        state_machine_mock.receive_buffer_.fill('\0');
    }
   
    std::shared_ptr<SocketMock> socket_mock = std::make_shared<SocketMock>();
    AcceptingMachineMock state_machine_mock;
    SocketMock::SystemErrorCode ec;
    size_t received_bytes;
};

TEST_F(AcceptingTest, ReceiveError) 
{
    // arrange
    ec = asio::error::broken_pipe;
    received_bytes = 51;

    EXPECT_CALL(state_machine_mock, process_event(Matcher<AcceptingFailed>(_))).Times(1);
    EXPECT_CALL(state_machine_mock, process_event(Matcher<RequestReceived>(_))).Times(0);

    // act
    none event;
    Accepting sut{};
    sut.on_entry(event, state_machine_mock);
    // assert
}

TEST_F(AcceptingTest, ReceiveData) 
{
    // arrange
    received_bytes = 6;
    ec = SocketMock::SystemErrorCode();

    EXPECT_CALL(state_machine_mock, process_event(Matcher<AcceptingFailed>(_))).Times(0);
    EXPECT_CALL(state_machine_mock, process_event(Matcher<RequestReceived>(_))).Times(1);

    // act
    none event;
    Accepting sut{};
    sut.on_entry(event, state_machine_mock);
    // assert
}

TEST_F(AcceptingTest, Exit) 
{
    // arrange
    
    // exit action is no longer expected to do anything

    // act
    none event;
    Accepting sut{};
    sut.on_exit(event, state_machine_mock);
    // assert
}

using BufferTestData = std::tuple<std::string, size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t>;

class ProcessEventFromBufferTest : public testing::TestWithParam<BufferTestData>
{
public:
    enum Param : size_t 
    {
        TestName = 0,
        FileNameEnd = 1,
        ModeEnd = 2,
        OptionNameEnd = 3,
        OptionValueEnd = 4,
        FileNameSize = 5,
        ModeSize = 6,
        OptionNameSize = 7,
        OptionValueSize = 8,
        ReceivedBytes = 9
    };
protected:

    ProcessEventFromBufferTest() 
    {
        test_utils::RandomIterator random_iter;
        std::copy(random_iter, random_iter + state_machine_mock.receive_buffer_.size(), state_machine_mock.receive_buffer_.begin());
    }

    template<size_t Index>
    typename std::tuple_element_t<Index, ParamType> Get() { return std::get<Index>(GetParam()); }
    
    virtual void SetUp() override 
    {
        received_bytes = Get<Param::ReceivedBytes>();

        state_machine_mock.receive_buffer_[Get<Param::FileNameEnd>()] = '\0';
        state_machine_mock.receive_buffer_[Get<Param::ModeEnd>()] = '\0'; 
        state_machine_mock.receive_buffer_[Get<Param::OptionNameEnd>()] = '\0'; 
        state_machine_mock.receive_buffer_[Get<Param::OptionValueEnd>()] = '\0';

        ON_CALL(state_machine_mock, process_event(Matcher<RequestReceived>(_))).WillByDefault(
        [this](RequestReceived event) 
        {
            EXPECT_EQ(event.file_name.size(), Get<Param::FileNameSize>()) << event.file_name;
            EXPECT_EQ(event.mode.size(), Get<Param::ModeSize>()) << event.mode;
        });

        EXPECT_CALL(state_machine_mock, process_event(Matcher<RequestReceived>(_))).Times(1);
    }

    AcceptingMachineMock state_machine_mock;
    size_t received_bytes;
};

TEST_P(ProcessEventFromBufferTest, ProcessEventFromBuffer) 
{
    // arrange
    EnterAccepting sut{};
    // act
    sut.ProcessEventFromBuffer(state_machine_mock, received_bytes);
    // assert
}

INSTANTIATE_TEST_SUITE_P(BasicValues, ProcessEventFromBufferTest, testing::Values(
    BufferTestData{"Basic", 8, 13, 37, 53, 6, 4, 23, 15, 54},
    BufferTestData{"EmptyValues", 2, 3, 4, 5, 0, 0, 0, 0, 6},
    BufferTestData{"EmptyValuesExceptLast", 2, 3, 4, 19, 0, 0, 0, 14, 20},
    BufferTestData{"EmptyValuesExceptFirst", 19, 20, 21, 22, 17, 0, 0, 0, 23},
    BufferTestData{"EmptyValuesExceptMiddle", 2, 3, 21, 22, 0, 0, 17, 0, 23}));
} // end anonymous namespace

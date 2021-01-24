#include <functional>
#include <span>

#include <boost/asio.hpp>

#include <gmock/gmock.h>

#include <zwiibac/tftp/state/waiting_for_ack.h>
#include <zwiibac/tftp/event/packet_received.h>
#include <zwiibac/tftp/event/timed_out.h>
#include <zwiibac/tftp/protocol/opcode.h>

namespace {

using namespace zwiibac::tftp;

class SocketMock 
{
public:
    using MutableBuffer = boost::asio::mutable_buffer;
    using SystemErrorCode = boost::system::error_code;
    using Endpoint = boost::asio::ip::udp::endpoint;

    MOCK_METHOD(void, async_receive_from, (MutableBuffer buffer, Endpoint& end_point, std::function<void(const SystemErrorCode&, size_t)>));
    MOCK_METHOD(void, cancel, ()); // should return size_t
};

class TimerMock 
{
public:
    using Duration = std::chrono::milliseconds;
    using SystemErrorCode = boost::system::error_code;

    MOCK_METHOD(void, expires_from_now, (const Duration&));
    MOCK_METHOD(void ,async_wait, (std::function<void(const SystemErrorCode&)>));
    MOCK_METHOD(void, cancel, ()); // should return size_t
};

namespace asio = boost::asio;

struct WaitMachineMock 
{
    using Endpoint = SocketMock::Endpoint;

    MOCK_METHOD(void, process_event, (PacketReceived event));
    MOCK_METHOD(void, process_event, (TimedOut event));

    SocketMock socket_;
    TimerMock timer_;
    Endpoint agreed_remote_endpoint_;
    Endpoint last_remote_endpoint_;
    Endpoint local_data_endpoint_;

    std::chrono::milliseconds time_out_{500};
    std::array<char, 4096> receive_buffer_;
    size_t resend_count_ = 0;
    size_t agreed_block_size_ = 512;
    size_t last_sent_block_ = 81;

    std::span<const char> received_packet_;
};

namespace msm = boost::msm;
using none = msm::front::none;

using ::testing::AtLeast;
using ::testing::Matcher;
using ::testing::_;

TEST(WaitStateTest, ReceiveAck) 
{
    // arrange
    WaitMachineMock state_machine_mock;
    none event;
    WaitingForAck sut;
    size_t expected_bytes_transfered = 50;

    ON_CALL(state_machine_mock.socket_, async_receive_from).WillByDefault(
        [bytes_transfered=expected_bytes_transfered]
        (SocketMock::MutableBuffer, SocketMock::Endpoint, std::function<void(const SocketMock::SystemErrorCode&, size_t)> handler)
        {
            handler(SocketMock::SystemErrorCode(), bytes_transfered);
        });

    EXPECT_CALL(state_machine_mock, process_event(Matcher<PacketReceived>(_))).Times(1);
    EXPECT_CALL(state_machine_mock, process_event(Matcher<TimedOut>(_))).Times(0);
    EXPECT_CALL(state_machine_mock.timer_, cancel()).Times(AtLeast(1));
    // act
    sut.on_entry(event, state_machine_mock);
    // assert
    EXPECT_EQ(state_machine_mock.received_packet_.size(), expected_bytes_transfered);
}

TEST(WaitStateTest, AbortReceive) 
{
    // arrange
    WaitMachineMock state_machine_mock;
    none event;
    WaitingForAck sut;


    ON_CALL(state_machine_mock.socket_, async_receive_from).WillByDefault(
        [](SocketMock::MutableBuffer, SocketMock::Endpoint, std::function<void(const SocketMock::SystemErrorCode&, size_t)> handler)
        {
            handler(asio::error::operation_aborted, 50);
        });

    EXPECT_CALL(state_machine_mock, process_event(Matcher<PacketReceived>(_))).Times(0);
    EXPECT_CALL(state_machine_mock, process_event(Matcher<TimedOut>(_))).Times(1);
    // act
    sut.on_entry(event, state_machine_mock);
    // assert
    EXPECT_EQ(state_machine_mock.received_packet_.size(), 0);
}

TEST(WaitStateTest, Timeout) 
{
    // arrange
    WaitMachineMock state_machine_mock;
    none event;
    WaitingForAck sut;

    ON_CALL(state_machine_mock.timer_, async_wait).WillByDefault(
        [](std::function<void(const SocketMock::SystemErrorCode&)> handler)
        {
            handler(SocketMock::SystemErrorCode());
        });
    EXPECT_CALL(state_machine_mock.timer_, expires_from_now(state_machine_mock.time_out_));
    EXPECT_CALL(state_machine_mock, process_event(Matcher<TimedOut>(_))).Times(0);
    EXPECT_CALL(state_machine_mock, process_event(Matcher<PacketReceived>(_))).Times(0);
    EXPECT_CALL(state_machine_mock.socket_, cancel()).Times(AtLeast(1));
    // act
    sut.on_entry(event, state_machine_mock);
    // assert
}

TEST(WaitStateTest, AbortTimer) 
{
    // arrange
    WaitMachineMock state_machine_mock;
    none event;
    WaitingForAck sut;

    ON_CALL(state_machine_mock.timer_, async_wait).WillByDefault(
        [](std::function<void(const SocketMock::SystemErrorCode&)> handler)
        {
            handler(asio::error::operation_aborted);
        });
    EXPECT_CALL(state_machine_mock, process_event(Matcher<TimedOut>(_))).Times(0);
    // act
    sut.on_entry(event, state_machine_mock);
    // assert
}

TEST(WaitStateTest, OnExit) 
{
    // arrange
    WaitMachineMock state_machine_mock;
    none event;
    WaitingForAck sut;

    EXPECT_CALL(state_machine_mock.socket_, cancel()).Times(AtLeast(1));
    EXPECT_CALL(state_machine_mock.timer_, cancel()).Times(AtLeast(1));    
    // act
    sut.on_exit(event, state_machine_mock);
    // assert
}

} // end anonymous namespace
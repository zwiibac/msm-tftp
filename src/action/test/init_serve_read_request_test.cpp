#include <boost/system/error_code.hpp>
#include <gmock/gmock-matchers.h>
#include <optional>
#include <string_view>
#include <string>
#include <tuple>
#include <limits>

#include <boost/noncopyable.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <zwiibac/tftp/protocol/error_code.h>
#include <zwiibac/tftp/protocol/opcode.h>
#include <zwiibac/tftp/protocol/mode.h>

#include <zwiibac/tftp/action/init_serve_read_request.h>

namespace {

using namespace zwiibac::tftp;

struct SocketMock : private boost::noncopyable
{
    using Endpoint = boost::asio::ip::udp::endpoint;
    using IpAddress = boost::asio::ip::address;
    using Protocol = boost::asio::ip::udp;
    using SystemErrorCode = boost::system::error_code;

    MOCK_METHOD(void, bind, (const Endpoint&, SystemErrorCode&));
    MOCK_METHOD(void, release, ());
    MOCK_METHOD(void, open, (const Protocol&, SystemErrorCode&));
};

struct FileStreamMock : private boost::noncopyable 
{
    // (const char* filename, ios_base::openmode mode = ios_base::in | ios_base::out);
    MOCK_METHOD(void, open, (const char* filename));
    MOCK_METHOD(bool, bad, ());
    MOCK_METHOD(bool, is_open, ());
};

struct Event 
{
    const OpCode op_code = OpCode::ReadRequest;
    const std::string_view file_name = "remoteFile.txt";
    const std::string_view mode = "SuperMode";
};

struct InitSendDataMachineMock : private boost::noncopyable
{
    SocketMock socket_;
    SocketMock::Endpoint local_data_endpoint_;
    FileStreamMock file_stream_;
    std::optional<ErrorCode> last_error_code_ = std::nullopt;
    SocketMock::IpAddress local_address_ = boost::asio::ip::address_v4::from_string("127.0.0.1");
    size_t agreed_block_size_ = 512;
    
    MOCK_METHOD(std::optional<Mode>, GetMode, (const std::string_view& mode));
    MOCK_METHOD(std::optional<std::string>, GetReadFilePath, (const std::string_view& file_name));
    MOCK_METHOD(int, CreateTid, ());
};

struct SourceState {};
struct TargetState {};

using testing::Return;
using testing::_;
using ::testing::StrEq;

class InitSendDataTest : public testing::Test
{
protected:    
    virtual void SetUp() override 
    {
        ON_CALL(state_machine_mock, GetMode).WillByDefault([&](auto){ return mode;});
        ON_CALL(state_machine_mock, GetReadFilePath(_)).WillByDefault([&](auto){ return file_name;});
        ON_CALL(state_machine_mock, CreateTid).WillByDefault([&](){ return tid;});
    }

    std::optional<Mode> mode;
    std::optional<std::string> file_name;
    int tid;
    std::optional<size_t> block_size;
    InitSendDataMachineMock state_machine_mock;
    SourceState source_state;
    TargetState target_state;
    std::optional<ErrorCode> expected_error_code;
};

TEST_F(InitSendDataTest, Do) 
{
    // arrange
    expected_error_code = std::nullopt;
    mode = Mode::NetAscii;    
    file_name = "/tmp/MyFileName.Txt";
    block_size = 1024;

    Event event;
    
    EXPECT_CALL(state_machine_mock, GetMode(event.mode)).Times(1);
    EXPECT_CALL(state_machine_mock, GetReadFilePath(event.file_name.data())).Times(1);
    EXPECT_CALL(state_machine_mock.file_stream_, open(StrEq(file_name.value().data()))).Times(1);

    // act
    InitServeReadRequest sut{};
    sut(event, state_machine_mock, source_state, target_state);
    // assert    
    EXPECT_EQ(state_machine_mock.agreed_block_size_, 512);
    EXPECT_EQ(state_machine_mock.last_error_code_, expected_error_code);
}

TEST_F(InitSendDataTest, IoError) 
{
    // arrange
    expected_error_code = ErrorCode::FileNotFound;
    mode = Mode::NetAscii;
    file_name = "MyFileName.Txt";
    block_size = 1024;
    
    ON_CALL(state_machine_mock.file_stream_, bad).WillByDefault(Return(true));
    Event event;
    // act
    InitServeReadRequest sut{};
    sut(event, state_machine_mock, source_state, target_state);
    // assert
    EXPECT_EQ(state_machine_mock.last_error_code_, expected_error_code);
}

TEST_F(InitSendDataTest, InvalidMode) 
{
    // arrange
    expected_error_code = ErrorCode::IllegalTftpOperation;
    mode = std::nullopt;
    file_name = "MyFileName.Txt";
    block_size = 1024;

    Event event;
    EXPECT_CALL(state_machine_mock, GetMode(event.mode)).Times(1);
    EXPECT_CALL(state_machine_mock.file_stream_, open(_)).Times(0);
    
    // act
    InitServeReadRequest sut{};
    sut(event, state_machine_mock, source_state, target_state);
    // assert    
    EXPECT_EQ(state_machine_mock.last_error_code_, expected_error_code);
}

TEST_F(InitSendDataTest, InvalidFilename) 
{
    // arrange
    expected_error_code = ErrorCode::FileNotFound;
    mode = Mode::NetAscii;
    file_name = std::nullopt;
    block_size = 1024;
    
    EXPECT_CALL(state_machine_mock.file_stream_, open(_)).Times(0);
    Event event;
    // act
    InitServeReadRequest sut{};
    sut(event, state_machine_mock, source_state, target_state);
    // assert    
    EXPECT_EQ(state_machine_mock.last_error_code_, expected_error_code);
}

TEST_F(InitSendDataTest, InvalidFilenameAndMode) 
{
    // arrange
    mode = std::nullopt;
    file_name = std::nullopt;
    block_size = 1024;
    
    EXPECT_CALL(state_machine_mock.file_stream_, open(_)).Times(0);
    Event event;
    // act
    InitServeReadRequest sut{};
    sut(event, state_machine_mock, source_state, target_state);
    // assert    
    EXPECT_NE(state_machine_mock.last_error_code_, std::nullopt);
}
    
} // end anonymous namespace

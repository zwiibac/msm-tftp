#pragma once

#include <chrono>
#include <functional>
#include <gmock/gmock-nice-strict.h>
#include <tuple>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <zwiibac/tftp/data/buffer.h>
#include <zwiibac/tftp/data/transfer_state.h>
#include <zwiibac/tftp/data/option.h>

#include <zwiibac/tftp/machine/session_data.h>
#include <zwiibac/tftp/machine/transition_table.h>
#include <zwiibac/tftp/machine/state_machine.h>

#include <zwiibac/tftp/protocol/tid.h>
#include <zwiibac/tftp/protocol/mode.h>
#include <zwiibac/tftp/protocol/options.h>

#include <zwiibac/tftp/test_utils/random_iterator.h>

#include "socket_mock.h"
#include "file_stream_mock.h"
#include "timer_mock.h"
#include "protocol_test_support.h"
#include "file_name_mock.h"

namespace zwiibac {
namespace tftp {
namespace testing {

std::pair<char*, size_t> GetBufferData(const boost::asio::mutable_buffer& buffer);

std::pair<const char*, size_t> GetBufferData(const boost::asio::const_buffer& buffer);

struct ConnectionFake
{
    using Endpoint = boost::asio::ip::udp::endpoint;
    using IpAddress = boost::asio::ip::address;
    using IoContext = boost::asio::io_service;

    SocketMock socket_;
    std::weak_ptr<SocketMock> listener_socket_;
    Endpoint agreed_remote_endpoint_;
    Endpoint last_remote_endpoint_;
    Endpoint local_data_endpoint_;

    IpAddress local_address_;

    inline void Reset() noexcept {};
};

struct FileFake
{
    ::testing::NiceMock<FileStreamMock> file_stream_;

    inline void DelegateFileStreamMockToFake() {file_stream_.DelegateToFake();}
    inline void Reset() noexcept {};
};

struct TimerFake
{
    ::testing::NiceMock<TimerMock> timer_;
    std::chrono::milliseconds time_out_{1000};

    inline void DelegateTimerMockToFake() {timer_.DelegateToFake();}
    inline void Reset() noexcept {};
};

template<class PartsWithIoContext, class ... Parts>
struct SessionDataMock : public PartsWithIoContext, Parts... 
{
    using IoContext = boost::asio::io_service;

    SessionDataMock(IoContext& io_context)
        : PartsWithIoContext(io_context)
        , Parts()... 
    {}

    MOCK_METHOD(void, Reset, ());

    void DoResetAll() noexcept 
    {
        PartsWithIoContext::Reset();
        ((Parts::Reset()) , ...);
    }
};

class BaseFixture : public ::testing::Test, protected ProtocolTestSupport
{
protected:
    using Data = SessionDataMock<SessionDataWithIoContext<>, ConnectionFake, FileFake, TimerFake, Buffer, TransferState, ::testing::NiceMock<FileNameMock>, Option, ModeDecoder, BlockSizeDecoder, TimeOutDecoder, TransferSizeDecoder, Tid>;
    using Server = StateMachine<Accepting, ReadRequestTransitionTable_t, Data>;

    SocketMock::IoContext io_context;
    Server sut{std::ref(io_context)};

    using Endpoint = SocketMock::Endpoint;
    using SendBuffer = SocketMock::SendBuffer;
    using ReceiveBuffer = SocketMock::ReceiveBuffer;
    using SystemErrorCode = SocketMock::SystemErrorCode;

    uint16_t number_of_blocks;
    size_t final_block_size;
    size_t agreed_block_size = 512;
    std::string file_data;
    Endpoint remote_end_point;

    std::shared_ptr<SocketMock> socket_mock = std::make_shared<SocketMock>();

    void SetUpFileData();

    void SetUpListenerSocket();

    void ExpectSendDataBlock(uint16_t block);

    void ExpectSendOack(std::initializer_list<const std::string> values);

    void ExpectSendError(ErrorCode error);

    void ExpectReceiveAck(uint16_t block);

    void ExpectReceiveTimeOut();

    void ExpectReceiveRequest(OpCode op, std::initializer_list<const std::string> values);

    void ExpectWhenReset(const std::function<void(const Server&)>& expectation);
};

} // end namespace testing
} // end namespace tftp
} // end namespace zwiibac
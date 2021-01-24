#include <functional>

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
    Endpoint last_remote_endpoint_;
    Endpoint local_data_endpoint_;

    std::chrono::milliseconds time_out_{500};
    std::array<char, 4096> receive_buffer_;
    size_t resend_count_ = 0;
    size_t agreed_block_size_ = 512;
    size_t last_sent_block_ = 81;
};

}

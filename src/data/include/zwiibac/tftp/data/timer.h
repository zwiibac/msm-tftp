#pragma  once

#include <boost/asio.hpp>

namespace zwiibac {
namespace tftp {

struct Timer 
{
    using WaitableTimer = boost::asio::basic_waitable_timer<std::chrono::steady_clock>;
    using IoContext = boost::asio::io_service;

    Timer(IoContext& io_context) 
        : timer_(io_context)
    {}

    inline void Reset() noexcept 
    {
        timer_.cancel();
    };

    WaitableTimer timer_;
    std::chrono::milliseconds time_out_{1000};
};

} // end namespace tftp
} // end namespace zwiibac
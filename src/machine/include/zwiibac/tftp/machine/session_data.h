#pragma once

#include <boost/asio.hpp>

namespace zwiibac {
namespace tftp {

template<class ... Parts>
struct SessionDataWithIoContext : public Parts... 
{
    using IoContext = boost::asio::io_service;

    SessionDataWithIoContext(IoContext& io_context)
        : Parts(io_context)... 
    {}

    void Reset() noexcept 
    {
        ((Parts::Reset()) , ...);
    }
};

template<class PartsWithIoContext, class ... Parts>
struct SessionData : public PartsWithIoContext, Parts... 
{
    using IoContext = boost::asio::io_service;

    SessionData(IoContext& io_context)
        : PartsWithIoContext(io_context)
        , Parts()... 
    {}

    void Reset() noexcept 
    {
        PartsWithIoContext::Reset();
        ((Parts::Reset()) , ...);
    }
};

} // end namespace tftp
} // end namespace zwiibac
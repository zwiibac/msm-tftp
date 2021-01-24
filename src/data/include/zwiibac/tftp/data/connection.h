#pragma  once

#include <memory>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

namespace zwiibac {
namespace tftp {

struct Connection 
{
    using Socket = boost::asio::ip::udp::socket;
    using Endpoint = Socket::endpoint_type;
    using IpAddress = boost::asio::ip::address;
    using IoContext = boost::asio::io_service;

    Connection(IoContext& io_context) 
        : io_context_(io_context) 
        , socket_(io_context)
    {}

    inline void Reset() noexcept 
    {
        socket_.close();
        socket_ = Socket(io_context_);
        agreed_remote_endpoint_ = Endpoint();
        last_remote_endpoint_ = Endpoint();
        local_data_endpoint_ = Endpoint();
    };
    
    IoContext& io_context_;
    std::weak_ptr<Socket> listener_socket_;
    Socket socket_;
    Endpoint agreed_remote_endpoint_;
    Endpoint last_remote_endpoint_;
    Endpoint local_data_endpoint_;
    IpAddress local_address_;
};

} // end namespace tftp
} // end namespace zwiibac
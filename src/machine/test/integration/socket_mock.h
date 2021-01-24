#pragma once

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>

#include <gmock/gmock.h>

class SocketMock : private boost::noncopyable
{
public:
    using SendBuffer = boost::asio::const_buffer;
    using ReceiveBuffer = boost::asio::mutable_buffer;
    using SystemErrorCode = boost::system::error_code;
    using Endpoint = boost::asio::ip::udp::endpoint;
    using IoContext = boost::asio::io_service;
    using Protocol = boost::asio::ip::udp;

    MOCK_METHOD(void, bind, (const Endpoint&, SystemErrorCode&));
    MOCK_METHOD(void, async_receive_from, (const ReceiveBuffer& buffer, Endpoint& end_point, std::function<void(const SystemErrorCode&, size_t)>));
    MOCK_METHOD(void, async_send_to, (const SendBuffer& buffer, const Endpoint&, std::function<void(const SystemErrorCode&, size_t)>));
    MOCK_METHOD(void, cancel, ()); // should return size_t
    MOCK_METHOD(void, release, ());
    MOCK_METHOD(void, open, (const Protocol&, SystemErrorCode&));
};
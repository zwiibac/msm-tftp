#pragma once

#include <algorithm>

#include<boost/asio.hpp>

#include <zwiibac/tftp/logger/logger.h>
#include <zwiibac/tftp/protocol/header.h>
#include <zwiibac/tftp/protocol/opcode.h>
#include <zwiibac/tftp/protocol/trace.h>
#include <zwiibac/tftp/event/accepting_failed.h>
#include <zwiibac/tftp/event/request_received.h>

namespace zwiibac {
namespace tftp {

struct EnterAccepting 
{
    template <class Evt,class Fsm>
    void operator()(const Evt& evt, Fsm& fsm) const
    {
        using SystemErrorCode = boost::system::error_code;
        using MutableBuffer = boost::asio::mutable_buffer;
        using Endpoint = boost::asio::ip::udp::endpoint;

        std::fill(fsm.receive_buffer_.begin(), fsm.receive_buffer_.end(), '\0');

        if (auto listener_socket = fsm.listener_socket_.lock()) 
        {
            listener_socket->async_receive_from(
                MutableBuffer(fsm.receive_buffer_.data(), fsm.receive_buffer_.size()),
                fsm.agreed_remote_endpoint_, 
                [&](const SystemErrorCode& accepting_ec, size_t bytes_received) 
                {
                    fsm.received_packet_ = {fsm.receive_buffer_.data(), bytes_received};

                    ZWIIB_LOG_IF(error, accepting_ec) << "Failed to receive from " 
                        << fsm.local_data_endpoint_ << " | " << accepting_ec << " " << accepting_ec.message();

                    if (accepting_ec) 
                    {
                        fsm.process_event(AcceptingFailed{accepting_ec, bytes_received});
                        return;
                    }

                    ProcessEventFromBuffer(fsm, bytes_received);
                });
        }
    }

    template<class Fsm>
    void ProcessEventFromBuffer(Fsm& fsm, size_t bytes_received) const
    {
        if (fsm.receive_buffer_[bytes_received-1] != '\0')
        {
            // in order for the null terminated strings to work the last received byte must be null.
            fsm.process_event(AcceptingFailed{});
        }

        auto iter = fsm.receive_buffer_.cbegin();
        const auto end = fsm.receive_buffer_.cbegin() + bytes_received;


        if (++iter == end || ++iter == end) 
        {
            // not even an opcode
            fsm.process_event(AcceptingFailed{});
        }

        OpCode op_code = ShortHeaderView::FromBuffer(fsm.receive_buffer_).OpCd();

        size_t parsed_bytes = ShortHeaderView::Size();

        std::string_view file_name = iter;
        if ((iter = std::find(iter, end, '\0')) == end || ++iter == end) 
        {
            // less arguments than expected
            fsm.process_event(RequestReceived{op_code, file_name});
            return;
        }

        std::string_view mode = iter;
        if ((iter = std::find(iter, end, '\0')) == end || ++iter == end) 
        {
            ZWIIB_LOG(trace) << fsm.local_data_endpoint_.port() << "|" <<  fsm.agreed_remote_endpoint_  << "|"
                << "received " << op_code << " <file=" << file_name << ", mode=" << mode << ">";
            fsm.process_event(RequestReceived{op_code, file_name, mode});
            return;
        }

        // still some options to parse
        ZWIIB_LOG(warning) << "unparsed options";
        ZWIIB_LOG(trace) << fsm.local_data_endpoint_.port() << "|" <<  fsm.agreed_remote_endpoint_ << "|"
            << "received " << op_code << " <file=" << file_name << ", mode=" << mode << ">";
        fsm.process_event(RequestReceived{op_code, file_name, mode});
    }
};

} // end namespace tftp
} // end namespace zwiibac
#pragma once

#include <algorithm>
#include <ctype.h>

#include<boost/asio.hpp>

#include <zwiibac/tftp/logger/logger.h>
#include <zwiibac/tftp/protocol/header.h>
#include <zwiibac/tftp/protocol/opcode.h>
#include <zwiibac/tftp/protocol/options.h>
#include <zwiibac/tftp/protocol/request.h>
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
        if (bytes_received < ShortHeaderView::Size() + 2 || fsm.receive_buffer_[bytes_received-1] != '\0')
        {
            // in order for the null terminated strings to work the last received byte must be null.
            fsm.process_event(AcceptingFailed{});
        }
    
        RequestReceived request_received_event;

        request_received_event.op_code = ShortHeaderView::FromBuffer(fsm.receive_buffer_).OpCd();

        RequestTokenizer request_tokens(
            fsm.receive_buffer_.cbegin() + ShortHeaderView::Size(), 
            fsm.receive_buffer_.cbegin() + bytes_received);

        auto token_iter = request_tokens.begin();
        
        request_received_event.file_name = *token_iter;            

        if (++token_iter == request_tokens.end()) 
        {
            fsm.process_event(request_received_event);
            return;
        }

        request_received_event.mode = *token_iter;

        if (++token_iter == request_tokens.end()) 
        {
            fsm.process_event(request_received_event);
            return;
        }

        do {
            auto key = *token_iter;

            if (++token_iter == request_tokens.end()) 
            {
                break;
            }

            if (fsm.IsBlockSizeOption(key)) 
            {
                request_received_event.block_size_= *token_iter;
                continue;
            }

            if (fsm.IsTimeOutOption(key)) 
            {
                request_received_event.time_out_ = *token_iter;
                continue;
            }

            if (fsm.IsTransferSizeOption(key)) 
            {
                request_received_event.transfer_size_ = *token_iter;
                continue;
            }

            ZWIIB_LOG(warning) << "unknown option \""<< key << "\"";
        } while (++token_iter != request_tokens.end());

        // ZWIIB_LOG(trace) << fsm.local_data_endpoint_.port() << "|" <<  fsm.agreed_remote_endpoint_ << "|"
        //     << "received " << request_received_event.op_code 
        //     << " <file=" << request_received_event.file_name 
        //     << ", mode=" << request_received_event.mode << ">";
        fsm.process_event(request_received_event);
        return;
    }
};

} // end namespace tftp
} // end namespace zwiibac
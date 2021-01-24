#pragma once

#include "zwiibac/tftp/protocol/header.h"
#include <span>
#include <ostream>

#include <boost/asio.hpp>

#include <zwiibac/tftp/logger/logger.h>

#include <zwiibac/tftp/event/packet_received.h>
#include <zwiibac/tftp/event/timed_out.h>

#include <zwiibac/tftp/protocol/trace.h>

namespace zwiibac {
namespace tftp {



struct EnterReceive 
{
    using SystemErrorCode = boost::system::error_code;
    using MutableBuffer = boost::asio::mutable_buffers_1;

    template <class Evt,class Fsm>
    void operator()(const Evt& evt, Fsm& fsm) const
    {
        fsm.socket_.async_receive_from(
            MutableBuffer(fsm.receive_buffer_.data(), fsm.receive_buffer_.size()), 
            fsm.last_remote_endpoint_, 
            [&](const SystemErrorCode& ec, size_t bytes_transferred) 
            {
                ZWIIB_LOG_IF(error, ec && ec != boost::asio::error::operation_aborted) 
                    << "Failed to receive data from " << fsm.last_remote_endpoint_ << " | " << ec << " " << ec.message();

                if (!ec) 
                {
                    fsm.timer_.cancel();
                    fsm.received_packet_ = {fsm.receive_buffer_.data(), bytes_transferred};
                    ZWIIB_LOG(trace) << fsm.local_data_endpoint_.port() << "|" <<  fsm.last_remote_endpoint_ 
                        << "|" << detail::RcvdPacket{fsm.receive_buffer_.data(), bytes_transferred};
                    fsm.process_event(PacketReceived());
                }
                else 
                {
                    fsm.received_packet_ = {fsm.receive_buffer_.data(), 0};
                    fsm.process_event(TimedOut());
                }               
            });
        fsm.timer_.expires_from_now(fsm.time_out_);
        fsm.timer_.async_wait([&](const SystemErrorCode& ec) 
            {
                if (ec != boost::asio::error::operation_aborted)
                {
                    ZWIIB_LOG(trace) << fsm.local_data_endpoint_.port() << "|" <<  fsm.agreed_remote_endpoint_  << "|" << "time out";
                    fsm.socket_.cancel();
                }
            });
    }
};

} // end namespace tftp
} // end namespace zwiibac
#pragma once

#include <boost/system/system_error.hpp>

#include <zwiibac/tftp/logger/logger.h>

#include <zwiibac/tftp/event/packet_sent.h>

#include <zwiibac/tftp/protocol/trace.h>

namespace zwiibac {
namespace tftp {

struct EnterSending 
{
    template <class Evt,class Fsm>
    void operator()(const Evt& evt, Fsm& fsm)
    {
        fsm.socket_.async_send_to(fsm.send_packet_, fsm.agreed_remote_endpoint_, 
            [&](const boost::system::error_code& ec, size_t bytes_transferred)
            {
                ZWIIB_LOG_IF(error, ec) << "Send data to " << fsm.agreed_remote_endpoint_ << " failed | " << ec << " " << ec.message();
                ZWIIB_LOG_IF(trace, !ec) << fsm.local_data_endpoint_.port() << "|" <<  fsm.agreed_remote_endpoint_ << "|"
                    << detail::SentPacket{static_cast<const char*>(fsm.send_packet_.data()), bytes_transferred};
                fsm.process_event(PacketSent{});
            });
    }
};

} // end namespace tftp
} // end namespace zwiibac
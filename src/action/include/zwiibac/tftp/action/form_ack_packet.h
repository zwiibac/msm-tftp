#pragma once

#include <zwiibac/tftp/protocol/opcode.h>
#include <zwiibac/tftp/protocol/header.h>
#include <zwiibac/tftp/protocol/error_code.h>

namespace zwiibac {
namespace tftp {

struct FormAckPacket 
{
    template <class Evt, class Fsm, class SourceState, class TargetState>
    void operator()(Evt const& ,Fsm& fsm, const SourceState&, const TargetState& ) const
    {
        const auto received_header = HeaderView::FromBuffer(fsm.received_packet_);

        auto header = HeaderProxy::FromBuffer(fsm.send_buffer_);
        header.SetOpCd(OpCode::Acknowledgment);
        header.SetWord(fsm.last_saved_block_);
        fsm.send_packet_ = {fsm.send_buffer_.data(), HeaderProxy::Size()};
    }
};

} // end namespace tftp
} // end namespace zwiibac
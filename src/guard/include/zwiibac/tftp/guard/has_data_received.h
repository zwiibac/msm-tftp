#pragma once

#include <zwiibac/tftp/protocol/opcode.h>
#include <zwiibac/tftp/protocol/header.h>

namespace zwiibac {
namespace tftp {

struct HasDataReceived 
{
    template <class Evt,class Fsm,class SourceState,class TargetState>
    constexpr bool operator()(const Evt&,const Fsm& fsm,const SourceState&,const TargetState&) const
    {
        const auto received_header = HeaderView::FromBuffer(fsm.received_packet_);
        return fsm.received_packet_.size() >= HeaderView::Size()
            && received_header.OpCd() == OpCode::Data
            && fsm.last_remote_endpoint_ == fsm.agreed_remote_endpoint_;
    }
};

} // end namespace tftp
} // end namespace zwiibac
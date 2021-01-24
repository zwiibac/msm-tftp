#pragma once

#include <zwiibac/tftp/protocol/opcode.h>
#include <zwiibac/tftp/protocol/header.h>

namespace zwiibac {
namespace tftp {

struct HasWriteRequestReceived 
{
    template <class Evt,class Fsm,class SourceState,class TargetState>
    constexpr bool operator()(const Evt&,const Fsm& fsm,const SourceState&,const TargetState&) const
    {
        auto received_header = HeaderView::FromBuffer(fsm.received_packet_);
        return received_header.OpCd() == OpCode::WriteRequest;
    }
};

} // end namespace tftp
} // end namespace zwiibac
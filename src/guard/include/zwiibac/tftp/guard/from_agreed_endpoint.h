#pragma once

#include <zwiibac/tftp/protocol/opcode.h>
#include <zwiibac/tftp/protocol/header.h>

namespace zwiibac {
namespace tftp {

struct FromAgreedEndpoint 
{
    template <class Evt,class Fsm,class SourceState,class TargetState>
    constexpr bool operator()(const Evt&,const Fsm& fsm,const SourceState&,const TargetState&) const
    {
        return fsm.last_remote_endpoint_ == fsm.agreed_remote_endpoint_;
    }
};

} // end namespace tftp
} // end namespace zwiibac
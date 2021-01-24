#pragma once

#include <zwiibac/tftp/protocol/opcode.h>
#include <zwiibac/tftp/protocol/header.h>

namespace zwiibac {
namespace tftp {

struct HasMoreToSend
{
    template <class Evt,class Fsm,class SourceState,class TargetState>
    constexpr bool operator()(const Evt&,const Fsm& fsm,const SourceState&,const TargetState&) const
    {
        return fsm.last_block_size_ == fsm.agreed_block_size_;
    }
};

} // end namespace tftp
} // end namespace zwiibac
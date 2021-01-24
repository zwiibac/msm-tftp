#pragma once

#include <zwiibac/tftp/logger/logger.h>

namespace zwiibac {
namespace tftp {

struct TimeOut 
{
    template <class Evt,class Fsm,class SourceState,class TargetState>
    void operator()(Evt const& ,Fsm& fsm,SourceState& ,TargetState& )
    {
        fsm.resend_count_ += 1;
    }
};

} // end namespace tftp
} // end namespace zwiibac
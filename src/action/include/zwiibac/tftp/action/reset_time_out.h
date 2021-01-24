#pragma once

namespace zwiibac {
namespace tftp {

struct ResetTimeOut 
{
    template <class Evt,class Fsm,class SourceState,class TargetState>
    void operator()(Evt const& ,Fsm& fsm,SourceState& ,TargetState& )
    {
        fsm.resend_count_ = 0;
    }
};

} // end namespace tftp
} // end namespace zwiibac
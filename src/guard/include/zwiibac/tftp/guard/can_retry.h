#pragma once

namespace zwiibac {
namespace tftp {

struct CanRetry 
{
    template <class Evt,class Fsm,class SourceState,class TargetState>
    constexpr bool operator()(const Evt&,const Fsm& fsm,const SourceState&,const TargetState&) const
    {
        return fsm.resend_count_ < 3;
    }
};

} // end namespace tftp
} // end namespace zwiibac
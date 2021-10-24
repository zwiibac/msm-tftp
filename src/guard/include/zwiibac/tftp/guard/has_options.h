#pragma once

namespace zwiibac {
namespace tftp {

struct HasOptions 
{
    template <class Evt,class Fsm,class SourceState,class TargetState>
    constexpr bool operator()(const Evt&,const Fsm& fsm,const SourceState&,const TargetState&) const
    {
        return fsm.use_block_size_option_ || fsm.use_time_out_option_ || fsm.use_transfer_size_option_;
    }
};

} // end namespace tftp
} // end namespace zwiibac
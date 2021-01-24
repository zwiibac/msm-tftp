#pragma once

namespace zwiibac {
namespace tftp {

struct ExitSending
{
    template <class Evt,class Fsm>
    void operator()(const Evt& evt, Fsm& fsm) const
    {
        fsm.socket_.cancel();
        fsm.timer_.cancel();
    }
};

} // end namespace tftp
} // end namespace zwiibac
#pragma once

namespace zwiibac {
namespace tftp {

struct ExitAccepting 
{
    template <class Evt,class Fsm>
    void operator()(const Evt& evt, Fsm& fsm) const
    {
        //fsm.socket_.cancel();
    }
};

} // end namespace tftp
} // end namespace zwiibac
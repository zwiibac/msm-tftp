#pragma once

namespace zwiibac {
namespace tftp {

template<class EntryAction, class ExitAction, class State>
struct StateBase : public State
{
    template <class Evt, class Fsm>
    void on_entry(const Evt& evt, Fsm& fsm)
    {
        EntryAction action;
        action(evt, fsm);
    }

    template <class Evt, class Fsm>
    void on_exit(const Evt& evt, Fsm& fsm)
    {
        ExitAction action;
        action(evt, fsm);
    }
};

} // end namespace tftp
} // end namespace zwiibac
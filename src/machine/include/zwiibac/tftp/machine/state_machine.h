#pragma once

#include <boost/asio.hpp>

// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>

namespace zwiibac {
namespace tftp {

template<class InitialState, class TransitionTable, class Data>
struct StateMachine_ : public Data, boost::msm::front::state_machine_def<StateMachine_<InitialState, TransitionTable, Data>> 
{
    using initial_state = InitialState;
    using transition_table = TransitionTable;

    using IoContext = boost::asio::io_service;
    
    StateMachine_(IoContext& io_context)
        : Data(io_context) 
    {}
};

template<class InitialState, class TransitionTable, class Data>
using StateMachine = boost::msm::back::state_machine<StateMachine_<InitialState, TransitionTable, Data> >;

} // end namespace tftp
} // end namespace zwiibac
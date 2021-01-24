#pragma once

namespace zwiibac {
namespace tftp {

template<class GuardA, class GuardB>
struct Or
{
    template <class Evt,class Fsm,class SourceState,class TargetState>
    constexpr bool operator()(const Evt& event,const Fsm& fsm,const SourceState& source_state,const TargetState& target_state) const
    {
        const GuardA guard_a;
        const GuardB guard_b;
        return guard_a(event, fsm, source_state, target_state) || guard_b(event, fsm, source_state, target_state);
    }
};

namespace detail {
template<class ... Guards>
struct And;

template<class GuardA, class ... Guards>
struct And<GuardA, Guards...>
{
    template <class Evt,class Fsm,class SourceState,class TargetState>
    constexpr bool operator()(const Evt& event,const Fsm& fsm,const SourceState& source_state,const TargetState& target_state) const
    {
        const GuardA guard_a;
        const And<Guards...> guard_b;
        return guard_a(event, fsm, source_state, target_state) && guard_b(event, fsm, source_state, target_state);
    }
};

template<>
struct And<>
{
    template <class Evt,class Fsm,class SourceState,class TargetState>
    constexpr bool operator()(const Evt& event,const Fsm& fsm,const SourceState& source_state,const TargetState& target_state) const
    {
        return true;
    }
};
} // end namespace detail

template<typename ... Guards>
struct And
{
    template <class Evt,class Fsm,class SourceState,class TargetState>
    constexpr bool operator()(const Evt& event,const Fsm& fsm,const SourceState& source_state,const TargetState& target_state) const
    {
        const detail::And<Guards...> guard;        
        return guard(event, fsm, source_state, target_state);
    }
};


template<class GuardA>
struct Not
{
    template <class Evt,class Fsm,class SourceState,class TargetState>
    constexpr bool operator()(const Evt& event,const Fsm& fsm,const SourceState& source_state,const TargetState& target_state) const
    {
        const GuardA guard_a;
        return !guard_a(event, fsm, source_state, target_state);
    }
};

} // end namespace tftp
} // end namespace zwiibac
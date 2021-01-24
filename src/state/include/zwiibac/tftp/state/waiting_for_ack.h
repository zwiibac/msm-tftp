#pragma once

#include <boost/msm/front/state_machine_def.hpp>

#include <zwiibac/tftp/state/entry_action/enter_receive.h>
#include <zwiibac/tftp/state/exit_action/exit_receive.h>
#include <zwiibac/tftp/state/state_base.h>

namespace zwiibac {
namespace tftp {

struct WaitingForAck : public StateBase<EnterReceive, ExitReceive, boost::msm::front::state<>> {};

} // end namespace tftp
} // end namespace zwiibac

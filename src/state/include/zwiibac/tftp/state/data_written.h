#pragma once

#include <boost/msm/front/state_machine_def.hpp>

namespace zwiibac {
namespace tftp {

struct DataWritten : public boost::msm::front::state<> {};

} // end namespace tftp
} // end namespace zwiibac
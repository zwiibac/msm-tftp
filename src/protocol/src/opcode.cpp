#include <algorithm>
#include <string>
#include <string_view>
#include <charconv>
#include <random>

#include <zwiibac/tftp/protocol/opcode.h>

namespace zwiibac {
namespace tftp {

std::ostream& operator<<(std::ostream& os, OpCode value)  { return os << ToString(value); }

} // end namespace tftp
} // end namespace zwiibac
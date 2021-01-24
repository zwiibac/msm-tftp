#include <zwiibac/tftp/protocol/error_code.h>

namespace zwiibac {
namespace tftp {

std::ostream& operator<<(std::ostream& os, ErrorCode value) { return os << ToString(value); }

} // end namespace tftp
} // end namespace zwiibac
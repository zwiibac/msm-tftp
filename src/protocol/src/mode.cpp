#include <zwiibac/tftp/protocol/mode.h>

namespace zwiibac {
namespace tftp {

std::ostream& operator<<(std::ostream& os, Mode value) 
{
    return os << ToString(value);
}

} // end namespace tftp
} // end namespace zwiibac
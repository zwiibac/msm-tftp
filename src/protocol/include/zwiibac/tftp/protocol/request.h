#include <string_view>

#include "opcode.h"

namespace zwiibac {
namespace tftp {

struct Request 
{
    const OpCode op_code;
    const std::string_view file_name;
    const std::string_view mode;
};

} // end namespace tftp
} // end namespace zwiibac
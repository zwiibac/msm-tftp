#pragma once

#include <string_view>

#include <zwiibac/tftp/protocol/opcode.h>

namespace zwiibac {
namespace tftp {

struct RequestReceived 
{
    const OpCode op_code = static_cast<OpCode>(0);
    const std::string_view file_name;
    const std::string_view mode;
};

} // end namespace tftp
} // end namespace zwiibac
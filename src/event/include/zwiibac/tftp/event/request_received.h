#pragma once

#include <string_view>

#include <zwiibac/tftp/protocol/opcode.h>

namespace zwiibac {
namespace tftp {

struct RequestReceived 
{
    OpCode op_code = static_cast<OpCode>(0);
    std::string_view file_name;
    std::string_view mode;
    std::string_view block_size_;
    std::string_view time_out_;
    std::string_view transfer_size_;
};

} // end namespace tftp
} // end namespace zwiibac
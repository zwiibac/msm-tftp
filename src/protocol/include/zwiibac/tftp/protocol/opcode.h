#pragma once

#include <optional>
#include <cstdint>
#include <ostream>

namespace zwiibac {
namespace tftp {

enum class OpCode : uint16_t // rfc1350
{
    ReadRequest = 1,        // Read request (RRQ)
    WriteRequest = 2,       // Write request (WRQ)
    Data = 3,               // Data (DATA)
    Acknowledgment = 4,     // Acknowledgment (ACK)
    Error = 5,              // Error (ERROR)
    OptionAcknowledgment =6 // Option Acknowledgment (OACK)
};

inline constexpr std::string_view ToString(OpCode value) noexcept 
{
    switch (value)
    {
    case OpCode::ReadRequest:
        return "RRQ";
    case OpCode::WriteRequest:
        return "WRQ";
    case OpCode::Data:
        return "DATA";
    case OpCode::Error:
        return "ERR";
    case OpCode::Acknowledgment:
        return "ACK";
    case OpCode::OptionAcknowledgment:
        return "OACK";
    default:
        return "<unknown OpCode>";
    }
}

std::ostream& operator<<(std::ostream& os, OpCode value);

} // end namespace tftp
} // end namespace zwiibac
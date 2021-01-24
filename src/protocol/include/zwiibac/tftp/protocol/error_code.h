#pragma once

#include <optional>
#include <cstdint>
#include <ostream>

namespace zwiibac {
namespace tftp {

enum class ErrorCode : uint16_t // rfc1350
{
    NotDefined = 0,           // Not defined, see error message (if any).
    FileNotFound = 1,         // File not found.
    AccessViolation = 2,     // Access violation.
    DiskFull = 3,             // Disk full or allocation exceeded.
    IllegalTftpOperation = 4, // Illegal TFTP operation.
    UnknownTransferId = 5,    // Unknown transfer ID.
    FileAlreadyExists = 6,    // File already exists.
    NoSuchUser = 7            // No such user.
};

inline constexpr std::string_view ToString(ErrorCode value) noexcept
{
    switch (value)
    {
    case ErrorCode::NotDefined:
        return "Not defined, see error message (if any).";
    case ErrorCode::FileNotFound:
        return "File not found.";
    case ErrorCode::AccessViolation:
        return "Access violation.";
    case ErrorCode::DiskFull:
        return "Disk full or allocation exceeded.";
    case ErrorCode::IllegalTftpOperation:
        return "Illegal TFTP operation";
    case ErrorCode::UnknownTransferId:
        return "Unknown transfer ID.";   
    case ErrorCode::FileAlreadyExists:
        return "File already exists.";   
    case ErrorCode::NoSuchUser:
        return "No such user.";   
    default:
        return "<unknown ErrorCode>";
    }
}

std::ostream& operator<<(std::ostream& os, ErrorCode value);

} // end namespace tftp
} // end namespace zwiibac
#pragma once

#include <cstdint>
#include <cstddef>

namespace zwiibac {
namespace tftp {

constexpr size_t kDataHeaderSize = 4; // DATA, ACK or ERROR
constexpr int kMinTid = 1024;
constexpr int kMaxTid = 65535;
constexpr size_t kMinDataBlockCount = 8; // rfc2348
constexpr size_t kMaxDataBlockCount = 65464; // rfc2348
constexpr size_t kDefaultDataBlockCount = 512; // rfc1350

constexpr size_t kBufferSize = 64*1024;

} // end namespace tftp
} // end namespace zwiibac
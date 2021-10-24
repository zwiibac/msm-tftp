#pragma once

#include <cstdint>
#include <string_view>

namespace zwiibac {
namespace tftp {

struct Tid
{
    const uint16_t min_tid_ = 1024;
    const uint16_t max_tid_ = 65535;
    uint16_t CreateTid();
    inline void Reset() noexcept {};
};

} // end namespace tftp
} // end namespace zwiibac
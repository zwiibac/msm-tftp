#pragma once

#include <array>

#include <zwiibac/tftp/protocol/limits.h>

namespace zwiibac {
namespace tftp {

struct Buffer 
{
    std::array<char, kBufferSize> receive_buffer_;
    std::array<char, kBufferSize> send_buffer_;

    inline void Reset() noexcept 
    {
        std::fill(receive_buffer_.begin(), receive_buffer_.end(), '\0');
        std::fill(send_buffer_.begin(), send_buffer_.end(), '\0');
    };
};

} // end namespace tftp
} // end namespace zwiibac
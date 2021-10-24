#pragma once

namespace zwiibac {
namespace tftp {

struct Option 
{
    bool use_block_size_option_ = false;
    bool use_time_out_option_ = false;
    bool use_transfer_size_option_ = false;

    inline void Reset() noexcept 
    {
        use_block_size_option_ = false;
        use_time_out_option_ = false;
        use_transfer_size_option_ = false;
    };
};

} // end namespace tftp
} // end namespace zwiibac
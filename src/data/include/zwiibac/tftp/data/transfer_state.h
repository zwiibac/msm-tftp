#pragma once

#include <fstream>
#include <cstddef>
#include <span>
#include <optional>

#include <boost/asio.hpp>

#include <zwiibac/tftp/protocol/error_code.h>

namespace zwiibac {
namespace tftp {

struct TransferState 
{
    size_t agreed_block_size_ = 512;
    size_t last_block_size_ = 0;
    size_t last_sent_block_ = 0;
    size_t last_saved_block_ = 0;
    size_t resend_count_ = 0;

    boost::asio::const_buffer send_packet_;
    std::span<const char> received_packet_;

    std::optional<ErrorCode> last_error_code_ = std::nullopt;

    inline void Reset() noexcept 
    {
        agreed_block_size_ = 512;
        last_block_size_ = 0;
        last_sent_block_ = 0;
        last_saved_block_ = 0;
        resend_count_ = 0;
        received_packet_ = std::span<const char>();
        send_packet_ = {nullptr, 0};
        last_error_code_ = std::nullopt;
    };
};

} // end namespace tftp
} // end namespace zwiibac
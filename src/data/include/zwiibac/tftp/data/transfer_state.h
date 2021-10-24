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
    static constexpr size_t kDefaultDataBlockCount = 512; // rfc1350

    size_t agreed_block_size_ = kDefaultDataBlockCount;
    size_t last_block_size_ = 0;
    size_t last_sent_block_ = 0;
    size_t last_saved_block_ = 0;
    size_t resend_count_ = 0;
    std::optional<size_t> file_size_ = std::nullopt;

    boost::asio::const_buffer send_packet_;
    std::span<const char> received_packet_;

    std::optional<ErrorCode> last_error_code_ = std::nullopt;

    inline void Reset() noexcept 
    {
        agreed_block_size_ = kDefaultDataBlockCount;
        last_block_size_ = 0;
        last_sent_block_ = 0;
        last_saved_block_ = 0;
        resend_count_ = 0;
        std::optional<size_t> file_size_ = std::nullopt;
        received_packet_ = std::span<const char>();
        send_packet_ = {nullptr, 0};
        last_error_code_ = std::nullopt;
    };
};

} // end namespace tftp
} // end namespace zwiibac
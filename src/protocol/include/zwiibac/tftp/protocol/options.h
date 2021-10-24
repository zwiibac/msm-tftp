#pragma once

#include <algorithm>
#include <optional>
#include <chrono>
#include <cstddef>
#include <string_view>
#include <charconv>

#include "case_invariant_equal.h"

namespace zwiibac {
namespace tftp {

struct BlockSizeDecoder 
{
    constexpr BlockSizeDecoder() = default; 

    inline constexpr bool IsBlockSizeOption(const std::string_view& key) const
    {
        using zwiibac::tftp::detail::Equal;
        return Equal(key, kBlockSizeOption);
    }

    inline std::optional<size_t> GetBlockSize(const std::string_view& value) const
    {
        size_t result;
        auto [iter, ec] {std::from_chars(value.data(), value.data() + value.size(), result)};
        if (ec != std::errc() || iter != value.data() + value.size() || !IsValidBlockSize(result))
        {
            return std::nullopt;
        }

        return result;
    }

    inline constexpr bool IsValidBlockSize(size_t value) const
    {
        return !(value < kMinDataBlockCount || value > kMaxDataBlockCount);
    }

    inline void Reset() noexcept {};

    static constexpr size_t kMinDataBlockCount = 8; // rfc2348
    static constexpr size_t kMaxDataBlockCount = 65464; // rfc2348
    static constexpr std::string_view kBlockSizeOption = "blksize"; // rfc2348 
};

struct TimeOutDecoder 
{
    constexpr TimeOutDecoder() = default; 

    inline constexpr bool IsTimeOutOption(const std::string_view& key) const
    {
        using zwiibac::tftp::detail::Equal;
        return Equal(key, kTimeOutOption);
    }

    inline std::optional<std::chrono::milliseconds> GetTimeOut(const std::string_view& value) const 
    {
        uint8_t result;
        auto [iter, ec] {std::from_chars(value.data(), value.data() + value.size(), result)};
        if (ec != std::errc() || iter != value.data() + value.size() || result == 0) 
        {
            return std::nullopt;
        }
        return std::chrono::seconds{result};
    }

    inline void Reset() noexcept {};

    static constexpr std::string_view kTimeOutOption = "timeout"; // rfc2349
};

struct TransferSizeDecoder
{
    constexpr TransferSizeDecoder() = default;

    inline constexpr bool IsTransferSizeOption(const std::string_view& key) const 
    {
        using zwiibac::tftp::detail::Equal;
        return Equal(key, kTransferSizeOption);
    }

    inline std::optional<size_t> GetTransferSize(const std::string_view& value) const
    {
        size_t result;
        auto [iter, ec] {std::from_chars(value.data(), value.data() + value.size(), result)};
        if (ec != std::errc() || iter != value.data() + value.size())
        {
            return std::nullopt;
        }

        return result;
    }

    inline void Reset() noexcept {};

    static constexpr std::string_view kTransferSizeOption = "tsize";
};

} // end namespace tftp
} // end namespace zwiibac
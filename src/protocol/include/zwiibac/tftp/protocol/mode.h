#pragma once

#include <optional>
#include <string_view>
#include <ostream>

namespace zwiibac {
namespace tftp {

enum class Mode 
{
    NetAscii,
    Octet,
    Mail
};

constexpr std::string_view kNetasciiMode = "netascii";
constexpr std::string_view kMailMode = "mail";
constexpr std::string_view kOctetMode = "octet";

inline constexpr std::optional<Mode> GetMode(const std::string_view& mode)
{
    auto Equal = [](const std::string_view& value1, const std::string_view& value2)
    {
        return std::equal(value1.begin(), value1.end(), 
        value2.begin(), value2.end(),
        [](auto a, auto b) { return tolower(a) == b; });
    };

    if (Equal(mode, kOctetMode)) 
        return std::optional<Mode>(Mode::Octet);

    if (Equal(mode, kNetasciiMode))
        return std::optional<Mode>(Mode::NetAscii);

    if (Equal(mode, kMailMode))
        return std::optional<Mode>(Mode::Mail);

    return std::nullopt;
};

inline constexpr std::string_view ToString(Mode mode) noexcept
{
    switch (mode)
    {
    case Mode::NetAscii:
        return kNetasciiMode;
    case Mode::Mail:
        return kMailMode;
    case Mode::Octet:
        return kMailMode;
    default:
        return "<unknown Mode>";
    }
};

std::ostream& operator<<(std::ostream& os, Mode value);

struct ModeDecoder 
{
    inline constexpr std::optional<Mode> GetMode(const std::string_view& mode) const noexcept
    {
        return zwiibac::tftp::GetMode(mode);
    }

    inline void Reset() noexcept {};
};

} // end namespace tftp
} // end namespace zwiibac
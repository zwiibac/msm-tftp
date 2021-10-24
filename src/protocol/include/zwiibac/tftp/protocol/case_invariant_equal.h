#pragma once

#include <algorithm>
#include <cstddef>
#include <string_view>

namespace zwiibac {
namespace tftp {
namespace detail {

constexpr inline bool Equal(const std::string_view& value1, const std::string_view& value2)
{
    return std::equal(value1.begin(), value1.end(), 
    value2.begin(), value2.end(),
    [](auto a, auto b) { return tolower(a) == b; });
};

} // end namespace detail
} // end namespace tftp
} // end namespace zwiibac
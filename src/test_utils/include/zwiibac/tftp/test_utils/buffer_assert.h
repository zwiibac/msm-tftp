#pragma once

#include <gtest/gtest.h>
#include <string>
#include <array>

#include <zwiibac/tftp/protocol/header.h>

namespace zwiibac {
namespace tftp {
namespace test_utils {

template<class Buffer>
inline void AssertEq(std::string expected_data, Buffer actual_buffer, size_t block_number, size_t block_size) 
{
    EXPECT_EQ(actual_buffer.size(), HeaderProxy::Size() + block_size);
    
    // could use std::span<const char> instead of const char*
    const char* actual_data = reinterpret_cast<const char*>(actual_buffer.data());

    for (int index=block_number*block_size; index<block_size; ++index) 
    {
        auto expected_value = expected_data[index];
        auto actual_value = actual_data[HeaderProxy::Size() + index];
        ASSERT_EQ(expected_value, actual_value) << "at element #" << index;
    }
}

} // end namespace test_utils
} // end namespace tftp
} // end namespace zwiibac

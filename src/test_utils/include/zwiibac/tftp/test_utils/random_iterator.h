#pragma once

#include <cstddef>
#include <iterator>
#include <random>

namespace zwiibac {
namespace tftp {
namespace test_utils {

struct RandomIterator
{
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = char;
    using difference_type = size_t;
    using reference = char;
    using pointer = const char*;

    RandomIterator() = default;
    RandomIterator(const RandomIterator& other) : count_(other.count_) {}
    RandomIterator(difference_type pos) : count_(pos) {}

    bool operator==(const RandomIterator& other) const { return count_ == other.count_; }
    bool operator!=(const RandomIterator& other) const { return count_ != other.count_; }

    reference operator*() const { return current_value_; }
    pointer operator->() const { return &current_value_; }

    RandomIterator& operator++() { ++count_; current_value_ = dist_(random_device_);return *this;}
    RandomIterator operator++(int) { RandomIterator tmp(*this); current_value_ = dist_(random_device_);return tmp;}

    RandomIterator& operator+=(difference_type steps) { count_ += steps; return *this; }
    RandomIterator operator+(difference_type steps) 
    {
        RandomIterator tmp(*this); 
        tmp.count_+=steps; 
        current_value_ = dist_(random_device_); 
        return tmp;
    }

private:
    size_t count_ = 0;
    std::random_device random_device_;
    std::uniform_int_distribution<char> dist_{'!', '~'};
    char current_value_ = dist_(random_device_);    
};

} // end namespace test_utils
} // end namespace tftp
} // end namespace zwiibac

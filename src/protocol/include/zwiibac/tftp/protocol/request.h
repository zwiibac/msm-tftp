#pragma once

#include <string_view>
#include <algorithm>
#include <iterator>

#include "opcode.h"

namespace zwiibac {
namespace tftp {


// struct Request 
// {
//     const OpCode op_code;
//     const std::string_view file_name;
//     const std::string_view mode;
// };

template<typename BufferIter>
class RequestTokenizer
{

public:
    class Iterator 
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::string_view;
        using difference_type = size_t;
        using reference = std::string_view;

    private:
        friend class RequestTokenizer;
        constexpr Iterator(const BufferIter& begin, const BufferIter& end) 
            : end_(end)
        {
            next_ = std::find(begin, end_, '\0');
            current_ = next_ != end_ ? begin : end_;
            if (next_ != end_) 
            {
                ++next_;
            }
        }

    public:
        constexpr Iterator(const Iterator& other) = default;

        constexpr reference inline operator*() const { return {current_}; }

        constexpr bool operator==(const Iterator& other) const 
        {
            return current_ == other.current_;   
        }

        constexpr bool operator!=(const Iterator& other) const 
        {
            return current_ != other.current_;
        }

        constexpr Iterator& operator++() 
        {
            current_ = next_;
            next_ = std::find(current_, end_, '\0');

            if (next_ == end_) 
            {
                current_ = end_;
                return *this;                
            }

            ++next_;
            return *this;
        }

    private:
        const BufferIter end_;
        BufferIter next_;
        BufferIter current_;
    };

    using interator = Iterator;

    constexpr RequestTokenizer(const BufferIter& begin, const BufferIter& end) 
        : begin_(begin)
        , end_(end)
    {};

    constexpr interator begin() const
    {
        return Iterator(begin_, end_);
    }

    constexpr interator end() const
    {
        return Iterator(end_, end_);
    }

private:
    const BufferIter begin_;
    const BufferIter end_;
};

} // end namespace tftp
} // end namespace zwiibac
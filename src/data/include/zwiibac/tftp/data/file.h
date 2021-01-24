#pragma once

#include <fstream>

#include <boost/asio.hpp>

namespace zwiibac {
namespace tftp {

struct File 
{
    std::fstream file_stream_;

    inline void Reset() noexcept 
    {
        
        file_stream_.close();
        file_stream_.clear();
    };
};

} // end namespace tftp
} // end namespace zwiibac
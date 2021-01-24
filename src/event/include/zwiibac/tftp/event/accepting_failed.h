#pragma once

#include <cstddef>

#include <boost/asio.hpp>

namespace zwiibac {
namespace tftp {

struct AcceptingFailed 
{
    const boost::system::error_code ec;
    const size_t bytes_received;
};

} // end namespace tftp
} // end namespace zwiibac
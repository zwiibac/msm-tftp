#include <bits/stdint-uintn.h>
#include <random>

#include <zwiibac/tftp/protocol/tid.h>

namespace zwiibac {
namespace tftp {

uint16_t Tid::CreateTid()
{
    static std::random_device random_device;
    static std::uniform_int_distribution<uint16_t> dist{min_tid_, max_tid_};
    return dist(random_device);
}

} // end namespace tftp
} // end namespace zwiibac
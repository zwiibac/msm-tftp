#pragma once

#include <span>
#include <ostream>

#include <cstdint>

#include <string_view>
#include <zwiibac/tftp/protocol/header.h>

namespace zwiibac {
namespace tftp {

namespace detail 
{

class Packet : public std::span<const char> 
{
public:
    template< class It >
    constexpr Packet(It first, size_type count) : span(first, count) 
    {}

    friend inline constexpr std::ostream& operator<<(std::ostream& os, const Packet& packet) 
    {
        const auto header = HeaderView::FromBuffer(packet);
        switch (header.OpCd()) 
        {
        case OpCode::Acknowledgment:
            return os << packet.Action() << " " << header.OpCd() << " <block=" << header.Word() << ">";
        case OpCode::Data:        
            return os << packet.Action() << " " << header.OpCd() << " <block=" << header.Word() << ", "
            << packet.size() - HeaderView::Size() << " bytes>";
        default:        
            return os << "received " << header.OpCd() << " <block=" << header.Word() << ", "
            << packet.size() - HeaderView::Size() << " bytes>";
        }
    }
private:
    virtual constexpr std::string_view Action() const {return "undef action";};
};

class RcvdPacket : public Packet
{
public:
    template< class It >
    constexpr RcvdPacket(It first, size_type count) : Packet(first, count) 
    {}
private:
    virtual constexpr std::string_view Action() const override {return "received";}
};

class SentPacket : public Packet
{
public:
    template< class It >
    constexpr SentPacket(It first, size_type count) : Packet(first, count)
    {}
private:
    virtual constexpr std::string_view Action() const override {return "sent";}
};

} // end namespace detail

} // end namespace tftp
} // end namespace zwiibac
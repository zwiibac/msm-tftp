#pragma once

#include <bits/stdint-uintn.h>
#include <type_traits>
#include <optional>
#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>
#include <string_view>
#include <ostream>
#include <tuple>

#include <boost/asio.hpp>

#include <zwiibac/tftp/protocol/opcode.h>

namespace zwiibac {
namespace tftp {

namespace detail {

class Header 
{    
public:
    Header() = delete;
    uint16_t opcode = 0;
    uint16_t word = 0;

    using OpCodeUType = std::underlying_type<OpCode>::type;
};

class ShortHeader 
{
public:
    ShortHeader() = delete;
    uint16_t opcode = 0;

    using OpCodeUType = std::underlying_type<OpCode>::type;
};

template<class Header>
class ObCodeGetter
{
protected:
    constexpr ObCodeGetter(Header header) : header_(header) {}
public:
    inline constexpr OpCode OpCd() const { return static_cast<OpCode>(ntohs(header_->opcode)); }
private:
    Header header_;
};

template<class Header>
class ObCodeSetter
{
protected:
    constexpr ObCodeSetter(Header header) : header_(header) {}
public:
    inline constexpr void SetOpCd(OpCode opcode) { header_->opcode = htons(static_cast<typename std::remove_pointer_t<Header>::OpCodeUType>(opcode)); }
private:
    Header header_;
};

template<class Header>
class WordGetter
{
protected:
    constexpr WordGetter(Header header) : header_(header) {}
public:
    inline constexpr uint16_t Word() const { return ntohs(header_->word); }
private:
    Header header_;
};

template<class Header>
class WordSetter
{
protected:
    constexpr WordSetter(Header header) : header_(header) {}
public:
    inline constexpr void SetWord(uint16_t word) { header_->word = htons(word); }
private:
    Header header_;
};

template<class Header, template<class> class ... Operations>
class Wrapper : public Operations<Header>...
{
private:
    constexpr Wrapper(Header header) 
        : Operations<Header>(header)... {}
public:
    inline static constexpr size_t Size() noexcept { return sizeof(std::remove_pointer_t<Header>); }
    inline constexpr static Wrapper FromHeader(Header header) { return {header};}
    template<class Buffer>
    inline constexpr  static Wrapper FromBuffer(const Buffer& buffer) { return {reinterpret_cast<Header>(buffer.data())};}
    template<class Buffer>
    inline constexpr static Wrapper FromBuffer(Buffer& buffer) { return {reinterpret_cast<Header>(buffer.data())};}
    template<class Iter>
    inline constexpr static Wrapper FromRange(Iter begin, Iter end) { return {reinterpret_cast<Header>(&(*begin))};}
};

using HeaderView = Wrapper<const Header*, ObCodeGetter, WordGetter>;
using HeaderProxy = Wrapper<Header*, ObCodeGetter, ObCodeSetter, WordGetter, WordSetter>;
using ShortHeaderView = Wrapper<const ShortHeader*, ObCodeGetter>;
using ShortHeaderProxy = Wrapper<ShortHeader*, ObCodeGetter, ObCodeSetter>;

} // end namespace detail

using HeaderView = detail::HeaderView;
using ShortHeaderView = detail::ShortHeaderView;
using HeaderProxy = detail::HeaderProxy;
using ShortHeaderProxy = detail::ShortHeaderProxy;

} // end namespace tftp
} // end namespace zwiibac

#pragma once

#include <bits/stdint-uintn.h>
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
};

template<class Header>
class WrapperBase
{
protected:
    constexpr WrapperBase() :  header_(nullptr) {}
    constexpr WrapperBase(Header header) : header_(header) {}
    Header header_;
};

template<class Base>
struct ObCodeGetter : private virtual Base 
{
    inline constexpr OpCode OpCd() const { return static_cast<OpCode>(ntohs(Base::header_->opcode)); }
};

template<class Base>
struct ObCodeSetter : private virtual Base 
{
    inline constexpr void SetOpCd(OpCode opcode) { Base::header_->opcode = htons(static_cast<Header::OpCodeUType>(opcode)); }
};

template<class Base>
struct WordGetter : private virtual Base
{
    inline constexpr uint16_t Word() const { return ntohs(Base::header_->word); }
};

template<class Base>
struct WordSetter : private virtual Base
{
    inline constexpr void SetWord(uint16_t word) { Base::header_->word = htons(word); }
};

template<class Header, template<class> class ... Operations>
class Wrapper : public Operations<WrapperBase<Header> >...
{
private:
    constexpr Wrapper(Header header) 
        : WrapperBase<Header>(header)
        , Operations<WrapperBase<Header> >()... {}
public:
    inline static constexpr size_t Size() noexcept { return sizeof(typename std::remove_pointer<Header>::type); }
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

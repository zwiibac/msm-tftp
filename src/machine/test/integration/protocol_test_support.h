#pragma once

#include <string>
#include <algorithm>

#include <zwiibac/tftp/protocol/error_code.h>
#include <zwiibac/tftp/protocol/opcode.h>
#include <zwiibac/tftp/protocol/header.h>

template <class Test>
class ProtocolTestSupport : public Test
{
protected:
    using OpCode = zwiibac::tftp::OpCode;
    using ErrorCode = zwiibac::tftp::ErrorCode;
    using HeaderProxy = zwiibac::tftp::HeaderProxy;
    using ShortHeaderProxy = zwiibac::tftp::ShortHeaderProxy;

    template<class Iter>
    size_t SetupRequest(Iter begin, Iter end, OpCode op, std::initializer_list<const std::string> values) 
    {
        Iter current = begin;

        using Header = ShortHeaderProxy;
        auto header = Header::FromRange(current, end);
        header.SetOpCd(op);
        current = current + Header::Size();
        for (const auto& value : values) 
        {
            current = std::copy(value.cbegin(), value.cend(), current);
            std::fill(current, current + 1, '\0');
            ++current;
        }

        return current - begin;
    }

    template<class Iter>
    void SetupError(Iter begin, Iter end, ErrorCode error_code, std::string error) 
    {
        using Header = HeaderProxy;
        auto header = Header::FromRange(begin, end);
        header.SetOpCd(OpCode::Error);
        header.SetWord(error_code);
        begin = begin + Header::Size();
        begin = std::copy(error.begin(), error.end(), begin);
        std::fill(begin, begin + 1, '\0');
    }

    template<class Buffer>
    size_t SetupAck(Buffer& buffer, uint16_t block, OpCode op_code=OpCode::Acknowledgment) 
    {
        return SetupAck(buffer.begin(), buffer.end(), block, op_code);
    }

    template<class Iter>
    size_t SetupAck(Iter begin, Iter end, uint16_t block, OpCode op_code=OpCode::Acknowledgment) 
    {
        using Header = HeaderProxy;
        auto header = Header::FromRange(begin, end);
        header.SetOpCd(op_code);
        header.SetWord(block);

        return Header::Size();
    }

    template<class Iter, class DataIter>
    void SetupData(Iter begin, Iter end, uint16_t block, size_t block_size, DataIter data_begin, DataIter data_end) 
    {
        using Header = HeaderProxy;
        auto header = Header::FromRange(begin, end);
        header.SetOpCd(OpCode::Data);
        header.SetWord(block);
        begin = begin + Header::Size();
        begin = std::copy(data_begin + (block * block_size), data_begin + (block * (block_size + 1)), begin, end);
    }
};


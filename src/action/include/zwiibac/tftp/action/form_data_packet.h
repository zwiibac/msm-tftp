#pragma once

#include <optional>
#include <zwiibac/tftp/protocol/opcode.h>
#include <zwiibac/tftp/protocol/header.h>
#include <zwiibac/tftp/protocol/error_code.h>

namespace zwiibac {
namespace tftp {

struct FormDataPacket 
{
    template <class Evt, class Fsm, class SourceState, class TargetState>
    void operator()(Evt const& ,Fsm& fsm, SourceState&, TargetState& )
    {
        fsm.file_stream_.read(&fsm.send_buffer_[HeaderProxy::Size()], fsm.agreed_block_size_);

        if (fsm.file_stream_.bad()) 
        {
            fsm.last_error_code_ = ErrorCode::DiskFull;
        }
        else 
        {
            fsm.last_block_size_ = fsm.file_stream_.gcount();
            fsm.last_sent_block_ += 1;
            fsm.send_packet_ = {fsm.send_buffer_.data(), HeaderProxy::Size() + fsm.last_block_size_};
            auto header = HeaderProxy::FromBuffer(fsm.send_buffer_);
            header.SetOpCd(OpCode::Data);
            header.SetWord(static_cast<int16_t>(fsm.last_sent_block_));
        }        
    }
};

} // end namespace tftp
} // end namespace zwiibac
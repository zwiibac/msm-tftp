#pragma once

#include <zwiibac/tftp/protocol/opcode.h>
#include <zwiibac/tftp/protocol/header.h>
#include <zwiibac/tftp/protocol/error_code.h>

namespace zwiibac {
namespace tftp {

struct FormProtocolErrorPacket 
{
    template <class Evt, class Fsm, class SourceState, class TargetState>
    void operator()(Evt const& ,Fsm& fsm, const SourceState&, const TargetState& ) const
    {
        auto header = HeaderProxy::FromBuffer(fsm.send_buffer_);
        header.SetOpCd(OpCode::Error);
        header.SetWord(static_cast<int16_t>(ErrorCode::IllegalTftpOperation));
        fsm.send_buffer_[HeaderProxy::Size()] = '\0';
        fsm.send_packet_ = {fsm.send_buffer_.data(), HeaderProxy::Size() + 1};
    }
};

} // end namespace tftp
} // end namespace zwiibac
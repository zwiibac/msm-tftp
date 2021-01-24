#pragma once

#include <optional>
#include <zwiibac/tftp/protocol/opcode.h>
#include <zwiibac/tftp/protocol/header.h>
#include <zwiibac/tftp/protocol/error_code.h>

namespace zwiibac {
namespace tftp {

struct WriteData 
{
    template <class Evt, class Fsm, class SourceState, class TargetState>
    void operator()(Evt const& ,Fsm& fsm, SourceState&, TargetState& )
    {
        fsm.file_stream_.write(
            &fsm.received_packet_[HeaderView::Size()], 
            fsm.received_packet_.size() - HeaderView::Size());

        if (fsm.file_stream_.fail()) 
        {
            fsm.last_error_code_ = ErrorCode::DiskFull;
        }
        else 
        {
            fsm.last_block_size_ = fsm.received_packet_.size() - HeaderView::Size();
            auto header = HeaderView::FromBuffer(fsm.received_packet_);
            fsm.last_saved_block_ = header.Word();
        }        
    }
};

} // end namespace tftp
} // end namespace zwiibac
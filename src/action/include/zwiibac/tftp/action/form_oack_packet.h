#pragma once

#include <chrono>
#include <optional>
#include <cassert>
#include <string_view>
#include <charconv>
#include <cstdlib>
#include <zwiibac/tftp/protocol/opcode.h>
#include <zwiibac/tftp/protocol/header.h>
#include <zwiibac/tftp/protocol/options.h>
#include <zwiibac/tftp/protocol/error_code.h>

namespace zwiibac {
namespace tftp {

class FormOackPacket 
{
public:
    template <class Evt, class Fsm, class SourceState, class TargetState>
    void operator()(Evt const& ,Fsm& fsm, SourceState&, TargetState& )
    {
        assert(fsm.use_block_size_option_ || fsm.use_time_out_option_ || fsm.use_transfer_size_option_);

        auto header = ShortHeaderProxy::FromBuffer(fsm.send_buffer_);
        header.SetOpCd(OpCode::OptionAcknowledgment);

        auto current = fsm.send_buffer_.begin() + ShortHeaderProxy::Size();
        
        if (fsm.use_block_size_option_) 
        {
            current = Copy(BlockSizeDecoder::kBlockSizeOption, current);
            current = Copy(current, fsm.send_buffer_.end(), fsm.agreed_block_size_);
        }
        
        if (fsm.use_time_out_option_) 
        {
            using std::chrono::duration_cast;
            using std::chrono::seconds;

            current = Copy(TimeOutDecoder::kTimeOutOption, current);
            current = Copy(current, fsm.send_buffer_.end(), 
                duration_cast<seconds>(fsm.time_out_).count());
        }

        if (fsm.use_transfer_size_option_) 
        {
            current = Copy(TransferSizeDecoder::kTransferSizeOption, current);
            current = Copy(current, fsm.send_buffer_.end(), fsm.file_size_.value());
        }

        fsm.last_sent_block_ = 0;
        fsm.last_saved_block_ = 0;
        fsm.send_packet_ = {fsm.send_buffer_.data(), (size_t) std::abs(current - fsm.send_buffer_.begin())};
    }

private:
    template<class Iter>
    Iter inline Copy(const std::string_view& value, Iter target) 
    {
        target = std::copy(value.cbegin(), value.cend(), target);
        std::fill(target, target + 1, '\0');
        return ++target;
    }

    template<class Iter, typename TNumber>
    Iter inline Copy(Iter target, Iter end, TNumber value) 
    {
        auto [ptr, ec] = std::to_chars(&(*target), &(*end), value);
        if (ec != std::errc()) 
        {
            // programming error buffer must be large enough for oack response
            std::exit((int)ec);
        }

        target += ptr - &(*target);
        std::fill(target, target + 1, '\0');
        return ++target;
    }
};

} // end namespace tftp
} // end namespace zwiibac
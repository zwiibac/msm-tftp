#pragma once

#include <zwiibac/tftp/logger/logger.h>

namespace zwiibac {
namespace tftp {

struct Reset 
{
    template <class Evt,class Fsm,class SourceState,class TargetState>
    void operator()(Evt const& ,Fsm& fsm,SourceState& ,TargetState& )
    {
        ZWIIB_LOG(trace) << fsm.local_data_endpoint_.port() << "|" <<  fsm.agreed_remote_endpoint_  << "|" << "reset";
        auto was_open = fsm.file_stream_.is_open();
        fsm.file_stream_.clear();
        fsm.file_stream_.close();
        ZWIIB_LOG_IF(error, was_open && fsm.file_stream_.fail()) << fsm.local_data_endpoint_.port() << "|" <<  fsm.agreed_remote_endpoint_  << "|" << "Failed to save file";

        fsm.Reset();
    }
};

} // end namespace tftp
} // end namespace zwiibac
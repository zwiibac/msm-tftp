#pragma once

#include <iostream>
#include <algorithm>
#include <system_error>

#include <boost/asio.hpp>

#include <zwiibac/tftp/logger/logger.h>

#include <zwiibac/tftp/protocol/error_code.h>

namespace zwiibac {
namespace tftp {

struct InitServeReadRequest 
{
    using Endpoint = boost::asio::ip::udp::endpoint;

    template <class Evt,class Fsm,class SourceState,class TargetState>
    void operator()(Evt const& request, Fsm& fsm, SourceState&, TargetState&)
    {
        using boost::asio::ip::address_v4;
        using SystemErrorCode = boost::system::error_code;

        fsm.local_data_endpoint_ = Endpoint(fsm.local_address_, fsm.CreateTid());

        SystemErrorCode open_ec;
        fsm.socket_.open(boost::asio::ip::udp::v4(), open_ec);
        ZWIIB_LOG_IF(error, open_ec) << "Failed to open udp socket" << " | " << open_ec << " " << open_ec.message() << std::endl;
        
        SystemErrorCode bind_ec;
        fsm.socket_.bind(fsm.local_data_endpoint_, bind_ec);
        ZWIIB_LOG_IF(error, bind_ec) << "Failed to bind socket to " << fsm.local_data_endpoint_ << " | " << bind_ec << " " << bind_ec.message() << std::endl;

        if (auto mode = fsm.GetMode(request.mode)) 
        {            
            if (auto file_name = fsm.GetReadFilePath(request.file_name)) 
            {
                if (auto requested_transfer_size = fsm.GetTransferSize(request.transfer_size_))
                {
                    if (auto transfer_size = fsm.GetFileSize(file_name.value())) 
                    {
                        fsm.use_transfer_size_option_ = requested_transfer_size == 0;
                        fsm.file_size_ = transfer_size;
                    }                    
                }

                fsm.file_stream_.open(file_name.value().data());
                if (fsm.file_stream_.bad())
                {
                    // no valid file name provided
                    fsm.last_error_code_ = ErrorCode::FileNotFound;
                }
            }
            else 
            {
                // no valid file name provided
                fsm.last_error_code_ = ErrorCode::FileNotFound;
            }            
        }
        else 
        {
            // no valid mode provided
            fsm.last_error_code_ = ErrorCode::IllegalTftpOperation;
        }

        if (auto time_out = fsm.GetTimeOut(request.time_out_)) 
        {
            fsm.time_out_ = time_out.value();
            fsm.use_time_out_option_ = true;
        }

        if (auto block_size = fsm.GetBlockSize(request.block_size_)) 
        {
            fsm.agreed_block_size_  = block_size.value();
            fsm.use_block_size_option_ = true;            
        }
    }
};

} // end namespace tftp
} // end namespace zwiibac
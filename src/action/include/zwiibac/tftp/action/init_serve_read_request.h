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
            fsm.agreed_block_size_ = 512;
            
            if (auto file_name = fsm.GetReadFilePath(request.file_name)) 
            {
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
        
    }
};

} // end namespace tftp
} // end namespace zwiibac
#pragma once

#include <string>
#include <memory>
#include <vector>

#include <boost/asio/io_context.hpp>

#include <boost/mpl/joint_view.hpp>
#include <boost/asio.hpp>

#include <zwiibac/tftp/logger/logger.h>

#include <zwiibac/tftp/data/buffer.h>
#include <zwiibac/tftp/data/transfer_state.h>
#include <zwiibac/tftp/data/connection.h>
#include <zwiibac/tftp/data/file.h>
#include <zwiibac/tftp/data/file_path.h>
#include <zwiibac/tftp/data/timer.h>
#include <zwiibac/tftp/protocol/tid.h>
#include <zwiibac/tftp/protocol/mode.h>

#include <zwiibac/tftp/machine/session_data.h>
#include <zwiibac/tftp/machine/transition_table.h>
#include <zwiibac/tftp/machine/state_machine.h>

namespace zwiibac {
namespace tftp {

class TftpServer 
{
private:
    using TransitionTable = boost::mpl::joint_view<CommonTransitionTable::type, boost::mpl::joint_view<WriteRequestTransitionTable::type, ReadRequestTransitionTable::type>>;
    using Data = SessionData<SessionDataWithIoContext<Connection, Timer>, File, Buffer, TransferState, FilePath, ModeDecoder, Tid>;
    using Session = StateMachine<Accepting, TransitionTable, Data>;
    using SystemErrorCode = boost::system::error_code;

public:
    using IoContext = Session::IoContext;
    using IpAddress = Session::IpAddress;

    TftpServer(IoContext& io_context, size_t number_of_parallel_requests) 
        : server_(number_of_parallel_requests)
        , listener_socket_(std::make_shared<Connection::Socket>(io_context, boost::asio::ip::udp::v4()))
    {
        for (auto& server : server_) 
        {
            server = std::make_unique<Session>(std::ref(io_context));
        }
    }

    void Start(const std::string& root_path, const std::string& ip, unsigned short port_num) 
    {
        SystemErrorCode ip_parse_ec;
        auto ip_address = boost::asio::ip::address_v4::from_string(ip, ip_parse_ec);
        ZWIIB_LOG_IF(error, ip_parse_ec) << "Failed to parse ip " << ip << " | " << ip_parse_ec << " " << ip_parse_ec.message();

        auto listener_endpoint = Connection::Endpoint(ip_address, port_num);

        SystemErrorCode bind_ec;        
        listener_socket_->bind(listener_endpoint, bind_ec);
        ZWIIB_LOG_IF(error, bind_ec) << "Failed to bind socket to " << listener_endpoint << " | " << bind_ec << " " << bind_ec.message();

        for (auto& session : server_) 
        {
            session->SetServerRootPath(root_path);
            session->local_address_ = ip_address;
            session->listener_socket_ = listener_socket_;
            session->start();
        }
    }

    void Stop() 
    {
        listener_socket_->cancel();
        listener_socket_.reset();
    }

private:
    std::vector<std::unique_ptr<Session> > server_;
    std::shared_ptr<Connection::Socket> listener_socket_;
};

} // end namespace tftp
} // end namespace zwiibac

#include <csignal>
#include <filesystem>

#include <zwiibac/tftp/logger/logger.h>
#include <zwiibac/tftp/server/server.h>

namespace {
    
using zwiibac::tftp::TftpServer;

TftpServer::IoContext io_context;

TftpServer server(io_context, 100);

void handle_stop (int param)
{
  server.Stop();
}

} // end anonymous namespace



int main() 
{
    using std::filesystem::temp_directory_path;

    ZWIIB_LOG_SET_LEVEL();
    
    std::signal(SIGINT, handle_stop);

    server.Start(temp_directory_path() /"zwiibac_tftp_server_test_driver_bb18da59-5513-44cd-9a56-38b89db54b64"/"server_root", "127.0.0.1", 1025);

    io_context.run();

    return 0;
}
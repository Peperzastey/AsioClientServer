#include "acs/conn/SyncTcpClient.hpp"
#include <asio/io_context.hpp>
#include <system_error>
#include <iostream>

using namespace acs;

/// The entry point to the client application.
/**
 * \todo use argv for remoteHost and service
 * \todo terminate connection on SIGINT (or do it somehow on the server)
 */
int main(int argc, char *argv[]) {
    asio::io_context context{}; 

    try {
        conn::SyncTcpClient tcpClient(context, "localhost", "daytime");
        tcpClient.receiveInfinitely(std::cout);
    } catch (const std::system_error &err) {
        std::cerr << "EXCEPTION CAUGHT:\n"
                << err.code() << ": " << err.what()
                << std::endl;
    }

    return 0;
}

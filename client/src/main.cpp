#include "acs/conn/SyncTcpClient.hpp"
#include "acs/util/Logger.hpp"
#include <asio/io_context.hpp>
#include <system_error>
#include <string_view>
#include <iostream>

using namespace acs;

constexpr std::string_view DEFAULT_REMOTE_HOST = "127.0.0.1";
constexpr conn::SyncTcpClient::port_t DEFAULT_REMOTE_PORT = 54321;

/// The entry point to the client application.
/**
 * \todo terminate connection on SIGINT (or do it somehow on the server)
 * \todo USAGE help argument (-h, --help)
 * \todo --port (-p) and --host (-h) options
 */
int main(int argc, char *argv[]) {
    util::Logger logger(std::cout, std::cerr);
    util::Logger::registerInstance(logger);

    asio::io_context context{};

    // ./client port host
    auto remoteHost = DEFAULT_REMOTE_HOST;
    auto remotePort = DEFAULT_REMOTE_PORT;
    if (argc > 1) {
        //TODO use std::from_chars (C++17)
        remotePort = std::atoi(argv[1]);
    }
    if (argc > 2) {
        remoteHost = argv[2];
    }

    try {
        conn::SyncTcpClient tcpClient(context, remoteHost, remotePort);
        tcpClient.receiveInfinitely(util::Logger::instance().log(), util::Logger::instance().logError());
    } catch (const std::system_error &err) {
        util::Logger::instance().logError() << "EXCEPTION CAUGHT:\n"
            << err.code() << ": " << err.what()
            << std::endl;
    }

    return 0;
}

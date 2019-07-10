#include "acs/conn/AsyncTcpClient.hpp"
#include "acs/cmd/AsyncCommandLoop.hpp"
#include "acs/cmd/CommandDispatcher.hpp"
#include "acs/util/Logger.hpp"
#include <asio/io_context.hpp>
#include <system_error>
#include <string_view>
#include <iostream>

using namespace acs;

constexpr std::string_view DEFAULT_REMOTE_HOST = "::1";
constexpr conn::AsyncTcpClient::port_t DEFAULT_REMOTE_PORT = 54321;

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
        // to prevent incorrect input from being treated as 0 so any port
        remotePort = std::atoi(argv[1]);
    }
    if (argc > 2) {
        remoteHost = argv[2];
    }

    try {
        proto::Protocol protocol{};
        conn::AsyncTcpClient tcpClient(context, protocol, remoteHost, remotePort);
        cmd::CommandDispatcher handler(tcpClient);
        cmd::AsyncCommandLoop loop(context, handler);
        //tcpClient.receiveInfinitelyAsync(util::Logger::instance().log(), util::Logger::instance().logError()); // called callback in ctor
        context.run();
    } catch (const std::system_error &err) {
        util::Logger::instance().logError() << "SYSTEM_ERROR CAUGHT:\n"
            << err.code() << ": " << err.what()
            << std::endl;
    //TODO create domain exception hierarchy
    } catch (const std::exception &err) {
        util::Logger::instance().logError() << "EXCEPTION CAUGHT:\n"
            << err.what()
            << std::endl;
    }

    return 0;
}

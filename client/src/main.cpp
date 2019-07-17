#include "acs/message/MessageRegistry.hpp"
#include "acs/message/MessageRegisterer.hpp"
#include "acs/message/EchoMessage.hpp"
#include "acs/context/ClientContext.hpp"
#include "acs/proto/PolymorphicMsgProtocol.hpp"
#include "acs/conn/AsyncTcpClient.hpp"
#include "acs/cmd/AsyncCommandLoop.hpp"
#include "acs/cmd/CommandDispatcher.hpp"
#include "acs/util/Logger.hpp"
#include <asio/io_context.hpp>
#include <system_error>
#include <string_view>
#include <iostream>
#include "framing.pb.h"

using namespace acs;
namespace ctx = ::acs::context;

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
        message::MessageRegistry msgRegistry{};
        proto::PolymorphicMsgProtocol protocol(msgRegistry);

        conn::AsyncTcpClient tcpClient(context, protocol, remoteHost, remotePort);
        cmd::CommandDispatcher handler(tcpClient);
        cmd::AsyncCommandLoop loop(context, handler);

        ctx::ClientContext clientContext{loop.getWriter()};

        message::MessageRegisterer msgRegisterer(msgRegistry);
        //TODO encapsulate message types in separate domain class/struct
        msgRegisterer.registerMessageType<message::EchoMessage<ctx::ClientContext>>(proto::FramePrefix::ECHO, clientContext);

        //tcpClient.receiveInfinitelyAsync(); // as callback in AsyncTcpClient ctor
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

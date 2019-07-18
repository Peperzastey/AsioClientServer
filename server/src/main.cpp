#include "acs/message/MessageRegistry.hpp"
#include "acs/message/MessageRegisterer.hpp"
#include "acs/message/EchoMessage.hpp"
#include "acs/proto/PolymorphicMsgProtocol.hpp"
#include "acs/conn/AsyncTcpServer.hpp"
#include "acs/util/Logger.hpp"
#include <asio/io_context.hpp>
#include <system_error>
#include <iostream>
#include <cstdlib>

using namespace acs;

constexpr unsigned short DEFAULT_PORT = 54321;

/// The entry point to the server application.
int main(int argc, char *argv[]) {
    util::Logger logger(std::cout, std::cerr);
    util::Logger::registerInstance(logger);

    asio::io_context context{};

    auto portNum = DEFAULT_PORT;
    if (argc > 1) {
        //TODO use std::from_chars (C++17)
        // to prevent incorrect input from being treated as 0 so any port
        portNum = std::atoi(argv[1]);
    }

    try {
        message::MessageRegistry msgRegistry{};
        proto::PolymorphicMsgProtocol protocol(msgRegistry);
        message::MessageRegisterer msgRegisterer(msgRegistry);
        //TODO encapsulate message types in separate domain class/struct
        msgRegisterer.registerMessageType<message::EchoMessage>(proto::FramePrefix::ECHO);

        conn::AsyncTcpServer tcpServer(context, protocol, portNum);

        util::Logger::instance().log() << "Server listening on port " << portNum << " on any interface."
            << std::endl;

        context.run();
    } catch (const std::system_error &err) {
        util::Logger::instance().logError() << "EXCEPTION CAUGHT:\n"
            << err.code() << ": " << err.what()
            << std::endl;
    }

    return 0;
}

#include "acs/echo/EchoClientHandler.hpp"
#include "acs/conn/TcpConnection.hpp"
#include "acs/util/Logger.hpp"
#include "framing.pb.h"
#include "echo.pb.h"
#include <utility>
#include <cassert>

namespace acs::echo {

void EchoClientHandler::handleStart() {
    using namespace std::string_literals;

    proto::EchoPacket message{};
    message.set_id(1);
    message.set_text(
        "Welcome to ACS chat!\n"
        "Just type your message."s
    );
    //message.set_type(proto::EchoPacket::TEST); //TODO SYSTEM / WELCOME
    //TEMP
    message.set_type(proto::EchoPacket::ECHO_RESPONSE);

    _connection->send(message, proto::FramePrefix::ECHO);

    //TODO _connection->receiveInfinitely(); ?
}

void EchoClientHandler::handleSendComplete() {
    //close();
    // _connection->close();
}

void EchoClientHandler::handleServerClose() {
    util::Logger::instance().log()
        << "Connection " << _connection->getId() << " close performed by server" << std::endl;
}

} // namespace acs::echo
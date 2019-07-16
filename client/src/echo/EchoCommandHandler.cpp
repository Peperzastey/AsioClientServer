#include "acs/echo/EchoCommandHandler.hpp"
#include "acs/message/EchoMessage.hpp"
#include "acs/conn/AsyncTcpClient.hpp"
#include "acs/util/Logger.hpp"
#include "echo.pb.h"
#include "framing.pb.h"
#include <utility>

namespace acs::echo {

//TODO string parameter instead of string_view -> can then use echoPacket.set_text(std::move(commandString))
void EchoCommandHandler::handleCommand(std::string_view commandString, CommandLoop &commandLoop, Client &client) {
    static std::size_t echoCommandId = 0;
    util::Logger::instance().log()
        << "DBG: handling echo command..." << std::endl;
    proto::EchoPacket echoPacket{};
    echoPacket.set_id(echoCommandId);
    echoPacket.set_text(commandString.data(), commandString.size());
    echoPacket.set_type(proto::EchoPacket::ECHO_REQUEST);

    //TEMP solution
    //message::EchoMessage message(proto::FramePrefix::ECHO, std::move(echoPacket));
    auto msg = std::make_unique<message::EchoMessage>(proto::FramePrefix::ECHO, std::move(echoPacket));

    client.send(*msg.get());

    ++echoCommandId;
}

void EchoCommandHandler::handleResult(std::string_view resultString, CommandLoop &commandLoop) {
    //TODO
}

} // namespace acs::echo

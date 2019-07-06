#include "acs/echo/EchoCommandHandler.hpp"
#include "acs/conn/AsyncTcpClient.hpp"
#include "acs/util/Logger.hpp"
#include "chat.pb.h"

namespace acs::echo {

//TODO string parameter instead of string_view -> can then use echoMessage.set_text(std::move(commandString))
void EchoCommandHandler::handleCommand(std::string_view commandString, CommandLoop &commandLoop, Client &client) {
    static std::size_t echoCommandId = 0;
    util::Logger::instance().log()
        << "DBG: handling echo command..." << std::endl;
    proto::ChatMessage echoMessage{};
    echoMessage.set_id(echoCommandId);
    echoMessage.set_text(commandString.data(), commandString.size());
    echoMessage.set_type(proto::ChatMessage::ECHO_REQUEST);

    client.send(echoMessage);

    ++echoCommandId;
}

void EchoCommandHandler::handleResult(std::string_view resultString, CommandLoop &commandLoop) {
    //TODO
}

} // namespace acs::echo

#include "acs/echo/EchoCommandHandler.hpp"
#include "acs/util/Logger.hpp"

namespace acs::echo {

void EchoCommandHandler::handleCommand(std::string_view commandString, CommandLoop &commandLoop, Client &client) {
    util::Logger::instance().log()
        << "DBG: handling echo command..." << std::endl;
    //TODO
}

void EchoCommandHandler::handleResult(std::string_view resultString, CommandLoop &commandLoop) {
    //TODO
}

} // namespace acs::echo

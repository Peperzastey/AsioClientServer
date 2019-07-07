#ifndef ACS_ECHO_COMMAND_HANDLER_HPP__
#define ACS_ECHO_COMMAND_HANDLER_HPP__

#include "acs/cmd/CommandHandler.hpp"

namespace acs::echo {

class EchoCommandHandler : public cmd::CommandHandler {
public:
    using CommandHandler::CommandHandler;

    void handleCommand(std::string_view commandString, CommandLoop &commandLoop, Client &client) override;
    void handleResult(std::string_view resultString, CommandLoop &commandLoop) override;
};

} // namespace acs::echo

#endif // ACS_ECHO_COMMAND_HANDLER_HPP__

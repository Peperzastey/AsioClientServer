#ifndef ACS_ECHO_CLIENT_HANDLER_HPP__
#define ACS_ECHO_CLIENT_HANDLER_HPP__

#include "acs/logic/ClientHandler.hpp"

namespace acs::echo {

/// Echo logic.
class EchoClientHandler : public logic::ClientHandler {
public:
    using logic::ClientHandler::ClientHandler;

    void handleStart() override;
    void handleSendComplete() override;
    void handleServerClose() override;
};

} // namespace acs::echo

#endif // ACS_ECHO_CLIENT_HANDLER_HPP__
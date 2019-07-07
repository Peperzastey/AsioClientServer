#ifndef ACS_TEST_CLIENT_HANDLER_HPP__
#define ACS_TEST_CLIENT_HANDLER_HPP__

#include "acs/logic/ClientHandler.hpp"

namespace acs::debug {

/// Test (debug) ClientHandler.
class TestClientHandler : public logic::ClientHandler {
public:
    using logic::ClientHandler::ClientHandler;

    void handleStart() override;
    void handleSendComplete() override;
    void handleServerClose() override;
};

} // namespace acs::debug

#endif // ACS_TEST_CLIENT_HANDLER_HPP__
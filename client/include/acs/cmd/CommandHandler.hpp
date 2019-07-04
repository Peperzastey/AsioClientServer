#ifndef ACS_COMMAND_HANDLER_HPP__
#define ACS_COMMAND_HANDLER_HPP__

#include <string_view>

namespace acs::conn {
class AsyncTcpClient;
} // namespace acs::conn

namespace acs::cmd {

class AsyncCommandLoop;
//using CommandLoop = AsyncCommandLoop;
// visible outside or only in this translation unit?

class CommandHandler {
public:
    //TODO add abstract base classes
    using CommandLoop = AsyncCommandLoop;
    using Client = conn::AsyncTcpClient;

public:
    // client as argument for handle methods (Dispatcher already has reference to the client)
    virtual void handleCommand(std::string_view commandString, CommandLoop &commandLoop, Client &client) = 0;
    virtual void handleResult(std::string_view resultString, CommandLoop &commandLoop) = 0;

    virtual ~CommandHandler() {}
};

} // namespace acs::cmd

#endif // ACS_COMMAND_HANDLER_HPP__

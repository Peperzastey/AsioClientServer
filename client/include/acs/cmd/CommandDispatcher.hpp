#ifndef ACS_COMMAND_DISPATCHER_HPP__
#define ACS_COMMAND_DISPATCHER_HPP__

#include <string_view>

namespace acs::conn {
class AsyncTcpClient;
} // namespace acs::conn

namespace acs::cmd {

class AsyncCommandLoop;
class CommandHandler;

/// Dispatches commands to relevant CommandHandler-s.
/**
 * \todo lazy-init CommandHandlers when needed
 * \todo use composite pattern? (CommandDispatcher as a 
 *  subclass of abstract CommandHandler). Then CommandLoop can
 *  use a concrete command handler or this command dispatcher
 *  interchangeably.
 */
class CommandDispatcher {
public:
    using CommandLoop = AsyncCommandLoop;
    using Client = conn::AsyncTcpClient;

public:
    /// Constructor.
    CommandDispatcher(Client &client);

    /*virtual*/ void dispatchCommand(std::string_view commandString, CommandLoop &commandLoop);
    /*virtual*/ void dispatchResult(std::string_view resultString, CommandLoop &commandLoop);

private:
    //TODO Register/Registry (static?) class for new handlers registration
    // (Open-closed principle)
    // or: subclassing of this class to add new handlers ?
    /// Returns lazy-init-ed no-op (default) handler.
    static CommandHandler& _getDefaultHandler();
    /// Returns echo command handler.
    static CommandHandler& _getEchoHandler();

private:
    // Commands config
    static constexpr std::string_view _ECHO_CMD_PREFIX = ">echo";

private:
    Client &_client;
};

} // namespace acs::cmd

#endif // ACS_COMMAND_DISPATCHER_HPP__

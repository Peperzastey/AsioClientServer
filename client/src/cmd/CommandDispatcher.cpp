#include "acs/cmd/CommandDispatcher.hpp"
#include "acs/echo/EchoCommandHandler.hpp"
#include "acs/util/Logger.hpp"

//TODO move to common and test all members of this namespace
namespace acs::misc {

// uses non-noexcept functions, so not noexcept
constexpr void trimWhitespace(std::string_view &sv, bool right = true, bool left = true) {
    if (left)
        sv.remove_prefix(std::min(sv.find_first_not_of(" \t"), sv.size()));
    if (right) {
        auto size = sv.size();
        sv.remove_suffix(std::min(
            (size - sv.find_last_not_of(" \t") - 1),
            size
        ));
    }
}

// std::string's are not constexpr, so no constexpr
/**
 * \param s string/string_view to check the prefix of
 * \param prefix prefix
 *   If this argument is of type const char* then it must
 *   be terminated with a null character. 
 */
template <typename StringType, typename SubstringType>
inline bool startsWith(StringType &&s, SubstringType &&prefix) {
    return (s.rfind(prefix, 0) == 0);
}

} // namespace acs::misc

namespace acs::cmd {

/*CommandHandler& CommandDispatcher::_getDefaultHandler() {
    //TODO
}*/

/**
 * \warning Not thread-safe.
 */
CommandHandler& CommandDispatcher::_getEchoHandler() {
    static echo::EchoCommandHandler handler{};
    return handler;
}

CommandDispatcher::CommandDispatcher(Client &client)
    : _client(client) {}

void CommandDispatcher::dispatchCommand(std::string_view commandString, CommandLoop &commandLoop) {
    misc::trimWhitespace(commandString);

    // parsing (rough-and-ready)
    //TODO parse "q", "quit"
    if (misc::startsWith(commandString, _ECHO_CMD_PREFIX)) {
        commandString.remove_prefix(_ECHO_CMD_PREFIX.size());
        //TODO must be at least 1 whitespace after the cmd prefix
        misc::trimWhitespace(commandString, false);
        _getEchoHandler().handleCommand(commandString, commandLoop, _client);
    } else {
        //TODO?
        //throw invalid_argument / domain_error
        //throw UnsupportedCommandException{};
        util::Logger::instance().logError("ERR: Unsupported command.");
        return;
    }
}

void CommandDispatcher::dispatchResult(std::string_view resultString, CommandLoop &commandLoop) {

}

} // namespace acs::cmd
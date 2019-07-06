#include "acs/cmd/AsyncCommandLoop.hpp"
#include "acs/cmd/CommandDispatcher.hpp"
//TODO (work only on POSIX systems)
//#if defined(BOOST_ASIO_HAS_POSIX_STREAM_DESCRIPTOR)
#include <unistd.h>
#include <functional>

namespace acs::cmd {

//TODO? use ::dup(STDIN/OUT_FILENO) ?
//TODO add _writer and use it instead of Logger for command result output
AsyncCommandLoop::AsyncCommandLoop(asio::io_context &ioContext, CommandDispatcher &handler)
    : _handler(&handler), _input(ioContext, STDIN_FILENO), _output(ioContext, STDOUT_FILENO), _reader(_input) {
    run();
}

//TODO use AsyncReader class
void AsyncCommandLoop::run() {
    _reader.readAsyncUntilDelimInfOccupy([this](std::string line) {
        //HEED beware dangling reference string_view!
        _handler->dispatchCommand(line, *this);
    });
    //TODO if "q"/"quit" -> close client app
}
/*std::string input;
while (std::getline(std::cin, input)) { // returns std::istream&
    //TODO regex
    //TODO compare ignore case
    if (input == "q"s || input == "quit"s) {
        std::cout << "Quitting. Bye.\n";
        break;
    }

    //TODO if only white spaces -> do not print
    std::cout << "ECHO: " << input << std::endl;
}
return 0;*/

} // namespace acs::cmd

//#else // defined(BOOST_ASIO_HAS_POSIX_STREAM_DESCRIPTOR)
//AsyncCommandLoop::AsyncCommandLoop(Client &clientConnection) {
//    util::Logger()::instance().logError()
//        << "This implementation works only on POSIX-compliant systems\n";
//}
//#endif // defined(BOOST_ASIO_HAS_POSIX_STREAM_DESCRIPTOR)

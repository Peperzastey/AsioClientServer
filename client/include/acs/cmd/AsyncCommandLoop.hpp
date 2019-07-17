#ifndef ACS_ASYNC_COMMAND_LOOP_HPP__
#define ACS_ASYNC_COMMAND_LOOP_HPP__

#include <asio/posix/stream_descriptor.hpp>
#include "acs/conn/AsyncReader.hpp"
#include "acs/conn/AsyncWriter.hpp"

namespace asio {
class io_context;
} // namespace asio

namespace acs::cmd {

class CommandDispatcher;

class AsyncCommandLoop {
public:
    //using Client = conn::AsyncTcpClient;
    using Stream = asio::posix::stream_descriptor;
    using Reader = conn::AsyncReader<Stream>;
    using Writer = conn::AsyncWriter<Stream>;

public:
    /// Create command loop and schedule it for asynchronous execution.
    AsyncCommandLoop(asio::io_context &ioContext, CommandDispatcher &handler/*, Client &clientConnection*/);

    /// The loop's logic.
    /**
     * Called by the constructor.
     */
    /*virtual*/ void run();

    //TODO?
    //void close();

    /// Set new command handler.
    void setHandler(CommandDispatcher &handler) noexcept { // inline
        _handler = &handler;
    }

    Writer& getWriter() noexcept { // inline
        return _writer;
    }

private:
    //Client &_client;
    CommandDispatcher *_handler; // unique_ptr ?
    Stream _input;
    Stream _output;
    Reader _reader;
    Writer _writer;
};

} // namespace acs::cmd

#endif // ACS_ASYNC_COMMAND_LOOP_HPP__

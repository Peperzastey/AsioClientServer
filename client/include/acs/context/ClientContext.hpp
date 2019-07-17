#ifndef ACS_CLIENT_CONTEXT_HPP__
#define ACS_CLIENT_CONTEXT_HPP__

#include "acs/cmd/AsyncCommandLoop.hpp"

namespace acs::conn {
class AsyncTcpClient;

template <typename Stream>
class AsyncWriter;
} // namespace acs::conn

namespace acs::context {

//TODO class with getters (const methods) ?
struct ClientContext {
    //conn::AsyncTcpClient &client;
    conn::AsyncWriter<cmd::AsyncCommandLoop::Stream> &stdoutWriter;
    //...
};

} // namespace acs::context

#endif // ACS_CLIENT_CONTEXT_HPP__

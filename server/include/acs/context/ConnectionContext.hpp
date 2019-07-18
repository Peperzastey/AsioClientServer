#ifndef ACS_SERVER_CONNECTION_CONTEXT_HPP__
#define ACS_SERVER_CONNECTION_CONTEXT_HPP__

namespace acs::conn {
class TcpConnection;
} // namespace acs::conn

namespace {
using Connection = acs::conn::TcpConnection;
} // unnamed namespace

namespace acs::context {

struct ConnectionContext {
    // ctor needed for emplace functions
    ConnectionContext(Connection &conn)
        : connection(conn) {}

    Connection &connection;
};

} // namespace acs::context

#endif // ACS_SERVER_CONNECTION_CONTEXT_HPP__

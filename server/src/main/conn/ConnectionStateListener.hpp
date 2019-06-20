#ifndef CONNECTION_STATE_LISTENER_HPP__
#define CONNECTION_STATE_LISTENER_HPP__

namespace cs::server::conn {

namespace tcp {
class TcpConnection;
} // namespace tcp

using Connection = tcp::TcpConnection;

/// Interface for observing Connection state changes.
class ConnectionStateListener {
public:
    virtual void connectionClosed(Connection&) {} // do nothing

    virtual ~ConnectionStateListener() /*noexcept?*/ = 0;
};

inline ConnectionStateListener::~ConnectionStateListener() = default;

} // namespace cs::server::conn

#endif // CONNECTION_STATE_LISTENER_HPP__
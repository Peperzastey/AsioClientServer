#ifndef ACS_CONNECTION_STATE_LISTENER_HPP__
#define ACS_CONNECTION_STATE_LISTENER_HPP__

namespace acs::conn {

class TcpConnection;
using Connection = TcpConnection;

/// Interface for observing Connection state changes.
class ConnectionStateListener {
public:
    virtual void connectionClosed(Connection&) {} // do nothing

    virtual ~ConnectionStateListener() /*noexcept?*/ = 0;
};

inline ConnectionStateListener::~ConnectionStateListener() = default;

} // namespace acs::conn

#endif // ACS_CONNECTION_STATE_LISTENER_HPP__
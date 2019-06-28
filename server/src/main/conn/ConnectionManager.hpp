#ifndef CONNECTION_MANAGER_HPP__
#define CONNECTION_MANAGER_HPP__

#include "ConnectionStateListener.hpp"
#include "util/SimpleLogger.hpp"
#include <list>
#include <ostream>

namespace asio {
class io_context;
}

namespace cs::server::conn {

using namespace cs::common;

/// Manages and holds owership of the connections.
/**
 * \todo Class template or depending on Connection (abstract) base class
 */
template <typename Connection>
class ConnectionManager final : public ConnectionStateListener {
public:
    /// Create and add new connection.
    /**
     * \return new created connection
     * \todo newConnection(Connection&&) overload/method ?
     */
    template <typename... Args>
    Connection& template_newConnection(Args&&... args) {
        return _connections.emplace_back(std::forward<Args>(args)...);
    }

    Connection& newConnection(asio::io_context &ioContext) {
        return _connections.emplace_back(ioContext, *this);
    }

    /// Close \a connection.
    /**
     * Postcondition: Passed reference is invalidated and should not be accessed.
     */
    void closeConnection(Connection &conn) {
        conn.close();
        //_purgeConnection(conn); -- called by connectionClosed event handler
    }

    /**
     * \param out \a ostream to print to
     * \todo Move somewhere else? (a helper/util class)
     */
    void printOpenConnections(std::ostream &out) const {
        out << "Open connections:\n";
        for (const auto& conn : _connections)
            out << "  " << conn << '\n';
    }

protected:
    void connectionClosed(Connection &conn) override {
        _purgeConnection(conn);
        util::log() << "Connection closed: " << conn << '\n';
    }

private:
    /// Destroy the Connection object.
    /**
     * Postcondition: Passed reference is invalidated and should not be accessed.
     * \param conn Connection object to destroy (purge).
     */
    void _purgeConnection(Connection &conn) {
        // collections are unique (in regard to operator==)
        _connections.remove(conn);
    }

private:
    /// Collection of open connections.
    /**
     * \note Important for this implementation:
     * Additions, removing and moving the elements within a std::list does not
     * invalidate the iterators or references.
     * An iterator is invalidated only when the corresponding element is deleted.
     */
    std::list<Connection> _connections;
};

} // namespace cs::server::conn

#endif // CONNECTION_MANAGER_HPP__
/** \file 
 *  \brief Definition of the \a AsyncTcpServer class.
 */
#ifndef ASYNCTCPSERVER_HPP__
#define ASYNCTCPSERVER_HPP__

#include "conn/ConnectionManager.hpp"
#include "TcpConnection.hpp"
#include <asio/ip/tcp.hpp>
#include <list>

namespace cs::core {
// forward declaration
class Application;
} // namespace cs::core

/// Contains server-side TCP connection functionality.
namespace cs::server::conn::tcp {

using namespace core;

/// Asynchronous TCP Server.
class AsyncTcpServer final {
public:
    /// Port number typedef.
    using port_t = std::uint16_t;
    /// IP protocol version typedef.
    //using ip_version_t = asio::ip::tcp::address_v4;
    static constexpr auto ipVersion = &asio::ip::tcp::v4;

    /// Constructor
    explicit AsyncTcpServer(Application &app, port_t portNumber, [[maybe_unused]] const std::string &service);

protected:
    void handleAccept(TcpConnection &connection, const std::error_code &error);

private:
    void startAccept();

private:
    /// Reference to the Application object.
    Application &_app;
    /// Server-side acceptor socket endpoint.
    asio::ip::tcp::acceptor _acceptor;
    ConnectionManager<TcpConnection> _connManager;
};

} // namespace cs::server::conn::tcp

#endif // ASYNCTCPSERVER_HPP__
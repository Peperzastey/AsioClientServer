/** \file 
 *  \brief Definition of the \a AsyncTcpServer class.
 */
#ifndef ACS_ASYNCTCPSERVER_HPP__
#define ACS_ASYNCTCPSERVER_HPP__

#include "acs/conn/ConnectionManager.hpp"
#include "acs/conn/TcpConnection.hpp"
#include <asio/ip/tcp.hpp>
#include <list>

namespace asio {
class io_context;
} // namespace asio

namespace acs::proto {
class PolymorphicMsgProtocol;
} // namespace acs::prot

/// Contains connection functionality.
namespace acs::conn {

/// Asynchronous TCP Server.
class AsyncTcpServer final {
public:
    using Protocol = proto::PolymorphicMsgProtocol;
    // Connection acceptor typedef.
    using Acceptor = asio::ip::tcp::acceptor;
    /// Port number typedef.
    using port_t = std::uint16_t;
    /// IP protocol version typedef.
    //using ip_version_t = asio::ip::tcp::address_v6;
    static constexpr auto ipVersion = &asio::ip::tcp::v6;

    /// Acceptor's backlog size.
    static constexpr int BACKLOG_SIZE = 32;

public:
    /// Constructor
    explicit AsyncTcpServer(asio::io_context &ioContext, Protocol &protocol, port_t portNumber, bool reuseAddress = true);

protected:
    void handleAccept(TcpConnection &connection, const std::error_code &error);

private:
    void startAccept();

private:
    /// Server-side acceptor socket endpoint.
    Acceptor _acceptor;
    Protocol *_protocol;
    ConnectionManager<TcpConnection> _connManager;
};

} // namespace acs::conn

#endif // ACS_ASYNCTCPSERVER_HPP__
#include "acs/conn/AsyncTcpServer.hpp"

#include "acs/conn/TcpConnection.hpp"
#include "acs/debug/TestClientHandler.hpp"
#include <asio/ip/v6_only.hpp>

namespace acs::conn {

AsyncTcpServer::AsyncTcpServer(asio::io_context &ioContext, port_t portNumber, bool reuseAddress)
    : _acceptor(ioContext) {
    asio::ip::tcp::endpoint endpoint(ipVersion(), portNumber);
    _acceptor.open(endpoint.protocol());
    if (reuseAddress)
        _acceptor.set_option(asio::socket_base::reuse_address(true));
    _acceptor.set_option(asio::ip::v6_only(false)); // listen on both IPv6 and IPv4 (Note: not supported on Windows XP)
    _acceptor.bind(endpoint);
    _acceptor.listen(BACKLOG_SIZE);

    startAccept();
}

/**
 * \warning Not Thread-Safe !
 */
void AsyncTcpServer::startAccept() {
    auto& connection = _connManager.newConnection<debug::TestClientHandler>(_acceptor.get_executor().context());

    _acceptor.async_accept(connection.getSocket(), [this, &connection](auto&&... params) {
        handleAccept(connection, std::forward<decltype(params)>(params)...);
    });
}

void AsyncTcpServer::handleAccept(TcpConnection& connection, const std::error_code &error) {
    if (!error)
        connection.start();

    _connManager.printOpenConnections(util::Logger::instance().log());

    startAccept();
}

} // namespace acs::conn
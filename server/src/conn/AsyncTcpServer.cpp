#include "acs/conn/AsyncTcpServer.hpp"

#include "acs/conn/TcpConnection.hpp"
#include "acs/util/SimpleLogger.hpp"

namespace acs::conn {

AsyncTcpServer::AsyncTcpServer(asio::io_context &ioContext, port_t portNumber, const std::string &service)
    : _acceptor(ioContext, asio::ip::tcp::endpoint(ipVersion(), portNumber)) {
    startAccept();
}

/**
 * \warning Not Thread-Safe !
 */
void AsyncTcpServer::startAccept() {
    auto& connection = _connManager.newConnection(_acceptor.get_executor().context());

    _acceptor.async_accept(connection.getSocket(), [this, &connection](auto&&... params) {
        handleAccept(connection, std::forward<decltype(params)>(params)...);
    });
}

void AsyncTcpServer::handleAccept(TcpConnection& connection, const std::error_code &error) {
    if (!error)
        connection.start();

    _connManager.printOpenConnections(util::log());

    startAccept();
}

} // namespace acs::conn
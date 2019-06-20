#include "AsyncTcpServer.hpp"

#include "core/Application.hpp"
#include "TcpConnection.hpp"

namespace cs::server::conn::tcp {

AsyncTcpServer::AsyncTcpServer(Application &app, port_t portNumber, [[maybe_unused]] const std::string &service)
    : _app(app), _acceptor(_app.getContext(), asio::ip::tcp::endpoint(ipVersion(), portNumber)) {
    startAccept();
}

/**
 * \warning Not Thread-Safe !
 * \todo &conn = std::move(connection) ?
 */
void AsyncTcpServer::startAccept() {
    auto& connection = _connManager.newConnection(_acceptor.get_executor().context());

    _acceptor.async_accept(connection.getSocket(), [this, &connection/*, conn = std::move(connection)*/](auto&&... params) /*mutable*/ {
        handleAccept(connection, std::forward<decltype(params)>(params)...);
    });
    //TODO try &conn = std::move(connection) ? -- in a test
}

void AsyncTcpServer::handleAccept(TcpConnection& connection, const std::error_code &error) {
    if (!error)
        connection.start();

    startAccept();
}

} // namespace cs::server::conn::tcp
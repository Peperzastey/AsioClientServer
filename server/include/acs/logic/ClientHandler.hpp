#ifndef ACS_CLIENT_HANDLER_HPP__
#define ACS_CLIENT_HANDLER_HPP__

namespace acs::conn {
class TcpConnection;
} // namespace acs::conn

/**
 * \todo change namespace name: acs::handler ?
 */
namespace acs::logic {

/// Base class for concrete ClientHandlers.
/**
 * \todo Classes: ChatHandler, GameHandler, ...
 */
class ClientHandler {
public:
    explicit ClientHandler(conn::TcpConnection &connection)
        : _connection(&connection) {}
    ClientHandler(const ClientHandler&) = delete;
    // move ctor ?
    virtual ~ClientHandler() = 0;

    virtual void handleStart(/*client*/) {}
    virtual void handleServerClose(/*reason*/) {}
    virtual void handleClientClose() {}

    // needed?
    virtual void handleSendComplete(/*error*/) {}
    //virtual void handleSendFailure(error, message) {}

    void close() {
        //TODO _connection->close();
    }

protected:
    //TODO Client _client; ?
    conn::TcpConnection *_connection;
};

inline ClientHandler::~ClientHandler() = default;

} // namespace acs::logic

#endif // ACS_CLIENT_HANDLER_HPP__
/** \file
 *  \brief Definition of the \a TcpConnection class.
 */
#ifndef ACS_TCPCONNECTION_HPP__
#define ACS_TCPCONNECTION_HPP__

#include "acs/conn/ConnectionStateListener.hpp"
#include "acs/logic/ClientHandler.hpp"
#include "acs/util/Logger.hpp"
#include "acs/util/Identity.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <system_error>
#include <ostream>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/write.hpp>
#include "chat.pb.h"
#include <cassert>

/// Contains server-side TCP connection functionality.
namespace acs::conn {

/// Represents server-side part of a TCP connection.
class TcpConnection {
public:
    /// Constructor.
    /**
     * \param ioContext IO Context of the Application object
     * \param responseFactory object of type response_factory_func_t used to create response to a request provided
     */
    template <typename ClientHandlerT>
    TcpConnection(asio::io_context &ioContext, ConnectionStateListener &observer, util::Identity<ClientHandlerT> handlerType)
        : _socket(ioContext), _id(generateConnId()), _handler(std::make_unique<typename decltype(handlerType)::type>(*this)), _observer(observer) {}
    // implement by hand - bool _writeInProgress, size_t _id, ... need to be copied (built-in type)
    // thus move ctor is temporarily disabled
    //TcpConnection(TcpConnection&&) = default;
    /// TcpConnection is not copy-constructible.
    TcpConnection(const TcpConnection&) = delete;
    /// TcpConnection is not copy-assignable.
    TcpConnection& operator=(const TcpConnection&) = delete;

    /// Start handling the connection.
    void start();
    /// Close the connection.
    void close();
    /// Send the \a message to the connected client.
    void send(const std::string &message /*, TODO WritePolicy policy = QueueWrite*/);
    void send(std::string &&message /*, WritePolicy policy = QueueWrite*/);

    /// Get server-side endpoint socket of this TcpConnection.
    asio::ip::tcp::socket& getSocket() noexcept {
        return _socket;
    }
    /// Get connection id.
    std::size_t getId() const noexcept {
        return _id;
    }

public:
    /// TcpConnection object can be compared for equality with other object of this class.
    /**
     * \note This is used for removing stored connection objects on the server.
     */
    bool operator==(const TcpConnection &other) const noexcept {
        return this->_id == other._id;
    }

protected:
    void handleWrite(const std::error_code &error, std::size_t bytesSend);

private:
    static std::size_t generateConnId();
    void doSend(/*WritePolicy*/);

private:
    /// Socket representing the server-side connection endpoint.
    asio::ip::tcp::socket _socket;
    /// Identifies the connection.
    std::size_t _id;
    /// Client-server interaction handler.
    std::unique_ptr<logic::ClientHandler> _handler;
    /// Message to send to the connected peer.
    /**
     * Holds the message until it is (asynchronously) sent.
     */
    mutable std::string _sendMsg;
    /// Indicates whether there is a compound write operation (see: Asio) currently in progress.
    /**
     * No other write operation on the same stream (socket) can interleave with any other compound operation.
     */
    mutable /*atomic*/ bool _writeInProgress = false;

protected:
    /// State changes observer.
    ConnectionStateListener &_observer;
};

inline std::ostream& operator<<(std::ostream &out, const TcpConnection &connection) {
    return out << "Connection(id= " << connection.getId() << " )";
}

} // namespace acs::conn

#endif // ACS_TCPCONNECTION_HPP__
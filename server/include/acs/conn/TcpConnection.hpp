/** \file
 *  \brief Definition of the \a TcpConnection class.
 */
#ifndef ACS_TCPCONNECTION_HPP__
#define ACS_TCPCONNECTION_HPP__

#include "acs/conn/ConnectionStateListener.hpp"
#include "acs/proto/Protocol.hpp"
#include "acs/logic/ClientHandler.hpp"
#include "acs/conn/AsyncReader.hpp"
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
        : _socket(ioContext), _id(_generateConnId()), _reader(_socket),
          _handler(std::make_unique<typename decltype(handlerType)::type>(*this)), _observer(observer) {}
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
    /**
     * \todo add Async to the names
     */
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
    /// Get protocol object.
    const proto::Protocol& getProtocol() const noexcept {
        return _protocol;
    }

    /// Set message descriptor fixed wire-size.
    /**
     * \note Must be set before invoking any \a receive method.
     */
    static void setMessageDescriptorSize(std::size_t size) noexcept {
        _MESSAGE_DESCR_SIZE = size;
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
    void _handleWrite(const std::error_code &error, std::size_t bytesSend);
    void _handleRead(const std::string &inputMessageData);

private:
    static std::size_t _generateConnId();
    /// Start receive infinite loop.
    void _startReceive();
    void _doSend(/*WritePolicy*/);

private:
    /// Socket representing the server-side connection endpoint.
    asio::ip::tcp::socket _socket;
    /// Identifies the connection.
    std::size_t _id;
    /// Reader.
    conn::AsyncReader<asio::ip::tcp::socket> _reader;
    /// Client-server interaction handler.
    std::unique_ptr<logic::ClientHandler> _handler;
    /// Protocol object.
    proto::Protocol _protocol;
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
    /// Message descriptor (framing protocol for ChatPacket) fixed wire-size.
    static std::size_t _MESSAGE_DESCR_SIZE;

protected:
    /// State changes observer.
    ConnectionStateListener &_observer;
};

inline std::ostream& operator<<(std::ostream &out, const TcpConnection &connection) {
    return out << "Connection(id= " << connection.getId() << " )";
}

} // namespace acs::conn

#endif // ACS_TCPCONNECTION_HPP__
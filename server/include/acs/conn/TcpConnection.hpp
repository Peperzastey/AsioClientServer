/** \file
 *  \brief Definition of the \a TcpConnection class.
 */
#ifndef ACS_TCPCONNECTION_HPP__
#define ACS_TCPCONNECTION_HPP__

#include "acs/conn/ConnectionStateListener.hpp"
#include "acs/proto/PolymorphicMsgProtocol.hpp"
#include "acs/logic/ClientHandler.hpp"
#include "acs/conn/AsyncWriter.hpp"
#include "acs/conn/AsyncReader.hpp"
#include "acs/util/Logger.hpp"
#include "acs/util/Identity.hpp"
#include "acs/context/ConnectionContext.hpp"
#include "acs/context/ContextResolver.hpp"
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
    using Protocol = proto::PolymorphicMsgProtocol;
    using Socket = asio::ip::tcp::socket;

public:
    /// Constructor.
    /**
     * \param ioContext IO Context of the Application object
     * \param responseFactory object of type response_factory_func_t used to create response to a request provided
     */
    template <typename ClientHandlerT>
    TcpConnection(asio::io_context &ioContext, Protocol &protocol, ConnectionStateListener &observer, util::Identity<ClientHandlerT> handlerType)
        : _socket(ioContext), _id(_generateConnId()), _writer(_socket), _reader(_socket),
          _handler(std::make_unique<typename decltype(handlerType)::type>(*this)), _protocol(&protocol),
          _contextId(context::generateNewContextId()), _observer(observer) {
        // register ConnectionContext for Message handlers
        auto&& connCtxResolver = context::ContextResolver<context::ConnectionContext>::instance();
        connCtxResolver.addNewContextInstance(_contextId, *this);
    }
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
    
    /*, TODO WritePolicy policy = QueueWrite*/
    void send(const Protocol::PacketType &packet, Protocol::Message::TypeId type);

    /// Get server-side endpoint socket of this TcpConnection.
    Socket& getSocket() noexcept {
        return _socket;
    }
    /// Get connection id.
    std::size_t getId() const noexcept {
        return _id;
    }
    /// Get protocol object.
    const Protocol& getProtocol() const noexcept {
        return *_protocol;
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
    void _handleRead(const std::string &inputMessageData);

private:
    static std::size_t _generateConnId();
    /// Start receive infinite loop.
    void _startReceive();

private:
    /// Socket representing the server-side connection endpoint.
    Socket _socket;
    /// Identifies the connection.
    std::size_t _id;
    /// Writer.
    conn::AsyncWriter<Socket> _writer;
    /// Reader.
    conn::AsyncReader<Socket> _reader;
    /// Client-server interaction handler.
    std::unique_ptr<logic::ClientHandler> _handler;
    /// Protocol object.
    Protocol *_protocol;
    /// Context id for this connection.
    context::ContextId _contextId;

protected:
    /// State changes observer.
    ConnectionStateListener &_observer;
};

inline std::ostream& operator<<(std::ostream &out, const TcpConnection &connection) {
    return out << "Connection(id= " << connection.getId() << " )";
}

} // namespace acs::conn

#endif // ACS_TCPCONNECTION_HPP__
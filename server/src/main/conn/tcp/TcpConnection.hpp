/** \file
 *  \brief Definition of the \a TcpConnection class.
 */
#ifndef TCPCONNECTION_HPP__
#define TCPCONNECTION_HPP__

#include "conn/ConnectionStateListener.hpp"
#include "util/SimpleLogger.hpp"
#include <memory>
#include <utility>
#include <string>
#include <string_view>
#include <system_error>
#include <ostream>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/write.hpp>
#include "chat.pb.h"

/// Contains server-side TCP connection functionality.
namespace cs::server::conn::tcp {

using namespace cs::common;

static std::string default_response_factory([[maybe_unused]] std::string_view request) {
    return "Default Response.";
}

static std::string chat_proto_response_factory([[maybe_unused]] std::string_view request) {
    using namespace ::cs::common::conn::protocol;
    ChatMessage message{};
    message.set_id(6);
    message.set_text("Test text\nNext test line.\n");
    message.set_type(ChatMessage::TEST);

    std::string output;
    auto result = message.SerializeToString(&output);
    if (!result) {
        util::log() << "Failed to serialize chat message." << std::endl;
    }
    return output; //(move)
}

/// Represents server-side part of a TCP connection.
class TcpConnection {
public:
    /// Typedef for pointer holding the sole ownership of an object of this class.
    using upointer = std::unique_ptr<TcpConnection>;
    /// Response factory function type typedef.
    /**
     * \todo function address/pointer ? std::string(*)(std::string_view)
     */
    //using response_factory_func_t = std::string(std::string_view);
    using response_factory_func_t = decltype(std::addressof(std::declval<std::string(std::string_view)>()));


    TcpConnection(asio::io_context &ioContext, ConnectionStateListener &observer, response_factory_func_t responseFactory = chat_proto_response_factory)
        : _socket(ioContext), _observer(observer), _responseFactory(responseFactory) {}

    // TcpConnection is not copy-constructible.
    TcpConnection(const TcpConnection&) = delete;
    // TcpConnection is not copy-assignable.
    TcpConnection& operator=(const TcpConnection&) = delete;


    /// Create an instance of TcpConnection.
    /**
     * \param ioContext IO Context of the Application object
     * \param responseFactory object of type response_factory_func_t used to create response to a request provided to it
     * 
     * \return upointer holding sole ownership of the created instance
     * 
     * \warning this implementation requires that TcpConnection has virtual destructor
     * \todo change make_shared_from_private lambda to use parameter pack
     * \todo perform leak check with Valgrind
     */
    static upointer create(asio::io_context &ioContext, ConnectionStateListener &observer, response_factory_func_t responseFactory = default_response_factory) {
        auto make_shared_from_private = [](auto&& context, auto&& observer, auto&& respFactory) {
            struct make_shared_enabler : public TcpConnection {
                using Ctx = decltype(context);
                using Observer = decltype(observer);
                using RF = decltype(respFactory);
                make_shared_enabler(Ctx&& ctx, Observer&& obs, RF&& rf) : TcpConnection(std::forward<Ctx>(ctx), std::forward<Observer>(obs), std::forward<RF>(rf)) {}
            };

            return std::make_unique<make_shared_enabler>(context, observer, respFactory);
        };

        return make_shared_from_private(ioContext, observer, responseFactory);
    }

    /// Start handling the connection.
    void start() {
        _sendMsg = _responseFactory({});

        /*asio::async_write(_socket, asio::buffer(_sendMsg), [uptr = std::move(this)](auto&&... params) {
            uptr->handleWrite(std::forward<decltype(params)>(params)...);
        }); ??? */
        asio::async_write(_socket, asio::buffer(_sendMsg), [this](auto&&... params) {
            handleWrite(std::forward<decltype(params)>(params)...);
        });
    }

    /// Close the connection.
    void close() {
        _observer.connectionClosed(*this);
    }

    /// Get server-side endpoint socket of this TcpConnection.
    inline asio::ip::tcp::socket& getSocket() noexcept {
        return _socket;
    }

    /// Get connection id.
    inline std::size_t getId() const noexcept {
        return _id;
    }

    /// Set a \a ResponseFactory that will be used to create responses to new requests.
    /**
     * Uses Strategy pattern.
     * 
     * \todo template <typename T> + static_asserts about T's methods / signature ?
     * \todo or overloads - one with functor argument, second with ...
     */
    void setResponseFactory(response_factory_func_t responseFactory) {
        _responseFactory = responseFactory;
    }

public:
    inline bool operator==(const TcpConnection &other) const noexcept {
        return this->_id == other._id;
    }

    virtual ~TcpConnection() = default;

protected:
    void handleWrite(const std::error_code &error, std::size_t bytesSend) {
        util::log() << "handleWrite" << std::endl;
        close();
    }

private:
    /**
     * \todo return socket's port number (opt. concat timestamp) ?
     */
    static std::size_t generateConnId() {
        static std::size_t nextId = 0;
        return nextId++;
    }

private:
    /// Socket representing the server-side connection endpoint.
    asio::ip::tcp::socket _socket;
    /// Response factory.
    response_factory_func_t _responseFactory;
    /// Message to send to the connected peer.
    /**
     * Holds the message until it is sent (asynchronously).
     */
    std::string _sendMsg;
    /// Identifies the connection.
    std::size_t _id = generateConnId();

protected:
    ConnectionStateListener &_observer;
};

inline std::ostream& operator<<(std::ostream &out, const TcpConnection &connection) {
    return out << "Connection(id= " << connection.getId() << " )";
}

} // namespace cs::server::conn::tcp

#endif // TCPCONNECTION_HPP__
/** \file
 *  \brief Definition of the \a TcpConnection class.
 */
#ifndef ACS_TCPCONNECTION_HPP__
#define ACS_TCPCONNECTION_HPP__

#include "acs/conn/ConnectionStateListener.hpp"
#include "acs/util/Logger.hpp"
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
namespace acs::conn {

static std::string default_response_factory([[maybe_unused]] std::string_view request) {
    return "Default Response.";
}

static std::string chat_proto_response_factory([[maybe_unused]] std::string_view request) {
    using namespace proto;
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
    /// Response factory function type typedef.
    /**
     * \todo function address/pointer ? std::string(*)(std::string_view)
     */
    //using response_factory_func_t = std::string(std::string_view);
    using response_factory_func_t = decltype(std::addressof(std::declval<std::string(std::string_view)>()));

    /// Constructor
    /**
     * \param ioContext IO Context of the Application object
     * \param responseFactory object of type response_factory_func_t used to create response to a request provided
     */
    TcpConnection(asio::io_context &ioContext, ConnectionStateListener &observer, response_factory_func_t responseFactory = chat_proto_response_factory)
        : _socket(ioContext), _observer(observer), _responseFactory(responseFactory) {}

    /// TcpConnection is not copy-constructible.
    TcpConnection(const TcpConnection&) = delete;
    /// TcpConnection is not copy-assignable.
    TcpConnection& operator=(const TcpConnection&) = delete;


    /// Start handling the connection.
    void start() {
        _sendMsg = _responseFactory({});

        asio::async_write(_socket, asio::buffer(_sendMsg), [this](auto&&... params) {
            handleWrite_close(std::forward<decltype(params)>(params)...);
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
    /// TcpConnection object can be compared for equality with other object of this class.
    /**
     * \note This is used for removing stored connection objects on the server.
     */
    inline bool operator==(const TcpConnection &other) const noexcept {
        return this->_id == other._id;
    }

protected:
    void handleWrite_close(const std::error_code &error, std::size_t bytesSend) {
        util::log() << "handleWrite" << std::endl;
        close();
    }

    void handleWrite(const std::error_code &error, std::size_t bytesSend) {
        util::log() << "handleWrite" << std::endl;
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
     * Holds the message until it is (asynchronously) sent.
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

} // namespace acs::conn

#endif // ACS_TCPCONNECTION_HPP__
/** \file 
 *  \brief Definition of the \a SyncTcpClient class.
 */
#ifndef ACS_ASYNCTCPCLIENT_HPP__
#define ACS_ASYNCTCPCLIENT_HPP__

#include "acs/proto/Protocol.hpp"
#include "acs/conn/AsyncWriter.hpp"
#include <ostream>
#include <asio/ip/tcp.hpp>

namespace asio {
class io_context;
} // namespace asio

namespace acs::proto {
class FramePrefix;
class ChatMessage;
} // namespace acs::proto

/// Contains connection functionality.
namespace acs::conn {

/// TCP connection client endpoint.
/**
 * Final class. Has no virtual destructor.
 * \todo Use builder pattern ?
 */
class AsyncTcpClient final {
public:
    /// Port number typedef.
    using port_t = std::uint16_t;
    using Socket = asio::ip::tcp::socket;
    using Protocol = proto::Protocol;

public:
    /// Constructor.
    /**
     * Connects to the provided \a remoteHost, \a service pair synchronously.
     * 
     * \param ioContext application's \a asio::io_context
     * \param remoteHost IP address of the remote to connect to
     * \param remotePort remote's port number
     */
    explicit AsyncTcpClient(asio::io_context &ioContext, Protocol &protocol, std::string_view remoteHost, port_t remotePort);

    /// Run infinite loop receiving data from the \a remoteHost.
    /**
     * Runs synchronously.
     * 
     * Returns when the \a remoteHost closes the connection.
     */
    void receiveInfinitelySync(std::ostream &out, std::ostream &errorOut);
    /**
     * \warning no other \a receive method can be called after calling this method
     */
    void receiveInfinitely(std::ostream &out, std::ostream &errorOut);
    
    void send(const proto::Protocol::MessageType &message);

public:
    /// Size of the receive buffer.
    static constexpr std::size_t RECV_BUFFER_SIZE = 128;

protected:
    void handleConnect(const std::error_code &error);
    static std::string _decodeMessage(const proto::ChatMessage &message);

private:
    /// Initialize framing protocol.
    /**
     * Must be called before any \a receive method.
     */
    static void _initializeFramingProtocol();
    /// Decode message size from framing protocol's prefix.
    /**
     * Pre-condition: Framing protocol must be initialized.
     * \see _initializeFramingProtocol
     */
    static std::size_t _decodeMessageSize(const proto::FramePrefix &prefix);

private:
    /// Client-side endpoint socket.
    Socket _socket;
    std::array<unsigned char, RECV_BUFFER_SIZE> _recvBuffer; // uint8_t
    Protocol *_protocol;
    conn::AsyncWriter<Socket> _writer;
    /// Framing protocol prefix size in bytes.
    static std::size_t _FRAME_PREFIX_SIZE;

};

} // namespace acs::conn

#endif // ACS_ASYNCTCPCLIENT_HPP__

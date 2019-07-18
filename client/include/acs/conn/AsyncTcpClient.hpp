/** \file 
 *  \brief Definition of the \a SyncTcpClient class.
 */
#ifndef ACS_ASYNCTCPCLIENT_HPP__
#define ACS_ASYNCTCPCLIENT_HPP__

#include "acs/proto/PolymorphicMsgProtocol.hpp"
#include "acs/conn/AsyncWriter.hpp"
#include "acs/conn/AsyncReader.hpp"
#include <ostream>
#include <asio/ip/tcp.hpp>

namespace asio {
class io_context;
} // namespace asio

namespace acs::proto {
class FramePrefix;
class ChatPacket;
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
    using Protocol = proto::PolymorphicMsgProtocol;

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
    
    /**
     * \warning no other \a receive method can be called after calling this method
     */
    void receiveInfinitely();
    
    void send(const Protocol::Message &message);
    void send(const Protocol::PacketType &packet, Protocol::Message::TypeId type);

public:
    /// Size of the receive buffer.
    static constexpr std::size_t RECV_BUFFER_SIZE = 128;

protected:
    void _handleConnect(const std::error_code &error);
    void _handleRead(const std::string &inputMessageData);

private:
    /// Client-side endpoint socket.
    Socket _socket;
    std::array<unsigned char, RECV_BUFFER_SIZE> _recvBuffer; // uint8_t
    Protocol *_protocol;
    conn::AsyncWriter<Socket> _writer;
    conn::AsyncReader<Socket> _reader;
};

} // namespace acs::conn

#endif // ACS_ASYNCTCPCLIENT_HPP__

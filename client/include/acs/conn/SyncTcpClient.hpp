/** \file 
 *  \brief Definition of the \a SyncTcpClient class.
 */
#ifndef ACS_SYNCTCPCLIENT_HPP__
#define ACS_SYNCTCPCLIENT_HPP__

#include <ostream>
#include <asio/ip/tcp.hpp>

namespace asio {
class io_context;
} // namespace asio

namespace acs::proto {
class ChatMessage;
} // namespace acs::proto

/// Contains connection functionality.
namespace acs::conn {

/// TCP connection client endpoint.
/**
 * Final class. Has no virtual destructor.
 * \todo Use builder pattern ?
 */
class SyncTcpClient final {
public:
    /// Port number typedef.
    using port_t = std::uint16_t;

    /// Constructor.
    /**
     * Connects to the provided \a remoteHost, \a service pair synchronously.
     * 
     * \param ioContext application's \a asio::io_context
     * \param remoteHost IP address of the remote to connect to
     * \param remotePort remote's port number
     */
    explicit SyncTcpClient(asio::io_context &ioContext, std::string_view remoteHost, port_t remotePort);

    /// Run infinite loop receiving data from the \a remoteHost.
    /**
     * Runs synchronously.
     * 
     * Returns when the \a remoteHost closes the connection.
     */
    void receiveInfinitely(std::ostream &out);

public:
    /// Size of the receive buffer.
    static constexpr std::size_t RECV_BUFFER_SIZE = 128;

protected:
    static std::string _decodeMessage(const proto::ChatMessage &message);

private:
    /// Client-side endpoint socket.
    asio::ip::tcp::socket _socket;
};

} // namespace acs::conn

#endif // ACS_SYNCTCPCLIENT_HPP__
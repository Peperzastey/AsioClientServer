/** \file 
 *  \brief Definition of the \a SyncTcpClient class.
 */
#ifndef SYNCTCPCLIENT_HPP__
#define SYNCTCPCLIENT_HPP__

#include <ostream>
#include <asio/ip/tcp.hpp>

//TODO class cs::core::Application;
namespace cs::core {
// forward declaration
class Application;
} // namespace cs::core

namespace cs::common::conn::protocol {
class ChatMessage;
} // namespace cs::common::conn::protocol

/// Contains TCP connection functionality.
namespace cs::conn::tcp {

using namespace core;
using namespace common::conn;

/// TCP connection client endpoint.
/**
 * Final class. Has no virtual destructor.
 * \todo Use builder pattern ?
 */
class SyncTcpClient final {
public:
    /// Constructor
    /**
     * Connects to the provided \a remoteHost, \a service pair synchronously.
     * 
     * \param app reference to the Application object
     * \param remoteHost hostname to connect to
     * \param service \a remoteHost's service to connect to
     * 
     * \todo string_view ?
     */
    explicit SyncTcpClient(Application &app, const std::string &remoteHost, const std::string &service);

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
    static std::string _decodeMessage(const protocol::ChatMessage &message);

private:
    /// Reference to the Application object.
    Application &_app;
    /// Client-side endpoint socket.
    asio::ip::tcp::socket _socket;
};

} // namespace cs::conn::tcp

#endif // SYNCTCPCLIENT_HPP__
#include "acs/conn/AsyncTcpClient.hpp"
#include "acs/util/Logger.hpp"
#include <string>
#include <array>
#include <system_error>
#include <asio/ip/tcp.hpp>
#include <asio/ip/address.hpp>
#include <asio/read.hpp>
#include <sstream>
#include "framing.pb.h"
#include "chat.pb.h"
//#define NO_ASSERT
#include <cassert>

namespace acs::conn {

AsyncTcpClient::AsyncTcpClient(asio::io_context &ioContext, Protocol &protocol, std::string_view remoteHost, port_t remotePort)
    : _socket(ioContext), _protocol(&protocol), _writer(_socket), _reader(_socket) {
    asio::ip::tcp::endpoint remote(asio::ip::make_address(remoteHost), remotePort);
    _socket.async_connect(remote, [this](auto&&... params) {
        _handleConnect(std::forward<decltype(params)>(params)...);
    });
}

/**
 * \todo refactor this method (duplication, complication)
 */
void AsyncTcpClient::receiveInfinitely() {
    _reader.readAsyncProtoInfOccupy([this](std::string data) {
            _handleRead(std::move(data));
        },
        _protocol
    );
}

void AsyncTcpClient::send(const Protocol::Message &message) {
    std::string wireMessage = _protocol->serialize(message);
    _writer.write(std::move(wireMessage));
}

void AsyncTcpClient::send(const Protocol::PacketType &packet, Protocol::Message::TypeId type) {
    std::string wirePacket = _protocol->serialize(packet, type);
    _writer.write(std::move(wirePacket));
}

void AsyncTcpClient::_handleConnect(const std::error_code &error) {
    if (error)
        throw std::system_error(error);

    // start reading (async-ly) data from the connected-to remote
    receiveInfinitely();
}

void AsyncTcpClient::_handleRead(const std::string &inputMessageData) {
    //TODO move deserialization to AsyncReader (or some AsyncReader-decorator/adaptor)
    auto message = _protocol->deserialize(inputMessageData.data(), inputMessageData.size());

    message->handle(0);
}

} // namespace acs::conn

#include "acs/conn/AsyncTcpClient.hpp"
#include "acs/util/Logger.hpp"
#include "acs/message/EchoHandler.hpp"
#include <string>
#include <array>
#include <system_error>
#include <asio/ip/tcp.hpp>
#include <asio/ip/address.hpp>
#include <asio/read.hpp>
#include <sstream>
#include "framing.pb.h"
#include "chat.pb.h"
//TODO #define NO_ASSERT
#include <cassert>

namespace acs::conn {

std::size_t AsyncTcpClient::_FRAME_PREFIX_SIZE = 0;

void AsyncTcpClient::_initializeFramingProtocol() {
    static bool alreadyInitialized = false;

    if (!alreadyInitialized) {
        proto::FramePrefix initDummy{};
        initDummy.set_size(1);
        _FRAME_PREFIX_SIZE = initDummy.ByteSizeLong();

        alreadyInitialized = true;
    }
}

std::size_t AsyncTcpClient::_decodeMessageSize(const proto::FramePrefix &prefix) {
    return prefix.size();
}

AsyncTcpClient::AsyncTcpClient(asio::io_context &ioContext, proto::Protocol &protocol, std::string_view remoteHost, port_t remotePort)
    : _socket(ioContext), _protocol(&protocol), _writer(_socket), _reader(_socket) {
    _initializeFramingProtocol();
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

void AsyncTcpClient::send(const proto::Protocol::MessageType &message) {
    std::string wireMessage = _protocol->serialize(message);
    _writer.write(std::move(wireMessage));
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
    message::EchoHandler::handleClient(*message.get());
}

std::string AsyncTcpClient::_decodeMessage(const proto::ChatPacket &message) {
    std::ostringstream decoded("Message received:\n");

    decoded << "Id:   " << message.id()
          << "\nType: ";

    switch (message.type()) {
        case proto::ChatPacket::NORMAL:
            decoded << "normal";
            break;
        case proto::ChatPacket::URGENT:
            decoded << "urgent";
            break;
        default:
            decoded << "unknown type";
            break;
    }

    decoded << '\n' << message.text() << std::endl;

    return decoded.str();
}

} // namespace acs::conn
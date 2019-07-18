#include "acs/conn/TcpConnection.hpp"
#include "acs/logic/ClientHandler.hpp"
#include <utility>

namespace acs::conn {

/**
 * \todo return socket's port number (opt. concat timestamp) ?
 */
std::size_t TcpConnection::_generateConnId() {
    static std::size_t nextId = 0;
    return nextId++;
}

void TcpConnection::start() {
    _handler->handleStart();
    _startReceive();
}

void TcpConnection::close() {
    _handler->handleServerClose();
    _observer.connectionClosed(*this);
}

void TcpConnection::send(const Protocol::PacketType &packet, Protocol::Message::TypeId type) {
    std::string wirePacket = _protocol->serialize(packet, type);
    _writer.write(std::move(wirePacket));
}

void TcpConnection::_handleRead(const std::string &inputMessageData) {
    //TODO move deserialization to AsyncReader (or some AsyncReader-decorator/adaptor)
    auto message = _protocol->deserialize(inputMessageData.data(), inputMessageData.size());

    message->handle(_contextId);
}

void TcpConnection::_startReceive() {
    _reader.readAsyncProtoInfOccupy([this](std::string data) {
            _handleRead(std::move(data));
        },
        _protocol
    );
}

} // namespace acs::conn
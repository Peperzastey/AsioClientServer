#include "acs/conn/SyncTcpClient.hpp"
#include <string>
#include <array>
#include <system_error>
#include <asio/ip/tcp.hpp>
#include <asio/ip/address.hpp>
#include <sstream>
#include "chat.pb.h"

namespace acs::conn {

SyncTcpClient::SyncTcpClient(asio::io_context &ioContext, std::string_view remoteHost, port_t remotePort)
    : _socket(ioContext) {    
    asio::ip::tcp::endpoint remote(asio::ip::make_address(remoteHost), remotePort);
    _socket.connect(remote);
}

void SyncTcpClient::receiveInfinitely(std::ostream &out) {
    std::array<char, RECV_BUFFER_SIZE> recvBuffer;
    std::size_t readLen = 0;
    std::error_code error;

    while (true) {
        recvBuffer.fill('\0');
        readLen = _socket.read_some(asio::buffer(recvBuffer), error);

        if (error == asio::error::eof)
            break; // Connection closed cleanly by peer.
        else if (error)
            throw std::system_error(error);
        
        //TODO collect whole message first!!!
        std::string dataString(recvBuffer.data(), readLen);
        proto::ChatMessage message{};
        //TODO use ParseFromArray
        if (!message.ParseFromString(dataString)) {
            out << "Failed to parse chat message." << std::endl;
        }
        auto decodedMessage = _decodeMessage(message);

        out << decodedMessage;
        //out.write(recvBuffer.data(), readLen);
        //TODO? flush
    }
}

std::string SyncTcpClient::_decodeMessage(const proto::ChatMessage &message) {
    std::ostringstream decoded("Message received:\n");

    decoded << "Id:   " << message.id()
          << "\nType: ";

    switch (message.type()) {
        case proto::ChatMessage::NORMAL:
            decoded << "normal";
            break;
        case proto::ChatMessage::URGENT:
            decoded << "urgent";
            break;
        case proto::ChatMessage::EVENT_SETTER:
            decoded << "event setter";
            break;
        case proto::ChatMessage::EVENT_DELETER:
            decoded << "event deleter";
            break;
        case proto::ChatMessage::TEST:
            decoded << "test";
            break;
        default:
            decoded << "unknown type";
            break;
    }

    decoded << '\n' << message.text() << std::endl;

    return decoded.str();
}

} // namespace acs::conn
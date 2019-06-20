#include "SyncTcpClient.hpp"
#include "core/Application.hpp"
#include <string>
#include <array>
#include <system_error>
#include <asio/ip/tcp.hpp>
#include <asio/connect.hpp>
#include <sstream>
#include "chat.pb.h"

using namespace cs::common::conn;

namespace cs::conn::tcp {

SyncTcpClient::SyncTcpClient(Application &app, const std::string &remoteHost, const std::string &service)
    : _app(app), _socket(_app.getContext()) {
    /*asio::ip::tcp::resolver resolver(app.getContext());
    auto endpoints = resolver.resolve(remoteHost, service);
    asio::connect(_socket, endpoints);*/
    /*std::array remotes{ asio::ip::tcp::endpoint(asio::ip::address_v4(), 54321) };
    asio::connect(_socket, remotes);*/
    asio::ip::tcp::endpoint remote(asio::ip::address_v4(), 54321);
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
        protocol::ChatMessage message{};
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

std::string SyncTcpClient::_decodeMessage(const protocol::ChatMessage &message) {
    std::ostringstream decoded("Message received:\n");

    decoded << "Id:   " << message.id()
          << "\nType: ";

    switch (message.type()) {
        case protocol::ChatMessage::NORMAL:
            decoded << "normal";
            break;
        case protocol::ChatMessage::URGENT:
            decoded << "urgent";
            break;
        case protocol::ChatMessage::EVENT_SETTER:
            decoded << "event setter";
            break;
        case protocol::ChatMessage::EVENT_DELETER:
            decoded << "event deleter";
            break;
        case protocol::ChatMessage::TEST:
            decoded << "test";
            break;
        default:
            decoded << "unknown type";
            break;
    }

    decoded << '\n' << message.text() << std::endl;

    return decoded.str();
}

} // namespace cs::conn::tcp
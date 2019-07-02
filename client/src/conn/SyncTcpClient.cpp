#include "acs/conn/SyncTcpClient.hpp"
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

std::size_t SyncTcpClient::_FRAME_PREFIX_SIZE = 0;

void SyncTcpClient::_initializeFramingProtocol() {
    static bool alreadyInitialized = false;

    if (!alreadyInitialized) {
        proto::FramePrefix initDummy{};
        initDummy.set_size(1);
        _FRAME_PREFIX_SIZE = initDummy.ByteSizeLong();

        alreadyInitialized = true;
    }
}

std::size_t SyncTcpClient::_decodeMessageSize(const proto::FramePrefix &prefix) {
    return prefix.size();
}

SyncTcpClient::SyncTcpClient(asio::io_context &ioContext, std::string_view remoteHost, port_t remotePort)
    : _socket(ioContext) {
    _initializeFramingProtocol();
    asio::ip::tcp::endpoint remote(asio::ip::make_address(remoteHost), remotePort);
    _socket.connect(remote);
}

void SyncTcpClient::receiveInfinitely(std::ostream &out, std::ostream &errorOut) {
    std::array<char, RECV_BUFFER_SIZE> recvBuffer;
    //TODO use asio::dynamic_buffer with max_capacity equal to max income message size
    std::size_t readLen = 0;
    std::error_code error;

    while (true) {
        //recvBuffer.fill('\0');

        //TODO try asio::read_until

        // read FramePrefix
        readLen = asio::read(_socket, asio::buffer(recvBuffer, _FRAME_PREFIX_SIZE), error); // blocking read
        if (error == asio::error::eof) {
            out << "\nConnection closed cleanly by peer" << std::endl;
            break;
        } else if (error)
            throw std::system_error(error);
        
        assert(readLen == _FRAME_PREFIX_SIZE);    
        
        //readLen = _socket.read_some(asio::buffer(recvBuffer), error);
        // read ChatMessage
        proto::FramePrefix prefix{}; //TODO move parsing to _decodeMessageSize ?
        if (!prefix.ParseFromArray(recvBuffer.data(), readLen)) {
            errorOut << "Failed to parse frame prefix" << std::endl;
            break; //TODO throw exception
        }
        const auto messageSize = _decodeMessageSize(prefix);
        out << "DBG: size of message to come: " << messageSize << std::endl;
        
        readLen = asio::read(_socket, asio::buffer(recvBuffer, messageSize), error);
        if (error == asio::error::eof) {
            out << "\nConnection closed cleanly by peer" << std::endl;
            break;
        } else if (error)
            throw std::system_error(error);

        assert(readLen == messageSize);
        // whole message collected

        proto::ChatMessage message{};
        //TODO use ParseFromArray
        if (!message.ParseFromArray(recvBuffer.data(), readLen)) {
            //TODO throw exception ?
            errorOut << "Failed to parse chat message" << std::endl;
        } else {
            auto decodedMessage = _decodeMessage(message);

            out << decodedMessage;
            //out.write(recvBuffer.data(), readLen);
            //TODO? flush
        }
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
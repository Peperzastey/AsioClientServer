#include "acs/echo/EchoClientHandler.hpp"
#include "acs/conn/TcpConnection.hpp"
#include "acs/util/Logger.hpp"
#include "framing.pb.h"
#include "chat.pb.h"
#include <utility>
#include <cassert>

namespace acs::echo {

void EchoClientHandler::handleStart() {
    using namespace std::string_literals;

    //TODO in CMake configure file (after (=>depends) GenerateProto target, before server)
    proto::FramePrefix framePrefix{};
    //TODO somewhere outside of TcpConnection <- reading FramePrefix and invoking ChatMessage collection
    //or FramePrefix as first part of ChatMessage and use SerializePartial...() ?
    
    //conn::TcpConnection::setFramePrefixSize(dummy.ByteSizeLong());  


    //TODO send FramePrefix first
    // + SerializeToArray(array of size PREFIX_SIZE + '\0'??)

    proto::ChatMessage message{};
    message.set_id(1);
    message.set_text(
        "Welcome to ACS chat!\n"
        "Just type your message."s
    );
    message.set_type(proto::ChatMessage::TEST); //TODO SYSTEM / WELCOME

    framePrefix.set_size(message.ByteSizeLong()); // in bytes!
    const auto PREFIX_SIZE = framePrefix.ByteSizeLong();    // all fields must be set!
    // framePrefix.user_id Don't Care
    //std::string serializedPrefix;
    std::string output;
    auto result = framePrefix.SerializeToString(&output); //CHECK: does it append or dismiss previous content?
    //solution: string.reserve(); SerializeToArray(string.data()+x, n);
    if (!result) {
        util::Logger::instance().logError() << "Failed to serialize frame prefix" << std::endl;
        //TODO throw exception
        return;
    }
    result = message.AppendToString(&output);
    if (!result) {
        util::Logger::instance().logError() << "Failed to serialize chat message" << std::endl;
        //TODO throw exception
        return;
    }

    //warning: string.size() - number of CharT objects, not bytes!
    //std::basic_string<unsigned char> ? / std::vector<uint8_t>
    /*util::Logger::instance().log() << "output.size: " << output.size()
        << "\nexpected: " << PREFIX_SIZE + framePrefix.size() 
        << "\nPREFIX_SIZE: " << PREFIX_SIZE
        << "\nmessage size: " << framePrefix.size()
        << std::endl;*/
    assert(output.size() == PREFIX_SIZE + framePrefix.size());

    _connection->send(std::move(output)); // 2 in 1 go
    //_connection->send(std::move(serializedMessage));

    //TODO _connection->receiveInfinitely(); ?
    _connection->receive(PREFIX_SIZE);
}

void EchoClientHandler::handleSendComplete() {
    //close();
    // _connection->close();
}

void EchoClientHandler::handleServerClose() {
    util::Logger::instance().log()
        << "Connection " << _connection->getId() << " close performed by server" << std::endl;
}

} // namespace acs::echo
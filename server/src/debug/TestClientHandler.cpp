#include "acs/debug/TestClientHandler.hpp"
#include "acs/conn/TcpConnection.hpp"
#include "acs/util/Logger.hpp"
#include "chat.pb.h"
#include "framing.pb.h"
#include <utility>

namespace acs::debug {

void TestClientHandler::handleStart() {
    using namespace proto;
    ChatPacket message{};
    message.set_id(6);
    message.set_text("Test text\nNext test line.\n");
    message.set_type(ChatPacket::NORMAL);

    _connection->send(message, proto::FramePrefix::CHAT);
}

void TestClientHandler::handleSendComplete() {
    //close();
    _connection->close();
}

void TestClientHandler::handleServerClose() {
    util::Logger::instance().log()
        << "Connection " << _connection->getId() << " close performed by server" << std::endl;
}

} // namespace acs::debug
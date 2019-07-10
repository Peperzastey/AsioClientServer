#include "acs/debug/TestClientHandler.hpp"
#include "acs/conn/TcpConnection.hpp"
#include "acs/util/Logger.hpp"
#include "chat.pb.h"
#include <utility>

namespace acs::debug {

void TestClientHandler::handleStart() {
    using namespace proto;
    ChatPacket message{};
    message.set_id(6);
    message.set_text("Test text\nNext test line.\n");
    message.set_type(ChatPacket::NORMAL);

    std::string output;
    auto result = message.SerializeToString(&output);
    if (!result) {
        util::Logger::instance().logError() << "Failed to serialize chat message" << std::endl;
        //TODO throw exception
        return;
    }

    _connection->send(std::move(output));
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
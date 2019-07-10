#ifndef ACS_ECHO_HANDLER_HPP__
#define ACS_ECHO_HANDLER_HPP__

#include "acs/util/Logger.hpp"
#include "acs/proto/Protocol.hpp"
#include "echo.pb.h"
#include <google/protobuf/message_lite.h>
#include <utility>
#include <stdexcept>
#include <cassert>

namespace acs::message {

/// Handles ECHO messages.
//template <typename Sender>
class EchoHandler /*: public MessageHandler*/ {
public:
    using Message = google::protobuf::MessageLite;

public:
    //TODO non-owning `(const) Message*` or owning `std::unique_ptr<Message>&&` ?
    static void handleClient(Message &message) {
        util::Logger::instance().log() // logDebug
            << "DBG: handle Echo receive client" << std::endl;
        //need dynamic_cast
        //TODO static_cast if we are 100% certain of dynamic (runtime) type - avoids the cost of the runtime check
        auto echoMessage = dynamic_cast<proto::EchoPacket*>(&message);
        if (echoMessage == nullptr)
            throw std::domain_error{"Provided message is not of type proto::EchoPacket"};
        //TEMP assume ECHO_RESPONSE MessageType
        assert(echoMessage->type() == proto::EchoPacket::ECHO_RESPONSE);

        // handler logic
        //TEMP display on logger
        util::Logger::instance().log()
            << "ECHO_RSP id:" << echoMessage->id()
            << "  text: \"\n" << echoMessage->text() << "\n\"\n";
    }

    template <typename Sender>
    static void handleServer(Message &message, Sender &sender) {
        util::Logger::instance().log() // logDebug
            << "DBG: handle Echo receive server" << std::endl;
        //need dynamic_cast
        //TODO static_cast if we are 100% certain of dynamic (runtime) type - avoids the cost of the runtime check
        auto echoMessage = dynamic_cast<proto::EchoPacket*>(&message);
        if (echoMessage == nullptr)
            throw std::domain_error{"Provided message is not of type proto::EchoPacket"};
        //TEMP assume ECHO_REQUEST MessageType
        assert(echoMessage->type() == proto::EchoPacket::ECHO_REQUEST);

        // handler logic
        echoMessage->set_type(proto::EchoPacket::ECHO_RESPONSE);
        //sender.getProtocol().serialize(*echoMessage);
        auto wireMessage = sender.getProtocol().serialize(message);
        sender.send(std::move(wireMessage));
    }
};

} // namespace acs::message

#endif // ACS_ECHO_HANDLER_HPP__

#ifndef ACS_CLIENT_ECHO_MESSAGE_HPP__
#define ACS_CLIENT_ECHO_MESSAGE_HPP__

#include "acs/message/Message.hpp"
#include "acs/message/BaseMessage.hpp"
#include "echo.pb.h"
#include "acs/util/Logger.hpp"

namespace acs::message {

//template <typename Sender>
class EchoMessage
    : public Message/*<Sender>*/,
      private BaseMessage<proto::EchoPacket> {
public:
    using BaseMessage::BaseMessage;
    using BaseMessage::Packet;
    //EchoMessage(Message::TypeId typeId, Packet &&packet)
    //    : BaseMessage(typeId, std::move(packet)) {}
    //using Packet = BaseMessage::Packet;

    TypeId getTypeId() const noexcept override {
        return _typeId;
    }
    std::size_t getByteSize() const override {
        return _packet.ByteSizeLong();
    }

private:
    void _doHandle(/*Sender &sender [[maybe_unused]]*/) override {
        //TEMP write to logger
        _decodePacket(util::Logger::instance().log());
    }

    void _doSerializeAppendToString(std::string &str) const override {
        if (!_packet.AppendToString(&str)) {
            //TODO get error/debug string from MessageLite
            throw std::runtime_error{"Failed to serialize packet. Packet type: " + _packet.GetTypeName()};
        }
    }

    void _decodePacket(std::ostream &ostr) const {
        ostr << "DBG: client::EchoMessage::handle" << std::endl;

        assert(_packet.type() == proto::EchoPacket::ECHO_RESPONSE);

        ostr << "ECHO_RSP id:" << _packet.id()
            << "  text: \"\n" << _packet.text() << "\n\"\n";
    }

private:
    //from BaseMessage:
    // proto::EchoPacket _packet;
    // TypeId _typeId;
};

} // namespace acs::message

#endif // ACS_CLIENT_ECHO_MESSAGE_HPP__

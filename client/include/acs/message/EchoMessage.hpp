#ifndef ACS_CLIENT_ECHO_MESSAGE_HPP__
#define ACS_CLIENT_ECHO_MESSAGE_HPP__

#include "acs/message/Message.hpp"
#include "acs/message/BaseMessage.hpp"
#include "echo.pb.h"
#include "acs/util/Logger.hpp"
#include <sstream>

namespace acs::message {

template <typename Context>
class EchoMessage
    : public Message,
      private BaseMessage<proto::EchoPacket> {
public:
    //using BaseMessage::BaseMessage;
    using BaseMessage::Packet;

    EchoMessage(Message::TypeId typeId, Packet &&packet, Context &&context)
        : BaseMessage(typeId, std::move(packet)),
          _context(std::move(context)) {}

    TypeId getTypeId() const noexcept override {
        return _typeId;
    }
    std::size_t getByteSize() const override {
        return _packet.ByteSizeLong();
    }

private:
    void _doHandle(ContextId contextId [[maybe_unused]]) override {
        //TEMP write to logger
        //TODO write to Output/Display-interface object
        //TODO use _context
        
        //_decodePacket(util::Logger::instance().log());
        std::ostringstream ostr{};
        _decodePacket(ostr);
        _context.stdoutWriter.write(ostr.str());
    }

    /**
     * \todo move to BaseMessage - make it Message subclass and inherit publicly
     */
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
    Context _context;
};

} // namespace acs::message

#endif // ACS_CLIENT_ECHO_MESSAGE_HPP__

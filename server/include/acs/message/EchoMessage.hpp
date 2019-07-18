#ifndef ACS_SERVER_ECHO_MESSAGE_HPP__
#define ACS_SERVER_ECHO_MESSAGE_HPP__

#include "acs/message/Message.hpp"
#include "acs/message/BaseMessage.hpp"
#include "echo.pb.h"
#include "acs/util/Logger.hpp"
#include <sstream>

namespace acs::message {

class EchoMessage
    : public Message,
      private BaseMessage<proto::EchoPacket> {
public:
    using BaseMessage::BaseMessage;
    using BaseMessage::Packet;

    TypeId getTypeId() const noexcept override {
        return _typeId;
    }
    std::size_t getByteSize() const override {
        return _packet.ByteSizeLong();
    }

private:
    /**
     * Can be called only once for any given object.
     */
    void _doHandle(ContextId contextId [[maybe_unused]]) override;

    void _doSerializeAppendToString(std::string &str) const override {
        if (!_packet.AppendToString(&str)) {
            throw std::runtime_error{"Failed to serialize packet. Packet type: " + _packet.GetTypeName()};
        }
    }

private:
    //from BaseMessage:
    // proto::EchoPacket _packet;
    // TypeId _typeId;
};

} // namespace acs::message

#endif // ACS_SERVER_ECHO_MESSAGE_HPP__

#ifndef ACS_BASE_MESSAGE_HPP__
#define ACS_BASE_MESSAGE_HPP__

#include "acs/message/Message.hpp"
#include <utility>

namespace acs::message {

//TODO change into a base-implementation Message public subclass (as in Java Collections Framework) ?
template <typename PacketT>
class BaseMessage {
public:
    using Packet = PacketT;

    BaseMessage(Message::TypeId typeId, Packet &&packet)
        : _typeId(typeId), _packet(std::move(packet)) {}

protected:
    ~BaseMessage() = default;

    Message::TypeId _typeId;
    Packet _packet;
};

} // namespace acs::message

#endif // ACS_BASE_MESSAGE_HPP__

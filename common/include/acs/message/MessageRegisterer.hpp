#ifndef ACS_MESSAGE_REGISTERER_HPP__
#define ACS_MESSAGE_REGISTERER_HPP__

#include "acs/message/MessageRegistry.hpp"
#include <google/protobuf/message_lite.h>
#include <memory>

namespace acs::message {

class MessageRegisterer {
public:
    //using MessageRegistry::MessageTypeId;
    using MessageTypeId = MessageRegistry::MessageTypeId;

public:
    explicit MessageRegisterer(MessageRegistry &registry)
        : _registry(registry) {}

    /// Register concrete Message type.
    /**
     * \a MessageT type parameter must be a complete type.
     */
    template <typename MessageT>
    void registerMessageType(MessageTypeId msgTypeId) {
        _registry.addMessageFactory(msgTypeId, [msgTypeId](const void *data, std::size_t size) {
            typename MessageT::Packet packet{};
            if (!packet.ParseFromArray(data, size)) {
                throw std::runtime_error{"Wrong packet format. Parse failed. Packet type: " + packet.GetTypeName()};
            }
            return std::make_unique<MessageT>(msgTypeId, std::move(packet));
        });
        //TODO check return value?
    }

private:
    MessageRegistry &_registry;
};

} // namespace acs::message

#endif // ACS_MESSAGE_REGISTERER_HPP__

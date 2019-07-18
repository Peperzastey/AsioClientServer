#ifndef ACS_MESSAGE_REGISTRY_HPP__
#define ACS_MESSAGE_REGISTRY_HPP__

#include "acs/message/Message.hpp"
#include <memory>
#include <utility>
#include <functional>
#include <unordered_map>

namespace acs::message {

class MessageRegistry {
public:
    using MessagePtr = std::unique_ptr<Message>;
    using MessageFactory = std::function<MessagePtr(const void *data, std::size_t size)>;
    using MessageTypeId = Message::TypeId;

public:
    static constexpr std::size_t DEFAULT_BUCKET_COUNT = 10;

public:
    explicit MessageRegistry(std::size_t mappingBucketCount = DEFAULT_BUCKET_COUNT)
        : _mapping(mappingBucketCount) {}

    /// Get concrete Message factory function.
    /**
     * \param msgTypeId MessageType id of the Message to get
     * \retrun const reference to the factory function
     * \throw std::out_of_range when no mapping for given \a msgTypeId argument exists
     */
    const MessageFactory& getMessageFactory(MessageTypeId msgTypeId) const {
        return _mapping.at(msgTypeId);
    }

    bool addMessageFactory(const MessageTypeId &msgTypeId, MessageFactory &&factory) {
        auto [iter, success] = _mapping.try_emplace(msgTypeId, std::move(factory));
        (void)iter;
        return success;
    }

private:
    std::unordered_map<MessageTypeId, MessageFactory> _mapping;
};

} // namespace acs::message

#endif // ACS_MESSAGE_REGISTRY_HPP__

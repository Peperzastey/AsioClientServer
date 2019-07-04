#ifndef ACS_PROTOCOL_HPP__
#define ACS_PROTOCOL_HPP__

#include <string>
#include <memory>
#include <google/protobuf/message_lite.h>

namespace acs::proto {

enum class FramingProtocolType {
    FRAMING_PROTOBUF_SIZE_TYPE = 0, // default
};

/// Serializes and deserializes messages according to the specified protocol.
/**
 * Handles the framing protocol during serialization and deserialization.
 * \todo abstract base class?
 */
class Protocol {
public:
    using MessageType = google::protobuf::MessageLite;

public:
    Protocol(FramingProtocolType framingType = FramingProtocolType{0});

    std::string serialize(const MessageType &message) const;
    std::unique_ptr<MessageType> deserialize(const void *data, int size) const;

    std::size_t getFramePrefixSize() const noexcept { // inline
        return _PREFIX_SIZE;
    }
    std::size_t getMessageSize(const void *prefixData, int size);

private:
    const std::size_t _PREFIX_SIZE;
};

} // namespace acs::proto

#endif // ACS_PROTOCOL_HPP__

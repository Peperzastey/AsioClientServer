#ifndef ACS_PROTOCOL_HPP__
#define ACS_PROTOCOL_HPP__

#include <string>
#include <memory>
#include <google/protobuf/message_lite.h>
#include "framing.pb.h"

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
    using MessagePtr = std::unique_ptr<MessageType>;

public:
    Protocol(FramingProtocolType framingType = FramingProtocolType{0});

    std::string serialize(const MessageType &message) const;
    /// Deserialize (parse) \a data into a \a MessageType(-derived class) object.
    /**
     * \param data buffer of size \a size containing the data to parse
     * \param size the size of the \data buffer to parse
     */
    MessagePtr deserialize(const void *data, int size) const;

    std::size_t getFramePrefixSize() const noexcept { // inline
        return _PREFIX_SIZE;
    }
    std::size_t getMessageSize(const void *prefixData, int size) const;

private:
    inline std::size_t _calculatePrefixSize() const;

private:
    // allocate on heap? - see: Protobuf Arena
    mutable proto::FramePrefix _deserFramePrefixCache;
    mutable proto::FramePrefix _serFramePrefixCache;
    const std::size_t _PREFIX_SIZE;
};

} // namespace acs::proto

#endif // ACS_PROTOCOL_HPP__

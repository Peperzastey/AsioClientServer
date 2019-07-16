#ifndef ACS_POLYMORPHIC_MSG_PROTOCOL_HPP__
#define ACS_POLYMORPHIC_MSG_PROTOCOL_HPP__

#include "acs/message/MessageRegistry.hpp"
#include "framing.pb.h"
#include <string>
#include <memory>
#include <google/protobuf/message_lite.h>

namespace acs::proto {

/// Serializes and deserializes messages according to the specified protocol.
/**
 * Handles the framing protocol during serialization and deserialization.
 */
class PolymorphicMsgProtocol {
public:
    using PacketType = google::protobuf::MessageLite;
    using MessagePtr = message::MessageRegistry::MessagePtr;
    using Message = message::Message;

public:
    explicit PolymorphicMsgProtocol(const message::MessageRegistry &msgRegistry)
        : _msgReg(msgRegistry), _PREFIX_SIZE(_calculatePrefixSize()) {}

    std::string serialize(const Message &message) const;
    /// Deserialize (parse) \a data into a \a Message(-derived class) object.
    /**
     * \param data buffer of size \a size containing the data to parse
     * \param size the size of the \data buffer to parse
     */
    MessagePtr deserialize(const void *data, int size) const;

    std::size_t getFramePrefixSize() const noexcept { // inline
        return _PREFIX_SIZE;
    }
    std::size_t getPacketSize(const void *prefixData, int size) const;

private:
    /*inline*/ std::size_t _calculatePrefixSize() const;

private:
    const message::MessageRegistry &_msgReg;
    mutable proto::FramePrefix _deserFramePrefixCache;
    mutable proto::FramePrefix _serFramePrefixCache;
    // must be constructed after _serFramePrefixCache
    const std::size_t _PREFIX_SIZE;
};

} // namespace acs::proto

#endif // ACS_POLYMORPHIC_MSG_PROTOCOL_HPP__

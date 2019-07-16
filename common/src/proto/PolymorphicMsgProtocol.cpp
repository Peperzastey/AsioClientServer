#include "acs/proto/PolymorphicMsgProtocol.hpp"
#include "acs/util/Logger.hpp"
#include "chat.pb.h"
#include <stdexcept>
#include <cassert>

namespace acs::proto {

std::string PolymorphicMsgProtocol::serialize(const Message &message) const {
    _serFramePrefixCache.Clear();
    _serFramePrefixCache.set_size(message.getByteSize());
    _serFramePrefixCache.set_type(message.getTypeId());

    std::string output;
    if (!_serFramePrefixCache.SerializeToString(&output)) {
        //TODO get error/debug string from MessageLite
        throw std::runtime_error{"Failed to serialize frame prefix"};
    }
    message.serializeAppendToString(&output);

    assert(output.size() == _PREFIX_SIZE + _serFramePrefixCache.size());
    return output;
}

/**
 * \pre getPacketSize must be called directly before calling this method
 * This method uses _deserFramePrefixCache value assigned in getPacketSize
 */
PolymorphicMsgProtocol::MessagePtr PolymorphicMsgProtocol::deserialize(const void *data, int size) const {
    return _msgReg.getMessageFactory(_deserFramePrefixCache.type())(data, size);
}

/**
 * Caches prefix frame packet.
 * \post Prefix frame packet is stored (cached) in _deserFramePrefixCache member
 */
std::size_t PolymorphicMsgProtocol::getPacketSize(const void *prefixData, int size) const {
    _deserFramePrefixCache.Clear();
    if (!_deserFramePrefixCache.ParseFromArray(prefixData, size)) {
        throw std::runtime_error{"Wrong frame prefix format. Parse failed"};
    }

    return _deserFramePrefixCache.size();
}

std::size_t PolymorphicMsgProtocol::_calculatePrefixSize() const {
    _serFramePrefixCache.set_size(1);
    _serFramePrefixCache.set_type(1);
    assert(_serFramePrefixCache.IsInitialized());
    auto prefixSize = _serFramePrefixCache.ByteSizeLong();
    _serFramePrefixCache.Clear();  
    return prefixSize;
}

} // namespace acs::proto

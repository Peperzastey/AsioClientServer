#include "acs/proto/Protocol.hpp"
#include "acs/util/Logger.hpp"
#include "chat.pb.h"
#include <stdexcept>
#include <cassert>

namespace acs::proto {

Protocol::Protocol(FramingProtocolType framingType)
    : _PREFIX_SIZE(_calculatePrefixSize()) {} //TODO is it fine to call this method in init list ?

std::string Protocol::serialize(const MessageType &message/*, enum MessageType type*/) const {
    _serFramePrefixCache.Clear();
    _serFramePrefixCache.set_size(message.ByteSizeLong());
    //TODO framePrefix.set_type() according to MessageType dynamic type !
    // message.GetTypeName() <- returns string -> so unordered_map<string, FramePrefix::MessageType> ? 
    //TEMP hardcoded ECHO message type
    _serFramePrefixCache.set_type(proto::FramePrefix::ECHO);

    std::string output;
    auto result = _serFramePrefixCache.SerializeToString(&output);
    if (!result) {
        //TODO get error/debug string from MessageLite
        throw std::runtime_error{"Failed to serialize frame prefix"};
    }
    result = message.AppendToString(&output);
    if (!result) {
        //TODO get error/debug string from MessageLite
        throw std::runtime_error{"Failed to serialize chat message"};
    }
    assert(output.size() == _PREFIX_SIZE + _serFramePrefixCache.size()); // framePrefix.type must be set

    return output;
}

//NOTE uses _deserFramePrefixCache value assigned in getMessageSize()
Protocol::MessagePtr Protocol::deserialize(const void *data, int size) const {
    //TEMP hardcoded ECHO message type
    assert(_deserFramePrefixCache.type() == proto::FramePrefix::ECHO);
    auto message = std::make_unique<proto::ChatPacket>();

    if (!message->ParseFromArray(data, size)) {
        //util::Logger::instance().logError() << "ERR: Failed to parse chat message" << std::endl;
        throw std::runtime_error{"Wrong chat message format. Parse failed"};
    }
    return message;
}

std::size_t Protocol::getMessageSize(const void *prefixData, int size) const {
    //TODO cache prefix frame message ?
    _deserFramePrefixCache.Clear();
    if (!_deserFramePrefixCache.ParseFromArray(prefixData, size)) {
        //util::Logger::instance().logError() << "ERR: Failed to parse frame prefix" << std::endl;
        //return {};
        throw std::runtime_error{"Wrong frame prefix format. Parse failed"};
    }
    
    return _deserFramePrefixCache.size();
}

std::size_t Protocol::_calculatePrefixSize() const {
    _serFramePrefixCache.set_size(1);
    _serFramePrefixCache.set_type(1);
    assert(_serFramePrefixCache.IsInitialized());
    auto prefixSize = _serFramePrefixCache.ByteSizeLong();
    _serFramePrefixCache.Clear();
    return prefixSize;
}

} // namespace acs::proto

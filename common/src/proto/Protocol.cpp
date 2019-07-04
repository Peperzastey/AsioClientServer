#include "acs/proto/Protocol.hpp"
#include "acs/util/Logger.hpp"
#include "framing.pb.h"

namespace {

std::size_t calculate_prefix_size() {
    acs::proto::FramePrefix prefixDummy{};
    prefixDummy.set_size(1);
    prefixDummy.set_type(1);
    return prefixDummy.ByteSizeLong();
}

} // unnamed namespace

namespace acs::proto {

Protocol::Protocol(FramingProtocolType framingType)
    : _PREFIX_SIZE(calculate_prefix_size()) {}

std::string Protocol::serialize(const MessageType &message/*, enum MessageType type*/) const {
    proto::FramePrefix framePrefix{};
    framePrefix.set_size(message.ByteSizeLong());
    //TODO framePrefix.set_type() according to MessageType dynamic type !
    // message.GetTypeName() <- returns string -> so unordered_map<string, FramePrefix::MessageType> ? 
    const auto PREFIX_SIZE = framePrefix.ByteSizeLong();

    std::string output;
    auto result = framePrefix.SerializeToString(&output);
    if (!result) {
        util::Logger::instance().logError() << "ERR: Failed to serialize frame prefix" << std::endl;
        //TODO throw exception
        return;
    }
    result = message.AppendToString(&output);
    if (!result) {
        util::Logger::instance().logError() << "ERR: Failed to serialize chat message" << std::endl;
        //TODO throw exception
        return;
    }
    assert(output.size() == PREFIX_SIZE + framePrefix.size()); // framePrefix.type must be set

    return output;
}

/*std::unique_ptr<Protocol::MessageType> Protocol::deserialize(const void *data, int size) const {
    //TODO
}*/

std::size_t Protocol::getMessageSize(const void *prefixData, int size) {
    proto::FramePrefix prefix{};
    if (!prefix.ParseFromArray(prefixData, size)) {
        util::Logger::instance().logError() << "ERR: Failed to parse frame prefix" << std::endl;
        return; //TODO throw exception
    }
    
    return prefix.size();
}

} // namespace acs::proto

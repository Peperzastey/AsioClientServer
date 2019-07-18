#include "acs/message/EchoMessage.hpp"
#include "acs/context/ContextResolver.hpp"
#include "acs/context/ConnectionContext.hpp"
#include "acs/conn/TcpConnection.hpp"
#include "framing.pb.h"

namespace acs::message {

void EchoMessage::_doHandle(ContextId contextId [[maybe_unused]]) {
    util::Logger::instance().log() // logDebug
        << "DBG: handle Echo receive server" << std::endl;

    assert(_packet.type() == proto::EchoPacket::ECHO_REQUEST);

    // handler logic
    _packet.set_type(proto::EchoPacket::ECHO_RESPONSE);

    auto& connContext = context::ContextResolver<context::ConnectionContext>::instance().getContextInstance(contextId);
    connContext.connection.send(_packet, proto::FramePrefix::ECHO);
    //auto wireMessage = sender.getProtocol().serialize(message);
    //sender.send(std::move(wireMessage));
}

} // namespace acs::message

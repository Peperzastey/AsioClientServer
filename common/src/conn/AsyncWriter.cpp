#include "acs/conn/AsyncWriter.hpp"
#include "acs/util/Logger.hpp"
#include <asio/write.hpp>
#include <asio/buffer.hpp>
#include <utility>

namespace acs::conn {
/*
template <typename AsyncWriteStream>
void AsyncWriter<AsyncWriteStream>::_handleWrite(const std::error_code &error, std::size_t bytesTransferred) {
    _writeInProgress = false;
    if (error == asio::error::eof) {
        //TODO invoke callback passed from caller
        util::Logger::instance().log()  // logDebug()
            << "DBG: Connection closed cleanly by peer during write" << std::endl;
        return;
    } else if (error)
        throw std::system_error(error);
    
    //_callback->asyncWriteComplete();
}

template <typename AsyncWriteStream>
void AsyncWriter<AsyncWriteStream>::_doWrite() {
    if (_writeInProgress) {
        //TODO execute policy / according to policy
        util::Logger::instance().logError()
            << "ERR: Write operation initialized when another is in progress" << std::endl;
        return;
    }

    _writeInProgress = true;
    asio::async_write(_stream, asio::buffer(_writeBuffer), [this](auto&&... params) {
        _handleWrite(std::forward<decltype(params)>(params)...);
    });
}
*/
} // namespace acs::conn

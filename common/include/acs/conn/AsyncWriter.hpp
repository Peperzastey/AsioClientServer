#ifndef ACS_ASYNC_WRITER_HPP__
#define ACS_ASYNC_WRITER_HPP__

#include "acs/util/Logger.hpp"
#include <asio/write.hpp>
#include <asio/buffer.hpp>
#include <utility>

namespace acs::conn {

/// Performs asio::async_write compound operation on the given AsyncWriteStream.
/**
 * \warning Not Thread-Safe.
 * \todo pimpl
 */
template <typename AsyncWriteStream>
class AsyncWriter /*: public Writer*/ {
public:
    using Stream = AsyncWriteStream;

public:
    explicit AsyncWriter(Stream &stream)
        : _stream(stream) {}
    AsyncWriter(const AsyncWriter&) = delete;

    // requires copying of the buffer
    //void write(const void *data, std::size_t sizeInBytes);
    // TODO?
    //void write(std::unique_ptr<unsigned char[]> &&data);
    void write(std::string &&data/*, TODO WritePolicy policy = QueueWrite*/) {
        if (_tryLockWriting(/*policy*/)) {
            _writeBuffer = std::move(data);
            _doWrite();
        }
    }
    void write(const std::string &data/*, TODO WritePolicy policy = QueueWrite*/) {
        if (_tryLockWriting(/*policy*/)) {
            _writeBuffer = data;
            _doWrite();
        }
    }

protected:
    //TODO move implementation to header file? (visibility for subclasses?)
    void _handleWriteFinished(const std::error_code &error, std::size_t bytesTransferred) {
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

private:
    bool _tryLockWriting(/*WritePolicy*/) {
        if (_writeInProgress) {
            //TODO execute policy / according to policy
            util::Logger::instance().logError()
                << "ERR: Write operation initialized when another is in progress" << std::endl;
            return false;
        }

        _writeInProgress = true;
        return true;
    }

    void _doWrite() {
        asio::async_write(_stream, asio::buffer(_writeBuffer), [this](auto&&... params) {
            _handleWriteFinished(std::forward<decltype(params)>(params)...);
        });
    }

private:
    Stream &_stream;
    mutable std::string _writeBuffer;
    mutable /*atomic*/bool _writeInProgress = false;
};

} // namespace acs::conn

#endif // ACS_ASYNC_WRITER_HPP__

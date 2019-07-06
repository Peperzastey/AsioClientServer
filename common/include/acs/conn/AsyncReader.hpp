#ifndef ACS_ASYNC_READER_HPP__
#define ACS_ASYNC_READER_HPP__

#include "acs/util/Logger.hpp"
#include "acs/proto/Protocol.hpp"
#include <asio/read_until.hpp>
#include <asio/buffer.hpp>
#include <functional>
#include <cassert>
#include <stdexcept>

namespace acs::conn {

//TODO
struct AsyncReadCallbacks {
    //readCompleted
    //readError - e.g. EOF
};

/// Performs asio::async_read compound operations on the given AsyncReadStream.
/**
 * \warning Not Thread-Safe.
 * \todo pimpl
 */
template <typename AsyncReadStream>
class AsyncReader /*: public Reader*/ {
public:
    using Stream = AsyncReadStream;
    using Callback = std::function<void(std::string)>;

public:
    static constexpr std::size_t MAX_READ_BUFFER_SIZE_BYTES = 1024;

// forward declaration
private:
    //inline bool _tryLockReading(/*ReadPolicy*/);
    inline void _unlockReading();
    bool _checkError(const std::error_code &error);
    inline void _doReadUntil(char delim, bool discardDelim);
    inline void _doReadUntilInfinitelyNoUnlock(char delim, bool discardDelim);
    inline void _doReadProtoFramePrefix(bool repeatReadProtoInf = false);
    inline void _doReadProtoMessage(std::size_t messageSize, bool repeatReadProto = false);

public:
    explicit AsyncReader(Stream &stream, std::size_t maxReadBufferSizeBytes = MAX_READ_BUFFER_SIZE_BYTES)
        : _stream(stream), _maxBufferSize(maxReadBufferSizeBytes) {}
    AsyncReader(const AsyncReader&) = delete;

    //TODO accept different new line sequences
    //TODO move setting buffer max size to one common place
    /// Read async-ly until \a delim.
    void readAsyncUntilDelim(Callback&& callback, char delim = '\n', bool discardDelim = true/*, TODO ReadPolicy policy = QueueRead*/) {
        if (_tryLockReading(/*policy*/)) {
            _callback = std::move(callback);
            _doReadUntil(delim, discardDelim);
        }
    }
    /// Read async-ly in an infinite loop.
    /**
     * Reads infinitely (until \a delim in one go).
     * Loop exits when an error occurs - e.g. EOF.
     */
    //TODO enable_if<bool_constant> with true_type, false_type ? <-- occupy param
    // or another method for occupy-type infinite loop
    void readAsyncUntilDelimInf(Callback callback, char delim = '\n', bool discardDelim = true/*, TODO ReadPolicy policy = QueueRead*//*, bool occupy = false*/) {
        // must remember args - esp. callback (not as a fields because those will be reassigned)!
        throw std::logic_error{"Function not yet implemented"};
    }

    /// Read async-ly in an infinite loop. Occupies this AsyncReader object (and its stream's read endpoint).
    /**
     * Reads infinitely (until \a delim in one go).
     * Loop exits when an error occurs - e.g. EOF.
     */
    void readAsyncUntilDelimInfOccupy(Callback&& callback, char delim = '\n', bool discardDelim = true/*, TODO ReadPolicy policy = QueueRead*/) {
        if (_tryLockReading(/*policy*/)) {
            _callback = std::move(callback);
            _doReadUntilInfinitelyNoUnlock(delim, discardDelim);
        }
    }

    //TODO MessageCallback ? - void(proto::Protocol::MesssagePtr) - call deserialize in AsyncReader ?
    void readAsyncProtoInfOccupy(Callback&& callback, const proto::Protocol *protocol) {
        if (_tryLockReading(/*policy*/)) {
            _callback = std::move(callback);
            _protocol = protocol;
            _doReadProtoFramePrefix(true);
        }
    }

protected:
//public:
    //TODO bytesTransferred instead of string::find ?
    void _handleReadUntilFinished(char delim, bool discardDelim, const std::error_code &error, std::size_t bytesTransferred) {
        // unlockReading() before an exception is thrown (in _checkError)
        // and after buffer is reinitailized (here)
        if (!_checkError(error)) {
            //TODO move to 1 common method
            // async_read_until may have read excessive data
            auto delimPos = _readBuffer.find(delim);
            assert(bytesTransferred == delimPos+1);
            auto requestedData = _readBuffer.substr(0, ((discardDelim) ? delimPos : delimPos+1));

            // reinitialize buffer (leaving any data after the delim for the next read op invocation)
            _readBuffer.erase(0, delimPos+1);

            _unlockReading();
            // may throw
            _callback(std::move(requestedData));
        }
    }

    void _handleReadUntilInfNoUnlockLoopstep(char delim, bool discardDelim, const std::error_code &error, std::size_t bytesTransferred) {
        // do not call _unlockReading() unless an error occurred
        // _checkError calls _unlockReading() on error   
        if (!_checkError(error)) {
            // async_read_until may have read excessive data
            auto delimPos = _readBuffer.find(delim);
            assert(bytesTransferred == delimPos+1);
            auto requestedData = _readBuffer.substr(0, ((discardDelim) ? delimPos : delimPos+1));
            //requestedData.shrink_to_fit();

            // reinitialize buffer (leaving any data after the delim for the next read op invocation)
            _readBuffer.erase(0, delimPos+1);

            // schedule next loop step
            _doReadUntilInfinitelyNoUnlock(delim, discardDelim);

            // may throw
            _callback(std::move(requestedData));
        }
    }

    void _handleReadProtoFramePrefixFinished(bool repeatReadProto, const std::error_code &error, std::size_t bytesTransferred) {
        if (!_checkError(error)) {
            // assert that the data counted as transferred do not include any excessive data and do include
            // any previously excessive data (from a previous async_read_until operation)
            assert(bytesTransferred == _protocol->getFramePrefixSize());
            auto messageSize = _protocol->getMessageSize(_readBuffer.data(), bytesTransferred);

            // Reinitialize buffer (leaving any data after the delim for the next read op invocation)
            // A previous async_read_until may have left more excessive data in the buffer than we need
            // so need to preserve the now-excessive data
            _readBuffer.erase(0, bytesTransferred);

            // read message
            _doReadProtoMessage(messageSize, repeatReadProto);
        }
    }

    void _handleReadProtoMessageFinished(/*std::size_t messageSize, */bool repeatReadProto, const std::error_code &error, std::size_t bytesTransferred) {
        if (!_checkError(error)) {
            auto requestedData = _readBuffer.substr(0, bytesTransferred);

            // Reinitialize buffer (leaving any data after the delim for the next read op invocation)
            // A previous async_read_until may have left more excessive data in the buffer than we need
            // so need to preserve the now-excessive data
            _readBuffer.erase(0, bytesTransferred);

            // read next message (starting with its prefix)
            if (repeatReadProto)
                // schedule next message read
                _doReadProtoFramePrefix(true);
            else {
                _unlockReading();
                _protocol = nullptr;
            }

            // may throw
            _callback(std::move(requestedData));
            
            if (!repeatReadProto)
                _callback = nullptr;
        }
    }

private:
    template <typename... Args>
    //using _HandlerMethod = std::function<void(Args...)>;
    using _HandlerMethod = std::function<void(AsyncReader*, Args..., const std::error_code&, std::size_t)>;
    // _HandlerMethod<Args...>&& handler <-- cannot deduct std::function type parameters from pointer to (member) function

    // handler methods are protected
    // not necessary
    /*template <typename... Args>
    friend _HandlerMethod<Args...>;*/

    template <typename... Args>
    auto _makeHandler(_HandlerMethod<char, bool> &&handler, Args&&... args) {
        return [this, handler = std::move(handler), args.../*copies*/](auto&&... asioHandlerParams) { // const std::error_code &error, std::size_t bytesTransferred
            handler(this, /*std::forward<Args>(args)...*/ std::move(args)..., std::forward<decltype(asioHandlerParams)>(asioHandlerParams)...);
        };
    }
    inline bool _tryLockReading(/*ReadPolicy*/);
private:
    Stream &_stream;
    std::size_t _maxBufferSize;
    // or std::reference_wrapper ?
    //std::function<Callback> _callback;
    Callback _callback;
    const proto::Protocol *_protocol = nullptr;
    mutable std::string _readBuffer;
    mutable /*atomic*/bool _readInProgress = false;
};


template <typename AsyncReadStream>
bool AsyncReader<AsyncReadStream>::_tryLockReading(/*ReadPolicy*/) {
    if (_readInProgress) {
        //TODO execute policy / according to policy
        util::Logger::instance().logError()
            << "ERR: Read operation initialized when another is in progress" << std::endl;
        return false;
    }

    _readInProgress = true;
    return true;
}

template <typename AsyncReadStream>
void AsyncReader<AsyncReadStream>::_unlockReading() { // inline
    //TODO execute ReadPolicy.unlocked(this)
    _readInProgress = false;
}

/// Check if an IO error occurred and handle it if so.
/**
 * Calls callbacks handling appropriate errors.
 * Throws exception if no such callback exists.
 * \post Reading is unlocked if an error occurred.
 * 
 * \param error \c std::error_code reference to check and handle
 * \return \c true if an error occured. \c false otherwise
 * \throw std::system_error if \a error is not handled by a callback
 */
template <typename AsyncReadStream>
bool AsyncReader<AsyncReadStream>::_checkError(const std::error_code &error) {
    if (error == asio::error::eof) {
        util::Logger::instance().log()  // logDebug()
            << "DBG: EOF encountered" << std::endl;
        //TODO invoke callback passed from caller
        //THEN return true;
    } else if (error == asio::error::no_buffer_space) {
        util::Logger::instance().logError()
            << "ERR: Buffer max size reached when reading" << std::endl;
    }
    if (error) {
        _unlockReading();
        throw std::system_error(error);
    }

    return false;
}

template <typename AsyncReadStream>
void AsyncReader<AsyncReadStream>::_doReadUntil(char delim, bool discardDelim) {
    asio::async_read_until(_stream, asio::dynamic_buffer(_readBuffer, _maxBufferSize), delim,
        _makeHandler(&AsyncReader::_handleReadUntilFinished, delim, discardDelim)
    );
}

/*void _doReadUntil(char delim, bool discardDelim) {
    asio::async_read_until(_stream, asio::dynamic_buffer(_readBuffer, _maxBufferSize), delim, [this, delim, discardDelim](auto&&... params) {
        _handleReadUntilDelimFinished(delim, discardDelim, std::forward<decltype(params)>(params)...);
    });
}*/

/**
 * Unlocks reading only on IO error.
 * \pre Callback, read buffer and stream's reading endpoint are exclusively occupied by a call to this method.
 */
template <typename AsyncReadStream>
void AsyncReader<AsyncReadStream>::_doReadUntilInfinitelyNoUnlock(char delim, bool discardDelim) {
    asio::async_read_until(_stream, asio::dynamic_buffer(_readBuffer, _maxBufferSize), delim,
        //TODO reuse this handler in next loop step invocation
        _makeHandler(&AsyncReader::_handleReadUntilInfNoUnlockLoopstep, delim, discardDelim)
    );
}
//TODO 1 method calling async_read_until(delim)

/**
 * Unlocks reading only on IO error.
 * \pre Callback, read buffer and stream's reading endpoint are exclusively occupied by a call to this method.
 * \pre _protocol is set to point to a Protocol object.
 */
template <typename AsyncReadStream>
void AsyncReader<AsyncReadStream>::_doReadProtoFramePrefix(bool repeatReadProtoInf) {
    assert(_protocol != nullptr);
    const auto framePrefixSize = _protocol->getFramePrefixSize();
    // pre-C++20 capacity check before reserve to prevent shrinking
    if (_readBuffer.capacity() < framePrefixSize)
        _readBuffer.reserve(framePrefixSize);
    // read the protocol's frame prefix
    asio::async_read(_stream, asio::dynamic_buffer(_readBuffer, framePrefixSize), [this, repeatReadProtoInf](auto&&... params) {
        _handleReadProtoFramePrefixFinished(repeatReadProtoInf, std::forward<decltype(params)>(params)...);
    });
}

/**
 * Unlocks reading only on IO error.
 * \pre Callback, read buffer and stream's reading endpoint are exclusively occupied by a call to this method.
 * \pre _protocol is set to point to a Protocol object.
 */
template <typename AsyncReadStream>
void AsyncReader<AsyncReadStream>::_doReadProtoMessage(std::size_t messageSize, bool repeatReadProto) {
    assert(_protocol != nullptr);
    // pre-C++20 capacity check before reserve to prevent shrinking
    if (_readBuffer.capacity() < messageSize)
        _readBuffer.reserve(messageSize);
    // read the message
    asio::async_read(_stream, asio::dynamic_buffer(_readBuffer, messageSize), [this/*, messageSize*/, repeatReadProto](auto&&... params) {
        _handleReadProtoMessageFinished(/*messageSize, */repeatReadProto, std::forward<decltype(params)>(params)...);
    });
}


} // namespace acs::conn

#endif // ACS_ASYNC_READER_HPP__

#ifndef ACS_ASYNC_READER_HPP__
#define ACS_ASYNC_READER_HPP__

#include "acs/util/Logger.hpp"
#include <asio/read_until.hpp>
#include <asio/buffer.hpp>
#include <functional>
#include <cassert>

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
    //using Callback = void(std::string/*, error*/); // or: void(*)(std::stream/*, error*/) ?
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
            //requestedData.shrink_to_fit();

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

} // namespace acs::conn

#endif // ACS_ASYNC_READER_HPP__

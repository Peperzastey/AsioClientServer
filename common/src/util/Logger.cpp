#include "acs/util/Logger.hpp"
#include <fstream>

namespace acs::util {

namespace {

/// Null-stream facility.
/**
 * \return std::ostream with \c /dev/null sink.
 * \note Unix-specific implementation.
 */
inline std::ostream& nullStream() {
    static std::ofstream stream;
    if (!stream.is_open())
        stream.open("/dev/null", std::ios_base::out | std::ios_base::app);
    return stream;
}

/// Null logger.
inline Logger& nullLogger() {
    static auto &nullStr = nullStream();
    static Logger nullLog(nullStr, nullStr);
    return nullLog;
}

} // unnamed namespace

Logger::outstream_t &Logger::nullSink = nullStream();
Logger *Logger::_instance = &(nullLogger());

Logger::Logger(outstream_t &out, outstream_t &errorOut) noexcept
    : _out(out), _errorOut(errorOut) {}

void Logger::registerInstance(Logger &loggerInstance) noexcept {
    _instance = &loggerInstance;
}

Logger& Logger::instance() noexcept {
    return *_instance;
}

} // namespace acs::util
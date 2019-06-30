#ifndef ACS_LOGGER_HPP__
#define ACS_LOGGER_HPP__

#include <iostream>

/// Contains utility functions.
namespace acs::util {

/// Simple logging facility.
class Logger {
public:
    using outstream_t = std::ostream;

public:
    explicit Logger(outstream_t &out, outstream_t &errorOut) noexcept;

    static void registerInstance(Logger &loggerInstance) noexcept;
    //TODO smart pointer _instance needed
    //template <typename... Args>
    //static void create(Args... args);
    static Logger& instance() noexcept;

    inline outstream_t& log() noexcept;
    inline void log(const std::string &message, bool breakLine = true, bool flush = true);

    inline outstream_t& logError() noexcept;
    inline void logError(const std::string &message, bool breakLine = true, bool flush = true);

private:
    outstream_t &_out;
    outstream_t &_errorOut;
    static Logger *_instance;
};

namespace {

inline void printLog(std::ostream &stream, const std::string &message, bool breakLine, bool flush) {
    stream << message;
    if (breakLine) stream << '\n';
    if (flush) stream << std::flush; 
}

} // unnamed namespace

Logger::outstream_t& Logger::log() noexcept {
    return _out;
}

void Logger::log(const std::string &message, bool breakLine, bool flush) {
    printLog(_out, message, breakLine, flush);
}

Logger::outstream_t& Logger::logError() noexcept {
    return _errorOut;
}

void Logger::logError(const std::string &message, bool breakLine, bool flush) {
    printLog(_errorOut, message, breakLine, flush);
}


// free functions

inline void log(const std::string &message, std::ostream &out = std::cout) {
    out << message << std::endl;
}

/**
 * \todo forward + decltype(auto)/auto return type ?
 */
inline std::ostream& log(std::ostream &out = std::cout) noexcept {
    return out;
}

/**
 * \todo remove it
 */
template< class CharT, class Traits >
inline std::basic_ostream<CharT, Traits>& endl( std::basic_ostream<CharT, Traits>& os ) {
    return std::endl(os);
}

} // namespace acs::util

#endif // ACS_LOGGER_HPP__
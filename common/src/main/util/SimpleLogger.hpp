#ifndef SIMPLELOGGER_HPP__
#define SIMPLELOGGER_HPP__

#include <iostream>

/// Contains common utility functions.
namespace cs::common::util {

namespace {

// hidden class
/*class Logger : public std::ostream {
public:
    template <typename T>
    Logger& operator<<(T&& message) {
        _outStream << message;
        return *this;
    }

    Logger(std::ostream &out) : _outStream(out) {}

private:
    std::ostream &_outStream;
};*/

} // namespace

inline void log(const std::string &message, std::ostream &out = std::cout) {
    out << message << std::endl;
}

inline std::ostream& log(std::ostream &out = std::cout) {
    return out;
}

/*inline Logger log(std::ostream &out = std::cout) {
    return Logger(out);
}*/

} // namespace cs::common::util

#endif // SIMPLELOGGER_HPP__
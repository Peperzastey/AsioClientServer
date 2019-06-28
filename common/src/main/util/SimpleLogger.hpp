#ifndef SIMPLELOGGER_HPP__
#define SIMPLELOGGER_HPP__

#include <iostream>

/// Contains common utility functions.
namespace cs::common::util {

inline void log(const std::string &message, std::ostream &out = std::cout) {
    out << message << std::endl;
}

/**
 * \todo forward + decltype(auto)/auto return type ?
 */
inline std::ostream& log(std::ostream &out = std::cout) noexcept {
    return out;
}

} // namespace cs::common::util

#endif // SIMPLELOGGER_HPP__
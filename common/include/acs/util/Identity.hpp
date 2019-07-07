#ifndef ACS_IDENTITY_HPP__
#define ACS_IDENTITY_HPP__

namespace acs::util {

template <typename T>
struct Identity { using type = T; };

} // namespace acs::util

#endif // ACS_IDENTITY_HPP__
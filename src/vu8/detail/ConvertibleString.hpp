#ifndef TSA_VU8_DETAIL_CONVERTIBLE_STRING_HPP
#define TSA_VU8_DETAIL_CONVERTIBLE_STRING_HPP

#include <string>

namespace vu8 { namespace detail {

/// A string that converts to char const * (useful for fusion::invoke)
struct ConvertibleString : std::string {
    operator char const *() { return c_str(); }
    ConvertibleString(char const *str) : std::string(str) {}
    ConvertibleString() {}
};

} }
#endif

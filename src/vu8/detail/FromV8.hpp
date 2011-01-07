#ifndef TSA_VU8_DETAIL_FROM_V8_HPP
#define TSA_VU8_DETAIL_FROM_V8_HPP

#include <vu8/detail/ConvertibleString.hpp>

#include <v8.h>

#include <string>
#include <stdexcept>

namespace vu8 { namespace detail {

typedef v8::Handle<v8::Value> ValueHandle;

template <class T>
struct FromV8;

template <>
struct FromV8<std::string> {
    static inline std::string exec(ValueHandle value) {
        if (! value->IsString())
            throw std::runtime_error("cannot make string from non-string type");

        v8::String::Utf8Value str(value);
        return *str;
    }
};

template <>
struct FromV8<ConvertibleString> {
    static inline ConvertibleString exec(ValueHandle value) {
        if (! value->IsString())
            throw std::runtime_error("cannot make string from non-string type");

        v8::String::Utf8Value str(value);
        return *str;
    }
};

} }
#endif

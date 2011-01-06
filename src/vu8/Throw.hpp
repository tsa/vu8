#ifndef TSA_VU8_THROW_HPP
#define TSA_VU8_THROW_HPP

#include <v8.h>

namespace vu8 {

static inline v8::Handle<v8::Primitive> Throw(char const *str) {
    v8::ThrowException(v8::String::New(str));
    return v8::Undefined();
}

}
#endif

#ifndef TSA_VU8_THROW_HPP
#define TSA_VU8_THROW_HPP

#include <v8.h>

namespace tsa { namespace vu8 {

static inline v8::Handle<v8::Primitive> Throw(char const *str) {
    v8::ThrowException(v8::String::New("incorrect arguments to loadmodule"));
    return v8::Undefined();
}

} }
#endif

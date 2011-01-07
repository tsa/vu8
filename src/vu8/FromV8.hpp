#ifndef TSA_VU8_FROM_V8_HPP
#define TSA_VU8_FROM_V8_HPP

#include <v8.h>

#include <vu8/detail/FromV8.hpp>

namespace vu8 {

using detail::ValueHandle;

template <class T>
static inline T FromV8(ValueHandle src) {
    return detail::FromV8<T>::exec(src);
}

}
#endif

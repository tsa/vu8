#ifndef TSA_VU8_STRING_HPP
#define TSA_VU8_STRING_HPP

#include <v8.h>

namespace tsa { namespace vu8 {

static inline const char* ToCString(const v8::String::Utf8Value& value) {
  return *value ? *value : "";
}

} }
#endif

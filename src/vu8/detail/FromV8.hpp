#ifndef TSA_VU8_DETAIL_FROM_V8_HPP
#define TSA_VU8_DETAIL_FROM_V8_HPP

#include <vu8/detail/ConvertibleString.hpp>

#include <v8.h>

#include <string>
#include <stdexcept>
#include <vector>

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

template <>
struct FromV8< v8::Handle<v8::Function> > {
    static inline v8::Handle<v8::Function> exec(ValueHandle value) {
        if (! value->IsFunction())
            throw std::runtime_error("expected javascript function");

        return value.As<v8::Function>();
    }
};

template <>
struct FromV8<int32_t> {
    static inline int32_t exec(ValueHandle value) {
        if (! value->IsNumber())
            throw std::runtime_error("expected javascript number");

        return value->ToInt32()->Value();
    }
};

template <>
struct FromV8<uint32_t> {
    static inline uint32_t exec(ValueHandle value) {
        if (! value->IsNumber())
            throw std::runtime_error("expected javascript number");

        return value->ToUint32()->Value();
    }
};

template <>
struct FromV8<int64_t> {
    static inline int64_t exec(ValueHandle value) {
        if (! value->IsNumber())
            throw std::runtime_error("expected javascript number");

        return value->ToNumber()->Value();
    }
};

template <>
struct FromV8<uint64_t> {
    static inline uint64_t exec(ValueHandle value) {
        if (! value->IsNumber())
            throw std::runtime_error("expected javascript number");

        return value->ToNumber()->Value();
    }
};


template <class T, class A>
struct FromV8< std::vector<T, A> > {
    static inline std::vector<T, A> exec(ValueHandle value) {
        if (! value->IsArray())
            throw std::runtime_error("expected javascript array");

        v8::Array *array = v8::Array::Cast(*value);
        std::vector<T, A> result;
        for (uint32_t i = 0; i < array->Length(); ++i) {
            v8::Local<v8::Value> obj = array->Get(i);
            result.push_back(FromV8<T>::exec(obj));
        }
        return result;
    }
};

template <>
struct FromV8<ValueHandle> {
    static inline ValueHandle exec(ValueHandle value) {
        return value;
    }
};

} }
#endif

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
struct FromV8Base {
    typedef T result_type;
};

template <class T>
struct FromV8;

template <>
struct FromV8<std::string> : FromV8Base<std::string> {
    static inline std::string exec(ValueHandle value) {
        if (! value->IsString())
            throw std::runtime_error("cannot make string from non-string type");

        v8::String::Utf8Value str(value);
        return *str;
    }
};

template <>
struct FromV8<ConvertibleString> : FromV8Base<ConvertibleString> {
    static inline ConvertibleString exec(ValueHandle value) {
        if (! value->IsString())
            throw std::runtime_error("cannot make string from non-string type");

        v8::String::Utf8Value str(value);
        return *str;
    }
};

// char const * and char const * have to be copied immediately otherwise
// the underlying memory will die due to the way v8 strings work.
template <>
struct FromV8<char const *> : FromV8<ConvertibleString> {};

template <>
struct FromV8<char const * const> : FromV8<ConvertibleString> {};

template <>
struct FromV8< v8::Handle<v8::Function> >
  : FromV8Base< v8::Handle<v8::Function> >
{
    static inline v8::Handle<v8::Function> exec(ValueHandle value) {
        if (! value->IsFunction())
            throw std::runtime_error("expected javascript function");

        return value.As<v8::Function>();
    }
};

template <>
struct FromV8<bool> : FromV8Base<bool> {
    static inline bool exec(ValueHandle value) {
        return value->ToBoolean()->Value();
    }
};

template <>
struct FromV8<int32_t> : FromV8Base<int32_t> {
    static inline int32_t exec(ValueHandle value) {
        if (! value->IsNumber())
            throw std::runtime_error("expected javascript number");

        return value->ToInt32()->Value();
    }
};

template <>
struct FromV8<uint32_t> : FromV8Base<uint32_t> {
    static inline uint32_t exec(ValueHandle value) {
        if (! value->IsNumber())
            throw std::runtime_error("expected javascript number");

        return value->ToUint32()->Value();
    }
};

template <>
struct FromV8<int64_t> : FromV8Base<int64_t> {
    static inline int64_t exec(ValueHandle value) {
        if (! value->IsNumber())
            throw std::runtime_error("expected javascript number");

        return static_cast<int64_t>(value->ToNumber()->Value());
    }
};

template <>
struct FromV8<uint64_t> : FromV8Base<uint64_t> {
    static inline uint64_t exec(ValueHandle value) {
        if (! value->IsNumber())
            throw std::runtime_error("expected javascript number");

        return static_cast<int64_t>(value->ToNumber()->Value());
    }
};

template <>
struct FromV8<double> : FromV8Base<double> {
    static inline double exec(ValueHandle value) {
        if (! value->IsNumber())
            throw std::runtime_error("expected javascript number");

        return value->ToNumber()->Value();
    }
};


template <class T, class A>
struct FromV8< std::vector<T, A> > : FromV8Base< std::vector<T, A> > {
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
struct FromV8<ValueHandle> : FromV8Base<ValueHandle> {
    static inline ValueHandle exec(ValueHandle value) {
        return value;
    }
};

////////////////////////////////////////////////////////////////////////////
// extracting classes
template <class T>
struct FromV8Ptr : FromV8Base<T> {
    static inline T exec(ValueHandle value) {
        if (! value->IsObject())
            throw std::runtime_error("expected object");

        v8::Local<v8::Object> obj = value->ToObject();

        if (! obj->InternalFieldCount())
            throw std::runtime_error("expected c++ wrapped object");

        return static_cast<T>(obj->GetPointerFromInternalField(0));
    }
};

template <class T>
struct FromV8<T *> : FromV8Ptr<T *> {};

template <class T>
struct FromV8<T const *> : FromV8Ptr<T const *> {};

//////////////////////////////////////////////////////////////////////////////
// deal with references
template <class T, class U>
struct FromV8Ref {
    typedef U result_type;

    static inline U exec(ValueHandle value) {
        if (! value->IsObject())
            throw std::runtime_error("expected object");

        v8::Local<v8::Object> obj = value->ToObject();

        if (! obj->InternalFieldCount())
            throw std::runtime_error("expected c++ wrapped object");

        return *static_cast<T *>(obj->GetPointerFromInternalField(0));
    }
};

template <class U>
struct FromV8Ref<std::string, U> : FromV8<std::string> {};

template <class T, class U, class R>
struct FromV8Ref<std::vector<T, U>, R> : FromV8< std::vector<T, U> > {};

template <class T>
struct FromV8<T const &> : FromV8Ref<T, T const&> {};

template <class T>
struct FromV8<T&> : FromV8Ref<T, T&> {};

} }
#endif

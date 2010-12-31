#ifndef TSA_VU8_MODULE_HPP
#define TSA_VU8_MODULE_HPP

#include <v8.h>

namespace tsa { namespace vu8 {

template <class T, class P> struct Class;

struct Module {
    Module& operator()(char const            *name,
                       v8::InvocationCallback callback)
    {
        func_->PrototypeTemplate()->
            Set(v8::String::New(name), v8::FunctionTemplate::New(callback));
        return *this;
    }

    template <class T, class P>
    Module& operator()(char const *name, Class<T, P>& clss) {
        func_->InstanceTemplate()->
            Set(v8::String::New(name),
                v8::FunctionTemplate::New(
                    &Class<T, P>::singleton_t::ConstructorFunction));

        clss.FunctionTemplate()->SetClassName(v8::String::New(name));
        return *this;
    }

    // this is a local handle so make it persistent if needs be
    v8::Local<v8::Object> NewInstance() {
        return func_->GetFunction()->NewInstance();
    }

    Module() : func_(v8::FunctionTemplate::New()) {}
  private:
    v8::Local<v8::FunctionTemplate> func_;
};

} }
#endif

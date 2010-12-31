#ifndef TSA_VU8_CLASS_HPP
#define TSA_VU8_CLASS_HPP

#include <vu8/Singleton.hpp>
#include <vu8/String.hpp>

#include <v8.h>

#include <iostream>

namespace tsa { namespace vu8 {

template <class T>
struct ArgAllocator {
    static inline T *New(const v8::Arguments& args) { return new T(args); }
};

template <class T>
struct NoArgAllocator {
    static inline T *New(const v8::Arguments& args) { return new T(); }
};


template < class T, class Allocator>
struct ClassSingleton : Singleton< ClassSingleton<T, Allocator> > {
    typedef ClassSingleton<T, Allocator> self;
    typedef v8::Handle<v8::Value> (T::*MethodCallback)(const v8::Arguments& args);

    v8::Persistent<v8::FunctionTemplate>& FunctionTemplate() {
        return func_;
    }

  public:
    static inline v8::Handle<v8::Value>
    ConstructorFunction(const v8::Arguments& args) {
        return self::Instance().WrapObject(args);
    }

    template <v8::Handle<v8::Value> (T::*Ptr)(const v8::Arguments&)>
    static inline v8::Handle<v8::Value> Forward(const v8::Arguments& args) {
        return ForwardBase<T, Ptr>(args);
    }

    template <class P, v8::Handle<v8::Value> (P::*Ptr)(const v8::Arguments&)>
    static inline v8::Handle<v8::Value> ForwardBase(const v8::Arguments& args) {
        v8::HandleScope scope;
        v8::Local<v8::Object> self = args.Holder();
        v8::Local<v8::External> wrap =
            v8::Local<v8::External>::Cast(self->GetInternalField(0));

        P& obj = *static_cast<P *>(wrap->Value());
        return scope.Close((obj.*Ptr)(args));
    }


    static inline void MadeWeak(v8::Persistent<v8::Value> object,
                                void                     *parameter)
    {
        T *obj = static_cast<T *>(parameter);
        delete(obj);
        object.Dispose();
        object.Clear();
    }

    v8::Handle<v8::Object> WrapObject(const v8::Arguments& args) {
        v8::HandleScope scope;
        T *wrap = Allocator::New(args);
        v8::Local<v8::Object> localObj = func_->GetFunction()->NewInstance();
        v8::Persistent<v8::Object> obj =
            v8::Persistent<v8::Object>::New(localObj);

        obj->SetInternalField(0, v8::External::New(wrap));
        obj.MakeWeak(wrap, &self::MadeWeak);
        return scope.Close(localObj);
    }

    ClassSingleton()
      : func_(v8::Persistent<v8::FunctionTemplate>::New(
                  v8::FunctionTemplate::New()))
    {
        func_->InstanceTemplate()->SetInternalFieldCount(1);
    }

  private:
    v8::Persistent<v8::FunctionTemplate> func_;
    friend class Singleton<self>;
};

class Nothing {};

// basic class
// T = class
// P = optional parent class
// Allocator = static wrapper object memory allocator, by default
//             supports zero-argument constructor
template <class T, template <class> class Allocator = NoArgAllocator>
struct BasicClass {
    typedef ClassSingleton< T, Allocator<T> >  singleton_t;

  private:

    typedef typename singleton_t::MethodCallback  MethodCallback;

    inline singleton_t& Instance() { return singleton_t::Instance(); }

  public:
    inline v8::Persistent<v8::FunctionTemplate>& FunctionTemplate() {
        return Instance().FunctionTemplate();
    }

    template <v8::Handle<v8::Value> (T::*Ptr)(const v8::Arguments&)>
    inline BasicClass& Method(char const *name) {
        FunctionTemplate()->PrototypeTemplate()->Set(
            v8::String::New(name),
            v8::FunctionTemplate::New(&singleton_t::template Forward<Ptr>));
        return *this;
    }

    template <class U, template <class> class V>
    BasicClass(BasicClass<U, V>& parent) {
        FunctionTemplate()->Inherit(parent.FunctionTemplate());
    };
    BasicClass() {};
};

// class with constructor
// T = class
template <class T>
struct Class : BasicClass<T, ArgAllocator> {
    typedef BasicClass<T, ArgAllocator> base;

    template <class U, template <class> class V>
    Class(BasicClass<U, V>& parent) : base(parent) {}
    Class() {}
};

} }
#endif

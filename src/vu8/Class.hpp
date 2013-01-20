#ifndef TSA_VU8_CLASS_HPP
#define TSA_VU8_CLASS_HPP

#include <vu8/ToV8.hpp>
#include <vu8/FromV8.hpp>
#include <vu8/Throw.hpp>
#include <vu8/Factory.hpp>
#include <vu8/CallFromV8.hpp>
#include <vu8/detail/Proto.hpp>
#include <vu8/detail/Singleton.hpp>
#include <vu8/detail/Class.hpp>

#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/adapted/mpl.hpp>
#include <boost/fusion/include/push_front.hpp>
#include <boost/fusion/include/join.hpp>

#include <iostream>
#include <stdexcept>

namespace vu8 {

namespace fu = boost::fusion;
namespace mpl = boost::mpl;

template < class T, class Factory = Factory<> >
struct Class;

template <class T>
struct Singleton;

template <class M, class Factory>
class ClassSingletonFactory {

    M& mixin() { return static_cast<M &>(*this); }

    static inline ValueHandle ConstructorFunction(const v8::Arguments& args) {
        return M::Instance().WrapObject(args);
    }

  public:
    v8::Persistent<v8::FunctionTemplate>& JSFunctionTemplateHelper() {
        return mixin().jsFunc_;
    }

    enum { HAS_NULL_FACTORY = false };

    ClassSingletonFactory()
      : jsFunc_(v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New(
            &ConstructorFunction)))
    {}

    v8::Persistent<v8::FunctionTemplate> jsFunc_;
};

template <class M>
struct ClassSingletonFactory<M, NoFactory> {
    enum { HAS_NULL_FACTORY = true };

    v8::Persistent<v8::FunctionTemplate>& JSFunctionTemplateHelper() {
        return static_cast<M &>(*this).func_;
    }
};

template <class T, class Factory>
class ClassSingleton
  : detail::LazySingleton< ClassSingleton<T, Factory> >,
    public ClassSingletonFactory<ClassSingleton<T, Factory>, Factory>
{
    friend class ClassSingletonFactory<ClassSingleton<T, Factory>, Factory>;

    typedef ClassSingleton<T, Factory> self;
    typedef ValueHandle (T::*MethodCallback)(const v8::Arguments& args);

    v8::Persistent<v8::FunctionTemplate>& ClassFunctionTemplate() {
        return func_;
    }

    v8::Persistent<v8::FunctionTemplate>& JSFunctionTemplate() {
        return this->JSFunctionTemplateHelper();
    }

    // invoke passing javascript object argument directly
    template <class P>
    static inline typename boost::enable_if<
        detail::PassDirectIf<P>, typename P::return_type >::type
    Invoke(T *obj, const v8::Arguments& args) {
        return (obj->* P::method_pointer)(args);
    }

    template <class P>
    static inline typename boost::disable_if<
        detail::PassDirectIf<P>, typename P::return_type >::type
    Invoke(T *obj, const v8::Arguments& args) {
        return CallFromV8<P>(*obj, args);
    }

    template <class P>
    static inline typename boost::disable_if<
        boost::is_same<void, typename P::return_type>, ValueHandle >::type
    ForwardReturn(T *obj, const v8::Arguments& args) {
        return ToV8(Invoke<P>(obj, args));
    }

    template <class P>
    static inline typename boost::enable_if<
        boost::is_same<void, typename P::return_type>, ValueHandle >::type
    ForwardReturn(T *obj, const v8::Arguments& args) {
        Invoke<P>(obj, args);
        return v8::Undefined();
    }

    // every method is run inside a handle scope
    template <class P>
    static inline ValueHandle Forward(const v8::Arguments& args) {
        v8::HandleScope scope;

        // this will kill without zero-overhead exception handling
        try {
            return scope.Close(
                ForwardReturn<P>(
                    retrieveNativeObjectPtr(args.Holder()),
                    args
                )
            );
        }
        catch (std::runtime_error const& e) {
            return Throw(e.what());
        }
    }

    static inline T* retrieveNativeObjectPtr(v8::Local<v8::Value> value)
    {
        while (value->IsObject()) {
            v8::Local<v8::Object> obj = value->ToObject();
            T * native = static_cast<T *>(obj->GetPointerFromInternalField(0));
            if (native) {
                return native;
            }
            value = obj->GetPrototype();
        }
        return NULL;
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
        T *wrap = detail::ArgFactory<T, Factory>::New(args);
        v8::Local<v8::Object> localObj = func_->GetFunction()->NewInstance();
        v8::Persistent<v8::Object> obj =
            v8::Persistent<v8::Object>::New(localObj);

        obj->SetPointerInInternalField(0, wrap);
        obj.MakeWeak(wrap, &self::MadeWeak);
        return scope.Close(obj);
    }

    ClassSingleton()
      : func_(v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New()))
    {
        if (! this->HAS_NULL_FACTORY)
            func_->Inherit(JSFunctionTemplate());
        func_->InstanceTemplate()->SetInternalFieldCount(1);
    }

    v8::Persistent<v8::FunctionTemplate> func_;

    friend class detail::LazySingleton<self>;
    friend class Class<T, Factory>;
    friend class Singleton<T>;
};

// Interface for registering C++ classes with v8
// T = class
// Factory = factory for allocating c++ object
//           by default Class uses zero-argument constructor
template <class T, class Factory>
struct Class {
    typedef ClassSingleton<T, Factory>  singleton_t;

  private:
    typedef typename singleton_t::MethodCallback  MethodCallback;

    inline singleton_t& Instance() { return singleton_t::Instance(); }

    // method helper
    template <class P>
    inline Class& Method(char const *name) {
        ClassFunctionTemplate()->PrototypeTemplate()->Set(
            v8::String::New(name),
            v8::FunctionTemplate::New(&singleton_t::template Forward<P>));
        return *this;
    }

  public:
    // method with any prototype
    template <class P, typename detail::MemFunProto<T, P>::method_type Ptr>
    inline Class& Set(char const *name) {
        return Method< detail::MemFun<T, P, Ptr> >(name);
    }

    template <class P, typename detail::MemFunProto<T const, P>::method_type Ptr>
    inline Class& Set(char const *name) {
        return Method< detail::MemFun<T const, P, Ptr> >(name);
    }

    // passing v8::Arguments directly but modify return type
    template <class R, R (T::*Ptr)(const v8::Arguments&)>
    inline Class& Set(char const *name) {
        return Set<R(const v8::Arguments&), Ptr>(name);
    }

    // passing v8::Arguments and return ValueHandle directly
    template <ValueHandle (T::*Ptr)(const v8::Arguments&)>
    inline Class& Set(char const *name) {
        return Method<ValueHandle(const v8::Arguments&), Ptr>(name);
    }

    inline v8::Persistent<v8::FunctionTemplate>& ClassFunctionTemplate() {
        return Instance().ClassFunctionTemplate();
    }

    inline v8::Persistent<v8::FunctionTemplate>& JSFunctionTemplate() {
        return Instance().JSFunctionTemplate();
    }

    // create javascript object which references externally created C++
    // class. It will not take ownership of the C++ pointer.
    static inline v8::Handle<v8::Object> ReferenceExternal(T *ext) {
        v8::HandleScope scope;
        v8::Local<v8::Object> obj =
            singleton_t::Instance().func_->GetFunction()->NewInstance();
        obj->SetPointerInInternalField(0, ext);
        return scope.Close(obj);
    }

    // As ReferenceExternal but delete memory for C++ object when javascript
    // object is deleted. You must use "new" to allocate ext.
    static inline v8::Handle<v8::Object> ImportExternal(T *ext) {
        v8::HandleScope scope;
        v8::Local<v8::Object> localObj =
            singleton_t::Instance().func_->GetFunction()->NewInstance();

        v8::Persistent<v8::Object> obj =
            v8::Persistent<v8::Object>::New(localObj);

        obj->SetPointerInInternalField(0, ext);
        obj.MakeWeak(ext, &singleton_t::MadeWeak);

        return scope.Close(obj);
    }

    template <class U, class V>
    Class(Class<U, V>& parent) {
        JSFunctionTemplate()->Inherit(parent.ClassFunctionTemplate());
    }
    Class() {}

    friend class Singleton<T>;
};

// Wrap a C++ singleton
template <class T>
struct Singleton : Class<T, NoFactory> {
    typedef Class<T> base;

    template <class U, class V>
    Singleton(Class<U, V>& parent, T* instance) : instance_(instance) {
        base::JSFunctionTemplate()->Inherit(parent.ClassFunctionTemplate());
    }
    Singleton(T *instance) : instance_(instance) {}

    v8::Persistent<v8::Object> NewInstance() {
        v8::HandleScope scope;
        v8::Persistent<v8::Object> obj =
            v8::Persistent<v8::Object>::New(
                this->Instance().func_->GetFunction()->NewInstance());

        obj->SetPointerInInternalField(0, instance_);
        return obj;
    }

  private:
    T *instance_;
};

}
#endif

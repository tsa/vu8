#ifndef TSA_VU8_CLASS_HPP
#define TSA_VU8_CLASS_HPP

#include <vu8/ToV8.hpp>
#include <vu8/FromV8.hpp>
#include <vu8/Throw.hpp>
#include <vu8/Factory.hpp>
#include <vu8/detail/Proto.hpp>
#include <vu8/detail/Singleton.hpp>
#include <vu8/detail/MakeArgStorage.hpp>
#include <vu8/detail/FromV8Arguments.hpp>
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

template <class T, class Factory>
class ClassSingleton : detail::LazySingleton< ClassSingleton<T, Factory> > {

    typedef ClassSingleton<T, Factory> self;
    typedef ValueHandle (T::*MethodCallback)(const v8::Arguments& args);

    v8::Persistent<v8::FunctionTemplate>& FunctionTemplate() {
        return func_;
    }

  public:
    static inline ValueHandle ConstructorFunction(const v8::Arguments& args) {
        return self::Instance().WrapObject(args);
    }

  private:

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
        typedef typename
            detail::MakeArgStorage<typename P::arguments>::type arg_tl;

        typedef typename mpl::push_front<arg_tl, T *&>::type mem_arg_tl;

        typedef typename fu::result_of::as_vector<arg_tl>::type     arg_vec;
        typedef typename fu::result_of::as_vector<mem_arg_tl>::type mem_arg_vec;

        fu::vector<T *&> parent(obj);
        mem_arg_vec cpp_args = fu::join(parent, arg_vec());

        detail::FromV8Arguments<1>(cpp_args, args);

        return boost::fusion::invoke(P::method_pointer, cpp_args);
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
        v8::Local<v8::Object> self = args.Holder();
        v8::Local<v8::External> wrap =
            v8::Local<v8::External>::Cast(self->GetInternalField(0));

        // this will kill without zero-overhead exception handling
        try {
            return scope.Close(ForwardReturn<P>(
                static_cast<T *>(wrap->Value()), args));
        }
        catch (std::runtime_error const& e) {
            return Throw(e.what());
        }
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

    v8::Persistent<v8::FunctionTemplate> func_;

    friend class detail::LazySingleton<self>;
    friend class Class<T, Factory>;
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
        FunctionTemplate()->PrototypeTemplate()->Set(
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

    inline v8::Persistent<v8::FunctionTemplate>& FunctionTemplate() {
        return Instance().FunctionTemplate();
    }

    template <class U, class V>
    Class(Class<U, V>& parent) {
        FunctionTemplate()->Inherit(parent.FunctionTemplate());
    }
    Class() {}
};

}
#endif

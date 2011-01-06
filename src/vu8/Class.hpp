#ifndef TSA_VU8_CLASS_HPP
#define TSA_VU8_CLASS_HPP

#include <vu8/ToV8.hpp>
#include <vu8/Throw.hpp>
#include <vu8/detail/Proto.hpp>
#include <vu8/detail/Singleton.hpp>
#include <vu8/detail/MakeArgStorage.hpp>

#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/adapted/mpl.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/invoke.hpp>
#include <boost/fusion/include/begin.hpp>
#include <boost/fusion/include/push_front.hpp>
#include <boost/fusion/include/join.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/front.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <stdexcept>

namespace tsa { namespace vu8 {

namespace fu = boost::fusion;
namespace mpl = boost::mpl;

template <class T>
struct ArgAllocator {
    static inline T *New(const v8::Arguments& args) { return new T(args); }
};

template <class T>
struct NoArgAllocator {
    static inline T *New(const v8::Arguments& args) { return new T(); }
};

template <class T, template <class> class Allocator = NoArgAllocator>
struct BasicClass;

template <class T, template <class> class Allocator>
class ClassSingleton : detail::Singleton< ClassSingleton<T, Allocator> > {

    typedef ClassSingleton<T, Allocator> self;
    typedef ValueHandle (T::*MethodCallback)(const v8::Arguments& args);

    v8::Persistent<v8::FunctionTemplate>& FunctionTemplate() {
        return func_;
    }

  public:
    static inline ValueHandle ConstructorFunction(const v8::Arguments& args) {
        return self::Instance().WrapObject(args);
    }

  private:
    template <class P>
    struct PassDirectIf : boost::is_same<
        const v8::Arguments&,
        typename mpl::front<typename P::arguments>::type> {};

    // invoke passing javascript object argument directly
    template <class P>
    static inline typename boost::enable_if<
        PassDirectIf<P>, typename P::return_type >::type
    Invoke(T *obj, const v8::Arguments& args) {
        return (obj->* P::method_pointer)(args);
    }

    template <class P>
    static inline typename boost::disable_if<
        PassDirectIf<P>, typename P::return_type >::type
    Invoke(T *obj, const v8::Arguments& args) {
        typedef typename mpl::transform<
            typename P::arguments,
            detail::MakeArgStorage<mpl::_1> >::type  arg_tl;

        typedef typename mpl::push_front<arg_tl, T *&>::type mem_arg_tl;

        typedef typename fu::result_of::as_vector<arg_tl>::type     arg_vec;
        typedef typename fu::result_of::as_vector<mem_arg_tl>::type mem_arg_vec;

        fu::vector<T *&> parent(obj);
        mem_arg_vec cpp_args = fu::join(parent, arg_vec());

        // TODO: iterate over arg_tl instantiating each cpp_args element
        //       based on the index and args

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
        T *wrap = Allocator<T>::New(args);
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

    friend class detail::Singleton<self>;
    friend class BasicClass<T, Allocator>;
};

// basic class
// T = class
// P = optional parent class
// Allocator = static wrapper object memory allocator, by default
//             supports zero-argument constructor
template <class T, template <class> class Allocator>
struct BasicClass {
    typedef ClassSingleton<T, Allocator>  singleton_t;

  private:
    typedef typename singleton_t::MethodCallback  MethodCallback;

    inline singleton_t& Instance() { return singleton_t::Instance(); }

  public:
    inline v8::Persistent<v8::FunctionTemplate>& FunctionTemplate() {
        return Instance().FunctionTemplate();
    }

    // method helper
    template <class P>
    inline BasicClass& Method(char const *name) {
        FunctionTemplate()->PrototypeTemplate()->Set(
            v8::String::New(name),
            v8::FunctionTemplate::New(&singleton_t::template Forward<P>));
        return *this;
    }

    // method with any prototype
    template <class P, typename detail::MemFunProto<T, P>::method_type Ptr>
    inline BasicClass& Method(char const *name) {
        return Method< detail::MemFun<T, P, Ptr> >(name);
    }

    // passing v8::Arguments directly but modify return type
    template <class R, R (T::*Ptr)(const v8::Arguments&)>
    inline BasicClass& Method(char const *name) {
        return Method<R(const v8::Arguments&), Ptr>(name);
    }

    // passing v8::Arguments and return ValueHandle directly
    template <ValueHandle (T::*Ptr)(const v8::Arguments&)>
    inline BasicClass& Method(char const *name) {
        return Method<ValueHandle(const v8::Arguments&), Ptr>(name);
    }

    template <class U, template <class> class V>
    BasicClass(BasicClass<U, V>& parent) {
        FunctionTemplate()->Inherit(parent.FunctionTemplate());
    };
    BasicClass() {};
};

// class with constructor from v8::Arguments
template <class T>
struct Class : BasicClass<T, ArgAllocator> {
    typedef BasicClass<T, ArgAllocator> base;

    template <class U, template <class> class V>
    Class(BasicClass<U, V>& parent) : base(parent) {}
    Class() {}
};

} }
#endif

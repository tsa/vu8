#ifndef TSA_VU8_MODULE_HPP
#define TSA_VU8_MODULE_HPP

#include <v8.h>

#include <vu8/config.hpp>
#include <vu8/detail/MakeArgStorage.hpp>
#include <vu8/detail/Proto.hpp>
#include <vu8/detail/FromV8Arguments.hpp>
#include <vu8/ToV8.hpp>
#include <vu8/Throw.hpp>

#define BOOST_FUSION_INVOKE_MAX_ARITY VU8_PP_ITERATION_LIMIT
#include <boost/fusion/include/invoke.hpp>
#include <boost/fusion/container/vector.hpp>

namespace vu8 {

namespace fu = boost::fusion;
namespace mpl = boost::mpl;

template <class T, class F> struct Class;
template <class T>          struct Singleton;

struct Module {
    // register v8 style callback
    inline Module& operator()(char const            *name,
                              v8::InvocationCallback callback)
    {
        obj_->Set(v8::String::New(name), v8::FunctionTemplate::New(callback));
        return *this;
    }

    template <class T, class F>
    inline Module& operator()(char const *name, Class<T, F>& clss) {
        obj_->Set(v8::String::New(name),
                v8::FunctionTemplate::New(
                    &Class<T, F>::singleton_t::ConstructorFunction));

        clss.FunctionTemplate()->SetClassName(v8::String::New(name));
        return *this;
    }

    template <class T>
    inline Module& operator()(char const *name, Singleton<T>& singleton) {
        obj_->Set(v8::String::New(name), singleton.NewInstance());

        singleton.FunctionTemplate()->SetClassName(v8::String::New(name));
        return *this;
    }

    template <class T>
    inline Module& operator()(char const *name, T *t) {
        obj_->Set(v8::String::New(name), v8::External::New(t));
        return *this;
    }

    // Set = operator()
    inline Module& Set(char const *name, v8::InvocationCallback callback) {
        return (*this)(name, callback);
    }

    template <class T, class F>
    inline Module& Set(char const *name, Class<T, F>& clss) {
        return (*this)(name, clss);
    }

    template <class T>
    inline Module& Set(char const *name, T *t) {
        return (*this)(name, t);
    }

    // method with any prototype
    template <class P, typename detail::FunProto<P>::function_type Ptr>
    inline Module& Set(char const *name) {
        return Method< detail::Fun<P, Ptr> >(name);
    }

    // this is a local handle so make it persistent if needs be
    inline v8::Local<v8::Object> NewInstance() {
        return obj_->NewInstance();
    }

    v8::Local<v8::ObjectTemplate> ObjectTemplate() {
        return obj_;
    }

  private:
    template <class P>
    static inline typename P::return_type Invoke(const v8::Arguments& args) {
        // TODO: replace with CallFromV8
        typedef typename
            detail::MakeArgStorage<typename P::arguments>::type arg_tl;

        typename fu::result_of::as_vector<arg_tl>::type  cpp_args;
        detail::FromV8Arguments<0>(cpp_args, args);

        return boost::fusion::invoke(P::function_pointer, cpp_args);
    }

    template <class P>
    static inline typename boost::disable_if<
        boost::is_same<void, typename P::return_type>, ValueHandle >::type
    ForwardReturn(const v8::Arguments& args) {
        return ToV8(Invoke<P>(args));
    }

    template <class P>
    static inline typename boost::enable_if<
        boost::is_same<void, typename P::return_type>, ValueHandle >::type
    ForwardReturn(const v8::Arguments& args) {
        Invoke<P>(args);
        return v8::Undefined();
    }

    // TODO: handle adapting return type
    template <class P>
    static inline ValueHandle Forward(const v8::Arguments& args) {
        v8::HandleScope scope;
        try {
            return scope.Close(ForwardReturn<P>(args));
        }
        catch (std::runtime_error const& e) {
            return scope.Close(Throw(e.what()));
        }
    }

    template <class P>
    inline Module& Method(char const *name) {
        obj_->Set(v8::String::New(name), v8::FunctionTemplate::New(&Forward<P>));
        return *this;
    }

  public:
    Module() : obj_(v8::ObjectTemplate::New()) {}

  private:
    v8::Local<v8::ObjectTemplate> obj_;
};

}
#endif

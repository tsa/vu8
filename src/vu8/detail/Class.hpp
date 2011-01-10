#ifndef TSA_VU8_DETAIL_CLASS_HPP
#define TSA_VU8_DETAIL_CLASS_HPP

#include <v8.h>

#include <boost/mpl/front.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/include/invoke.hpp>

namespace vu8 { namespace detail {

template <class P>
struct PassDirectIf : boost::is_same<
    const v8::Arguments&,
    typename mpl::front<typename P::arguments>::type> {};

template <class F>
struct ArgFactory {
    static inline typename F::type *New(const v8::Arguments& args) {
        typedef typename
            detail::MakeArgStorage<typename F::arguments>::type arg_tl;

        typedef typename fu::result_of::as_vector<arg_tl>::type arg_vec;
        arg_vec cpp_args;

        detail::FromV8Arguments<0>(cpp_args, args);
        return boost::fusion::invoke(F(), cpp_args);
    }
};

template <class T>
struct ArgFactory< V8ArgFactory<T> > {
    static inline T *New(const v8::Arguments& args) {
        return new T(args);
    }
};

} }
#endif

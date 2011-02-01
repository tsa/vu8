#ifndef TSA_VU8_DETAIL_CLASS_HPP
#define TSA_VU8_DETAIL_CLASS_HPP

#include <vu8/config.hpp>

#include <v8.h>

#include <boost/mpl/front.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/fusion/container/vector.hpp>

#define BOOST_FUSION_INVOKE_MAX_ARITY VU8_PP_ITERATION_LIMIT
#include <boost/fusion/include/invoke.hpp>

namespace vu8 { namespace detail {

template <class P>
struct PassDirectIf : boost::is_same<
    const v8::Arguments&,
    typename mpl::front<typename P::arguments>::type> {};

template <class T, class F>
class ArgFactory {
    typedef typename F::template Construct<T>  factory_t;

  public:
    static inline typename factory_t::type *New(const v8::Arguments& args) {
        typedef typename
            detail::MakeArgStorage<typename factory_t::arguments>::type arg_tl;

        typedef typename fu::result_of::as_vector<arg_tl>::type arg_vec;
        arg_vec cpp_args;

        detail::FromV8Arguments<0>(cpp_args, args);
        return boost::fusion::invoke(factory_t(), cpp_args);
    }
};

template <class T>
struct ArgFactory<T, V8ArgFactory> {
    static inline T *New(const v8::Arguments& args) {
        return new T(args);
    }
};

} }
#endif

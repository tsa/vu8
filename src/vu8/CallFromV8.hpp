#ifndef BOOST_PP_IS_ITERATING
#   ifndef TSA_VU8_CALL_FROM_V8_HPP
#   define TSA_VU8_CALL_FROM_V8_HPP
#       include <vu8/config.hpp>
#       include <vu8/FromV8.hpp>

#       include <boost/preprocessor/repetition.hpp>
#       include <boost/preprocessor/iteration/iterate.hpp>
#       include <boost/fusion/container/vector.hpp>

#       include <v8.h>

#       ifndef VU8_CALL_FROM_V8_MAX_SIZE
#         define VU8_CALL_FROM_V8_MAX_SIZE VU8_PP_ITERATION_LIMIT
#       endif

namespace vu8 {

namespace fu  = boost::fusion;
namespace mpl = boost::mpl;

template <class P, class T, int N>
struct CallFromV8Helper;

template <class P, class T>
static inline typename P::return_type
CallFromV8(T& obj, const v8::Arguments& args) {
    return CallFromV8Helper<
        P, T, fu::result_of::size<typename P::arguments>::value
    >::exec(obj, args);
}

}

#       define BOOST_PP_ITERATION_LIMITS (0, VU8_CALL_FROM_V8_MAX_SIZE - 1)
#       define BOOST_PP_FILENAME_1       "vu8/CallFromV8.hpp"
#       include BOOST_PP_ITERATE()
#   endif // include guard

#else // BOOST_PP_IS_ITERATING

#  define n BOOST_PP_ITERATION()
#  define VU8_CALL_V8_from_v8_args(z, n, data) \
    FromV8<typename mpl::at_c<args_tl, n>::type>(args[ n ])

namespace vu8 {

template <class P, class T>
struct CallFromV8Helper<P, T, n> {
    static inline typename P::return_type
    exec(T& obj, const v8::Arguments& args)
    {
        typedef typename P::arguments  args_tl;

        return (obj.*P::method_pointer)(
            BOOST_PP_ENUM(n, VU8_CALL_V8_from_v8_args, ~)
        );
    }
};

}
#   undef VU8_CALL_V8_from_v8_args
#   undef n

#endif

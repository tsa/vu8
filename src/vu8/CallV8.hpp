#ifndef BOOST_PP_IS_ITERATING
#   ifndef TSA_VU8_CALL_V8_HPP
#   define TSA_VU8_CALL_V8_HPP
#       include <vu8/config.hpp>

#       include <boost/preprocessor/repetition.hpp>
#       include <boost/preprocessor/iteration/iterate.hpp>

#       include <v8.h>

#       ifndef VU8_CALL_V8_MAX_SIZE
#         define VU8_CALL_V8_MAX_SIZE VU8_PP_ITERATION_LIMIT
#       endif

namespace vu8 {

typedef v8::Handle<v8::Function> FunctionHandle;
typedef v8::Handle<v8::Value>    ValueHandle;

/// Calls a V8 function, converting C++ arguments to v8::Value arguments
/// @tparam T... C++ types of arguments
/// @param func  v8 function to call
/// @param args...  C++ arguments to convert to JS arguments using ToV8
ValueHandle CallV8(FunctionHandle& func) {
    ValueHandle argv[] = {};
    func->Call(func, 0, argv);
}

}

#       define BOOST_PP_ITERATION_LIMITS (1, VU8_CALL_V8_MAX_SIZE - 1)
#       define BOOST_PP_FILENAME_1       "vu8/CallV8.hpp"
#       include BOOST_PP_ITERATE()
#   endif // include guard

#else // BOOST_PP_IS_ITERATING

#  define n BOOST_PP_ITERATION()
#  define VU8_CALL_V8_args(z, n, data) BOOST_PP_CAT(T,n) BOOST_PP_CAT(arg,n)
#  define VU8_CALL_V8_tov8(z, n, data) ToV8(BOOST_PP_CAT(arg,n))

namespace vu8 {

template <BOOST_PP_ENUM_PARAMS(n, class T)>
ValueHandle CallV8(FunctionHandle& func, BOOST_PP_ENUM(n, VU8_CALL_V8_args, ~))
{
    ValueHandle argv[] = {
        BOOST_PP_ENUM(n, VU8_CALL_V8_tov8, ~)
    };
    func->Call(func, n, argv);
}

}
#   undef VU8_CALL_V8_tov8
#   undef VU8_CALL_V8_arg
#   undef n

#endif

#ifndef BOOST_PP_IS_ITERATING
#   ifndef TSA_VU8_FACTORY_HPP
#   define TSA_VU8_FACTORY_HPP
#       include <vu8/config.hpp>

#       include <boost/preprocessor/repetition.hpp>
#       include <boost/preprocessor/arithmetic/sub.hpp>
#       include <boost/preprocessor/punctuation/comma_if.hpp>
#       include <boost/preprocessor/iteration/iterate.hpp>

#       include <boost/mpl/vector.hpp>


#       ifndef VU8_FACTORY_MAX_SIZE
#         define VU8_FACTORY_MAX_SIZE VU8_PP_ITERATION_LIMIT
#       endif

#       define VU8_FACTORY_header(z, n, data) class BOOST_PP_CAT(T,n) = none

namespace vu8 {

struct none {};

// primary template

template <class C, BOOST_PP_ENUM(VU8_FACTORY_MAX_SIZE, VU8_FACTORY_header, ~)>
struct Factory;

}

#       define BOOST_PP_ITERATION_LIMITS (0, VU8_FACTORY_MAX_SIZE - 1)
#       define BOOST_PP_FILENAME_1       "vu8/Factory.hpp"
#       include BOOST_PP_ITERATE()
#   endif // include guard

#else // BOOST_PP_IS_ITERATING

#  define n BOOST_PP_ITERATION()
#  define VU8_FACTORY_default(z, n, data) data
#  define VU8_FACTORY_args(z, n, data) BOOST_PP_CAT(T,n) BOOST_PP_CAT(arg,n)


namespace vu8 {

// specialization pattern
template <class C BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, class T)>
struct Factory<
    C,
    BOOST_PP_ENUM_PARAMS(n,T)
    BOOST_PP_COMMA_IF(n)
    BOOST_PP_ENUM(BOOST_PP_SUB(VU8_FACTORY_MAX_SIZE,n), VU8_FACTORY_default, none)
>
{
    // boost::functional::factory does the same but boost-1.37 doesn't have it
    typedef C type;
    typedef boost::mpl::vector<BOOST_PP_ENUM_PARAMS(n,T)> arguments;

    C *operator()(BOOST_PP_ENUM(n, VU8_FACTORY_args, ~)) {
        return new C(BOOST_PP_ENUM_PARAMS(n,arg));
    }
};

}
#   undef VU8_FACTORY_default
#   undef VU8_FACTORY_args
#   undef n

#endif

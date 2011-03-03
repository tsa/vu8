#ifndef TSA_VU8_DETAIL_FROM_V8_ARGUMENTS_HPP
#define TSA_VU8_DETAIL_FROM_V8_ARGUMENTS_HPP

// TODO: remove me

#include <vu8/FromV8.hpp>

#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/size.hpp>
#include <boost/fusion/include/value_at.hpp>

#include <boost/mpl/int.hpp>

#include <boost/utility/enable_if.hpp>

#include <stdexcept>
#include <iostream>

namespace vu8 { namespace detail {

namespace fu = boost::fusion;

template <int first, int current, int last, class T>
static inline typename boost::disable_if_c<(current < last), void>::type
FromV8Arguments(T& dest, const v8::Arguments& args) {}

template <int first, int current, int last, class T>
static inline typename boost::enable_if_c<(current < last), void>::type
FromV8Arguments(T& dest, const v8::Arguments& args) {
    fu::at_c<current>(dest) = vu8::FromV8<
        typename fu::result_of::value_at<T, boost::mpl::int_<current> >::type
    >(args[current - first]);

    FromV8Arguments<first, current + 1, last>(dest, args);
}

/// Converts v8::Arguments into fusion::vector of C++ types
template <int offset, class T>
static inline void FromV8Arguments(T& dest, const v8::Arguments& args) {
    enum { last = fu::result_of::size<T>::value };

    if (args.Length() < last - offset) {
        throw std::runtime_error("function not supplied with enough arguments");
    }
    return FromV8Arguments<offset, offset, last>(dest, args);
}

/// Converts v8::Arguments into fusion::vector of C++ types
/// @arg S Supplimentary fusion::vector to for stack storage of converted types
template <int offset, class T, class S>
static inline void FromV8Arguments(T& dest, const v8::Arguments& args, S& strings) {
}

} }
#endif

#ifndef TSA_VU8_UTIL_MAKE_ARG_STORAGE_HPP
#define TSA_VU8_UTIL_MAKE_ARG_STORAGE_HPP

#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/remove_reference.hpp>

namespace tsa { namespace vu8 { namespace detail {

template <class T>
struct MakeArgStorage : boost::remove_const<
    typename boost::remove_reference<T>::type
> {};

} } }
#endif


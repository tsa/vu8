#ifndef TSA_VU8_UTIL_REMOVE_CONST_REFERENCE_HPP
#define TSA_VU8_UTIL_REMOVE_CONST_REFERENCE_HPP

#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/remove_reference.hpp>

namespace tsa { namespace vu8 { namespace util {

template <class T>
struct RemoveConstReference : boost::remove_const<
    typename boost::remove_reference<T>::type
> {};

} } }
#endif


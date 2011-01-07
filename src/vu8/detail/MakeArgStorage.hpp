#ifndef TSA_VU8_UTIL_MAKE_ARG_STORAGE_HPP
#define TSA_VU8_UTIL_MAKE_ARG_STORAGE_HPP

#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/transform.hpp>

#include <vu8/detail/ConvertibleString.hpp>

namespace vu8 { namespace detail {

namespace mpl = boost::mpl;

template <class T>
struct MakeArgStorageHelperRedirect : mpl::identity<T> {};

template <>
struct MakeArgStorageHelperRedirect<char const *>
  : mpl::identity<ConvertibleString> {};

template <class T>
struct MakeArgStorageHelper : MakeArgStorageHelperRedirect<
    typename boost::remove_const<
        typename boost::remove_reference<T>::type >::type > {};

template <class T>
struct MakeArgStorage
  : mpl::transform<T, detail::MakeArgStorageHelper<mpl::_1> > {};

} }
#endif


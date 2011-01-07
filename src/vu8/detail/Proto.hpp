#ifndef TSA_VU8_DETAIL_PROTO_HPP
#define TSA_VU8_DETAIL_PROTO_HPP

#include <vu8/detail/ProtoHelper.hpp>

namespace vu8 { namespace detail {

/// Function
/// @arg P prototype trait of function
/// @arg Ptr function pointer
template <class P, typename FunProto<P>::function_type Ptr>
struct Fun : FunProto<P> {
    typedef typename FunProto<P>::function_type function_type;
    static function_type function_pointer;
};

template <class P, typename FunProto<P>::function_type Ptr>
typename Fun<P, Ptr>::function_type
Fun<P, Ptr>::function_pointer = Ptr;

/// Member function
/// @arg C class type
/// @arg P prototype trait of function
/// @arg Ptr function pointer
template <class C, class P, typename MemFunProto<C, P>::method_type Ptr>
struct MemFun : MemFunProto<C, P> {
    typedef typename MemFunProto<C, P>::method_type method_type;
    static method_type method_pointer;
};

template <class C, class P, typename MemFunProto<C, P>::method_type Ptr>
typename MemFun<C, P, Ptr>::method_type
MemFun<C, P, Ptr>::method_pointer = Ptr;

} }
#endif

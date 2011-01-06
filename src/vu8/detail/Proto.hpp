#ifndef TSA_VU8_PROTO_HPP
#define TSA_VU8_PROTO_HPP

#include <boost/mpl/vector.hpp>

namespace vu8 { namespace detail {

namespace mpl = boost::mpl;

template <class R>
struct FunProtoBase {
    typedef R return_type;
};

//////////////////////////////////////////////////////////////////////
template <class T>
struct FunProto;

template <class R>
struct FunProto<R ()> {
    typedef mpl::vector<>   arguments;
    typedef R(*function_type)();
};

template <class R, class A1>
struct FunProto<R (A1)> : FunProtoBase<R> {
    typedef mpl::vector<A1>   arguments;
    typedef R(*function_type)(A1);
};

template <class R, class A1, class A2>
struct FunProto<R (A1, A2)> : FunProtoBase<R> {
    typedef mpl::vector<A1, A2>   arguments;
    typedef R(*function_type)(A1, A2);
};

//////////////////////////////////////////////////////////////////////
template <class C, class T>
struct MemFunProto;

template <class C, class R>
struct MemFunProto<C, R ()> : FunProtoBase<R> {
    typedef mpl::vector<>   arguments;
    typedef R(C::*method_type)();
};

template <class C, class R, class A1>
struct MemFunProto<C, R (A1)> : FunProtoBase<R> {
    typedef mpl::vector<A1>   arguments;
    typedef R(C::*method_type)(A1);
};

template <class C, class R, class A1, class A2>
struct MemFunProto<C, R (A1, A2)> : FunProtoBase<R> {
    typedef mpl::vector<A1, A2>   arguments;
    typedef R(C::*method_type)(A1, A2);
};

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

#ifndef TSA_VU8_SINGLETON_HPP
#define TSA_VU8_SINGLETON_HPP

namespace tsa { namespace vu8 {

template <class T>
class Singleton {
    struct object_creator {
        object_creator() { Singleton<T>::Instance(); }
        inline void do_nothing() const {}
    };
    static object_creator create_object;

    typedef T object_type;

  public:
    static object_type& Instance() {
        static object_type obj;
        create_object.do_nothing();
        return obj;
    }

  protected:
    Singleton() {}
};

template <typename T>
typename Singleton<T>::object_creator
Singleton<T>::create_object;

} }
#endif

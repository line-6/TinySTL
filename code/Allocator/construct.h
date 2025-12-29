/*
    一般而言，C++的内存配置操作和释放操作是这样的：
        Foo* p = new Foo();
        delete p;
    这其中的new和delete内含两阶段操作：(1) 调用 ::operator new配置内存 并 调用Foo::Foo()构造对象内容
    (2) 调用Foo::~Foo()析构 并 调用 ::operator delete 释放内存.
    "construct.h" 定义了construct() 和 destroy()， 负责对象的构造和析构.
    "alloc.h" 则负责内存空间的配置和释放

*/

#include <new>

#include "Utils/type_traits.h"

namespace TinySTL {

/*
    将T2类型的初值设到T1类型指针所指的空间上
    只构造，不分配内存
*/
template<class T1, class T2>
inline void construct(T1* p, T2 value) {
    new (p) T1(value);
}

/*
    只析构，不释放内存
*/
template<class T>
inline void destroy(T* p) {
    p->~T();
}

/*
    利用traits批量析构对象
*/
template<class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
    using is_POD_type = typename type_traits<ForwardIterator>::is_POD_type;
    _destroy_aux(first, last, is_POD_type());
}

/*
    如果元素的 value_type 有 non-trivial destructor
*/
template<class ForwardIterator>
inline void _destroy_aux(ForwardIterator first, ForwardIterator last,
                        false_type) {
    for (; first != last; ++ first) destory(&*first);   //迭代器不是真正的地址
}

/*
    有 trivial destructor，因为如果每个对象的析构函数都无关痛痒，那么对其反复调用
    是对效率的伤害。
*/
template<class ForwardIterator>
inline void _destroy_aux(ForwardIterator first, ForwardIterator last,
                        true_type) {}

/* 特化 */
inline void destroy(char*, char*) {}
inline void destroy(wchar_t*, wchar_t*) {}



}// namespace TinySTL
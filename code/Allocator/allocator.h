#pragma once

#include "alloc.h"
#include "construct.h"
#include <cstddef>

namespace TinySTL {
// 默认二级适配器
template<class T, class Alloc = _default_alloc>
class simpleAlloc {
public:
    using value_type = T;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
public:
    static T* allocate();
    static T* allocate(size_t n);
    static void deallocate(T* p);
    static void deallocate(T* p, size_t n);

    static void construct(T* p);
    static void construct(T* p, const T& value);
    static void destroy(T* p);
    static void destroy(T* first, T* last);
};

template<class T, class Alloc>
T* simpleAlloc<T, Alloc>::allocate() {
    return reinterpret_cast<T*>(Alloc::allocate(sizeof(T)));
}

template<class T, class Alloc>
T* simpleAlloc<T, Alloc>::allocate(size_t n) {
    if (n == 0) return 0;
    return reinterpret_cast<T*>(Alloc::allocate(sizeof(T) * n));
}

template<class T, class Alloc>
void simpleAlloc<T, Alloc>::deallocate(T* p) {
    Alloc::deallocate(reinterpret_cast<void*>(p), sizeof(T));
}

template<class T, class Alloc>
void simpleAlloc<T, Alloc>::deallocate(T* p, size_t n) {
    if (n == 0) return;
    Alloc::deallocate(reinterpret_cast<void*>(p), sizeof(T) * n);
}

template<class T, class Alloc>
void simpleAlloc<T, Alloc>::construct(T* p) {
    new (p) T();
}

template<class T, class Alloc>
void simpleAlloc<T, Alloc>::construct(T* p, const T& value) {
    new (p) T(value);
}

template<class T, class Alloc>
void simpleAlloc<T, Alloc>::destroy(T* p) {
    p->~T();
}

template<class T, class Alloc>
void simpleAlloc<T, Alloc>::destroy(T* first, T* last) {
    for (; first != last; first ++) {
        first->~T();
    }
}
}
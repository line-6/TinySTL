#pragma once

#include "Allocator/allocator.h"
#include "Allocator/uninitialized.h"
#include "algobase/stl_algobase.h"
#include "construct.h"
#include "stl_iterator.h"
#include "type_traits.h"
#include <cstddef>
#include <initializer_list>

namespace TinySTL {
template <class T, class Alloc = simpleAlloc<T>>
class vector {
public:
    using value_type = T;
    using pointer = value_type *;
    using iterator = value_type *;
    using const_iterator = const value_type *;
    using reverse_iterator = __reverse_iterator<iterator>;
    using const_reverse_iterator = __reverse_iterator<const_iterator>;
    using reference = value_type &;
    using const_reference = const value_type &;
    using size_type = size_t;
    using difference_type_ = ptrdiff_t;

private:// data member
    // [start, finish) -> 已构造，可用元素
    // [start, end_of_storage) -> 已分配，但未构造的原始内存空间
    iterator start;
    iterator finish;
    iterator end_of_storage;

private:// allocate and construct aux functions
    using data_allocator = Alloc;
    
    void fill_initialize(size_type n, const value_type &value) {
        start = allocate_and_fill(n, value);
        finish = start + n;
        end_of_storage = finish;
    }
    template <class Integer>
    void initialize_aux(Integer n, Integer val, true_type) {
        fill_initialize(static_cast<size_type>(n),
                     static_cast<value_type>(val));
    }
    template <class InputIterator>
    void initialize_aux(InputIterator first, InputIterator last, false_type) {
        start = allocate_and_copy(first, last);
        finish = end_of_storage = start + TinySTL::distance(first, last);
    }

    // 分配内存 + 构造
    // 无论是fill还是copy的构造，都要考虑数据是否为POD类型
    // 若为POD，直接fill/copy，反之调用construct构造对象
    iterator allocate_and_fill(size_type n, const value_type &value) {
        iterator result = data_allocator::allocate(n);  // 拿内存
        TinySTL::uninitialized_fill_n(result, n, value);    // 构造对象
        return result;
    }
    template <class InputIterator>
    iterator allocate_and_copy(InputIterator first, InputIterator last) {
        size_type n = TinySTL::distance(first, last);
        iterator result = data_allocator::allocate(n);
        TinySTL::uninitialized_copy(first, last, result);
    }

    // 析构 + 释放内存
    void deallocate() noexcept {
        if (start) data_allocator::deallocate(start, end_of_storage - start);
    }
    void destory_and_deallocate() noexcept{
        TinySTL::destroy(start, finish);    // 析构
        deallocate();   // 释放内存
    }

public:// swap
    void swap(vector &) noexcept;

public:// ctor && dtor
    // 默认构造
    vector() : start(nullptr), finish(nullptr), end_of_storage(nullptr) {}
    explicit vector(size_type n) { fill_initialize(n, value_type()); }
    vector(size_type n, const value_type& value) { fill_initialize(n, value); }

    template<class InputIterator>
    vector(InputIterator first, InputIterator last) {
        initialize_aux(first, last, is_integral<InputIterator>());
    }
    // 使用初始化列表的构造
    vector(std::initializer_list<T>);
    // 拷贝构造
    vector(const vector &);
    // 移动构造
    vector(vector &&) noexcept;

    ~vector() {
        destory_and_deallocate();
    }

    // 拷贝赋值
    vector &operator= (const vector &);
    // 移动赋值
    vector &operator= (vector &&) noexcept;

public: // getter

public: // setter

public: // interface for size and capacity

public: // compare operator

public: // push && pop
    void push_back(const value_type& value);
    void pop_back() {
        --finish;
        TinySTL::destroy(finish);
    }
public: // erase

private: // aux_interface for insert

public: // insert

private: // aux_interface for assign

public: // assign

};

// swap
template <class T, class Alloc>
inline void vector<T, Alloc>::swap(vector &rhs) noexcept{
    TinySTL::swap(start, rhs.start);
    TinySTL::swap(finish, rhs.finish);
    TinySTL::swap(end_of_storage, rhs.end_of_storage);
}

// ctor 
template <class T, class Alloc>
inline vector<T, Alloc>::vector(std::initializer_list<T> il) {
    start = allocate_and_copy(il.begin(), il.end());
    finish = end_of_storage = start + il.size();
}

template <class T, class Alloc>
inline vector<T, Alloc>::vector(const vector& rhs) {
    start = allocate_and_copy(rhs.begin(), rhs.end());
    finish = end_of_storage = start + rhs.size();
}

template <class T, class Alloc>
inline vector<T, Alloc>::vector(vector&& rhs) noexcept{
    start = rhs.start;
    finish = rhs.finish;
    end_of_storage = rhs.end_of_storage;
    rhs.start = rhs.finish = rhs.end_of_storage = nullptr;
}

template <class T, class Alloc>
inline vector<T, Alloc> &vector<T, Alloc>::operator=(const vector& rhs) {
    // copy-and-swap 技法，保证强异常安全
    vector temp(rhs);
    swap(temp);
    return *this;
}

template <class T, class Alloc>
inline vector<T, Alloc> &vector<T, Alloc>::operator=(vector&& rhs) noexcept{
    if (this != &rhs) {
        destory_and_deallocate();
        start = rhs.start;
        finish = rhs.finish;
        end_of_storage = rhs.end_of_storage;
        rhs.start = rhs.finish = rhs.end_of_storage = nullptr;
    }
    return *this;
}

// push
template <class T, class Alloc>
inline void vector<T, Alloc>::push_back(const value_type& value) {
    if (finish != end_of_storage) {
        TinySTL::construct(finish, value);
        ++finish;
    } else {
        
    }
}
}
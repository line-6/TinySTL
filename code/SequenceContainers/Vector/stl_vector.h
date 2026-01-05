#pragma once

#include "Allocator/allocator.h"
#include "Allocator/uninitialized.h"
#include "algobase/stl_algobase.h"
#include "construct.h"
#include "stl_iterator.h"
#include "type_traits.h"
#include <cstddef>
#include <exception>
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
        return result;
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
    const_iterator begin() const noexcept { return start; }
    const_iterator end() const noexcept { return finish; }
    const_reference front() const noexcept { return *begin(); }
    const_reference back() const noexcept { return *(end() - 1); }
    const_reference operator[](const size_type n) const noexcept {
        return *(start + n); }
    size_type size() const noexcept {
        return static_cast<size_type>(finish - start);
    }
    size_type capacity() const noexcept {
        return static_cast<size_type>(end_of_storage - start);
    }
    bool empty() const noexcept {
        return start == finish;
    }

public: // setter
    iterator begin() noexcept { return start; }
    iterator end() noexcept { return finish; }
    reference operator[](const size_type n) noexcept { return *(start + n); }
    reference front() noexcept { return *begin(); }
    reference back() noexcept { return *(end() - 1); }

public: // interface for size and capacity
    void resize(size_type, const value_type &);
    void resize(size_type new_size) { resize(new_size, value_type()); }
    void reserve(size_type);
    void shrink_to_fit() noexcept {
        vector temp(*this);
        swap(temp);
    }
public: // compare operator
    bool operator== (const vector&) const noexcept;
    bool operator!= (const vector& rhs) const noexcept {
        return !(*this == rhs);
    }
public: // push && pop
    void push_back(const value_type& value);
    void pop_back() {
        --finish;
        TinySTL::destroy(finish);
    }
public: // erase
    iterator erase(iterator, iterator);
    iterator erase(iterator position) { return erase(position, position + 1); }
    void clear() { erase(begin(), end()); }
private: // aux_interface for insert
    void insert_aux(iterator, const value_type &);
    void fill_insert(iterator, size_type, const value_type &);
public: // TODO : insert
    iterator insert(iterator, const value_type &);
private: // aux_interface for assign

public: // TODO: assign

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

// interface for size and capacity
template <class T, class Alloc>
inline void vector<T, Alloc>::resize(size_type new_size, const value_type &value) {
    if (new_size < size()) {
        erase(begin() + new_size, end());
    } else {
        fill_insert(end(), new_size - size(), value);
    }
}

template<class T, class Alloc>
inline void vector<T, Alloc>::reserve(size_type new_capacity) {
    if (new_capacity <= capacity()) return;
    T *new_start = data_allocator::allocate(new_capacity);
    T *new_finish = TinySTL::uninitialized_copy(start, finish, new_start);
    destory_and_deallocate();
    start = new_start;
    finish = new_finish;
    end_of_storage = start + new_capacity;
}

// compare
template<class T, class Alloc>
bool vector<T, Alloc>::operator==(const vector &rhs) const noexcept {
    if (size() != rhs.size()) {
    return false;
  } else {
    iterator ptr1 = start;
    iterator ptr2 = rhs.start;
    for (; ptr1 != finish && ptr2 != rhs.finish; ++ptr1, ++ptr2)
      if (*ptr1 != *ptr2) return false;
    return true;
  }
}

// push
template <class T, class Alloc>
inline void vector<T, Alloc>::push_back(const value_type& value) {
    if (finish != end_of_storage) {
        TinySTL::construct(finish, value);
        ++finish;
    } else {
        insert_aux(end(), value);
    }
}

// erase
template<class T, class Alloc>
inline typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(
    iterator first, iterator last) {
    iterator i = TinySTL::copy(last, finish, first);
    TinySTL::destroy(i, finish);
    finish -= (last - first);
    return first;
}

// aux_interface for insert
template <class T, class Alloc>
inline void vector<T, Alloc>::insert_aux(iterator position, const value_type &value) {
    if (finish != end_of_storage) {// needn't expand
        TinySTL::construct(finish, *(finish - 1));
        ++ finish;
        value_type value_copy = value;
        TinySTL::copy_backward(position, finish - 2, finish - 1);
        *position = value_copy;
    } else {// expand
        const size_type old_size = size();
        const size_type new_size = 
            old_size ? old_size * 2 : 1;
        iterator new_start = data_allocator::allocate(new_size);
        iterator new_finish = new_start;
        try {
            new_finish = TinySTL::uninitialized_copy(
                start, position, new_start); // copy position-before segment
            construct(new_finish, value);
            ++new_finish;
            new_finish = TinySTL::uninitialized_copy(
                position, finish, new_finish); // copy position-after segment
        } catch (std::exception &) {
            // commit or rollback
            destroy(new_start, new_finish);
            data_allocator::deallocate(new_start, new_size);
            throw;
        }
        destory_and_deallocate();
        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + new_size;
    }
}

template <class T, class Alloc>
inline void vector<T, Alloc>::fill_insert(iterator position, size_type n, 
                                        const value_type &value) {
    if (n <= 0) return;
    if (static_cast<size_type>(end_of_storage - finish) >= n) {// needn't expand
    value_type value_copy = value;
    const size_type elems_after = finish - position;
    iterator old_finish = finish;
    if (elems_after > n) {
      TinySTL::uninitialized_copy(finish - n, finish, finish);
      finish += n;
      TinySTL::copy_backward(position, old_finish - n, old_finish);
      TinySTL::fill(position, position + n, value_copy);
    } else {
      TinySTL::uninitialized_fill_n(finish, n - elems_after,
                                    value_copy);
      finish += n - elems_after;
      TinySTL::uninitialized_copy(position, old_finish, finish);
      finish += elems_after;
      TinySTL::fill(position, old_finish, value_copy);// complement
    }
  } else {// expand
        const size_type old_size = size();
        const size_type new_size = old_size + TinySTL::max(old_size, n);
        iterator new_start = data_allocator::allocate(new_size);
        iterator new_finish = new_start;
        try {
        new_finish =
            TinySTL::uninitialized_copy(start, position, new_start);
        new_finish =
            TinySTL::uninitialized_fill_n(new_finish, n, value);
        new_finish =
            TinySTL::uninitialized_copy(position, finish, new_finish);
        } catch (std::exception &) {
        TinySTL::destroy(new_start, new_finish);
        data_allocator::deallocate(new_start, new_size);
        throw;
        }
        destory_and_deallocate();
        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + new_size;
    }
}

// insert
template<class T, class Alloc>
inline typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(
    iterator position, const value_type &value) {
  size_type n = position - begin();
  if (finish != end_of_storage && position == end()) {
    construct(finish, value);
    ++finish;
  } else
    insert_aux(position, value);
  return begin() + n;
}

}
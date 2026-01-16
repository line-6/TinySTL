#pragma once

#include "SequenceContainers/Vector/stl_vector.h"
#include "algobase/stl_algobase.h"
#include "allocator.h"
#include "stl_iterator.h"
#include <cstddef>

namespace TinySTL {
template <class Value>
struct hashtable_node {
    Value val;
    hashtable_node* next;
};

template<class Value, class Key, class HashFunc, class ExtractKey,
        class EqualKey, class Alloc>
class hashtable;

template <class Value, class Key, class HashFunc, class ExtractKey,
        class EqualKey, class Alloc>
class hashtable_iterator;

template <class Value, class Key, class HashFunc, class ExtractKey,
        class EqualKey, class Alloc>
class hashtable_const_iterator;


template <class Value, class Key, class HashFunc, class ExtractKey,
        class EqualKey, class Alloc>
struct hashtable_iterator {
    using _hashtable = TinySTL::hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;
    using iterator = hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;
    using const_iterator = hashtable_const_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;
    using node = hashtable_node<Value>;

    using iteratoe_category = forward_iterator_tag;
    using value_type = Value;
    using difference_type_t = ptrdiff_t;
    using size_type = size_t;
    using reference = Value &;
    using pointer = Value *;

    node *cur; // 当前指向的节点
    _hashtable *ht; // 保持与hashtable的连接

    hashtable_iterator(node *n, _hashtable *tab): cur(n), ht(tab) {}
    hashtable_iterator() = default;
    reference operator*() const noexcept { return cur->val; }
    pointer operator->() const noexcept { return &(operator*()); }
    iterator &operator++() noexcept;
    iterator operator++(int) noexcept;
    bool operator==(const iterator &rhs) const noexcept {
        return cur = rhs.cur;
    }
    bool operator!=(const iterator& rhs) const noexcept {
        return cur != rhs.cur;
    }
};

template <class Value, class Key, class HashFunc, class ExtractKey,
        class EqualKey, class Alloc>
typename hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::iterator &
hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::operator++() noexcept {
    const node* old = cur;
    cur = cur->next;
    if (!cur) {
        // 拿到当前node在hashtable中的节点位置
        // 向后遍历，找到第一个有节点的node
    }
    return *this;
}


template<class Value, class Key, class HashFunc, class ExtractKey,
        class EqualKey, class Alloc = simpleAlloc<Value>>
class hashtable {
    friend struct hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;
    friend struct hashtable_const_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;

public:
    using hasher = HashFunc;
    using key_equal = EqualKey;
    using size_type = size_t;
    using value_type = Value;
    using key_type = Key;
    using reference = Value &;
    using pointer = Value *;
    using allocator_type = Alloc;

    using iterator = hashtable_iterator<Value, Key, HashFunc,
                        ExtractKey, EqualKey, Alloc>;
    using const_iterator = hashtable_const_iterator<Value, Key, 
                        HashFunc, ExtractKey, EqualKey, Alloc>;

private: // data member
    hasher hash;
    key_equal equals;
    ExtractKey get_key;

    using node = hashtable_node<Value>;
    using node_allocator = simpleAlloc<node>;

    TinySTL::vector<node *> buckets;
    size_type num_elements;

private: // allocate && deallocate

};

}
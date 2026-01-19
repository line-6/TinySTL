/*
hashtable的几乎所有操作，核心都是:
1. 根据key值找到对应的bucket
2. 根据操作目的遍历此bucket
3. 若要插入，则对当前bucket头插
*/
#pragma once

#include "SequenceContainers/Vector/stl_vector.h"
#include "algobase/stl_algobase.h"
#include "allocator.h"
#include "stl_iterator.h"
#include <cstddef>
#include <algorithm>
#include <exception>

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

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
struct hashtable_const_iterator {
  // alias declarations
  using _hashtable =
      TinySTL::hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
  using iterator =
      hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
  using const_iterator =
      hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey,
                               Alloc>;
  using node = hashtable_node<Value>;

  // iterator declarations
  using iterator_category = forward_iterator_tag;
  using value_type = Value;
  using difference_type = ptrdiff_t;
  using size_type = size_t;
  using reference = const Value &;
  using pointer = const Value *;

  const node *cur;
  const _hashtable *ht;

  hashtable_const_iterator(const node *n, const _hashtable *tab)
      : cur(n), ht(tab) {}
  hashtable_const_iterator() = default;
  hashtable_const_iterator(const iterator &it) : cur(it.cur), ht(it.ht) {}// add implicit convert
  reference operator*() const noexcept { return cur->val; }
  pointer operator->() const noexcept { return &(operator*()); };
  const_iterator &operator++() noexcept;
  const_iterator operator++(int) noexcept;
  bool operator==(const const_iterator &rhs) const noexcept {
    return cur == rhs.cur;
  }
  bool operator!=(const const_iterator &rhs) const noexcept {
    return cur != rhs.cur;
  }
};

// data for hashtable
enum { _stl_num_primes = 28 };
static const unsigned long _stl_prime_list[_stl_num_primes] = {
    53ul, 97ul, 193ul, 389ul, 769ul,
    1543ul, 3079ul, 6151ul, 12289ul, 24593ul,
    49157ul, 98317ul, 196613ul, 393241ul, 786433ul,
    1572869ul, 3145739ul, 6291469ul, 12582917ul, 25165843ul,
    50331653ul, 100663319ul, 201326611ul, 402653189ul, 805306457ul,
    1610612741ul, 3221225473ul, 4294967291ul};

// 找出大于n的最小质数
inline unsigned long _stl_next_prime(unsigned long n) noexcept {
    const unsigned long *first = _stl_prime_list;
    const unsigned long *last = _stl_prime_list + _stl_num_primes;
    const unsigned long *pos = std::lower_bound(first, last, n); // TODO
    return pos == last ? *(last - 1) : *pos;
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
    node *new_node(const value_type &obj) {
        node *n = node_allocator::allocate();
        try {
            construct(&n->val, obj);
            return n;
        } catch(std::exception &) {
            node_allocator::deallocate(n);
            return nullptr;
        }
    }

    void delete_node(node *n) {
        destroy(&n->val);
        node_allocator::deallocate(n);
    }

private: // interface for bucket
    void init_buckets(size_type n) {
        size_type n_buckets = _stl_next_prime(n);
        buckets.reserve(n_buckets);
        while (n_buckets --) {
            buckets.insert(buckets.end(), static_cast<node *>(nullptr));
        } // TODO: vector还未实现批量insert
        num_elements = 0;
    }

    size_type bkt_num_key(const key_type &key, size_t n) const noexcept {
        return hash(key) % n;
    }
    size_type bkt_num(const value_type &obj, size_type n) const noexcept {
        return bkt_num_key(get_key(obj), n);
    }
    size_type bkt_num_key(const key_type &key) const noexcept {
        return bkt_num_key(key, buckets.size());
    }
    size_type bkt_num(const value_type &obj) const noexcept {
        return bkt_num_key(get_key(obj));
    }

    void erase_bucket(size_type n, node *first, node *last);
    void erase_bucket(size_type n, node *last);

private: // aux interface
    pair<iterator, bool> insert_unique_noreseize(const value_type &);
    iterator insert_equal_noresize(const value_type &);
    void copy_from(const hashtable &);

public: // ctor && dtor
    hashtable(size_type n, const hasher &hf, const key_equal &eql,
            const ExtractKey &ext) 
            : hash(hf), equals(eql), get_key(ext), num_elements(0) {
        init_buckets(n);
    }

    hashtable(size_type n, const hasher &hf, const key_equal &eql)
      : hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0) {
    init_buckets(n);
  }

  ~hashtable() { clear(); }

public: // getter
    hasher hash_func() const noexcept { return hash; }
    key_equal key_eq() const noexcept { return equals; }
    size_type bucket_count() const noexcept { return buckets.size(); }
    size_type max_bucket_count() const noexcept {
        return _stl_prime_list[_stl_num_primes - 1];
    }
    size_type size() const noexcept { return num_elements; }
    size_type max_size() const noexcept { return size_type(-1); }
    bool empty() const noexcept { return size() == 0; }

    const_iterator cbegin() const {
        for (size_type n = 0; n < buckets.size(); ++n)
            if (buckets[n]) return const_iterator(buckets[n], this);
        return cend();
    }

    const_iterator cend() const { return const_iterator(nullptr, this); }

public: // setter
    iterator begin() {
        for (size_type n = 0; n < buckets.size(); ++n)
            if (buckets[n]) return iterator(buckets[n], this);
        return end();
    }

    iterator end() { return iterator(nullptr, this); }
    void resize(size_type);

public: // aux_interface for insert && erase
    template<class InputIterator>
    void insert_unique(InputIterator, InputIterator, input_iterator_tag);
    template<class InputIterator>
    void insert_equal(InputIterator, InputIterator, input_iterator_tag);
    template<class ForwardIterator>
    void insert_unique(ForwardIterator, ForwardIterator, forward_iterator_tag);
    template<class ForwardIterator>
    void insert_equal(ForwardIterator, ForwardIterator, forward_iterator_tag);

public: // find
    reference find_or_insert(const value_type &);// 有则返回引用，无则插入并返回引用
    iterator find(const key_type &);
    const_iterator find(const key_type &) const;
    size_type count(const key_type &) const;    

public: //insert
    pair<iterator, bool> insert_unique(const value_type &);
    iterator insert_equal(const value_type &);
    template<class InputIterator>
    void insert_unique(InputIterator first, InputIterator last) {
        insert_unique(first, last, iterator_category_t<InputIterator>());
    }
    template<class InputIterator>
    void insert_equal(InputIterator first, InputIterator last) {
        insert_equal(first, last, iterator_category_t<InputIterator>());
    }

public: // erase
    size_type erase(const key_type &);
    void erase(const iterator &);
    void erase(iterator, iterator);
    void erase(const const_iterator &);
    void erase(const_iterator, const_iterator);
    void clear();

public: // copy
    hashtable(const hashtable &rhs) : hash(rhs.hash), equals(rhs.equals),
                get_key(rhs.get_key), num_elements(0) {
        copy_from(rhs);
    }
    hashtable &operator=(const hashtable &rhs) {
        if (&rhs != this) {
        clear();
        hash = rhs.hash;
        equals = rhs.equals;
        get_key = rhs.get_key;
        copy_from(rhs);
        }
        return *this;
    }

public: // swap
    void swap(hashtable &rhs) noexcept {
        std::swap(hash, rhs.hash);
        std::swap(equals, rhs.equals);
        std::swap(get_key, rhs.get_key);
        buckets.swap(rhs.buckets);
        std::swap(num_elements, rhs.num_elements);
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
        size_type bucket = ht->bkt_num(old->val);
        while(!cur && ++bucket < ht->buckets.size()) {
            cur = ht->buckets[bucket];
        }
    }
    return *this;
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
inline typename hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey,
                                   Alloc>::iterator
hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::
operator++(int) noexcept {
    iterator temp = *this;
    ++*this;
    return temp;
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
typename hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey,
                                  Alloc>::const_iterator &
hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::
operator++() noexcept {
  const node *old = cur;
  cur = cur->next;
  if (!cur) {
    size_type bucket = ht->bkt_num(old->val);
    while (!cur && ++bucket < ht->buckets.size()) cur = ht->buckets[bucket];
  }
  return *this;
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
inline typename hashtable_const_iterator<Value, Key, HashFcn, ExtractKey,
                                         EqualKey, Alloc>::const_iterator
hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::
operator++(int) noexcept {
    const_iterator temp = *this;
    ++*this;
    return temp;
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::erase_bucket(
    const size_type n, node *first, node *last) {
    node *cur = buckets[n];
    if (cur == first)
        erase_bucket(n, last);
    else {
        node *next;
        for (next = cur->next; next != first; cur = next, next = next->next) ;
        while (next != last) {
            cur->next = next->next;
            delete_node(next);
            next = cur->next;
            --num_elements;
        }
    }
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::erase_bucket(
    const size_type n, node *last) {
    node *cur = buckets[n];
    while (cur != last) {
        node *next = cur->next;
        delete_node(cur);
        cur = next;
        buckets[n] = cur;
        --num_elements;
    }
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::resize(
    size_type num_elements_hint) {
    // 是否重建表格的原则为：若元素个数大于bucket.size()
    const size_type old_n = buckets.size();
    if (num_elements_hint > old_n) { // 需要扩容
        const size_type n = _stl_next_prime(num_elements_hint);
        if (n > old_n) {
            TinySTL::vector<node *> temp(n, static_cast<node *>(nullptr));
            try {
                // 处理每一个旧bucket
                for (size_type bucket = 0; bucket < old_n; bucket ++) {
                    node *first = buckets[bucket];
                    while(first) {
                        // 找出节点应该位于new_bucket的何处
                        size_type new_bucket = bkt_num(first->val, n);
                        buckets[bucket] = first->next; // first 和 bucket分离
                        first->next = temp[new_bucket]; // 头插
                        temp[new_bucket] = first;
                        first = buckets[bucket];
                    }
                }
                buckets.swap(temp);
            } catch(std::exception &) {
                clear();
            }
        }
    }
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
inline pair<typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey,
                               Alloc>::iterator,
            bool>
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey,
          Alloc>::insert_unique_noreseize(const value_type &obj) {
    const size_type n = bkt_num(obj);
    node *first = buckets[n];
    for (node *cur; cur; cur = cur->next) {
        if (equals(get_key(cur->val), get_key(obj))) 
            return pair<iterator, bool>(iterator(cur, this), false);
    }
    // 已离开循环或根本未进入，创造新节点并将其作为bucket头部
    node *temp = new_node(obj);
    temp->next = first;
    buckets[n] = temp;
    ++num_elements;
    return pair<iterator, bool>(iterator(temp, this), true);
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::iterator
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey,
          Alloc>::insert_equal_noresize(const value_type &obj) {
    const size_type n = bkt_num(obj);
    node *first = buckets[n];
    for (node *cur = first; cur; cur = cur->next) {
        if (equals(get_key(cur->val), get_key(obj))) {
            node *temp = new_node(obj);
            temp->next = cur->next;
            cur->next = temp;
            ++num_elements;
            return iterator(temp, this);
        }
    }
    node *temp = new_node(obj);
    temp->next = first;
    buckets[n] = temp;
    ++num_elements;
    return iterator(temp, this);
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::reference
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::find_or_insert(
    const value_type &obj) {
    resize(num_elements + 1);

    size_type n = bkt_num(obj);
    node *first = buckets[n];

    for (node *cur = first; cur; cur = cur->next)
        if (equals(get_key(cur->val), get_key(obj))) return cur->val;

    node *temp = new_node(obj);
    temp->next = first;
    buckets[n] = temp;
    ++num_elements;
    return temp->val;
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::iterator
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::find(
    const key_type &key) {
    size_type n = bkt_num_key(key);
    node *first;
    for (first = buckets[n]; first && !equals(get_key(first->val), key);
        first = first->next) {
    }
    return iterator(first, this);
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey,
                   Alloc>::const_iterator
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::find(
    const key_type &key) const {
    size_type n = bkt_num_key(key);
    node *first;
    for (first = buckets[n]; first && !equals(get_key(first->val), key);
        first = first->next) {
    }
    return const_iterator(first, this);
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::size_type
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::count(
    const key_type &key) const {
    const size_type n = bkt_num_key(key);
    size_type result = 0;
    for (const node *cur = buckets[n]; cur; cur = cur->next)
        if (equals(get_key(cur->val), key)) ++result;
    return result;
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
inline pair<typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey,
                               Alloc>::iterator,
            bool>
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::insert_unique(
    const value_type &obj) {
    resize(num_elements + 1);// 判断是否需要扩充
    return insert_unique_noreseize(obj);
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
inline typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey,
                          Alloc>::iterator
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::insert_equal(
    const value_type &obj) {
    resize(num_elements + 1);
    return insert_equal_noresize(obj);
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
template<class InputIterator>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::insert_unique(
    InputIterator first, InputIterator last, input_iterator_tag) {
    for (; first != last; ++first) insert_unique(*first);
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
template<class InputIterator>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::insert_equal(
    InputIterator first, InputIterator last, input_iterator_tag) {
    for (; first != last; ++first) insert_equal(*first);
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
template<class ForwardIterator>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::insert_unique(
    ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
    size_type n = distance(first, last);
    resize(num_elements + n);
    for (; n > 0; --n, ++first) insert_unique_noreseize(*first);
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
template<class ForwardIterator>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::insert_equal(
    ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
    size_type n = distance(first, last);
    resize(num_elements + n);
    for (; n > 0; --n, ++first) insert_equal_noreseize(*first);
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::size_type
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::erase(
    const key_type &key) {
    const size_type n = bkt_num_key(key);
    node *first = buckets[n];
    size_type erased = 0;

    if (first) {
        node *cur = first;
        node *next = cur->next;
        while (next) {
            if (equals(get_key(next->val), key)) {
                cur->next = next->next;
                delete_node(next);
                next = cur->next;
                ++erased;
                --num_elements;
            } else {
                cur = next;
                next = cur->next;
            }
        }
        if (equals(get_key(first->val), key)) {
            buckets[n] = first->next;
            delete_node(first);
            ++erased;
            --num_elements;
        }
    }
    return erased;
}
template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::erase(
    const iterator &pos) {
    node *p = pos.cur;
    if (p) {
        const size_type n = bkt_num(p->val);
        node *cur = buckets[n];

        if (cur == p) {
            buckets[n] = cur->next;
            delete_node(cur);
            --num_elements;
        } else {
            node *next = cur->next;
            while (next) {
                if (next == p) {
                    cur->next = next->next;
                    delete_node(next);
                    --num_elements;
                    break;
                } else {
                    cur = next;
                    next = cur->next;
                }
            }
        }
    }
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::erase(
    iterator first, iterator last) {
    size_type f_bucket = first.cur ? bkt_num(first.cur->val) : buckets.size();
    size_type l_bucket = last.cur ? bkt_num(last.cur->val) : buckets.size();

    if (first.cur == last.cur)
        return;
    else if (f_bucket == l_bucket)
        erase_bucket(f_bucket, first.cur, last.cur);
    else {
        erase_bucket(f_bucket, first.cur, nullptr);
        for (size_type n = f_bucket + 1; n < l_bucket; ++n)
            erase_bucket(n, nullptr);
        if (l_bucket != buckets.size()) erase_bucket(l_bucket, last.cur);
    }
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::erase(
    const const_iterator &pos) {
    erase(
        iterator(const_cast<node *>(pos.cur), const_cast<hashtable *>(pos.ht)));
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::erase(
    const_iterator first, const_iterator last) {
    erase(iterator(const_cast<node *>(first.cur),
                 const_cast<hashtable *>(first.ht)),
        const_cast<node *>(last.cur), const_cast<hashtable *>(last.ht));
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
inline void
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::clear() {
    for (size_type i = 0; i != buckets.size(); ++i) {
        node *cur = buckets[i];
        while (cur) {
            node *next = cur->next;
            delete_node(cur);
            cur = next;
        }
        buckets[i] = nullptr;
    }
    num_elements = 0;
    // clear并没有释放vector
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
inline void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey,
                      Alloc>::copy_from(const hashtable &rhs) {
    buckets.clear();
    buckets.reserve(rhs.buckets.size());
    buckets.insert(buckets.end(), rhs.buckets.size(),
                    static_cast<node *>(nullptr));
    try {
        for (size_type i = 0; i != rhs.buckets.size(); ++i) {
            // 复制每一个vector元素
            if (const node *cur = rhs.buckets[i]) {
                node *copy = new_node(cur->val);
                buckets[i] = copy;
                for (node *next = cur->next; next;
                        cur = next, next = next->next) {
                    copy->next = new_node(next->val);
                    copy = copy->next;
                }
            }
        }
        num_elements = rhs.num_elements;
    } catch (std::exception &) {
        clear();
    }
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
bool operator==(
    const hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> &lhs,
    hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> &rhs) {
    using node = typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey,
                                    Alloc>::node;
    if (lhs.buckets.size() != rhs.buckets.size()) return false;
    for (int n = 0; n < lhs.buckets.size(); ++n) {
        node *cur1 = lhs.buckets[n];
        node *cur2 = rhs.buckets[n];
        for (; cur1 && cur2 && cur1->val == cur2->val;
            cur1 = cur1->next, cur2 = cur2->next) {
        }
        if (cur1 || cur2) return false;
    }
    return true;
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
bool operator!=(
        const hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> &lhs,
        hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> &rhs) {
    return !(lhs == rhs);
}

template<class Value, class Key, class HashFcn, class ExtractKey,
         class EqualKey, class Alloc>
inline void swap(
    hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> &lhs,
    hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> &rhs) noexcept {
    lhs.swap(rhs);
}

}


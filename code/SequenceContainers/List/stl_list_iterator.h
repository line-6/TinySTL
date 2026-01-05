#pragma once

#include "Iterator/stl_iterator.h"
#include "stl_list_node.h"
#include <cstddef>

namespace TinySTL {

template<class T>
struct _list_iterator {
    using self = _list_iterator<T>;
    using link_type = _list_node<T> *;
    using iterator_category = bidirectional_iterator_tag;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using difference_type = ptrdiff_t;

    // data member
    link_type node;

    // ctor
    _list_iterator() {}
    explicit _list_iterator(link_type x) : node(x) {}

    // dtor (trivial)


    bool operator== (const self& rhs) const noexcept{ return node == rhs.node; }
    bool operator!= (const self& rhs) const noexcept{ return node != rhs.node; }

    // deference
    reference operator*() const { return node->data; }

    // member access
    pointer operator->() const { return &(operator*()); }

    // ++i
    self &operator++() {
        node = node->next;
        return *this;
    }
    // i++
    self operator++(int i) {
        self temp = *this;
        ++(*this);
        return temp;
    }

    // --i
    self &operator--() {
        node = node->prev;
        return *this;
    }
    // i--
    self operator--(int i) {
        self temp = *this;
        --(*this);
        return temp;
    }
};

template<class T>
struct _list_const_iterator {
  // alias declarations
  using iterator = _list_iterator<T>;
  using self = _list_const_iterator<T>;
  using link_type = _list_node<T> *;

  using iterator_category = bidirectional_iterator_tag;
  using value_type = T;
  using pointer = const T *;
  using reference = const T &;
  using difference_type = ptrdiff_t;

  // data member
  link_type node;// raw pointer link to list_node

  // ctor
  _list_const_iterator() {}
  explicit _list_const_iterator(link_type x) : node(x) {}
  _list_const_iterator(const iterator &x) : node(x.node) {}

  // dtor(trivial)

  bool operator==(const self &rhs) const noexcept { return node == rhs.node; }
  bool operator!=(const self &rhs) const noexcept { return node != rhs.node; }

  // dererence
  reference operator*() const { return node->data; }

  // member access
  pointer operator->() const { return &(operator*()); }

  // increasement
  self &operator++() {
    node = node->next;
    return *this;
  }

  self operator++(int i) {
    self temp = *this;
    ++(*this);
    return temp;
  }

  // decreasement
  self &operator--() {
    node = node->prev;
    return *this;
  }

  self operator--(int i) {
    self temp = *this;
    --(*this);
    return temp;
  }
};
    
}
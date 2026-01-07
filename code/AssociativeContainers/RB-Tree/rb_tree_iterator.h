#pragma once

#include "Iterator/stl_iterator.h"
#include "rb_tree_node.h"
#include <cstddef>

namespace TinySTL {

struct rb_tree_base_iterator {
    using base_ptr = _rb_tree_node_base::base_ptr;
    using iterator_category = bidirectional_iterator_tag;
    using difference_type = ptrdiff_t;

    base_ptr node;

    // ++
    void increment() {
        if (node->right) { //存在右子，则下一节点必为右子树的最左下角
            node = node->right;
            while(node->left) node = node->left;
        } else { // 不存在右子
            base_ptr p = node->parent;
            while (node == p->right) {  // 不断上溯，找到第一个不为右子的祖先
                node = p;
                p = p->parent;
            }
            if (node->right != p) node = p;
        }
    }
    // --
    void decrement() {
        if (node->color == rb_tree_red && node->parent->parent == node) {
            // 特例，迭代器指向 end() 时触发
            node = node->right;
        } else if (node->left) { // 存在左子，则下一节点必为左子树的最右下角
            base_ptr l = node->left;
            while(l->right) l = l->right;
            node = l;
        } else {
            base_ptr p = node->parent;
            while(node == p->left) {
                node = p;
                p = p->parent;
            }
            node = p;
        }
    }
};

template <class T, class Ref, class Ptr>
struct rb_tree_iterator : public rb_tree_base_iterator {
    using value_type = T;
    using reference = Ref;
    using pointer = Ptr;
    using iterator = rb_tree_iterator<T, T &, T *>;
    using const_iterator = rb_tree_iterator<T, const T &, const T *>;
    using self = rb_tree_iterator<T, Ref, Ptr>;
    using link_type = _rb_tree_node<T> *;

    rb_tree_iterator() {}
    rb_tree_iterator(link_type x) { node = x; }
    rb_tree_iterator(const iterator &it) { node = it.node; }

    reference operator*() const {
        return reinterpret_cast<link_type>(node)->value_field;
    }

    pointer operator->() const { return &(operator*()); }

    self &operator++() {
        increment();
        return *this;
    }

    self operator++(int) {
        self temp = *this;
        increment();
        return temp;
    }

    self &operator--() {
        decrement();
        return *this;
    }

    self operator--(int) {
        self temp = *this;
        decrement();
        return temp;
    }
};

inline bool operator==(const rb_tree_base_iterator &lhs,
                       const rb_tree_base_iterator &rhs) {
  return lhs.node == rhs.node;
}

inline bool operator!=(const rb_tree_base_iterator &lhs,
                       const rb_tree_base_iterator &rhs) {
  return lhs.node != rhs.node;
}

}
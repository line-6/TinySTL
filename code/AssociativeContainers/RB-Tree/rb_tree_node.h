#pragma once

namespace TinySTL {

using rb_tree_color_type = bool;
const rb_tree_color_type rb_tree_red = false;
const rb_tree_color_type rb_tree_black = true;

struct _rb_tree_node_base {
    using color_type = rb_tree_color_type;
    using base_ptr = _rb_tree_node_base *;

    // data member
    color_type color;
    base_ptr parent;
    base_ptr left;
    base_ptr right;

    // get min/max ptr
    static base_ptr minimum(base_ptr root) {
        while(root->left) root = root->left;
        return root;
    }

    static base_ptr maximum(base_ptr root) {
        while(root->right) root = root->right;
        return root;
    }
};

template <class T>
struct _rb_tree_node : public _rb_tree_node_base {
    using link_type = _rb_tree_node<T> *;
    T value_field;
};

}
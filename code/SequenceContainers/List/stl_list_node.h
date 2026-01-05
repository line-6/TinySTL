#pragma once

namespace TinySTL {

template <class T>
struct _list_node {
    T data;
    _list_node* next;
    _list_node* prev;
};

}
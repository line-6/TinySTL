/*
    临时测试文件
*/

#pragma once

#include "stl_iterator.h"
#include <cstddef>

struct Node {
    int val_;
    Node* next_;
    Node(int val, Node* next = nullptr) : val_(val), next_(next) {}
};

class MyForwardIterator 
    : public TinySTL::iterator<TinySTL::forward_iterator_tag, 
                        int> {
public:
private:
    Node* cur;
public:
    explicit MyForwardIterator(Node* p = nullptr) : cur(p) {}

    int& operator* () const {
        return cur->val_;
    }

    MyForwardIterator& operator++ () {
        cur = cur->next_;
        return *this;
    }

    MyForwardIterator operator++ (int) {
        MyForwardIterator temp = *this;
        cur = cur->next_;
        return temp;
    }

    bool operator== (const MyForwardIterator& rhs) const {
        return cur == rhs.cur;
    }

    bool operator!= (const MyForwardIterator& rhs) const {
        return cur != rhs.cur;
    }
};
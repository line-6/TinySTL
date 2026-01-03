/*
    临时测试文件
*/

#include "my_test.h"
#include "stl_iterator.h"
#include <iostream>

int main() {
    Node n0(0, nullptr);
    Node n1(1, &n0);
    Node n2(2, &n1);
    Node n3(3, &n2);

    MyForwardIterator it_begin(&n3);
    MyForwardIterator it_end(nullptr);

    auto d = TinySTL::distance(it_begin, it_end);
    std::cout << "distance = " << d << std::endl;

    return 0;
}
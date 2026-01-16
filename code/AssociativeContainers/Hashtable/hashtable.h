#pragma once

#include "SequenceContainers/Vector/stl_vector.h"
#include "algobase/stl_algobase.h"
#include <cstddef>

namespace TinySTL {
template <class Value>
struct hashtable_node {
    Value val;
    hashtable_node* next;
};

}
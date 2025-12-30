#include "alloc.h"
#include <cstddef>

namespace TinySTL {
    char* _default_alloc::start_free = nullptr;
    char* _default_alloc::end_free = nullptr;
    size_t _default_alloc::heap_size = 0;
    _default_alloc::obj* volatile _default_alloc::free_list[NFREELISTS] = {
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
    };
    
    void (*_malloc_alloc::_malloc_alloc_oom_handler)() = nullptr;
}
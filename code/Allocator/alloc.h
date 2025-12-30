/*
    采用双层级配置器，配置区块超过128B，调用第一级配置器，直接使用malloc和free;
    小于128B时，调用第二级配置器，使用memory pool方式.
    二级配置器设置16个自由链表，分别管理区块大小为8,16,24,...,128bytes的区块，
    注意：freelist保存的是若干个空闲块，allocate时应从对应的freelist拿走，
    deallocate时应把块放回对应的freelist
*/
#pragma once

#include <cstddef>
#include <cstdlib>  // malloc and free
#include <cstring>  // memcpy
#include <new>      // bad_alloc

namespace TinySTL {
// 一级配置器
class _malloc_alloc {
// 以下方法用于处理内存不足的情况
private:
    static void* oom_malloc(size_t);
    static void* oom_realloc(void*, size_t);
    static void (*_malloc_alloc_oom_handler)(); // 函数指针，用于内存分配失败的处理

public:
    static void* allocate(size_t n) {
        void* result = malloc(n);   // 直接使用malloc
        if (result == nullptr) result = oom_malloc(n);
        return result;
    }

    static void deallocate(void* p, size_t /* n */) {
        free(p);    // 直接使用free
    }

    static void* reallocate(void* p, size_t /* old_sz */, size_t new_sz) {
        void* result = realloc(p, new_sz);
        if (result == nullptr) result = oom_realloc(p, new_sz);
        return result;
    }

    // 模拟 set_new_handler
    // 自定义内存分配失败的处理
    static void (* set_malloc_handler(void (*f) ())) () {
        void (* old) () = _malloc_alloc_oom_handler;
        _malloc_alloc_oom_handler = f;
        return (old);
    }
};

inline void* _malloc_alloc::oom_malloc(size_t n) {
    void (* my_malloc_handler)();
    void* result;
    for (;;) {  // 不断尝试释放，配置
        my_malloc_handler = _malloc_alloc_oom_handler;
        if (my_malloc_handler == nullptr) throw std::bad_alloc();
        (*my_malloc_handler)();
        result = malloc(n);
        if (result) return result;
    }
}

inline void* _malloc_alloc::oom_realloc(void* p, size_t n) {
    void (* my_malloc_handler)();
    void* result;
    for (;;) {  // 不断尝试释放，配置
        my_malloc_handler = _malloc_alloc_oom_handler;
        if (my_malloc_handler == nullptr) throw std::bad_alloc();
        (*my_malloc_handler)();
        result = realloc(p, n);
        if (result) return result;
    }
}

// freelist 参数设定
// 区块对齐，区块上限，freelists个数
enum _freelist_setting {
    ALIGN = 8,
    MAX_BYTES = 128,
    NFREELISTS = MAX_BYTES / ALIGN
};

// 二级配置器
class _default_alloc {
private:
    // bytes 对齐
    static size_t ROUND_UP(size_t bytes) {
        return (((bytes) + static_cast<size_t>(ALIGN) - 1) & ~(static_cast<size_t>(ALIGN) - 1));
    }

    // free_list节点
    // union特性导致不需要占用额外内存
    union obj {
        union obj* free_list_link;  //指向下一节点
        char client_data[1];        // 指向资源
    };

    static obj* volatile free_list[NFREELISTS];
    // 根据区块的bytes大小，决定使用第 n 号free_list. n从0算起
    static size_t FREELIST_INDEX(size_t bytes) {
        return (bytes + static_cast<size_t>(ALIGN) - 1) / static_cast<size_t>(ALIGN) - 1;
    }
    // 传回一个大小为n的对象，并且可能加入大小为n的其它区块到free_list  
    static void* refill(size_t n);
    // 配置一大块空间，可容纳nobjs个大小为size的区块
    // 如果不便配置 nobjs可能会降低   
    static char* chunk_alloc(size_t size, int& nobjs);
    //内存池起始位置，只在chunk_alloc()中变化
    static char* start_free;
    //内存池结束位置，只在chunk_alloc()中变化
    static char* end_free;
    static size_t heap_size;

public:
    static void* allocate(size_t n);
    static void deallocate(void* p, size_t n);
    static void* reallocate(void* p, size_t old_sz, size_t new_sz);
};

/*
    当free_list无可用区块时，重新填充空间
    新空间取自内存池，默认获取20个节点(区块)
    若内存池不足，则获取的将小于20
*/
inline void* _default_alloc::refill(size_t n) {
    int nobjs = 20;
    // 尝试调用chunk_alloc,注意nobjs以pass-by-reference传入
    char* chunk = chunk_alloc(n, nobjs);
    obj* volatile *my_free_list;
    obj* result;
    obj *current_obj, *next_obj;
    
    if (nobjs == 1) return chunk;
    my_free_list = free_list + FREELIST_INDEX(n);

    // 在chunk空间内建立freelist
    result = reinterpret_cast<obj*>(chunk);
    // 引导free_list指向内存池分配的空间
    // chunk指向的内存直接分给用户，free_list指向剩下（19或更少）的区块
    *my_free_list = next_obj = reinterpret_cast<obj*>(chunk + n);
    for (int i = 1;; i ++) {
        current_obj = next_obj;
        next_obj = 
            reinterpret_cast<obj*>(reinterpret_cast<char*>(next_obj) + n);
        if (nobjs == i + 1) {
            current_obj->free_list_link = nullptr;
            break;
        } else {
            current_obj->free_list_link = next_obj;
        }
    }
    return result;
}

// 默认size为8的整数倍
inline char* _default_alloc::chunk_alloc(size_t size, int& nobjs) {
    char* result;
    size_t total_bytes = size * nobjs;
    size_t bytes_left = end_free - start_free;  // 内存池剩余空间
    if (bytes_left >= total_bytes) {
        // 容量满足需求
        result = start_free;
        start_free += total_bytes;
        return result;
    } else if (bytes_left >= size) {
        // 容量至少满足一个区块需求
        nobjs = static_cast<int>(bytes_left / size);
        total_bytes = size * nobjs;
        result = start_free;
        start_free += total_bytes;
        return result;
    } else {
        // 内存池一个区块都无法提供
        // 新需求的大小为实际需求量的两倍，再加上一个随配置次数增大而愈来愈大的附加量
        size_t bytes_to_get = 
            2 * total_bytes + ROUND_UP(heap_size >> 4);
        // 以下操作让内存池的残余零头还有利用价值
        if (bytes_left > 0) {
            // 内存池还有零头，先配给适当的freelist
            // 首先寻找适当的freelist
            obj* volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
            reinterpret_cast<obj*>(start_free)->free_list_link = *my_free_list;
            *my_free_list = reinterpret_cast<obj*>(start_free);
        }
        // 配置 heap 空间以补充内存池
        start_free = reinterpret_cast<char*>(malloc(bytes_to_get));
        if (!start_free) {
            // heap 空间不足分配失败
            obj* volatile * my_free_list;
            obj* p;
            // 试着监视我们手上拥有的东西。我们不打算配置较小的区块，
            // 那在多进程机器上容易导致灾难。
            // 以下搜寻适当的 freelist，适当是指
            // "尚有未用区块，且区块够大"的freelist
            for (size_t i = size; i <= static_cast<size_t>(MAX_BYTES);
                i += static_cast<size_t>(ALIGN)) {
                my_free_list = free_list + FREELIST_INDEX(i);
                p = *my_free_list;
                if (p) {
                    // freelist内尚有未用区块
                    // 调整freelist以释出未用区块
                    *my_free_list = p->free_list_link;
                    start_free = reinterpret_cast<char*>(p);
                    end_free = start_free + i;
                    // 递归调用自己，为了修正 nobjs，必然进入 else if 分支
                    return chunk_alloc(size, nobjs);
                }
            }
            end_free = nullptr; // 到处都找不到内存
            // 调用第一级配置器，这会触发 OOM 处理机制或 bad_alloc 异常
            start_free = reinterpret_cast<char*>(_malloc_alloc::allocate(bytes_to_get));
        }
        heap_size += bytes_to_get;  // 已占用的堆内存
        end_free = start_free + bytes_to_get;
        return chunk_alloc(size, nobjs); // 修正nobjs
    }
}   

inline void *_default_alloc::allocate(size_t n) {
    obj* volatile *my_free_list;
    obj* result;
    // n > 128，采用第一级配置器
    if (n > MAX_BYTES) return (_malloc_alloc::allocate(n));
    // 选择采用第几区块
    my_free_list = free_list + FREELIST_INDEX(n);
    result = *my_free_list;
    if (result == nullptr) {
        // 未找到可用free_list，准备填充free_list
        void* r = refill(ROUND_UP(n));
        return r;
    }
    // 调整freelist
    *my_free_list = result->free_list_link;
    return result;
}

inline void _default_alloc::deallocate(void *p, size_t n) {
    if (n > static_cast<size_t>(MAX_BYTES)) {
        _malloc_alloc::deallocate(p, n);
    } else {
        // 寻找对应的freelist
        obj* volatile *my_free_list = free_list + FREELIST_INDEX(n);
        obj* q = reinterpret_cast<obj*>(p);
        // 回收区块，纳入 freelist
        q->free_list_link = *my_free_list;
        *my_free_list = q;
    }
}

inline void *_default_alloc::reallocate(void *p, size_t old_sz, size_t new_sz) {
    void* result;
    size_t copy_sz;
    if (old_sz > MAX_BYTES && new_sz > MAX_BYTES) {
        return realloc(p, new_sz);
    }
    if (ROUND_UP(old_sz) == ROUND_UP(new_sz)) return p;
    result = allocate(new_sz);
    copy_sz = new_sz > old_sz ? old_sz : new_sz;
    memcpy(result, p, copy_sz);
    deallocate(p, old_sz);
    return result;
}

}// namespace TinySTL
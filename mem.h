#ifndef SSC_MEM_H
#define SSC_MEM_H

#include <stdlib.h>

#include "util/List.h"
#include "sys.h"
#include "fmt.h"

namespace ssc {

inline bool is_power_of2(ulen v) {
    return (v & (v-1)) == 0;
}

ulen next_pow_of2(ulen v);

/// The default allocator used with collections. Simply
/// calls to malloc and free.
///
class DynAllocator {
public:
    inline void* alloc(ulen size) {
        return std::malloc(size);
    }
    inline void free(void* ptr) {
        return std::free(ptr);
    }
};

/// A linear allocator. Will expand allocation by chunk_size
/// when out of memory.
///
/// NOTE: free(void*) does nothing. Memory is only cleaned up once
/// the allocator is destroyed (when it's destructor is called).
///
class ArenaAllocator {
     // TODO(maddie): Is this really what the alignment should be?
    const ulen DEFAULT_ALIGNMENT=2*sizeof(void*);
public:   

    ArenaAllocator(ulen chunk_size) :
        chunk_size(chunk_size)
    {}

    template<typename T>
    T* alloc() {
        return alloc<T>(DEFAULT_ALIGNMENT);
    }

    template<typename T>
    T* alloc(ulen align) {
        return (T*) alloc(sizeof(T), align);
    }

    void* alloc(ulen size) {
        return alloc(size, DEFAULT_ALIGNMENT);
    }

    void* alloc(ulen size, ulen align) {
        DBG_ASSERT(is_power_of2(align), "Must align on 2^n boundries");

        if (!cur_chunk)
            alloc_new_chunk();

        uintptr_t aligned_offset=get_aligned_offset(align);
        uintptr_t rel_offset=aligned_offset-(uintptr_t)cur_chunk;

        if (rel_offset+size > chunk_size || !cur_chunk) {
            alloc_new_chunk();
            // Must realign!
            aligned_offset = get_aligned_offset(align);
            rel_offset = aligned_offset-(uintptr_t)cur_chunk;
        }

        offset = rel_offset + size;
        return (void*)aligned_offset;
    }

    void free(void* ptr);

    ~ArenaAllocator();

private:
    uintptr_t get_aligned_offset(ulen align);

    inline void alloc_new_chunk() {
        cur_chunk=std::malloc(chunk_size);
        chunks.add(cur_chunk);
        offset = 0;
    }

    List<void*> chunks;
    void* cur_chunk=nullptr;
    ulen  offset;
    ulen  chunk_size;
};

}

#endif

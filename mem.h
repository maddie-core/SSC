#ifndef SSC_MEM_H
#define SSC_MEM_H

#include <stdlib.h>
#include <vector> // TODO(maddie): replace with custom list

#include "sys.h"
#include "core_types.h"
#include "fmt.h"

namespace ssc {

inline bool is_power_of2(ulen v) {
    return (v & (v-1)) == 0;
}

class ArenaAllocator {
public:   

    ArenaAllocator(ulen chunk_size) :
        chunk_size(chunk_size)
    {}

    template<typename T>
    T* alloc() {
        // TODO(maddie): Is this really what the alignment should be?
        return alloc<T>(2*sizeof(void*));
    }

    template<typename T>
    T* alloc(ulen align) {
        DBG_ASSERT(is_power_of2(align), "Must align on 2^n boundries");

        if (!cur_chunk)
            alloc_new_chunk();

        uintptr_t aligned_offset=get_aligned_offset(align);
        uintptr_t rel_offset=aligned_offset-(uintptr_t)cur_chunk;

        if (rel_offset+sizeof(T) > chunk_size || !cur_chunk) {
            alloc_new_chunk();
            // Must realign!
            aligned_offset = get_aligned_offset(align);
            rel_offset = aligned_offset-(uintptr_t)cur_chunk;
        }

        offset = rel_offset + sizeof(T);
        return (T*)aligned_offset;
    }

    void free(void* ptr);

    ~ArenaAllocator();

private:
    uintptr_t get_aligned_offset(ulen align);

    inline void alloc_new_chunk() {
        cur_chunk=malloc(chunk_size);
        chunks.push_back(cur_chunk);
        offset = 0;
    }

    std::vector<void*> chunks;
    void* cur_chunk=nullptr;
    ulen  offset;
    ulen  chunk_size;
};

}

#endif

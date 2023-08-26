#include "mem.h"

void ssc::ArenaAllocator::free(void* ptr) {
    // Compatibility only
}

uintptr_t ssc::ArenaAllocator::get_aligned_offset(ulen align) {
    uintptr_t pos=(uintptr_t)cur_chunk+offset; 
    uintptr_t r=pos&(align-1);
    if (r) {
        // Not aligned. Aligning
        pos += align - r;
    }
    return pos;
}

ssc::ArenaAllocator::~ArenaAllocator() {
    for (void* chunk : chunks)
        std::free(chunk);
}

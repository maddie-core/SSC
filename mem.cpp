#include "mem.h"

ulen ssc::next_pow_of2(ulen v) {
    --v;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	++v;
	return v;
}

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

#include "fmt.h"
#include "mem.h"

int main() {
    ssc::ArenaAllocator allocator(1024);
    for (ulen i=0; i<10000; i++) {
        int* data=allocator.alloc<int>();
        *data = i;
        ssc::println("data=%s", *data);
    }
}

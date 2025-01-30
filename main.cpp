#include <print>

#include "pool.h"
#include "sus.h"

void* susalloc_dbg(SusAllocator& sus, size_t size, int i) {
    void* ptr = sus.alloc(size);
    SusBlock* block = ((SusBlock*)ptr) - 1;

    std::println("ptr{:d} = {:p}, SusBlock[size={:d}, next={:p}]", i, ptr, block->size, (void*)(block->next));

    return ptr;
}

int main() {
    // sus allocator
    void* ptr1;
    void* ptr2;
    void* ptr3;
    void* ptr4;
    void* ptr5;
    
    {
        SusAllocator sus;
        ptr1 = susalloc_dbg(sus, 512, 1);
        ptr2 = susalloc_dbg(sus, 256, 2);
        ptr3 = susalloc_dbg(sus, 1024, 3);
        ptr4 = susalloc_dbg(sus, 512, 4);

        sus.free(ptr1);
        sus.free(ptr2);
        sus.free(ptr3);
        sus.free(ptr4);

        ptr5 = susalloc_dbg(sus, 128, 5);
    }


    // pool allocator
    PoolAllocator pool(1024, 3);

    void* last = nullptr;
    void* ptr = nullptr;
    for (int i = 0; i < 10; i++) {
        last = ptr;
        ptr = pool.alloc();
        std::println("ptr = {:p} -> {:p}", ptr, *(void**)ptr);
    }

    std::println("\nres:");
    std::println("ptr1 = {:p} -> {:p}", last, *(void**)last);
    std::println("ptr2 = {:p} -> {:p}", ptr, *(void**)ptr);
}
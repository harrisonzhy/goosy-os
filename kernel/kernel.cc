#include "kernel.hh"

using namespace console;
using namespace allocator;

BuddyAllocator k_allocator;
Console k_console;

extern "C" void main() {
    // k_allocator.print_memory_map();
    auto a1 = k_allocator.kmalloc(1 << 27);
    auto a5 = k_allocator.kmalloc(1 << 27);
    auto a2 = k_allocator.kmalloc(1 << 12);
    auto a3 = k_allocator.kmalloc(1 << 27);
    auto a4 = k_allocator.kmalloc(1 << 12);
    auto a6 = k_allocator.kmalloc(1 << 13);
    auto a7 = k_allocator.kmalloc(1 << 15);
    k_allocator.kfree(a6);
    k_allocator.kfree(a1);
    k_allocator.kfree(a2);
    k_allocator.kfree(a4);
    k_allocator.kfree(a7);
    k_allocator.kfree(a3);
    k_allocator.kfree(a5);
    

    // k_console.print(a1, " ", a2, " ", a3, " ", a4, "\n");
    k_allocator.print_memory_map();
}
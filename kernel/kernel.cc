#include "kernel.hh"

using namespace console;
using namespace allocator;

BuddyAllocator k_allocator;
Console k_console;

extern "C" void main() {
    // k_allocator.print_memory_map();
    auto a10 = k_allocator.kmalloc(1 << 27);
    auto a11 = k_allocator.kmalloc(1 << 27);
    k_allocator.kfree(a10);
    k_allocator.kfree(a11);
    // k_console.print("hello", 1, 6, 1);
    k_allocator.print_memory_map();
}
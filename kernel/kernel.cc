#include "kernel.hh"

using namespace console;
using namespace allocator;

BuddyAllocator k_allocator;
Console k_console;

extern "C" void main() {
    // k_allocator.print_memory_map();
    auto a1 = k_allocator.kmalloc(1 << 16);
    auto a2 = k_allocator.kmalloc(8192);
    auto a3 = k_allocator.kmalloc(8192);
    auto a4 = k_allocator.kmalloc(4096);
    // k_allocator.kmalloc(8192);
    
    k_console.print(a1, " ", a2, " ", a3, " ", a4);
    // k_allocator.print_memory_map();
}
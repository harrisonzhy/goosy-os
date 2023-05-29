#include "kernel.hh"

using namespace console;
using namespace allocator;

BuddyAllocator k_allocator;
Console k_console;

extern "C" void main() {
    auto a1 = k_allocator.kmalloc(1 << 14);
    // k_allocator.print_memory_map();

    auto a2 = k_allocator.kmalloc(1 << 14);
    // k_allocator.print_memory_map();

    // auto a3 = k_allocator.kmalloc(1 << 17);
    // // k_allocator.print_memory_map();

    // auto a4 = k_allocator.kmalloc(1 << 18);
    // k_allocator.print_memory_map();

    // u32 const buddy_addr = ((a1 - 0x200000) ^ (1 << 17)) + 0x200000;

    // k_allocator.kfree(a1);
    // k_allocator.kfree(a2);
    // k_allocator.kfree(a3);
    // k_allocator.kfree(a4);
    // k_allocator.print_memory_map();

    // k_allocator.kmalloc(1 << 17);
    // k_allocator.print_memory_map();

    // k_allocator.kmalloc(1 << 17);
    // k_allocator.print_memory_map();

    // k_allocator.kmalloc(1 << 17);
    // k_allocator.print_memory_map();

    // k_allocator.kmalloc(1 << 17);
    // k_allocator.print_memory_map();
}
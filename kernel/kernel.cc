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
    auto a5 = k_allocator.kmalloc(1 << 27);
    auto a6 = k_allocator.kmalloc(1 << 27);
    auto a7 = k_allocator.kmalloc(1 << 27);
    auto a8 = k_allocator.kmalloc(1 << 12);
    auto a9 = k_allocator.kmalloc(1 << 12);
    auto a10 = k_allocator.kmalloc(1 << 27);
    auto a11 = k_allocator.kmalloc(1 << 27);
    k_allocator.kmalloc(1 << 27);
    k_allocator.kmalloc(1 << 27);
    k_allocator.kmalloc(1 << 27);
    k_allocator.kmalloc(1 << 27);
    k_allocator.kmalloc(1 << 27);
    k_allocator.kmalloc(1 << 27);
    k_allocator.kmalloc(1 << 27);
    k_allocator.kmalloc(1 << 27);
    k_allocator.print_memory_map();
}
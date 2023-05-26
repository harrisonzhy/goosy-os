#include "kernel.hh"

using namespace console;
using namespace allocator;

BuddyAllocator k_allocator;
Console k_console;

extern "C" void main() {
    k_allocator.kmalloc(1 << 17);
    k_allocator.print_memory_map();

    k_allocator.kmalloc(1 << 17);
    k_allocator.print_memory_map();

    k_allocator.kmalloc(1 << 17);
    k_allocator.print_memory_map();

    k_allocator.kmalloc(1 << 17);
    k_allocator.print_memory_map();

    k_allocator.kmalloc(1 << 17);
    k_allocator.print_memory_map();

    k_allocator.kmalloc(1 << 17);
    k_allocator.print_memory_map();
}
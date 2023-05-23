#include "kernel.hh"

using namespace console;
using namespace allocator;

    BuddyAllocator b;

extern "C" void main() {
    // const auto addr = reinterpret_cast<Block*>(b.kmalloc(4096));
    for (auto i = 0; i < 100; ++i) {
        // Console::print(i, " ");
    }
    const auto addr = b.kmalloc(4096);
    Console::print("Address: ", addr);
    b.print_memory_map();
}
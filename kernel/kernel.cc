#include "kernel.hh"

using namespace console;
using namespace allocator;

BuddyAllocator B;

extern "C" void main() {
    for (auto i = 0; i < 50; ++i) {
        Console::print(0);
    }
    // B.print_memory_map();
}
#include "kernel.hh"

using namespace console;
using namespace allocator;

BuddyAllocator b;

extern "C" void main() {
    for (auto i = 0; i < 124; ++i) {
        Console::print(185);
    }
    
}
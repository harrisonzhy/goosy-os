#include "kernel.hh"

using namespace console;
using namespace allocator;

extern "C" void main() {
    // Array<int, 20> a;
    //a.fill(124);
    // for (usize i = 0; i < a.len(); ++i) {
    //     Console::print(a[i]);
    // }
    // u8 i = 1;
    // Console::print(i, " ");
    // i |= (1 << 7);
    // Console::print(i, " ");
    // i |= (1 << 7);
    // Console::print(i, " ");

    auto b = BuddyAllocator();
    auto address = b.kalloc(4096);
    Console::print(address);
}
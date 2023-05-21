#include "kernel.hh"

using namespace console;
using namespace allocator;

BuddyAllocator b;

extern "C" void main() {
    Array<int, 0x200000> a;
    a.fill(124);
    for (usize i = 0; i < a.len(); ++i) {
        // Console::print("   ", a[i], "  ");
    }
    // u8 i = 1;
    // Console::print(i, " ");
    // i |= (1 << 7);
    // Console::print(i, " ");
    // i |= (1 << 7);
    // Console::print(i, " ");
    // extern int __bss_end__;
    // uptr bss_end = (uptr) & __bss_end__;
    // Console::print(bss_end);

    auto address = b.kalloc(4096);
    Console::print(address);
    // Console::print(addr);
}
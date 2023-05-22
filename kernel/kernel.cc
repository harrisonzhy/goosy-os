#include "kernel.hh"

using namespace console;
using namespace allocator;

    BuddyAllocator b;

extern "C" void main() {
    const auto addr = reinterpret_cast<Block*>(b.simple_kmalloc(4096));
    Console::print(addr[1].m_data);
}
#include "kernel.hh"

using namespace console;

extern "C" void main() {
    Array<int, 1000> a;
    a.fill(124);
    for (usize i = 0; i < a.len(); ++i) {
        Console::print(a[i]);
    }
}

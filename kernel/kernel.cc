#include "kernel.hh"

using namespace console;

extern "C" void kernel_main() {
    Console::init();
    for (auto i = 0; i < 500; ++i) {
        Console::print(124);
        Console::print("Test text");
        Console::print(124);
    }
}
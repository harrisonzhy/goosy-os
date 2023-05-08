#include "kernel.hh"

using namespace console;

extern "C" void kernel_main() {
    Console::init();
    Console::print((u32)69);
}
#include "kernel.hh"

using namespace console;

extern "C" void kernel_main() {
    Console::init();
    Console::write("Hello world :)");
}
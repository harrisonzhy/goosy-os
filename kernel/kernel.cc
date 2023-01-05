#include "kernel.hh"

extern "C" void kernel_main () {
    console::init();
    console::writestring("Hello world! \n");
}
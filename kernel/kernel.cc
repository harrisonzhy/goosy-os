#pragma once
#include "tty.hh"
#include <../kernel/tty.cc>

extern "C" void kernel_main () {
    console::init();
    console::writestring("Hello world! \n");
}
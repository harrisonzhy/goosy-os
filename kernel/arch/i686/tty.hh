#pragma once
#include "../../../libc/include/int.hh"
#include "../../../libc/include/string.hh"
#include "vga.hh"

namespace console {
    void init ();
    void putchar (char c);
    void write (const char* str, usize size);
    void writestring (const char* str);
}
#pragma once
#include "vga.hh"
#include "int.hh"
#include "string.hh"

namespace console {
    void init ();
    void putchar (char c);
    void write (const char* str, usize size);
    void writestring (const char* str);
}
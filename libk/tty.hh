#pragma once
#include "../libc/int.hh"
#include "vga.hh"

#ifdef __cplusplus
extern "C" {
#endif

namespace console {
    void init ();
    void putchar (char c);
    void write (const char* str, usize size);
    void writestring (const char* str);
}

#ifdef __cplusplus
}
#endif
#pragma once
#include "vga.hh"

#ifdef __cplusplus
extern "C" {
#endif

namespace console {
    class Console {
        public :
            static void init ();
            static void setcolor(u8 color);
            static void scroll();
            static void newline();
            static void putchar (char c);
            static void write (const char* str);
    };
}

#ifdef __cplusplus
}
#endif
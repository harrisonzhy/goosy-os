#pragma once
#include "vga.hh"

namespace console {
    class Console {
        public :
            static void init();
            static void setcolor(u8 color);
            static void scroll();
            static void newline();
            static void putchar(char c);
            static void write(const char* str);

            static void update_cursor(u16 trow, u16 tcolumn);
    };
}
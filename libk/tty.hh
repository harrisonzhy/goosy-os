#pragma once
#include "vga.hh"
#include "ports.hh"
#include "strings.hh"

namespace console {
    class Console {
        public :
            static void update_cursor(u16 trow, u16 tcolumn);
            static void init();
            static void setcolor(u8 color);
            static void scroll();
            static void newline();
            static void putchar(char c);
            static void write(const char* str);

            template<typename T, typename ... Types>
            void print(T&& var1, Types&& ... var2) {
                put(var1);
                print(var2 ...);
            }

            template<usize S, typename ... Types>
            void print(char const (&var1)[S], Types && ... var2) {
                print(str(var1, S), var2 ...);
            }

            // `put(*)' overloads here
            void put(char const c) { putchar(c); }
            void put(u32 num) {}
    };
}
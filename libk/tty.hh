#pragma once
#include "vga.hh"
#include "ports.hh"
#include "strings.hh"

namespace console {
    class Console {
        public :
            static void update_cursor(u16 trow, u16 tcolumn);
            static void update_pos(i8 change);
            static void init();
            static void scroll();
            static void new_line();
            static void write(const char* str);
            static void put_char(const char c);
            static auto num_digits(usize num, u8 base) -> u8;

            // `print(*...)' overloads
            static void print();

            template<typename T, typename... Types>
            static void print(T&& var1, Types&&... var2) {
                put(var1);
                print(var2...);
            }

            template<usize S, typename... Types>
            static void print(const char (&var1)[S], Types&&... var2) {
                print(str(var1, S), var2...);
            }

            template<typename... Types>
            static void print_line(Types&&... var2) {
                print(var2...);
                new_line();
                update_cursor(current_row, current_column);
            }

            // `put(*)' overloads
            static void put(const str string);
            static void put(u32 num);
            static void put(const char c);
            static void put(signed& num);
            static void put(const u16 num);
            static void put(const i16 num);
            static void put(const u8 num);
            static void put(const i8 num);

        private :
            static const isize VGA_WIDTH  = 80;
            static const isize VGA_HEIGHT = 25;
            static u8 current_row;
            static u8 current_column;
            static u16* console_page;
    };
}
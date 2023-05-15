#pragma once
#include "vga.hh"
#include "ports.hh"
#include "strings.hh"

namespace console {
    class Console {
        public :
            Console() {
                current_row = 0;
                current_column = 0;
                console_page = (u16*)(0xB8000);
                for (auto i = 0; i < VGA_HEIGHT; ++i) {
                    for (auto j = 0; j < VGA_WIDTH; ++j) {
                        auto n = i * VGA_WIDTH + j;
                        console_page[n] = vga_entry(' ', VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
                    }
                }
                ports::outb(CURSOR_OFFSET, CURSOR_START);
                ports::outb(CURSOR_CONTROL, CURSOR_SHAPE);
                ports::outb(CURSOR_OFFSET, CURSOR_START);
                // upper two bits are reserved
                auto existing = ports::inb(CURSOR_CONTROL) & 0xC;
                // enable cursor by setting bit 5 to 0, and set start position to 0
                ports::outb(CURSOR_CONTROL, existing);
                ports::outb(CURSOR_OFFSET, CURSOR_END);
                // upper three bits are reserved for cursor end
                existing = ports::inb(CURSOR_CONTROL) & 0xE;
                // set end position to 0xF (block)
                ports::outb(CURSOR_CONTROL, existing);
            }

            static void update_cursor(u16 trow, u16 tcolumn);
            static void update_pos(i8 change);
            static void scroll();
            static void new_line();
            static void write(const char* str);
            static void put_char(const char c);
            static auto num_digits(usize num, u8 base) -> u8;

            // `print(* ...)' overloads
            static void print();

            template<typename T, typename ... Types>
            static void print(T&& var1, Types&& ... var2) {
                put(var1);
                print(var2 ...);
            }

            template<usize S, typename ... Types>
            static void print(const char (&var1)[S], Types&& ... var2) {
                print(str(var1, S), var2 ...);
            }

            template<typename ... Types>
            static void print_line(Types&& ... var2) {
                print(var2 ...);
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
        
            const u16 CURSOR_START   = 0xA;
            const u16 CURSOR_END     = 0xB;
            const u16 CURSOR_SHAPE   = 0x20;
            const u16 CURSOR_CONTROL = 0x3D5;
            const u16 CURSOR_OFFSET  = 0x3D4;
    };
}
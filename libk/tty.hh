#pragma once
#include "vga.hh"
#include "ports.hh"
#include "strings.hh"

namespace console {
    class Console {
        public :
            Console() : current_row(0), current_column(0), console_page((u16*)0xB8000) {
                for (auto i = 0; i < VGA_HEIGHT; ++i) {
                    for (auto j = 0; j < VGA_WIDTH; ++j) {
                        auto const n = i * VGA_WIDTH + j;
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

            // `print(...)' overloads
            void print();

            template<typename T, typename ... Types>
            void print(T&& var1, Types&& ... var2) {
                put(var1);
                print(var2 ...);
            }

            template<usize S, typename ... Types>
            void print(const char (&var1)[S], Types&& ... var2) {
                print(str(var1, S), var2 ...);
            }

            template<typename ... Types>
            void print_line(Types&& ... var2) {
                print(var2 ...);
                new_line();
                update_cursor(current_row, current_column);
            }

            inline __attribute__((always_inline)) void update_cursor(u16 const row, u16 const column) {
                u16 const pos = row * VGA_WIDTH + column;
                ports::outb(0x3D4, 0xF);
                ports::outb(0x3D5, (const u8)(pos & 0xFF));
                ports::outb(0x3D4, 0xE);
                ports::outb(0x3D5, (const u8)((pos >> 8) & 0xFF));
            }

            // `put(...)' overloads
            void put(str const string);
            void put(u32 num);
            void put(char const c);
            void put(signed const& num);
            void put(u16 const num);
            void put(i16 const num);
            void put(u8 const num);
            void put(i8 const num);

            void update_pos(i8 const change);
            void scroll();
            void new_line();
            void write(char* const str);
            void put_char(char const c);
            auto num_digits(usize num, u8 base) -> u8;

        private :
            const isize VGA_WIDTH  = 80;
            const isize VGA_HEIGHT = 25;
            u16 current_row;
            u16 current_column;
            u16* console_page;
        
            const u16 CURSOR_START   = 0xA;
            const u16 CURSOR_END     = 0xB;
            const u16 CURSOR_SHAPE   = 0x20;
            const u16 CURSOR_CONTROL = 0x3D5;
            const u16 CURSOR_OFFSET  = 0x3D4;
    };
}
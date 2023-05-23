#include "tty.hh"



console::Console C;
namespace console {
    inline __attribute__((always_inline)) void Console::update_cursor(u16 row, u16 column) {
        const u16 pos = row * VGA_WIDTH + column;
        ports::outb(0x3D4, 0xF);
        ports::outb(0x3D5, (const u8)(pos & 0xFF));
        ports::outb(0x3D4, 0xE);
        ports::outb(0x3D5, (const u8)((pos >> 8) & 0xFF));
    }

    void Console::scroll() {
        for (auto i = 1; i < VGA_HEIGHT; ++i) {
            for (auto j = 0; j < VGA_WIDTH; ++j) {
                console_page[(i - 1) * VGA_WIDTH + j] = console_page[i * VGA_WIDTH + j];
            }
        }
        for (auto i = 0; i < VGA_WIDTH; ++i) {
            auto n = (VGA_HEIGHT - 1) * VGA_WIDTH + i;
            console_page[n] = vga_entry(' ', VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        }
    }

    void Console::put_char(const char c) {
        if (c == '\n') {
            ++current_row;
            if (current_row == VGA_HEIGHT) {
                current_row = VGA_HEIGHT - 1;
                scroll();
            }
            return;
        }
        console_page[current_row * VGA_WIDTH + current_column] = vga_entry(c, VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        // ++current_column;
        // if (current_column == VGA_WIDTH) {
        //     current_column = 0;
        //     current_row = VGA_HEIGHT - 1;
        //     scroll();
        // }
    }

    void Console::put(const str string) {
        for (const auto& c : string) {
            put_char(c);
        }
    }

    void Console::update_pos(i8 delta) {
        if (current_column == 0 && delta <= 0) {
            return;
        }

        const auto new_column = current_column + delta;
        if (new_column >= current_column && new_column < VGA_WIDTH) {
            current_column = new_column;
        }
        else if (new_column >= VGA_WIDTH) {
            current_column = new_column % VGA_WIDTH;
            current_row = current_row + (new_column - current_column) / VGA_WIDTH;
            if (current_row >= VGA_HEIGHT) {
                current_row = VGA_HEIGHT - 1;
                scroll();
            }
        }
        // TODO: finish this case for backspaces
        else if (new_column < current_column) {
            current_column = new_column;
        }
    }

    auto Console::num_digits(usize num, u8 base) -> u8 {
        u8 digits = 0;
        do {
            ++digits;
            num /= base;
        }
        while (num > 0);
        return digits;
    }

    void Console::put(u32 num) {
        const auto digits = num_digits(num, 10);
        update_pos(digits);
        do {
            const auto digit = static_cast<char>(num % 10 + '0');
            update_pos(-1);
            put_char(digit);
            num /= 10;
        }
        while (num > 0);
        update_pos(digits);
    }

    void Console::print() {
        update_cursor(current_row, current_column);
    }

    void Console::put(const char c)  { put_char(c);   }
    void Console::put(signed& num)   { put(u32(num)); }
    void Console::put(const u16 num) { put(u32(num)); }
    void Console::put(const i16 num) { put(u32(num)); }
    void Console::put(const u8 num)  { put(u32(num)); }
    void Console::put(const i8 num)  { put(u32(num)); }

    u8 Console::current_row;
    u8 Console::current_column;
    u16* Console::console_page;
}
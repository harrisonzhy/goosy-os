#include "tty.hh"

console::Console C;
namespace console {
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

    void Console::new_line() {
        ++current_row;
        if (current_row >= VGA_HEIGHT) {
            current_row = VGA_HEIGHT - 1;
            scroll();
        }
        current_column = 0;
    }

    void Console::put_char(const char c) {
        if (c == '\n') {
            new_line();
            --current_column;
            return;
        }
        auto const n = current_row * VGA_WIDTH + current_column;
        console_page[n] = vga_entry(c, VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    }

    void Console::update_pos(i8 const delta) {
        if (delta > 0) {
            current_column += delta % VGA_WIDTH;
            current_row += delta / VGA_WIDTH;
            if (current_column >= VGA_WIDTH) {
                current_row += current_column / VGA_WIDTH;
                current_column %= VGA_WIDTH;
            }
            if (current_row >= VGA_HEIGHT) {
                current_row = VGA_HEIGHT - 1;
                scroll();
            }
        }
        else if (delta == -1) {
            if (current_column == 0) {
                current_column = VGA_WIDTH - 1;
                --current_row;
            }
            else {
                --current_column;
            }
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
        auto const digits = num_digits(num, 10);
        update_pos(digits);
        do {
            auto const digit = static_cast<char>(num % 10 + '0');
            update_pos(-1);
            put_char(digit);
            num /= 10;
        }
        while (num > 0);
        update_pos(digits);
    }

    void Console::put(str const string) {
        for (auto const& c : string) {
            put_char(c);
            update_pos(1);
        }
    }

    void Console::print() {
        update_cursor(current_row, current_column);
    }

    void Console::put(char const c)  { put_char(c);   }
    void Console::put(signed const& num)   { put(u32(num)); }
    void Console::put(u16 const num) { put(u32(num)); }
    void Console::put(i16 const num) { put(u32(num)); }
    void Console::put(u8 const num)  { put(u32(num)); }
    void Console::put(i8 const num)  { put(u32(num)); }
}
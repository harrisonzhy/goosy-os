#include "tty.hh"

console::Console C;
namespace console {
    namespace {
        const u16 CURSOR_START   = 0xA;
        const u16 CURSOR_END     = 0xB;
        const u16 CURSOR_SHAPE   = 0x20;
        const u16 CURSOR_CONTROL = 0x3D5;
        const u16 CURSOR_OFFSET  = 0x3D4;
    }

    static u8   current_row;
    static u8   current_column;
    static u16* console_page;

    inline __attribute__((always_inline)) void Console::update_cursor(u16 trow, u16 tcolumn) {
        const u16 pos = trow * VGA_WIDTH + tcolumn;
        ports::outb(0x3D4, 0xF);
        ports::outb(0x3D5, (const u8)(pos & 0xFF));
        ports::outb(0x3D4, 0xE);
        ports::outb(0x3D5, (const u8)((pos >> 8) & 0xFF));
    }

    void Console::init() {
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

    void Console::scroll() {
        for (auto i = 0; i < VGA_HEIGHT - 1; ++i) {
            for (auto j = 0; j < VGA_WIDTH; ++j) {
                console_page[i * VGA_WIDTH + j] = console_page[(i + 1) * VGA_WIDTH + j];
            }
        }
        for (auto i = 0; i < VGA_HEIGHT; ++i) {
            auto n = (VGA_HEIGHT - 1) * VGA_WIDTH; 
            console_page[n] = vga_entry(' ', VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        }
    }

    void Console::new_line() {
        current_column = 0;
        ++current_row;
        if (current_row == VGA_HEIGHT) {
            --current_row;
            scroll();
        }
    }

    void Console::update_pos(i8 change) {
        i32 new_column = current_column + change;
        if (new_column < 0) {
            current_column = 0;
            if (current_row > 0) {
                --current_row;
            }
        }
        else if (new_column >= VGA_WIDTH) {
            current_column = new_column - VGA_WIDTH;
            ++current_row;
            if (current_row == VGA_HEIGHT) {
                scroll();
            }
            else {
                current_column = new_column;
            }
        }
    }

    void Console::put_char(const char c) {
        if (c == '\n') {
            new_line();
            return;
        }
        console_page[current_row * VGA_WIDTH + current_column] = vga_entry(c, VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        ++current_column;
        if (current_column == VGA_WIDTH) {
            current_column = 0;
        }
        if (current_row == VGA_HEIGHT) {
            new_line(); // `newline()' changes `current_row' and scrolls
        }
    }

    void Console::putback_char(const char c) {
        console_page[current_row * VGA_WIDTH + current_column] = vga_entry(c, VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        if (current_column == 0) {
            if (current_row != 0) {
                current_column = VGA_WIDTH - 1;
                --current_row;
            }
        }
        else {
            --current_column;
        }
    }

    void Console::write(const char* str) {
        auto i = 0;
        while (str[i]) {
            put_char(str[i]);
            update_cursor(current_row, current_column);
            ++i;
        }
    }

    auto Console::num_digits(usize num, u8 base) -> u8 {
        u8 digits = 0;
        do {
            num /= base;
            ++digits;
        }
        while (num > 0);
        return digits;
    }

    // `print(*)' overloads

    void Console::print() {
        update_cursor(current_row, current_column);
    }

    template<typename T, typename... Types>
    void Console::print(T&& var1, Types&&... var2) {
        put(var1);
        print(var2...);
    }

    template<usize S, typename... Types>
    void Console::print(const char (&var1)[S], Types&&... var2) {
        print(str(var1, S), var2...);
    }

    template<typename... Types>
    void Console::print_line(Types&&... var2) {
        print(var2...);
        new_line();
        update_cursor(current_row, current_column);
    }

    // `put(*)' overloads

    void Console::put(const str string) {
        for (const auto c : string) {
            put_char(c);
        }
    }

    void Console::put(u32 num) {
        const auto digits = num_digits(num, 10);
        update_pos(digits - 1);
        do {
            const auto digit = char(num % 10 + '0');
            putback_char(digit);
            num /= 10;
        }
        while (num > 0);
        update_pos(digits + 1);
    }

    void Console::put(const char c)  { put_char(c);   }
    void Console::put(signed& num)   { put(u32(num)); }
    void Console::put(const u16 num) { put(u32(num)); }
    void Console::put(const i16 num) { put(u32(num)); }
    void Console::put(const u8 num)  { put(u32(num)); }
    void Console::put(const i8 num)  { put(u32(num)); }
}
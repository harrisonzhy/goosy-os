namespace console {
    inline __attribute__((always_inline)) void Console::update_cursor(u16 row, u16 column) {
        const u16 pos = row * VGA_WIDTH + column;
        ports::outb(0x3D4, 0xF);
        ports::outb(0x3D5, (const u8)(pos & 0xFF));
        ports::outb(0x3D4, 0xE);
        ports::outb(0x3D5, (const u8)((pos >> 8) & 0xFF));
    }

    void Console::scroll() {
        for (auto i = 0; i < VGA_HEIGHT - 1; ++i) {
            for (auto j = 0; j < VGA_WIDTH; ++j) {
                console_page[i * VGA_WIDTH + j] = console_page[(i + 1) * VGA_WIDTH + j];
            }
        }
        for (auto i = 0; i < VGA_HEIGHT - 1; ++i) {
            auto n = VGA_HEIGHT * VGA_WIDTH + i;
            console_page[n] = vga_entry(' ', VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        }
    }

    void Console::new_line() {
        current_column = 0;
        if (current_row >= VGA_HEIGHT) {
            --current_row;
            scroll();
        }
    }

    void Console::put_char(const char c) {
        if (c == '\n') {
            new_line();
            return;
        }
        console_page[current_row * VGA_WIDTH + current_column] = vga_entry(c, VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        ++current_column;
        // if (current_column == VGA_WIDTH) {
        //     current_column = 0;
        // }
        // if (current_row == VGA_HEIGHT) {
        //     new_line(); // `newline()' changes `current_row' and scrolls
        // }
        update_pos(1);
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

    void Console::put(const str string) {
        for (const auto &c : string) {
            put_char(c);
            update_pos(1);
        }
    }

    void Console::update_pos(i8 change) {
        if (change == 0) {
            return;
        }
        const i32 new_column = current_column + change - 1;
        
        if (new_column >= 0 && new_column < VGA_WIDTH) {
            current_column = new_column;
        }
        else if (new_column >= VGA_WIDTH) {
            current_column = new_column % VGA_WIDTH;
            ++current_row;
            if (current_row >= VGA_HEIGHT) {
                --current_row;
                scroll();
            }
        }
        else if (new_column < 0) {
            if (current_row > 0) {
                current_column = VGA_WIDTH - 1;
                --current_row;
            }
        }
    }

    void Console::put(u32 num) {
        Array<char, 10> a;
        a.fill((char)0);

        const auto digits = num_digits(num, 10);
        int i = 0;
        while (num > 0) {
            const auto digit = char(num % 10 + '0');
            a[i] = digit;
            // put_char(digit);
            ++i;
            num /= 10;
        }
        // while (num > 0);

        for (auto j = 0; j < 10; ++j) {
            put_char(a[j]);
        }


        update_pos(0);
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
#include "tty.hh"
#include "../libc/string.hh"

#ifdef __cplusplus
extern "C" {
#endif

namespace console {
    namespace {
        const isize VGA_WIDTH  = 80;
        const isize VGA_HEIGHT = 25;
        isize    trow;
        isize    tcolumn;
        u8       tcolor;
        u16*     tbuffer;
    }

    void Console::init () {
        trow = 0;
        tcolumn = 0;
        tcolor = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        tbuffer = (u16*) 0xB8000;
        for (auto i = 0; i < VGA_HEIGHT; ++i) {
            for (auto j = 0; j < VGA_WIDTH; ++j) {
                auto n = i * VGA_WIDTH + j;
                tbuffer[n] = vga_entry(' ', tcolor);
            }
        }
    }

    void Console::setcolor (u8 color) {
        tcolor = color;
    }

    void Console::scroll () {
        for (auto i = 0; i < VGA_HEIGHT - 1; ++i) {
            for (auto j = 0; j < VGA_WIDTH; ++j) {
                tbuffer[i * VGA_WIDTH + j] = tbuffer[(i + 1) * VGA_WIDTH + j];
            }
        }
        for (auto i = 0; i < VGA_HEIGHT; ++i) {
            tcolor = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
            auto n = (VGA_HEIGHT - 1) * VGA_WIDTH; 
            tbuffer[n] = vga_entry(' ', tcolor);
        }
    }

    void Console::newline () {
        tcolumn = 0;
        ++trow;
        if (trow == VGA_HEIGHT) {
            --trow;
            Console::scroll();
        }
    }

    void Console::putchar (const char c) {
        if (c == '\n') {
            Console::newline();
            return;
        }
        tbuffer[trow * VGA_WIDTH + tcolumn] = vga_entry((unsigned char)c, tcolor);
        ++tcolumn;
        if (tcolumn == VGA_WIDTH) {
            tcolumn = 0;
        }
        if (trow == VGA_HEIGHT) {
            Console::newline(); // `newline()' changes `trow' and scrolls
        }
    }

    void Console::write (const char* str) {
        auto i = 0;
        while (str[i]) {
            Console::putchar(str[i]);
            ++i;
        }
    }
}

#ifdef __cplusplus
}
#endif

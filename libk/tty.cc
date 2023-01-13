#include "tty.hh"
#include "../libc/string.hh"

#ifdef __cplusplus
extern "C" {
#endif

namespace console {
    namespace {
        const usize VGA_WIDTH  = 80;
        const usize VGA_HEIGHT = 25;
        usize    trow;
        usize    tcolumn;
        u8       tcolor;
        u16*     tbuffer;
    }

    void init () {
        trow = 0;
        tcolumn = 0;
        tcolor = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        tbuffer = (u16*)0xB8000;
        for (auto i = 0; i != VGA_HEIGHT; ++i) {
            for (auto j = 0; j != VGA_WIDTH; ++j) {
                auto n = i*VGA_WIDTH+j;
                tbuffer[n] = vga_entry(' ', tcolor);
            }
        }
    }

    void setcolor (u8 color) {
        tcolor = color;
    }

    void scroll () {
        for (auto i = 0; i != VGA_HEIGHT-1; ++i) {
            for (auto j = 0; j != VGA_WIDTH; ++j) {
                tbuffer[i*VGA_WIDTH+j] = tbuffer[(i+1)*VGA_WIDTH+j];
            }
        }
        for (auto k = 0; k != VGA_HEIGHT; ++k) {
            tcolor = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
            auto n = (VGA_HEIGHT-1)*VGA_WIDTH; 
            tbuffer[n] = vga_entry(' ', tcolor);
        }
    }

    void newline () {
        tcolumn = 0;
        ++trow;
        if (trow == VGA_HEIGHT) {
            --trow;
            scroll();
        }
    }

    void putchar (char c) {
        if (c == '\n') {
            newline();
            return;
        }
        tbuffer[trow*VGA_WIDTH+tcolumn] = vga_entry((unsigned char)c, tcolor);
        ++tcolumn;
        if (tcolumn == VGA_WIDTH) {
            tcolumn = 0;
        }
        if (trow == VGA_HEIGHT) {
            newline(); // already changes `trow` and scrolls
        }
    }

    void write (const char* data, usize len) {
        for (auto i = 0; i != (int)len; ++i) {
            putchar(data[i]);
        }
    }

    void writestring (const char* str) {
        write(str, strlen(str));
    }
}

#ifdef __cplusplus
}
#endif
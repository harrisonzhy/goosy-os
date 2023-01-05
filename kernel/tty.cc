#include "tty.hh"

namespace console {
    namespace {
        const size_t VGA_WIDTH = 80;
        const size_t VGA_HEIGHT = 25;
        const uint16_t VGA_MEMORY = 0xB8000;
        size_t    trow;
        size_t    tcolumn;
        uint8_t   tcolor;
        uint16_t* tbuffer;
    }

    void init () {
        trow = 0;
        tcolumn = 0;
        tcolor = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        tbuffer = (uint16_t*)VGA_MEMORY;
        for (auto i = 0; i != VGA_HEIGHT; ++i) {
            for (auto j = 0; j != VGA_WIDTH; ++j) {
                auto n = i*VGA_WIDTH + j;
                tbuffer[n] = vga_entry(' ', tcolor);
            }
        }
    }

    void setcolor (uint8_t color) {
        tcolor = color;
    }

    void scroll () {
        for (auto i = 0; i != VGA_WIDTH-1; ++i) {
            for (auto j = 0; j != VGA_HEIGHT; ++j) {
                tbuffer[i*VGA_WIDTH+j] = tbuffer[(i+1)*VGA_WIDTH+j];
            }
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

    void putchar (int8_t c) {
        if (c == '\n') {
            newline();
            return;
        }
        tbuffer[trow*VGA_WIDTH + tcolumn] = vga_entry((uint8_t)c, tcolor);
        ++tcolumn;
        if (tcolumn == VGA_WIDTH) {
            tcolumn = 0;
        }
        if (trow == VGA_HEIGHT) {
            newline(); // already increments/decrements trow and scrolls
        }
    }

    void write (const int8_t* data, size_t size) {
        for (auto i = 0; i != size; ++i) {
            putchar(data[i]);
        }
    }

    void writestring (const int8_t* str) {
        write(str, strlen(str));
    }
}
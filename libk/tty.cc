#include "tty.hh"
#include "ports.hh"

namespace console {
    namespace {
        const isize VGA_WIDTH  = 80;
        const isize VGA_HEIGHT = 25;
        u8   trow;
        u8   tcolumn;
        u8   tcolor;
        u16* console_page;
    }

    namespace {
        const u16 CURSOR_START   = 0xA;
        const u16 CURSOR_END     = 0xB;
        const u16 CURSOR_SHAPE   = 0x20;
        const u16 CURSOR_CONTROL = 0x3D5;
        const u16 CURSOR_OFFSET  = 0x3D4;
    }
    
    void Console::update_cursor(u16 trow, u16 tcolumn) {
        const u16 pos = trow * VGA_WIDTH + tcolumn;
        ports::outb(0x3D4, 0x0F);
        ports::outb(0x3D5, (const u8)(pos & 0xFF));
        ports::outb(0x3D4, 0x0E);
        ports::outb(0x3D5, (const u8)((pos >> 8) & 0xFF));
    }

    auto get_cursor_position() -> u16 {
        u16 pos = 0;
        ports::outb(0x3D4, 0x0F);
        pos |= ports::inb(0x3D5);
        ports::outb(0x3D4, 0x0E);
        pos |= ((u16) ports::inb(0x3D5)) << 8;
        return pos;
    }

    void Console::init() {
        trow = 0;
        tcolumn = 0;
        tcolor = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        console_page = (u16*)(0xB8000);
        for (auto i = 0; i < VGA_HEIGHT; ++i) {
            for (auto j = 0; j < VGA_WIDTH; ++j) {
                auto n = i * VGA_WIDTH + j;
                console_page[n] = vga_entry(' ', tcolor);
            }
        }
        ports::outb(CURSOR_OFFSET, CURSOR_START);
        ports::outb(CURSOR_CONTROL, CURSOR_SHAPE);
        ports::outb(CURSOR_OFFSET, CURSOR_START);
        // upper two bits are reserved
        auto existing = ports::inb(CURSOR_CONTROL) & 0xC;
        // enable cursor by setting bit 5 to 0 and set start position to 0
        ports::outb(CURSOR_CONTROL, existing);
        ports::outb(CURSOR_OFFSET, CURSOR_END);
        // upper three bits are reserved for cursor end
        existing = ports::inb(CURSOR_CONTROL) & 0xE;
        // set end position to 0xF
        ports::outb(CURSOR_CONTROL, existing);
    }

    void Console::setcolor(u8 color) {
        tcolor = color;
    }

    void Console::scroll() {
        for (auto i = 0; i < VGA_HEIGHT - 1; ++i) {
            for (auto j = 0; j < VGA_WIDTH; ++j) {
                console_page[i * VGA_WIDTH + j] = console_page[(i + 1) * VGA_WIDTH + j];
            }
        }
        for (auto i = 0; i < VGA_HEIGHT; ++i) {
            tcolor = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
            auto n = (VGA_HEIGHT - 1) * VGA_WIDTH; 
            console_page[n] = vga_entry(' ', tcolor);
        }
    }

    void Console::newline() {
        tcolumn = 0;
        ++trow;
        if (trow == VGA_HEIGHT) {
            --trow;
            Console::scroll();
        }
    }

    void Console::putchar(const char c) {
        if (c == '\n') {
            Console::newline();
            return;
        }
        console_page[trow * VGA_WIDTH + tcolumn] = vga_entry((unsigned char)c, tcolor);
        ++tcolumn;
        if (tcolumn == VGA_WIDTH) {
            tcolumn = 0;
        }
        if (trow == VGA_HEIGHT) {
            Console::newline(); // `newline()' changes `trow' and scrolls
        }
    }

    void Console::write(const char* str) {
        auto i = 0;
        while (str[i]) {
            Console::putchar(str[i]);
            Console::update_cursor(trow, tcolumn);
            ++i;
        }
    }
}

#pragma once
#include <../kernel/vga.hh>
#include <../libk/int.hh>
#include <../libk/string.hh>

namespace console {
    void init ();
    void putchar (char c);
    void write (const char* str, usize size);
    void writestring (const char* str);
}
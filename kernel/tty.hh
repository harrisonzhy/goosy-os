#pragma once
#include <../kernel/vga.hh>
#include <../libk/int.hh>
#include <../libk/string.hh>

namespace console {
    void init ();
    void putchar (i8 c);
    void write (const i8* str, usize size);
    void writestring (const i8* str);
}
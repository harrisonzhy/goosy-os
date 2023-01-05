#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <../kernel/vga.hh>
#include <../libk/string.hh>

namespace console {
    void init ();
    void putchar (int8_t c);
    void write (const int8_t* str, size_t size);
    void writestring (const int8_t* str);
}
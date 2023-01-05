#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <../kernel/vga.hh>
#include <../libk/string.hh>

namespace console {
    void init ();
    void putchar (char c);
    void write (const char* str, size_t size);
    void writestring (const char* str);
}
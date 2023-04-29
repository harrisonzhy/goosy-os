#include "stdio.hh"

__attribute__((__noreturn__))
void abort () {
    asm volatile("hlt");
    while (1);
    __builtin_unreachable();
}

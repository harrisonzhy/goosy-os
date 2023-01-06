#include "stdio.hh"

__attribute__((__noreturn__))
void abort () {
    // printf("KERNEL: PANIC -> abort()\n");
    asm volatile("hlt");

    while (1);
    __builtin_unreachable();
}
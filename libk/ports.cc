#include "ports.hh"

namespace ports {
    void outb(u16 port, u8 val) {
        __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
    }

    auto inb(u16 port) -> u8 {
        u8 ret;
        __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
        return ret;
    }

    void io_wait() {
        __asm__ __volatile__("outb %%al, $0x80" : : "a"(0));
    }
}
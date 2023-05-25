#include "ports.hh"

namespace ports {
    void outb(u16 port, u8 val) {
        asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
    }

    auto inb(u16 port) -> u8 {
        u8 ret = 0;
        asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
        return ret;
    }

    void io_wait() {
        outb(0x80, 0);
    }
}
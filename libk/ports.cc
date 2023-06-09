#include "ports.hh"

namespace ports {
    void outb(u16 port, u8 val) {
        __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
    }

    auto inb(u16 port) -> u8 {
        u8 ret = 0;
        __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
        return ret;
    }

    void io_wait() {
        outb(0x80, 0);
    }
}
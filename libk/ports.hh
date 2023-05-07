#pragma once
#include "int.hh"

namespace ports {
    void outb(u16 port, u8 val);
    auto inb(u16 port) -> u8;
    void io_wait();
}
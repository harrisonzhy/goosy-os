#include "../libk/pic.hh"

using namespace pic;

void Pic::remap() {
    // save masks
    u8 const master_mask = ports::inb(PIC_1_DATA);
    u8 const slave_mask = ports::inb(PIC_2_DATA);

    // ICW1 : initialize in cascade mode
    ports::outb(PIC_1_CMD, ICW1_INIT | ICW1_ICW4);
    ports::io_wait();
    ports::outb(PIC_2_CMD, ICW1_INIT | ICW1_ICW4);
    ports::io_wait();

    // ICW2 : set master and slave vector offsets
    ports::outb(PIC_1_DATA, PIC1_OFFSET);
    ports::io_wait();
    ports::outb(PIC_2_DATA, PIC2_OFFSET);
    ports::io_wait();

    // ICW3 : slave at IRQ 0x2 (0b100) with cascade ID 0b10
    ports::outb(PIC_1_DATA, 0b100);
    ports::io_wait();
    ports::outb(PIC_2_DATA, 0b10);
    ports::io_wait();

    // ICW4 : use 8086 mode
    ports::outb(PIC_1_DATA, ICW4_8086);
    ports::io_wait();
    ports::outb(PIC_2_DATA, ICW4_8086);
    ports::io_wait();

    // restore prior masks
    ports::outb(PIC_1_DATA, master_mask);
    ports::outb(PIC_2_DATA, slave_mask);
}

void Pic::send_eoi(u8 const irq) {
    if (irq >= 0x8) {
        ports::outb(PIC_2_CMD, PIC_EOI);
    }
    ports::outb(PIC_1_CMD, PIC_EOI);
}

void Pic::disable() {
    ports::outb(PIC_2_DATA, PIC_DISABLE);
    ports::outb(PIC_1_DATA, PIC_DISABLE);
}

void Pic::set_irq_mask(u8 irq) {
    u16 port;
    u8 val;
    if (irq < 0x8) {
        port = PIC_1_DATA;
    }
    else {
        irq -= 0x8;
        port = PIC_2_DATA;
    }
    val = ports::inb(port) | (1 << irq);
    ports::outb(port, val);
}

void Pic::clear_irq_mask(u8 irq) {
    u16 port;
    u8 val;
    if (irq < 0x8) {
        port = PIC_1_DATA;
    }
    else {
        port = PIC_2_DATA;
        irq -= 0x8;
    }
    val = ports::inb(port) & ~(1 << irq);
    ports::outb(port, val);
}

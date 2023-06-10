#pragma once
#include "../libk/ports.hh"

u16 static constexpr const PIC_1_CMD    = 0x20;
u16 static constexpr const PIC_1_DATA   = 0x21;
u16 static constexpr const PIC_2_CMD    = 0xA0;
u16 static constexpr const PIC_2_DATA   = 0xA1;

u16 static constexpr const PIC1_OFFSET  = 0x20; // map master IRQ [0x0-0x7] -> [0x20-0x27]
u16 static constexpr const PIC2_OFFSET  = 0x28; // map slave IRQ [0x8-0x15] -> [0x28-0x2F]
u16 static constexpr const PIC_EOI      = 0x20;
u16 static constexpr const PIC_DISABLE  = 0xFF;

u16 static constexpr const ICW1_ICW4    = 0x01; // ICW4 is present
u16 static constexpr const ICW1_CASCADE = 0x02; // cascade mode
u16 static constexpr const ICW1_INTV4   = 0x04; // call address interval
u16 static constexpr const ICW1_LEVEL   = 0x08; // level triggered mode
u16 static constexpr const ICW1_INIT    = 0x10; // initialize
u16 static constexpr const ICW4_8086    = 0x01; // 8086/88 (MCS-80/85) mode

namespace pic {
    class Pic {
        public :
            Pic(Pic const& _) = delete;
            Pic() {
                disable();
                remap();

                __asm__ __volatile__("sti");
            }

            void remap() {
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

            void send_eoi(u8 const irq) {
                if (irq >= 0x8) {
                    ports::outb(PIC_2_CMD, PIC_EOI);
                }
                ports::outb(PIC_1_CMD, PIC_EOI);
            }

            void disable() {
                ports::outb(PIC_2_DATA, PIC_DISABLE);
                ports::outb(PIC_1_DATA, PIC_DISABLE);
            }

            // mask and ignore the IRQ by setting bit `irq' in IMR
            void set_irq_mask(u8 irq) {
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

            // unmask the IRQ by clearing bit `irq' in IMR
            void clear_irq_mask(u8 irq) {
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
    };
}
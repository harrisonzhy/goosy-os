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

            void remap();

            void send_eoi(u8 const irq);

            void disable();

			// mask and ignore the IRQ by setting bit `irq' in IMR
            void set_irq_mask(u8 irq);

            // unmask the IRQ by clearing bit `irq' in IMR
            void clear_irq_mask(u8 irq);
	};
}

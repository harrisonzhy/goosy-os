#pragma once
#include "../libk/tty.hh"
#include "../libk/int.hh"
#include "../libk/array.hh"

auto constexpr const TRAP_GATE_FLAGS      = 0x8F; // P = 1, DPL = 0b00, S = 0, type = 0b1111
auto constexpr const INT_GATE_FLAGS       = 0x8E; // P = 1, DPL = 0b00, S = 0, type = 0b1110
auto constexpr const INT_GATE_FLAGS_USER  = 0xEE; // P = 1, DPL = 0b11, S = 0, type = 0b1110
auto constexpr const PAGESIZE = 0x1000;

using namespace console;
extern Console k_console;

namespace idt {
    class IdtEntry {
        public :
            IdtEntry(IdtEntry const& _) = delete;
            IdtEntry() : m_isr_lower(0), m_selector(0), m_reserved(0), m_flags(0), m_isr_upper(0) {}
            
            u16 m_isr_lower;
            u16 m_selector;
            u8 m_reserved;
            u8 m_flags;
            u16 m_isr_upper;
    }__attribute__((packed));

    class IdtRegister {
        public :
            IdtRegister(IdtRegister const& _) = delete;
            IdtRegister() {}

            u16 m_limit;
            u32 m_base;
    }__attribute__((packed));

    class IdtFrame {
        public :
            IdtFrame(IdtFrame const& _) = delete;
            IdtFrame() {}
            
            u32 eip;
            u32 cs;
            u32 eflags;
            u32 esp;
            u32 ss;
    }__attribute__((packed));

    // default exception handler
    __attribute__((interrupt)) void handle_default_excp(IdtFrame const& frame) {
        k_console.print("EXCEPTION HANDLED, ERRNO: null\n");
    }

    // default exception handler with error code
    __attribute__((interrupt)) void handle_default_excp_errno(IdtFrame const& frame, u32 const errno) {
        k_console.print("EXCEPTION HANDLED, ERRNO: ", errno, "\n");
    }

    // default interrupt handler
    __attribute__((interrupt)) void handle_default_int(IdtFrame const& frame) {
        k_console.print("INTERRUPT HANDLED\n");
    }

    class Idt {
        public :
            Idt(Idt const& _) = delete;
            Idt() {
                _idtr.m_limit = static_cast<u16 const>(8 * NUM_ENTRIES);
                _idtr.m_base = reinterpret_cast<u32 const>(&_entries[0]);

                Array<u8, 0x8> excp_errno_entries = {8, 10, 11, 12, 13, 14, 17, 21};
                for (u8 i = 0; i < 0x20; ++i) {
                    auto errno_entry = false;
                    for (u8 j = 0; j < excp_errno_entries.len(); ++j) {
                        errno_entry += (i == excp_errno_entries[j]);
                    }
                    if (!errno_entry) {
                        add_isr(i, reinterpret_cast<void*>(handle_default_excp), TRAP_GATE_FLAGS);
                    }
                    else {
                        add_isr(i, reinterpret_cast<void*>(handle_default_excp_errno), TRAP_GATE_FLAGS);
                    }
                }
                for (u16 i = 0x20; i < NUM_ENTRIES; ++i) {
                    add_isr(i, reinterpret_cast<void*>(handle_default_int), INT_GATE_FLAGS);
                }

                __asm__ __volatile__("lidt %0" : : "memory"(_idtr));
            }

            void add_isr(u8 const entry_num, void const* isr, u8 const flags) {
                auto descriptor = &_entries[entry_num];
                descriptor->m_isr_lower = reinterpret_cast<u32 const>(isr) & 0xFFFF;
                descriptor->m_selector = 0x08;
                descriptor->m_reserved = 0x0;
                descriptor->m_flags = flags;
                descriptor->m_isr_upper = (reinterpret_cast<u32 const>(isr) >> 16) & 0xFFFF;
            }

            IdtEntry& operator [] (usize const idx) { return _entries[idx]; }
            IdtEntry const& operator [] (usize const idx) const { return _entries[idx]; }

        private :
            usize static constexpr const NUM_ENTRIES = 0x100;
            Array<IdtEntry, NUM_ENTRIES> _entries;
            IdtRegister _idtr;
    }__attribute__((aligned(PAGESIZE)));
};
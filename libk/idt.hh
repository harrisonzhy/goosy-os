#pragma once
#include "tty.hh"
#include "int.hh"
#include "array.hh"

auto constexpr const TRAP_GATE_FLAGS     = 0x8F; // P = 1, DPL = 0b00, S = 0, type = 0b1111
auto constexpr const INT_GATE_FLAGS      = 0x8E; // P = 1, DPL = 0b00, S = 0, type = 0b1110
auto constexpr const INT_GATE_FLAGS_USER = 0xEE; // P = 1, DPL = 0b11, S = 0, type = 0b1110
auto constexpr const PAGESIZE            = 0x1000;

using namespace console;
extern Console k_console;

namespace idt {
    class IdtEntry {
        public :
            constexpr IdtEntry() : _isr_low(0), _selector(0x8), _reserved(0x0), _flags(0), _isr_high(0) {}

			void set(u8 const entry_num, void const* isr, u8 const flags) {
                _isr_low = reinterpret_cast<u32 const>(isr) & 0xFFFF;
                _flags = flags;
                _isr_high = (reinterpret_cast<u32 const>(isr) >> 16) & 0xFFFF;
            }
        
		private :
            u16 _isr_low;
            u16 _selector;
            u8 _reserved;
            u8 _flags;
            u16 _isr_high;
    }__attribute__((aligned(8), packed));

    class IdtRegister {
        public :
            constexpr IdtRegister(IdtRegister const& _) = delete;
            constexpr IdtRegister() : _limit(0), _base(0) {}

			constexpr void set_limit(u16 const limit) {
				_limit = limit;
			}

			constexpr void set_base(u32 const base) {
				_base = base;
			}

		private :
            u16 _limit;
            u32 _base;
    }__attribute__((packed));

    class IdtFrame {
        public :
            constexpr IdtFrame(IdtFrame const& _) = delete;
            constexpr IdtFrame() : _eip(0), _cs(0), _eflags(0), _esp(0), _ss(0) {}
            
            u32 _eip;
            u32 _cs;
            u32 _eflags;
            u32 _esp;
            u32 _ss;
    }__attribute__((packed));

    void print_stack_frame(IdtFrame const& frame) {
        k_console.switch_color(Console::DEFAULT_FG, Console::DEFAULT_BG);
        k_console.print_line("\%eip   ", reinterpret_cast<u32 const*>(frame._eip));
        k_console.print_line("\%cs    ", reinterpret_cast<u32 const*>(frame._cs));
        k_console.print_line("eflags ", reinterpret_cast<u32 const*>(frame._eflags));
        k_console.print_line("\%esp   ", reinterpret_cast<u32 const*>(frame._esp));
        k_console.print_line("\%ss    ", reinterpret_cast<u32 const*>(frame._ss));
    }

    // [0x00] #DE exception handler
    __attribute__((interrupt)) void handle_div_err(IdtFrame& frame) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x00] | ERRCODE 0x??"); 
        print_stack_frame(frame);
        while (true);
    }
    
    // [0x01] #DB exception handler
    __attribute__((interrupt)) void handle_debug(IdtFrame& frame) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x01] | ERRCODE 0x??"); 
        print_stack_frame(frame);
        while (true);
    }
    
    // [0x02] #-- exception handler
    __attribute__((interrupt)) void handle_nonmaskable_int(IdtFrame& frame) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x02] | ERRCODE 0x??"); 
        print_stack_frame(frame);
        while (true);
    }
    
    // [0x03] #BP exception handler
    __attribute__((interrupt)) void handle_breakpoint(IdtFrame& frame) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x03] | ERRCODE 0x??"); 
        print_stack_frame(frame);
        while (true);
    }

    // [0x04] #OF exception handler
    __attribute__((interrupt)) void handle_overflow(IdtFrame& frame) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x04] | ERRCODE 0x??"); 
        print_stack_frame(frame);
        while (true);
    }
    
    // [0x05] #BR exception handler
    __attribute__((interrupt)) void handle_bound_exceeded(IdtFrame& frame) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x05] | ERRCODE 0x??"); 
        print_stack_frame(frame);
        while (true);
    }
    
    // [0x06] #UD exception handler
    __attribute__((interrupt)) void handle_invalid_opcode(IdtFrame& frame) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x06] | ERRCODE 0x??"); 
        print_stack_frame(frame);
        while (true);
    }

    // [0x07] #NM exception handler
    __attribute__((interrupt)) void handle_device_navail(IdtFrame& frame) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x07] | ERRCODE 0x??"); 
        print_stack_frame(frame);
        while (true);
    }

    // [0x08] #DF exception handler
    __attribute__((interrupt)) void handle_double_fault(IdtFrame& frame, u32 const errcode) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x08] | ERRCODE ", errcode);
        print_stack_frame(frame);
        while (true);
    }

    // [0x0A] #TS exception handler
    __attribute__((interrupt)) void handle_invalid_tss(IdtFrame& frame, u32 const errcode) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x0A] | ERRCODE ", errcode); 
        print_stack_frame(frame);
        while (true);
    }

    // [0x0B] #NP exception handler
    __attribute__((interrupt)) void handle_segment_npresent(IdtFrame& frame, u32 const errcode) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x0B] | ERRCODE ", errcode); 
        print_stack_frame(frame);
        while (true);
    }

    // [0x0C] #SS exception handler
    __attribute__((interrupt)) void handle_ss_fault(IdtFrame& frame, u32 const errcode) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x0C] | ERRCODE ", errcode); 
        print_stack_frame(frame);
        while (true);
    }

    // [0x0D] #GP exception handler
    __attribute__((interrupt)) void handle_gen_prot_fault(IdtFrame& frame, u32 const errcode) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x0D] | ERRCODE ", errcode); 
        print_stack_frame(frame);
        while (true);
    }

    // [0x0E] #PF exception handler
    __attribute__((interrupt)) void handle_page_fault(IdtFrame& frame, u32 const errcode) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x0E] | ERRCODE ", errcode); 
        print_stack_frame(frame);
        while (true);
    }

    // [0x10] #MF exception handler
    __attribute__((interrupt)) void handle_x87_fp_excp(IdtFrame& frame) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x10] | ERRCODE 0x??"); 
        print_stack_frame(frame);
        while (true);
    }

    // [0x11] #AC exception handler
    __attribute__((interrupt)) void handle_align_check(IdtFrame& frame, u32 const errcode) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x11] | ERRCODE ", errcode); 
        print_stack_frame(frame);
        while (true);
    }

    // [0x12] #MC exception handler
    __attribute__((interrupt)) void handle_mach_check(IdtFrame& frame) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x12] | ERRCODE 0x??");
        print_stack_frame(frame);
        while (true);
    }

    // [0x13] #XF exception handler
    __attribute__((interrupt)) void handle_simd_fp_excp(IdtFrame& frame) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x13] | ERRCODE 0x??"); 
        print_stack_frame(frame);
        while (true);
    }

    // [0x14] #VE exception handler
    __attribute__((interrupt)) void handle_virt_excp(IdtFrame& frame) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x14] | ERRCODE 0x??"); 
        print_stack_frame(frame);
        while (true);
    }

    // [0x15] #CP exception handler
    __attribute__((interrupt)) void handle_control_prot_excp(IdtFrame& frame, u32 const errcode) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x15] | ERRCODE ", errcode); 
        print_stack_frame(frame);
        while (true);
    }

    // [0x1C] #HV exception handler
    __attribute__((interrupt)) void handle_hypervis_inj_excp(IdtFrame& frame) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x1C] | ERRCODE 0x??"); 
        print_stack_frame(frame);
        while (true);
    }

    // [0x1D] #VC exception handler
    __attribute__((interrupt)) void handle_vmm_comm_excp(IdtFrame& frame, u32 const errcode) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x1D] | ERRCODE ", errcode); 
        print_stack_frame(frame);
        while (true);
    }

    // [0x1E] #SX exception handler
    __attribute__((interrupt)) void handle_security_excp(IdtFrame& frame,u32 const errcode) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x1E] | ERRCODE ", errcode); 
        print_stack_frame(frame);
        while (true);
    }

    // default exception handler
    __attribute__((interrupt)) void handle_default_excp(IdtFrame& frame) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x??] | ERRCODE 0x??"); 
        print_stack_frame(frame);
        while (true);
    }

    // default exception handler with error code
    __attribute__((interrupt)) void handle_default_excp_errcode(IdtFrame& frame, u32 const errcode) {
        k_console.switch_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_WHITE);
        k_console.print_line("EXCEPTION [0x??] | ERRCODE ", errcode);
        print_stack_frame(frame);
        while (true);
    }

    // default interrupt handler
    __attribute__((interrupt)) void handle_default_int(IdtFrame& frame) {
        k_console.switch_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_WHITE);
        k_console.print_line("INTERRUPT [0x??]");
        print_stack_frame(frame);
    }

    class Idt {
        public :
            Idt(Idt const& _) = delete;
            Idt() {
                _idtr.set_limit(static_cast<u16 const>(8 * NUM_ENTRIES));
                _idtr.set_base(reinterpret_cast<u32 const>(&_entries[0]));

                // add exceptions (INT 0x00-0x1F)
                _entries[0x00].set(0x00, reinterpret_cast<void*>(handle_div_err), TRAP_GATE_FLAGS);
                _entries[0x01].set(0x01, reinterpret_cast<void*>(handle_debug), TRAP_GATE_FLAGS);
                _entries[0x02].set(0x02, reinterpret_cast<void*>(handle_nonmaskable_int), INT_GATE_FLAGS);
                _entries[0x03].set(0x03, reinterpret_cast<void*>(handle_breakpoint), TRAP_GATE_FLAGS);
                _entries[0x04].set(0x04, reinterpret_cast<void*>(handle_overflow), TRAP_GATE_FLAGS);
                _entries[0x05].set(0x05, reinterpret_cast<void*>(handle_bound_exceeded), TRAP_GATE_FLAGS);
                _entries[0x06].set(0x06, reinterpret_cast<void*>(handle_invalid_opcode), TRAP_GATE_FLAGS);
                _entries[0x07].set(0x07, reinterpret_cast<void*>(handle_device_navail), TRAP_GATE_FLAGS);
                _entries[0x08].set(0x08, reinterpret_cast<void*>(handle_double_fault), TRAP_GATE_FLAGS);
                _entries[0x0A].set(0x0A, reinterpret_cast<void*>(handle_invalid_tss), TRAP_GATE_FLAGS);
                _entries[0x0B].set(0x0B, reinterpret_cast<void*>(handle_segment_npresent), TRAP_GATE_FLAGS);
                _entries[0x0C].set(0x0C, reinterpret_cast<void*>(handle_ss_fault), TRAP_GATE_FLAGS);
                _entries[0x0D].set(0x0D, reinterpret_cast<void*>(handle_gen_prot_fault), TRAP_GATE_FLAGS);
                _entries[0x0D].set(0x0E, reinterpret_cast<void*>(handle_page_fault), TRAP_GATE_FLAGS);
                _entries[0x10].set(0x10, reinterpret_cast<void*>(handle_x87_fp_excp), TRAP_GATE_FLAGS);
                _entries[0x11].set(0x11, reinterpret_cast<void*>(handle_align_check), TRAP_GATE_FLAGS);
                _entries[0x12].set(0x12, reinterpret_cast<void*>(handle_mach_check), TRAP_GATE_FLAGS);
                _entries[0x13].set(0x13, reinterpret_cast<void*>(handle_simd_fp_excp), TRAP_GATE_FLAGS);
                _entries[0x14].set(0x14, reinterpret_cast<void*>(handle_virt_excp), TRAP_GATE_FLAGS);
                _entries[0x15].set(0x15, reinterpret_cast<void*>(handle_control_prot_excp), TRAP_GATE_FLAGS);
                _entries[0x1C].set(0x1C, reinterpret_cast<void*>(handle_hypervis_inj_excp), TRAP_GATE_FLAGS);
                _entries[0x1D].set(0x1D, reinterpret_cast<void*>(handle_vmm_comm_excp), TRAP_GATE_FLAGS);
                _entries[0x1E].set(0x1E, reinterpret_cast<void*>(handle_security_excp), TRAP_GATE_FLAGS);

                for (u16 i = 0x20; i < NUM_ENTRIES; ++i) {
                    _entries[i].set(i, reinterpret_cast<void*>(handle_default_int), INT_GATE_FLAGS);
                }

                __asm__ __volatile__("lidt %0" : : "memory"(_idtr));
            }

            IdtEntry& operator [] (usize const idx) { return _entries[idx]; }
            IdtEntry const& operator [] (usize const idx) const { return _entries[idx]; }

            usize static constexpr const NUM_ENTRIES = 0x100;
            Array<IdtEntry, NUM_ENTRIES> _entries;
			
        private :
            IdtRegister _idtr;
    }__attribute__((aligned(16)));
};

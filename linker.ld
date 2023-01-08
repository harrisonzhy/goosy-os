ENTRY (_start)

SECTIONS {
    . = 1M;

    .text ALIGN(0x1000) : {
        *(.multiboot)
        *(.text)
        *(.gnu.linkonce.t*)
    }

    .rodata ALIGN(0x1000) : {
        start_ctors = .;
        *(SORT(.ctors*))
        end_ctors = .;

        start_dtors = .;
        *(SORT(.dtors*))
        end_dtors = .;

        *(.rodata*)
        *(.gnu.linkonce.r*)
    }

    .data ALIGN(0x1000) : {
        *(.data)
        *(.gnu.linkonce.d*)
    }

    .bss : {
        sbss = .;
        *(COMMON)
        *(.BSS)
        *(.gnu.linkonce.b*)
        ebss = .;
    }

    .init : {
        *(.init)
    }

    .fini : {
        *(.fini)
    }
}
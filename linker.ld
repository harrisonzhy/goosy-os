SECTIONS {
    . = 0x1000;

    .text BLOCK(4K) : ALIGN(4K) {
        *(.text)
    }

    .data BLOCK(4K) : ALIGN(4K) {
        *(.data)
        *(.gnu.linkonce.r*)
    }

    .rodata BLOCK(4K) : ALIGN(4K) {
        __ctors_start__ = .;
        *(SORT(.ctors*))
        __ctors_end__ = .;

        __ctors_start__ = .;
        *(SORT(.dtors*))
        __ctors_end__ = .;

        *(.rodata)
        *(.gnu.linkonce.r*)
    }

    .bss BLOCK(4K) : ALIGN(4K) {
        *(COMMON)
        *(.bss)
        __bss_end__ = .;
    }

    .init : {
        *(.init)
    }

    .fini : {
        *(.fini)
    }
}
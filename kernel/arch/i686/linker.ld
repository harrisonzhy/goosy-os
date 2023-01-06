ENTRY(_start)

SECTIONS {
    /* load kernel sections starting at 1 MiB */
    .= 1M;

    /* load multiboot header */
    .text BLOCK(4K) : ALIGN(4K) {
        *(.multiboot)
        *(.text)
    }

    /* load read-only data */
    .rodata BLOCK(4K) : ALIGN(4K) {
        *(.rodata)
    }

    /* load initialized read-write data */
    .data block(4K) : ALIGN(4K) {
        *(.data)
    }

    /* load uninitialized read-write data, stack */
    .bss BLOCK(4K) : ALIGN(4K) {
        *(COMMON)
        *(.bss)
    }
}
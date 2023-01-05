MBALIGN  equ  1<<0               ; aligns loaded modules on page boundaries
MEMINFO  equ  1<<1               ; provides memory map
MBFLAGS  equ  MBALIGN | MEMINFO  ; multiboot flag field
MAGIC    equ  0X1BADB002         ; magic number
CHECKSUM equ -(MAGIC+MBFLAGS)    ; prove multiboot status

; constants are defined as in the multiboot standard
; MAGIC is in the first 8 KiB of kernel, in an isolated section

section .multiboot
align 4
    dd MAGIC
    dd MBFLAGS
    dd CHECKSUM

section .bss
align 16
stack_bottom:
resb 16384 ; 16 KiB
stack_top:

; _start is the entry point to the kernel. jump here
;       once kernel has been loaded
section .text
global _start:function (_start.end-_start)
_start: ; protected mode
    mov     esp, stack_top
    ; TODO: initialize crucial processor state
    ;   - load GDT
    ;   - enable paging
    ;   - setup global constructors
    extern kernel_main
    call    kernel_main
    ; do other stuff
    cli         ; disable interrupts

    .hang:  
        hlt
        jmp     .hang   ; hang if nothing left to do

    .end:

; assemble boot.s using `nasm -felf32 boot.s -o boot.o`
MBALIGN  equ  1<<0               ; aligns loaded modules on page boundaries
MEMINFO  equ  1<<1               ; provides memory map
MBFLAGS  equ  MBALIGN | MEMINFO  ; multiboot flag field
MAGIC    equ  0X1BADB002         ; magic number
CHECKSUM equ -(MAGIC+MBFLAGS)    ; prove multiboot status

; declare header as in the multiboot standard
; MAGIC is in the first 8 KiB of kernel in an isolated section
SECTION .multiboot
align 4
    dd MAGIC
    dd MBFLAGS
    dd CHECKSUM

; reserve a stack
SECTION .bss
align 16
stack_bottom:
resb 16384 ; 16 KiB
stack_top:

; entry point to kernel. jump here once kernel is loaded
section .text
global _start:function (_start.end-_start)
_start: ; protected mode
    mov     esp, stack_top
    ; TODO: initialize crucial processor state
    ;   - load GDT
    ;   - enable paging
    
    ; setup global constructors
    call    _init
    [extern  kernel_main]
    call    kernel_main
    cli         ; disable interrupts

    .hang:  
        hlt
        jmp     .hang   ; hang if nothing left to do

    .end:

; assemble boot.s using `nasm -felf32 boot.s -o boot.o`
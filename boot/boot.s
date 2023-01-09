KERNEL_OFFSET equ 0x1000

[bits 16] ; BIOS jumps to bootloader in real mode
[org 0x7c00]
    mov     [BOOT_DRIVE], dl
    xor     ax, ax
    mov     es, ax
    mov     bp, 0x9000 ; setup stack
    mov     sp, bp
    call    load_kernel ; load kernel from disk at 0x1000
    call    switch_protmode
    jmp     $ ; never gets here

%include "boot/disk.s"
%include "boot/gdt.s"
%include "boot/vga.s"

; bx: memory location to place read data
; dh: number of sectors to read
; dl: disk to read from

[bits 16]
load_kernel:
    mov     bx, KERNEL_OFFSET
    mov     al, 2 ; change as kernel grows in size
    call    disk_load
    ret

[bits 16]
switch_protmode:
    cli
    lgdt    [gdt_descriptor] ; load gdt descriptor
    mov     eax, cr0
    or      eax, 0x1 ; protected mode
    mov     cr0, eax
    jmp     CODE_SEG:init_protmode

[bits 32]
init_pm:
    mov     ax, DATA_SEG ; update segment regs
    mov     ds, ax
    mov     ss, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax

    mov     ebp, 0x90000 ; put base pointer on stack top
    mov     esp, ebp

    call    BEGIN_PROTMODE

[bits 32]
BEGIN_PROTMODE:    
    call    KERNEL_OFFSET
    jmp     $


BOOT_DRIVE db 0

; signature
times 510-($-$$) db 0
dw 0xaa55
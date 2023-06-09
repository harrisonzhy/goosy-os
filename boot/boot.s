KERNEL_OFFSET equ 0x1000

[bits 16] ; BIOS jumps to bootloader in real mode
[org 0x7C00]
    mov     [BOOT_DRIVE], dl
    xor     ax, ax
    mov     es, ax
    mov     bp, 0x9000 ; setup stack at top of free space
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
    mov     al, 64 ; sectors to load
    call    disk_load
    ret

[bits 16]
switch_protmode:
    cli
    lgdt    [gdt_descriptor] ; load gdt descriptor
    mov     eax, cr0
    or      eax, 0x1 ; protected mode
    mov     cr0, eax
    jmp     CODE_SEG: init_protmode

[bits 32]
init_protmode:
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
    mov     ebx, PROTMODE_MSG
    call    vga_print ; print `PROTMODE_MSG'
    call    KERNEL_OFFSET
    jmp     $


BOOT_DRIVE db 0
PROTMODE_MSG db "Entered protected mode (32 bit)", 0

; signature
times 510 - ($ - $$) db 0
dw 0xAA55
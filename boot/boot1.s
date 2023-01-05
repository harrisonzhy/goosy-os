KERNEL_OFFSET equ 0x1000

[bits 16] ; BIOS jumps to bootloader in real mode
[org 0x7c00] ; bootloader offset
    mov [BOOT_DRIVE], dl ; store boot drive
    ; setup stack
    xor ax, ax
    mov es, ax
    mov bp, 0x9000
    mov sp, bp

    call load_kernel ; load kernel from disk at 0x1000
    call switch_prot

    jmp $ ; shouldn't get here

%include "disk.s"
%include "gdt.s"
%include "switch_prot.s"

; bx -> memory location to place read data
; dh -> number of sectors to read
; dl -> disk to read from

[bits 16]
load_kernel:
    mov bx, KERNEL_OFFSET
    mov dh, 2 ; reads 2 sectors, need to increment in future as kernel grows
    mov dl, [BOOT_DRIVE]
    call disk_load
    ret

[bits 32]
BEGIN_PROT:
    call KERNEL_OFFSET ; give control to kernel
    jmp $ ; kernel should not return

; bootloader
BOOT_DRIVE db 0 ; disk
times 510-($-$$) db 0 ; padding
dw 0xaa55 ; magic number
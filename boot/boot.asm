KERNEL_OFFSET equ 0x1000
PIC1_COMMAND  equ 0x20
PIC1_DATA     equ 0x21
PIC2_COMMAND  equ 0xA0
PIC2_DATA     equ 0xA1
IO_WAIT       equ 0x80
ICW_8086      equ 0x01

[org 0x7c00] ; bootloader offset
    xor ax, ax
    mov es, ax
    mov bp, 0x9000 ; set the stack
    mov sp, bp

    ; call pic_disable
    
    call load_kernel

    call switch_to_pm
    jmp $ ; this will actually never be executed

%include "boot/boot_utils.asm"
%include "boot/gdt.asm"
%include "boot/vga.asm"

; Disable the PIC so we can initialize the APIC later
pic_disable:
    mov al, 0x11
    out PIC1_DATA, al
    out PIC2_DATA, al
    mov al, 0x20
    out PIC1_DATA, al
    mov al, 0x28
    out PIC1_DATA, al
    mov al, 0x04
    out PIC1_DATA, al
    mov al, 0x02
    out PIC2_DATA, al
    mov al, 0x01
    out PIC1_DATA, al
    out PIC2_DATA, al
    mov al, 0x00
    out PIC1_DATA, al
    out PIC2_DATA, al
    mov al, 0xff
    out PIC2_DATA, al
    out PIC1_DATA, al
    ret

io_wait:
    mov al, 0
    out 0x80, al
    ret

[bits 16]
load_kernel:
    mov bx, KERNEL_OFFSET
    mov al, 64
    call disk_load
    ret

[bits 16]
switch_to_pm:
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:init_pm

[bits 32]
init_pm:
    mov ax, DATA_SEG ; 5. update the segment registers
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ebp, 0x90000 ; 6. update the stack right at the top of the free space
    mov esp, ebp

    call BEGIN_PM

[bits 32]
BEGIN_PM: ; after the switch we will get here
    mov ebx, MSG_PROT_MODE
    call print_vga ; Note that this will be written at the top left corner
    call KERNEL_OFFSET
    jmp $
  

BOOT_DRIVE db 0
MSG_PROT_MODE db "Loaded 32-bit protected mode", 0

; bootsector
times 510-($-$$) db 0
dw 0xaa55

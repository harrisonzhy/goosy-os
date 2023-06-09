BIOS_DISK_READ  equ 0x02
BIOS_DISK_WRITE equ 0x0E
BIOS_DISK_PRINT equ 0x10
BIOS_DISK_INTR  equ 0x13

disk_load:
    push    ax
    mov     ah, BIOS_DISK_READ
    ; mov     al, dh        ; read `dh' sectors
    mov     cl, 0x02        ; read from sector 2 (sector 1 is boot)
    mov     ch, 0x00        ; read from cylinder 0
    mov     dh, 0x00        ; read from head 0

    int     BIOS_DISK_INTR    ; BIOS disk interrupt
    pop     dx
    sub     dl, al
    je      disk_load_done  ; done if carry flag is 0

disk_load_done:
    ret
    
disk_error:
    mov     si, DISKERR_MSG
    call    bios_print
    jmp     hang

sector_error:
    mov     si, SECTERR_MSG
    call    bios_print
    jmp     hang

bios_print:
    lodsb
    or      al, al
    jz      bios_print_done
    mov     ah, BIOS_DISK_WRITE
    mov     bh, 0
    int     BIOS_DISK_PRINT
    jmp     bios_print

bios_print_done:
    ret

hang:
    jmp     $


; error messages
DISKERR_MSG db "DISK ERROR", 0
SECTERR_MSG db "SECT ERROR", 0

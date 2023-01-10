READMODE     equ 0x02
BIOSDISKINTR equ 0x13

disk_load:
    push    ax
    mov     ah, READMODE
    ; mov     al, dh        ; read `dh` sectors
    mov     cl, 0x02        ; read from sector 2 (sector 1 is boot)
    mov     ch, 0x00        ; read from cylinder 0
    mov     dh, 0x00        ; read from head 0

    int     BIOSDISKINTR    ; BIOS disk interrupt
    pop     dx
    sub     dl, al
    je      done            ; done if carry flag is 0
    
    done:
    ret
    
disk_error:
    ; add error msg
    jmp     hang

sector_error:
    ; add error msg
    jmp     hang

hang:
    jmp     $

bios_print:
    ; need to implement

; error messages
diskerr_msg db "DiskErr", 0
secterr_msg db "SectErr", 0
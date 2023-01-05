READMODE equ 0x02

disk_load:
    pusha
    push dx
    
    mov ah, READMODE
    mov al, dh          ; read `dh` sectors
    mov cl, 0x02        ; read from sector 2 (sector 1 is boot)
    mov ch, 0x00        ; read from cylinder 0
    mov dh, 0x00        ; read from head 0

    int 0x13            ; BIOS interrupt for disk access
    jc disk_error       ; error if carry bit is set

    pop dx              ; original num sectors to read
    cmp al, dh          ; set `al` to `dl`, the num sectors actually read
    jne sector_error    ; error if al and d_ are not equal
    popa
    ret

disk_error:
    ; TODO: print error to screen
    jmp hang

sector_error:
    ; TODO: print error to screen
    jmp hang

hang:
    jmp $
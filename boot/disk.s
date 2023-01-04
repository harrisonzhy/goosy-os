READMODE equ 0x02

disk_load:
    pusha
    push dx
    
    mov ah, READMODE
    mov al, dh          ; read `dh` sectors
    mov cl, 0x02        ; read from sector 2 (sector 1 is boot)
    mov ch, 0x00        ; read from cylinder 0
    mov dh, 0x0         ; read from head 0

    int 0x13            ; BIOS stuff
    jc disk_error       ; jump if carry bit is 1 (error)

    pop dx              ; original num sectors to read
    cmp al, dh          ; set `al` to num sectors actually read
    jne sector_error    ; jump if al and d_ are not equal (error)
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
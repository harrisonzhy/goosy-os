; null segment descriptor (32 bit)
gdt_start:
    dd 0x0
    dd 0x0

; code segment descriptor
gdt_code: 
    dw 0xFFFF    ; segment length, bits 0-15
    dw 0x0       ; segment base, bits 0-15
    db 0x0       ; segment base, bits 16-23
    db 10011010b ; flags (8 bits)
    db 11001111b ; flags (4 bits) + segment length, bits 16-19
    db 0x0       ; segment base, bits 24-31

; data segment descriptor
;   same as code segment descriptor but removed write perms
gdt_data:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10010010b ; changed write flag
    db 11001111b
    db 0x0

gdt_end:

; gdt descriptor
gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; size (16 bit)
    dd gdt_start ; address (32 bit)


; `boot.s' constants
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
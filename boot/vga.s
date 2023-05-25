VGA_SEGMENT equ 0xB8000

[bits 32]
vga_print:
    pusha
    mov     edx, VGA_SEGMENT

vga_print_string:
    mov     al, [ebx]
    mov     ah, 0xF
    cmp     al, 0 ; print until end of string
    je      vga_print_string_done

    mov     [edx], ax ; if not done, continue and loop
    add     ebx, 1 ; increment char pos
    add     edx, 2
    jmp     vga_print_string

vga_print_string_done:
    popa
    ret
BITS 32

; dimensions
VGA_WIDTH                  equ 80
VGA_HEIGHT                 equ 25

; colors
VGA_COLOR_BLACK            equ 0
VGA_COLOR_BLUE             equ 1
VGA_COLOR_GREEN            equ 2
VGA_COLOR_CYAN             equ 3
VGA_COLOR_RED              equ 4
VGA_COLOR_MAGENTA          equ 5
VGA_COLOR_BROWN            equ 6
VGA_COLOR_LIGHT_GREY       equ 7
VGA_COLOR_DARK_GREY        equ 8
VGA_COLOR_LIGHT_BLUE       equ 9
VGA_COLOR_LIGHT_GREEN      equ 10
VGA_COLOR_LIGHT_CYAN       equ 11
VGA_COLOR_LIGHT_RED        equ 12
VGA_COLOR_LIGHT_MAGENTA    equ 13
VGA_COLOR_LIGHT_BROWN      equ 14
VGA_COLOR_WHITE            equ 15

; `t` prefix denotes terminal

global kernel
kernel:
    mov     dh, VGA_COLOR_LIGHT_GREY
    mov     dl, VGA_COLOR_BLACK
    call    t_setcolor
    mov     esi, print_string
    call    t_writestring
    jmp     $

; args   -> `dl` y, `dh` x
; retval -> dx: index with 0xB8000 offset at VGA buffer
t_getidx:
    push    ax      ; preserve regs
    shl     dh, 1   ; x2 because words are 2 bytes

    mov     al, VGA_WIDTH
    mul     dl
    mov     dl, al

    shl     dl, 1
    add     dl, dh
    mov     dh, 0
    pop     ax
    ret

t_setcolor:
    shl     dl, 4
    or      dl, dh
    mov     [t_color], dl
    ret

; args   -> `dl` bg color, `dh` fg color
; retval -> none
t_putentryat:
    pusha
    call    t_getidx
    mov     ebx, edx

    mov     dl, [t_color]
    mov     byte [0xB8000 + ebx], al
    mov     byte [0xB8001 + ebx], dl
    popa
    ret

; args -> `al` ascii char
t_putchar:
    mov     dx, [t_cursorpos]  ; loads t_column at `dh`, t_row at `dl`
    call    t_putentryat

    inc     dh
    cmp     dh, VGA_WIDTH
    jne     .cursor_moved

    mov     dh, 0
    inc     dl
    cmp     dl, VGA_HEIGHT
    jne     .cursor_moved
    mov     dl, 0

    .cursor_moved:
        mov     [t_cursorpos], dx  ; store new cursor pos
        ret

; args   -> `cx` length of string, `esi` string location
; retval -> none
t_write:
    pusha

    .loop:
        mov     al, [esi]
        call    t_putchar

        dec     cx
        cmp     cx, 0
        je      .done

        inc     esi
        jmp     .loop

    .done:
        popa
        ret

; args   -> `esi` '\0' delimited string location 
; retval -> `ecx` length of given string
t_strlen:
    push    eax
    push    esi
    mov     ecx, 0

    .loop:
        mov     al, [esi]
        cmp     al, 0
        je      .done

        inc     esi
        inc     ecx
        jmp     .loop

    .done:
        pop     esi
        pop     eax
        ret

; args   -> `esi` '\0' delimited string location 
; retval -> none
t_writestring:
    pusha
    call    t_strlen
    call    t_write
    popa
    ret

print_string     db "hello world", 0xA, 0 ; 0xA is line feed
t_color         db 0
t_cursorpos:
t_column        db 0
t_row           db 0

; assemble kernel.s using `nasm -felf32 kernel.s -o kernel.o`
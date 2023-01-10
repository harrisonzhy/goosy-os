[bits 32]
[extern kernel_main]
[extern _init]
[extern _fini]
; mov bx, HELLO_STRING
; call print_string_pm
global _start
_start:
    call _init
    call kernel_main
    call _fini
    jmp $

; HELLO_STRING db "Hello, there", 0

; this is how constants are defined
; VIDEO_MEMORY equ 0xb8000
; WHITE_ON_BLACK equ 0x0f ; the color byte for each character

; print_string_pm:
    ; pusha
    ; mov edx, VIDEO_MEMORY

; print_string_pm_loop:
    ; mov al, [ebx] ; [ebx] is the address of our character
    ; mov ah, WHITE_ON_BLACK

    ; cmp al, 0 ; check if end of string
    ; je print_string_pm_done

    ; mov [edx], ax ; store character + attribute in video memory
    ; add ebx, 1 ; next char
    ; add edx, 2 ; next video memory position

    ; jmp print_string_pm_loop

; print_string_pm_done:
    ; popa
    ; ret

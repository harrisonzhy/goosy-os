SECTION .init
global _init
_init:
    push    ebp
    mov     ebp, esp
    ; gcc places crtbegin.o's .init section contents here

SECTION .fini
global _fini
_fini:
    push    ebp
    mov     ebp, esp
    ; gcc places crtbegin.o's .fini section contents here
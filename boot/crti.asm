SECTION .init
global _init
_init:
    push ebp
    mov ebp, esp
    ; gcc will put crtbegin.o's .init contents here

SECTION .fini
global _fini
_fini:
    push ebp
    mov ebp, esp
    ; gcc will put crtbegin.o's .fini contents here

section .init
global _init:function
_init:
    push    ebp
    movl    esp, ebp
    ; gcc places contents of crtbegin.o .init section here

section .fini
global _fini:function
_fini:
    push    ebp
    movl    esp, ebp
    ; gcc places contents of crtbegin.o .fini section here
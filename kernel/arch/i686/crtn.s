section .init
    ; gcc places contents of crtend.o .init section here
    popl    ebp
    ret

section .fini
    ; gcc places contents of crtend.o .fini section here
    popl    ebp
    ret
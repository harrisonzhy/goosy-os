SECTION .init
    ; GCC will put crtend.o's .fini section content here
    pop ebp
    ret

SECTION .fini
    ; GCC will put crtend.o's .fini section content here
    pop ebp
    ret

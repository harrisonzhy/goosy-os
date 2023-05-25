SECTION .init
    ; gcc places crtend.o's .init section content here
    pop     ebp
    ret

SECTION .fini
    ; gcc places crtend.o's .fini section content here
    pop     ebp
    ret
[bits 32]

[extern _init]
[extern _fini]
[extern kernel_main]

global _start
_start:
    call    _init
    call    kernel_main
    call    _fini
    jmp     $
[extern _init]
[extern _fini]
[extern kernel_main]

[bits 32]

global _start
_start:
    call    _init
    call    kernel_main
    call    _fini
    jmp     $
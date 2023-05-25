[bits 32]

[extern _init]
[extern _fini]
[extern main]

global _start
_start:
    call    _init
    call    main ; call kernel `main()'
    call    _fini
    jmp     $
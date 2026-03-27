section .text.boot
[BITS 32]
global _entry
extern _start

_entry:
    call _start
    cli
    hlt

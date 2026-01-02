section .text
global _start
_start:
    mov eax, 42
.loop:
    jmp .loop
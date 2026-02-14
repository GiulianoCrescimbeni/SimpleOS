global gdt_load
global gdt_flush_tss

SEGSEL_KERNEL_CS equ 0x08
SEGSEL_KERNEL_DS equ 0x10

section .text

; load the gdt into the cpu, and enter the kernel segments
gdt_load:
    mov     eax, [esp+4]
    lgdt    [eax]               ; load gdt table

    ; load cs segment by doing a far jump
    jmp     SEGSEL_KERNEL_CS:.reload_segments_registers

.reload_segments_registers:
    mov     ax, SEGSEL_KERNEL_DS
    mov     ds, ax
    mov     ss, ax
    mov     es, ax
    mov     gs, ax
    mov     fs, ax  

    ret

gdt_flush_tss:
    mov     ax, [esp+4]         ; Load TSS offset
    ltr     ax                  ; Load Task Register
    ret
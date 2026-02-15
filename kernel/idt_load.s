global  idt_load
global  enable_interrupt
global  disable_interrupt

section .text

; load_idt - Loads the interrupt descriptor table (IDT).
; stack: [esp + 4] the address of the first entry in the IDT
;        [esp    ] the return address
idt_load:
    mov     eax, [esp+4]    ; load the address of the IDT into register eax
    lidt    [eax]           ; load the IDT
    ret                     ; return to the calling function

enable_interrupt:
    sti
    ret

disable_interrupt:
    cli
    ret
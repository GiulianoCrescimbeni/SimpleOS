extern kmain                    ; find the main file of the kernel
global loader                   ; the entry symbol for ELF

MAGIC_NUMBER equ 0x1BADB002     ; define the magic number constant for the multiboot protocol
ALIGN_MODULES   equ 0x00000001  ; tell GRUB to align modules

; calculate the checksum (all options + checksum should equal 0)
CHECKSUM        equ -(MAGIC_NUMBER + ALIGN_MODULES)

section .text:                  ; start of the text (code) section
align 4                         ; the code must be 4 byte aligned
    dd MAGIC_NUMBER             ; write the magic number to the machine code,
    dd ALIGN_MODULES            ; write the align modules instruction
    dd CHECKSUM                 ; and the checksum

loader:
    call kmain                  ; call the entry point of the kernel
.loop:
    jmp .loop                   ; infinite loop if the kmain function return (debug)
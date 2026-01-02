extern kmain                    ; find the main file of the kernel
extern pag_init
global loader                   ; the entry symbol for ELF

MAGIC_NUMBER equ 0x1BADB002     ; define the magic number constant for the multiboot protocol
ALIGN_MODULES   equ 0x00000003  ; tell GRUB to align modules

; calculate the checksum (all options + checksum should equal 0)
CHECKSUM        equ -(MAGIC_NUMBER + ALIGN_MODULES)

section .multiboot
align 4
    dd MAGIC_NUMBER             ; write the magic number to the machine code,
    dd ALIGN_MODULES            ; write the align modules instruction
    dd CHECKSUM                 ; and the checksum

section .text:
align 4           

loader:
    
    push ebx                    ; save EBX in stack
    call pag_init
    pop esi             
    mov ebx, higher_half
    jmp ebx

higher_half:

    push esi
    call kmain
    cli
                                
.loop:
    hlt
    jmp .loop                   ; infinite loop if the kmain function return (debug)
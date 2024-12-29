#include "stdint.h"
#include "gdt.h"
#include "idt.h"
#include "idt_load.h"
#include "interrupt.h"
#include "../drivers/framebuffer.h"
#include "../drivers/serial.h"
#include "kprint.h"
#include "multiboot.h"

char gdt_loaded_message[] = "GDT Loaded\n";
char idt_loaded_message[] = "IDT Loaded\n";
char welcome_message[] = "Welcome to SimpleOS\n";

typedef void (*call_module_t)(void);

void kmain(unsigned int ebx) {
    disable_interrupt();
    gdt_init();
    kprint(gdt_loaded_message, 1);
    idt_init();
    kprint(idt_loaded_message, 1);
    clear_screen();
    kprint(welcome_message, 0);
    enable_interrupt();

    multiboot_info_t *mbinfo = (multiboot_info_t *) ebx;
    unsigned int address_of_module = mbinfo->mods_addr;
    call_module_t start_program = (call_module_t) address_of_module;
    start_program();

    while (1) {
        // infinite loop to avoid return to loader
    }
}

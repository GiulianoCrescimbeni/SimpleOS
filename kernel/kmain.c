#include <stdint.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/idt_load.h>
#include <kernel/interrupt.h>
#include <kernel/kprint.h>
#include <kernel/paging.h>
#include <kernel/multiboot.h>
#include <drivers/framebuffer.h>
#include <drivers/serial.h>

char gdt_loaded_message[] = "GDT Loaded\n";
char idt_loaded_message[] = "IDT Loaded\n";
char pag_loaded_message[] = "Paging Enabled\n";
char welcome_message[] = "\n\n .d8888b.  d8b                        888           .d88888b.   .d8888b.  888 \nd88P  Y88b Y8P                        888          d88P\" \"Y88b d88P  Y88b 888 \nY88b.                                 888          888     888 Y88b.      888 \n \"Y888b.   888 88888b.d88b.  88888b.  888  .d88b.  888     888  \"Y888b.   888 \n    \"Y88b. 888 888 \"888 \"88b 888 \"88b 888 d8P  Y8b 888     888     \"Y88b. 888 \n      \"888 888 888  888  888 888  888 888 88888888 888     888       \"888 Y8P \nY88b  d88P 888 888  888  888 888 d88P 888 Y8b.     Y88b. .d88P Y88b  d88P  \"  \n \"Y8888P\"  888 888  888  888 88888P\"  888  \"Y8888   \"Y88888P\"   \"Y8888P\"  888 \n                             888                                              \n                             888                                              \n                             888                                              \n";

typedef void (*call_module_t)(void);

void kmain(unsigned int ebx) {
    disable_interrupt();
    kprint(pag_loaded_message, 0);
    gdt_init();
    kprint(gdt_loaded_message, 0);
    idt_init();
    kprint(idt_loaded_message, 0);
    clear_screen();
    kprint(welcome_message, 0);
    enable_interrupt();

    multiboot_info_t *mbinfo = (multiboot_info_t *) ebx + 0xC0000000;
    unsigned int address_of_module = mbinfo->mods_addr;
    call_module_t start_program = (call_module_t) address_of_module;
    start_program();

    while (1) {
        // infinite loop to avoid return to loader
    }
}

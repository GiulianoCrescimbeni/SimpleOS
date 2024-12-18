#include "stdint.h"
#include "gdt.h"
#include "idt.h"
#include "idt_load.h"
#include "interrupt.h"
#include "../drivers/framebuffer.h"
#include "../drivers/serial.h"
#include "kprint.h"

char gdt_loaded_message[] = "GDT Loaded\n";
char idt_loaded_message[] = "IDT Loaded\n";
char welcome_message[] = "Welcome to SimpleOS\n";

void kmain(void) {
    disable_interrupt();
    gdt_init();
    kprint(gdt_loaded_message, 1);
    idt_init();
    kprint(idt_loaded_message, 1);
    clear_screen();
    kprint(welcome_message, 0);
    enable_interrupt();

    while (1) {
        // infinite loop to avoid return to loader
    }
}

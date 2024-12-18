#include "stdint.h"
#include "gdt.h"
#include "idt.h"
#include "interrupt.h"
#include "../drivers/framebuffer.h"
#include "../drivers/serial.h"
#include "kprint.h"

char gdt_loaded_message[] = "GDT Loaded ";
char idt_loaded_message[] = "IDT Loaded ";
char welcome_message[] = "Welcome to SimpleOS";

void kmain(void) {
    // TODO: Make a proper function
    asm volatile ("cli");
    gdt_init();
    kprint(gdt_loaded_message, 11, 1);
    idt_init();
    kprint(idt_loaded_message, 11, 1);
    clear_screen();
    kprint(welcome_message, 19, 0);
    //TODO: Make a proper function
    asm volatile ("sti");

    while (1) {
        // infinite loop to avoid return to loader
    }
}

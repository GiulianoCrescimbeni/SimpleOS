#include "stdint.h"
#include "gdt.h"
#include "../drivers/framebuffer.h"
#include "../drivers/serial.h"
#include "kprint.h"

char gdt_loaded_message[] = "GDT Loaded";
char welcome_message[] = "Welcome to SimpleOS";

void kmain(void) {
    gdt_init();
    kprint(gdt_loaded_message, 10, 1);
    clear_screen();
    kprint(welcome_message, 19, 0);

    while (1) {
        // infinite loop to avoid return to loader
    }
}

#include "../drivers/framebuffer.h"

char welcome_message[] = "Welcome to SimpleOS";

void kmain(void) {
    clear_screen();
    write(welcome_message, 19);

    while (1) {
        // infinite loop to avoid return to loader
    }
}

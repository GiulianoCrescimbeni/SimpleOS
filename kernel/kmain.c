#include "../drivers/framebuffer.h"
#include "../drivers/serial.h"

char welcome_message[] = "Welcome to SimpleOS";
char debug_message[] = "This is a debug message";

void kmain(void) {
    clear_screen();
    write(welcome_message, 19);
    serial_write(debug_message, 23);

    while (1) {
        // infinite loop to avoid return to loader
    }
}

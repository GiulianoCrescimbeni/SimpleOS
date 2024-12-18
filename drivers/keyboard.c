#include "../kernel/stdint.h"
#include "../kernel/kprint.h"
#include "io.h"
#include "../kernel/pic.h"

// Mapping table  Scancode -> ASCII
static char scancode_to_ascii[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,      // 0x00-0x0E
    '	', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\'',    // 0x0F-0x1D
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',            // 0x1E-0x29
    0, 0, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,                     // 0x2A-0x35
    '*', 0,  ' ', 0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,           // 0x36-0x45
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0                    // 0x46-0x58
};

/**
 * keyboard_interrupt_handler:
 * Manage keyboard interrupt, converts the scancode to ASCII
 */
void keyboard_interrupt_handler() {
    // Read the scancode from the io port
    uint8_t scancode = inb(0x60);

    // Ignores breakcodes
    if (scancode & 0x80) {
        return;
    }

    char ascii_char = scancode_to_ascii[scancode];

    if (ascii_char != 0) {
        char buffer[2] = {ascii_char, 0};
        kprint(buffer, 0);
    }
}

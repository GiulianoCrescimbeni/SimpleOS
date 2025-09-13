#include <stdint.h>
#include <kernel/kprint.h>
#include <kernel/pic.h>
#include <drivers/io.h>
#include <drivers/framebuffer.h>

// Mapping table  Scancode -> ASCII
static char scancode_to_ascii_lower[] = {
    '\\',  0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\'', 236, 0,             // 0x00-0x0E
    0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 138, '+', '\n',                // 0x0F-0x1D
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 149, 133, 0,                        // 0x1E-0x29
    0, 151, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', 0,                        // 0x2A-0x35
    '*', 0,  ' ', 0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,                   // 0x36-0x45
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0                            // 0x46-0x58
};

static char scancode_to_ascii_upper[] = {
    '|',  0, '!', '"', 163, 36, 37, 38, 47, '(', ')', '=', '?', '^', '<',               // 0x00-0x0E 
    0,  'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 138, '*', '\n',                // 0x0F-0x1D
    0,  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 231, 176, 0,                       // 0x1E-0x29
    0,  167, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ';', ':', '_', 0,                       // 0x2A-0x35
    '*', 0,  ' ', 0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,                   // 0x36-0x45
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0                            // 0x46-0x58
};

static int shift_pressed = 0; // Keeps track of whether Shift is pressed

/**
 * keyboard_interrupt_handler:
 * Manage keyboard interrupt, converts the scancode to ASCII
 */
void keyboard_interrupt_handler() {
    // Read the scancode from the IO port
    uint8_t scancode = inb(0x60);

    // Check for Backspace
    if (scancode == 0x0E) { // Backspace scancode
        delete();
        return;
    }

    // Check for break codes (key release events)
    if (scancode & 0x80) {
        scancode &= 0x7F; // Remove the break bit
        if (scancode == 0x2A || scancode == 0x36) { // Left Shift or Right Shift
            shift_pressed = 0; // Shift released
        }
        return;
    }

    // Check for make codes (key press events)
    if (scancode == 0x2A || scancode == 0x36) { // Left Shift or Right Shift
        shift_pressed = 1; // Shift pressed
        return;
    }

    // Get the ASCII character based on Shift state
    char ascii_char = shift_pressed ? scancode_to_ascii_upper[scancode] : scancode_to_ascii_lower[scancode];

    if (ascii_char != 0) {
        char buffer[2] = {ascii_char, 0}; // Null-terminated string
        kprint(buffer, 0);
    }
}
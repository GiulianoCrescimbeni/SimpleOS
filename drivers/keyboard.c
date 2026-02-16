#include <stdint.h>
#include <kernel/kprint.h>
#include <kernel/pic.h>
#include <drivers/io.h>
#include <drivers/framebuffer.h>

#define KB_BUFFER_SIZE 256

char kb_buffer[KB_BUFFER_SIZE];
uint32_t kb_write_ptr = 0;
uint32_t kb_read_ptr = 0;

// Helper function to add a char to the buffer (Producer)
void keyboard_push_char(char c) {
    uint32_t next_write = (kb_write_ptr + 1) % KB_BUFFER_SIZE;
    
    // Check if buffer is full (we drop the key if full to avoid overwrite)
    if (next_write != kb_read_ptr) {
        kb_buffer[kb_write_ptr] = c;
        kb_write_ptr = next_write;
    }
}

// Function called by the System Call to retrieve a key (Consumer)
char keyboard_get_char() {
    if (kb_read_ptr == kb_write_ptr) {
        return 0; // Buffer is empty
    }

    char c = kb_buffer[kb_read_ptr];
    kb_read_ptr = (kb_read_ptr + 1) % KB_BUFFER_SIZE;
    return c;
}

// Mapping table Scancode -> ASCII
static char scancode_to_ascii_lower[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*',
    0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static char scancode_to_ascii_upper[] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*',
    0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static int shift_pressed = 0; // Keeps track of whether Shift is pressed

/**
 * keyboard_interrupt_handler:
 * Manage keyboard interrupt, converts the scancode to ASCII
 */
void keyboard_interrupt_handler() {
    // Read the scancode from the IO port
    uint8_t scancode = inb(0x60);

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
        // Save in buffer for syscall
        keyboard_push_char(ascii_char);
    }
}
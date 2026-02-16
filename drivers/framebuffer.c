#include <drivers/framebuffer.h>
#include <drivers/io.h>

/* Memory Configuration */
char *fb = (char *) 0xC00B8000; // Framebuffer address (Higher Half)

/* Screen Dimensions */
#define COLS 80
#define ROWS 25

/* Cursor Position (X, Y) */
unsigned int cursor_x = 0;
unsigned int cursor_y = 0;

/* I/O Ports */
#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT    0x3D5
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND  15

/* Default Colors */
#define FB_DEFAULT_FOREGROUND 0x0F // Bright White
#define FB_DEFAULT_BACKGROUND 0x00 // Black

/** * fb_move_cursor: 
 * Updates the hardware cursor (the blinking line) via I/O ports.
 */
void fb_move_cursor(unsigned int x, unsigned int y) {
    unsigned short pos = y * COLS + x;
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT,    pos & 0x00FF);
}

/** * fb_scroll: 
 * Shifts the entire screen content up by one line.
 * It is called when the cursor goes below the last row.
 */
void fb_scroll() {
    if (cursor_y < ROWS) {
        return;
    }

    for (int i = 0; i < (ROWS - 1) * COLS * 2; i++) {
        fb[i] = fb[i + COLS * 2];
    }

    unsigned int last_line_offset = (ROWS - 1) * COLS * 2;
    for (int i = last_line_offset; i < ROWS * COLS * 2; i += 2) {
        fb[i] = ' ';
        fb[i + 1] = ((FB_DEFAULT_BACKGROUND & 0x0F) << 4) | (FB_DEFAULT_FOREGROUND & 0x0F);
    }

    cursor_y = ROWS - 1;
}

/** * fb_write_char: 
 * Writes a single character to the screen, handling special characters 
 * like newline, backspace, and tab. It also manages line wrapping and scrolling.
 */
void fb_write_char(char c) {
    if (c == '\n') {
        // Newline
        cursor_x = 0;
        cursor_y++;
    } 
    else if (c == '\b') {
        // Backspace
        if (cursor_x > 0) {
            cursor_x--;
        } else if (cursor_y > 0) {
            cursor_y--;
            cursor_x = COLS - 1;
        }
    }
    else if (c == '\r') {
        // Carriage Return: Reset X to 0
        cursor_x = 0;
    }
    else if (c == '\t') {
        // Tab
        cursor_x = (cursor_x + 4) & ~(4 - 1); 
    }
    else if (c >= ' ') { 
        // Printable Characters
        unsigned int location = (cursor_y * COLS + cursor_x) * 2;
        fb[location] = c;
        fb[location + 1] = ((FB_DEFAULT_BACKGROUND & 0x0F) << 4) | (FB_DEFAULT_FOREGROUND & 0x0F);
        cursor_x++;
    }

    // Line Wrap
    if (cursor_x >= COLS) {
        cursor_x = 0;
        cursor_y++;
    }

    // Scroll Management
    fb_scroll();

    // Update the visual hardware cursor
    fb_move_cursor(cursor_x, cursor_y);
}

/** * write: 
 * Wrapper function called by kprint to write a buffer to the screen.
 */
int write(char *string, unsigned int len) {
    for (unsigned int i = 0; i < len; i++) {
        fb_write_char(string[i]);
    }
    return len;
}

/** * clear_screen: 
 * Clears the entire screen and resets the cursor to (0,0).
 */
void clear_screen() {
    for (int i = 0; i < COLS * ROWS * 2; i += 2) {
        fb[i] = ' ';
        fb[i + 1] = ((FB_DEFAULT_BACKGROUND & 0x0F) << 4) | (FB_DEFAULT_FOREGROUND & 0x0F);
    }
    cursor_x = 0;
    cursor_y = 0;
    fb_move_cursor(0, 0);
}
#include "io.h"

char *fb = (char *) 0x000B8000; // frame buffer memory address
int cursor_position = 0; // cursor position for the frame buffer

/* The I/O ports */
#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

/* Default write colors */
#define FB_DEFAULT_FOREGROUND 0x00
#define FB_DEFAULT_BACKGROUND 0x0F

/** fb_move_cursor:
*  Moves the cursor of the framebuffer to the given position
*
*  @param pos The new position of the cursor
*/
void fb_move_cursor(unsigned short pos)
{
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT,    pos & 0x00FF);
}

/** fb_write_cell:
*  Writes a character with the given foreground and background to position i
*  in the framebuffer.
*
*  @param i  The location in the framebuffer
*  @param c  The character
*  @param fg The foreground color
*  @param bg The background color
*/
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg) {
    fb[i * 2] = c;
    fb[i * 2 + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
}

/** fb_write_cell:
*  Clear the screen writing black characters and resetting the cursor
*/
void clear_screen() {
    unsigned short cells = 80 * 25;
    for (unsigned short i = 0; i < cells; i++) {
        fb_write_cell(i, ' ', 0x00, 0x00);
    }

    // cursor reset
    cursor_position = 0;
    fb_move_cursor(0);
}

/**
* Write a string (buffer) to the framebuffer.
*
* @param string: Pointer to the string to write.
* @param len: Length of the string.
* @return: The number of characters written.
*/
int write(char *string, unsigned int len) {
    for (unsigned int i = 0; i < len; i++) {
        fb_write_cell(cursor_position, string[i], FB_DEFAULT_FOREGROUND, FB_DEFAULT_BACKGROUND);
        cursor_position++;
        fb_move_cursor(cursor_position);
    }
    return len;
}
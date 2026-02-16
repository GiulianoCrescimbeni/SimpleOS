#include <drivers/framebuffer.h>
#include <drivers/serial.h>

/** * kprint:
 * Prints a string either to the screen (using the framebuffer driver) 
 * or to the serial port (COM1), depending on the mode.
 *
 * @param string  Pointer to the string to print
 * @param mode    Output mode (0 for screen, 1 for serial/COM1)
 * @return        The number of characters printed
 */
int kprint(char* string, int mode) {
    int i = 0; 
    while (string[i] != '\0') {
        if (mode == 0) {
            // Screen Mode: The framebuffer driver handles special chars (\n, \b) 
            // and scrolling logic internally. We just pass the character.
            write(&string[i], 1);
        } else {
            // Serial Mode: Send output to COM1 (useful for debugging)
            serial_write(&string[i], 1);
        }
        i++;
    }
    return i;
}
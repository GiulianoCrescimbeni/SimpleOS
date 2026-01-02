#include <drivers/framebuffer.h>
#include <drivers/serial.h>

/** kprint:
*  Print a string, either on screen using the frame buffer or
*  on com1 using serial driver 
*
*  @param string    Pointer to the string to print
*  @param mode      The print modality (0 for screen print, 1 for com1 print)
*  @return          The number of printed char
*/

int kprint(char* string, int mode) {
    int i = 0; // Counter for characters
    while (string[i] != '\0') { // Process until null terminator
        if (string[i] == '\n') {
            if (mode == 0) {
                fb_new_line();
            } else {
                serial_write(&string[i], 1); // Send '\n' to serial output
            }
        } else {
            if (mode == 0) {
                write(&string[i], 1);
            } else {
                serial_write(&string[i], 1);
            }
        }
        i++; // Move to the next character
    }

    return i; // Return the number of characters printed
}
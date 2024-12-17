#include "../drivers/framebuffer.h"
#include "../drivers/serial.h"

/** kprint:
*  Print a string, either on screen using the frame buffer or
*  on com1 using serial driver 
*
*  @param string    Pointer to the string to print
*  @param len       The lenght of the string
*  @param mode      The print modality (0 for screen print, 1 for com1 print)
*  @return          The number of printed char
*/
int kprint(char* string, int len, int mode) {
    if(mode == 0) {
        write(string, len);
    } else {
        serial_write(string, len);
    }
    return len;
}
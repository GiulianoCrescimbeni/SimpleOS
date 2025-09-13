#include <drivers/framebuffer.h>
#include <drivers/serial.h>
#include <stdarg.h>

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

/**
void itoa(int value, char* str, int base) {
    char* digits = "0123456789ABCDEF";
    char buffer[32];
    int i = 0, is_negative = 0;

    if (value == 0) {
        str[0] = '0'; str[1] = '\0';
        return;
    }

    if (base == 10 && value < 0) {
        is_negative = 1;
        value = -value;
    }

    while (value > 0) {
        buffer[i++] = digits[value % base];
        value /= base;
    }

    if (is_negative) {
        buffer[i++] = '-';
    }

    int j = 0;
    while (i > 0) {
        str[j++] = buffer[--i];
    }
    str[j] = '\0';
}
*/
/**

int printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    int i = 0, j = 0;

    while (format[i] != '\0') {
        if (format[i] == '%') {
            i++;
            if (format[i] == 'd') {
                int num = va_arg(args, int);
                char num_str[16];
                itoa(num, num_str, 10);
                kprint(num_str, 0);
                j += 0;
            } else if (format[i] == 'x') {
                int num = va_arg(args, int);
                char num_str[16];
                itoa(num, num_str, 16);
                kprint("0x", 0);
                kprint(num_str, 0);
                j += 0;
            } else if (format[i] == 's') {
                char* str = va_arg(args, char*);
                kprint(str, 0);
                j += 0;
            } else if (format[i] == 'c') {
                char c = (char)va_arg(args, int);
                char temp[2] = {c, '\0'};
                kprint(temp, 0);
                j += 0;
            } else {
                kprint("%", 0);
                char temp[2] = {format[i], '\0'};
                kprint(temp, 0);
            }
        } else {
            char temp[2] = {format[i], '\0'};
            kprint(temp, 0);
        }
        i++;
    }

    va_end(args);
    return j;
}*/
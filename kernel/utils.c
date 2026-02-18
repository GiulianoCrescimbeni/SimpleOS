#include <stdint.h>
#include <stdarg.h>
#include <kernel/kprint.h>
 #include <kernel/idt_load.h>
#include <kernel/lock.h>

spinlock_t video_lock = 0;

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

void utoa(unsigned int value, char* str, int base) {
    char* digits = "0123456789ABCDEF";
    char buffer[32];
    int i = 0;

    if (value == 0) {
        str[0] = '0'; str[1] = '\0';
        return;
    }

    while (value > 0) {
        buffer[i++] = digits[value % base];
        value /= base;
    }

    int j = 0;
    while (i > 0) {
        str[j++] = buffer[--i];
    }
    str[j] = '\0';
}

int printf(const char* format, ...) {
    uint32_t flags = acquire_irqsave(&video_lock);
    va_list args;
    va_start(args, format);
    
    int i = 0, j = 0;

    while (format[i] != '\0') {
        if (format[i] == '%') {
            i++;
            if (format[i] == 'd') {
                int num = va_arg(args, int);
                char num_str[32];
                itoa(num, num_str, 10);
                kprint(num_str, 0);
            } 
            else if (format[i] == 'u') {
                unsigned int num = va_arg(args, unsigned int);
                char num_str[32];
                utoa(num, num_str, 10);
                kprint(num_str, 0);
            } 
            else if (format[i] == 'x') {
                unsigned int num = va_arg(args, unsigned int);
                char num_str[32];
                utoa(num, num_str, 16);
                kprint("0x", 0);
                kprint(num_str, 0);
            } 
            else if (format[i] == 's') {
                char* str = va_arg(args, char*);
                if (!str) str = "(null)";
                kprint(str, 0);
            } 
            else if (format[i] == 'c') {
                char c = (char)va_arg(args, int);
                char temp[2] = {c, '\0'};
                kprint(temp, 0);
            } 
            else {
                // Caso %% o % sconosciuto
                char temp[2] = {format[i], '\0'};
                if (format[i] != '%') kprint("%", 0);
                kprint(temp, 0);
            }
        } else {
            char temp[2] = {format[i], '\0'};
            kprint(temp, 0);
        }
        i++;
    }

    va_end(args);
    release_irqrestore(&video_lock, flags);
    return j;
}

void *memset(void* dest, int val, uint32_t len) {
    uint8_t* ptr = (uint8_t*)dest;
    for (uint32_t i = 0; i < len; i++) {
        ptr[i] = (uint8_t)val;
    }
    return ptr;
}

void kstrncpy(char *dest, const char *src, int n) {
    int i;
    for (i = 0; i < n && src[i] != '\0'; i++) dest[i] = src[i];
    for ( ; i < n; i++) dest[i] = '\0';
}
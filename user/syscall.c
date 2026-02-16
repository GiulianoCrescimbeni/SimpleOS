#include <user/syscall.h>

void syscall_exit() {
    asm volatile(
        "mov $0, %eax;"
        "int $0x80"
    ); 
}

// System Call Write (ID 1)
void syscall_write(char *msg) {
    asm volatile( 
        "mov $1, %%eax;" 
        "mov %0, %%ebx;"
        "int $0x80;"
        : 
        : "r" (msg)
        : "eax", "ebx"
    );
}

// System Call Read Char (ID 2)
char syscall_read_char() {
    char c;
    asm volatile(
        "mov $2, %%eax;" 
        "int $0x80;" 
        "mov %%al, %0;" 
        : "=r" (c) 
        : 
        : "eax" 
    );
    return c;
}

void syscall_read_line(char *buffer, int max_len) {
    int i = 0;
    
    for(int k=0; k<max_len; k++) buffer[k] = 0;

    while (i < max_len - 1) {
        char c = syscall_read_char();
        
        if (c != 0) { 
            // SEND
            if (c == '\n') { 
                syscall_write("\n"); 
                buffer[i] = '\0';
                break; 
            }
            // BACKSPACE
            else if (c == '\b') { 
                if (i > 0) {
                    i--;
                    buffer[i] = '\0';
                    syscall_write("\b \b"); 
                }
            }
            // NORMAL CHARACTER
            else {
                if (c >= 32 && c <= 126) {
                    buffer[i] = c;
                    i++;
                    buffer[i] = '\0';
                    
                    char echo[2] = {c, '\0'};
                    syscall_write(echo);
                }
            }
        }
    }
}
#include <user/shell.h>
#include <user/syscall.h>

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void shell_main() {
    char command_buffer[64];

    syscall_write("\n--- Welcome to SimpleOS Shell ---\n");

    while(1) {
        syscall_write("\nSimpleOS> "); 
        
        syscall_read_line(command_buffer, 64);

        if (strcmp(command_buffer, "help") == 0) {
             syscall_write("\nCommands available:\n");
             syscall_write(" - help: show this menu\n");
             syscall_write(" - exit: terminate the shell\n");
        } 
        else if (strcmp(command_buffer, "exit") == 0) {
            syscall_write("\nExiting Shell...\n");
            syscall_exit();
        }
        else if (command_buffer[0] != '\0') {
             syscall_write("\nUnknown command: ");
             syscall_write(command_buffer);
             syscall_write("\n");
        }
    }
}
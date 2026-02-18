#include <user/shell.h>
#include <user/syscall.h>
#include <kernel/fs.h>
#include <kernel/utils.h>

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int str_starts_with(const char *s1, const char *s2) {
    while (*s2) {
        if (*s1 != *s2) return 0;
        s1++; s2++;
    }
    return 1;
}

int strlen(const char *str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

int atoi(char *str) {
    int res = 0;
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] >= '0' && str[i] <= '9')
            res = res * 10 + str[i] - '0';
    }
    return res;
}

void shell_main() {
    char command_buffer[64];
    char file_buffer[2048];

    syscall_write("\n--- Welcome to SimpleOS Shell ---\n");

    while(1) {
        syscall_write("\nSimpleOS> "); 
        
        syscall_read_line(command_buffer, 64);

        if (strcmp(command_buffer, "help") == 0) {
            syscall_write("\nCommands available:\n");
            syscall_write(" - help: show this menu\n");
            syscall_write(" - exit: terminate the shell\n");
            syscall_write(" - ls: list files in the current directory\n");
            syscall_write(" - cat <filename>: display the contents of a file\n");
            syscall_write(" - touch <filename>: create a new empty file\n");
            syscall_write(" - write <filename> <content>: write content to a file\n");
            syscall_write(" - rm <filename>: delete a file\n");
            syscall_write(" - uptime: show system uptime in seconds\n");
            syscall_write(" - sleep: sleep for a half second\n");
            syscall_write(" - ps: list running processes\n");
            syscall_write(" - kill <pid>: kill a process by PID\n");
            syscall_write(" - clear: clear the screen\n");
        } 
        else if (strcmp(command_buffer, "exit") == 0) {
            syscall_write("\nExiting Shell...\n");
            syscall_exit();
        }
        else if (strcmp(command_buffer, "ls") == 0) {
            int i = 0;
            syscall_write("\nFiles:\n");
            
            while(1) {
                int flags = syscall_list_files(i, file_buffer);
                
                if (flags != -1) {
                    syscall_write(" - ");
                    syscall_write(file_buffer);
                    int len = strlen(file_buffer);
                    while(len < 15) { 
                        syscall_write(" "); 
                        len++; 
                    }

                    if (flags & FS_STATIC) {
                        syscall_write("[Read Only]\n");
                    } else {
                        syscall_write("[Read/Write]\n");
                    }

                    i++;
                } else {
                    break;
                }
            }
        }
        else if (str_starts_with(command_buffer, "cat ")) {
            char *filename = command_buffer + 4;
            
            for(int k=0; k<2048; k++) file_buffer[k] = 0;

            int bytes = syscall_read_file(filename, file_buffer);
            
            if (bytes >= 0) {
                syscall_write("\n");
                syscall_write(file_buffer);
                syscall_write("\n");
            } else {
                syscall_write("\nError: File not found.\n");
            }
        }
        else if (str_starts_with(command_buffer, "touch ")) {
            char *filename = command_buffer + 6;
            if (syscall_create_file(filename) == 0) {
                syscall_write("File created.\n");
            } else {
                syscall_write("Error creating file\n");
            }
        }
        else if (str_starts_with(command_buffer, "write ")) {
            char *args = command_buffer + 6;
            
            int i = 0;
            while (args[i] != ' ' && args[i] != 0) i++;
            
            if (args[i] == ' ') {
                args[i] = 0;
                char *filename = args;
                char *content = args + i + 1;
                
                int len = strlen(content);
                int written = syscall_write_file(filename, content, len);
                
                if (written == 0) {
                    syscall_write("Error: File not found or Read-Only.\n");
                }
            } else {
                syscall_write("Usage: write <filename> <text>\n");
            }
        }
        else if (str_starts_with(command_buffer, "rm ")) {
            char *filename = command_buffer + 3;
            
            int res = syscall_delete_file(filename);
            
            if (res == 0) {
                syscall_write("File deleted.\n");
            } else if (res == -1) {
                syscall_write("Error: File not found.\n");
            } else if (res == -2) {
                syscall_write("Error: Cannot delete Read-Only file.\n");
            } else {
                syscall_write("Unknown error.\n");
            }
        }
        else if (strcmp(command_buffer, "uptime") == 0) {
            uint32_t ticks = syscall_get_ticks();
            uint32_t seconds = ticks / 100;
            
            char num_buf[32];
            itoa(seconds, num_buf, 10);
            
            syscall_write("System Up Time: ");
            syscall_write(num_buf);
            syscall_write(" seconds.\n");
        }
        else if (str_starts_with(command_buffer, "sleep")) {
            syscall_write("Sleeping...\n");
            syscall_sleep(100);
            syscall_write("Woke up!\n");
        }
        else if (str_starts_with(command_buffer, "kill ")) {
            char *pid_str = command_buffer + 5;
            int pid = atoi(pid_str);
            
            if (pid > 0) {
                int res = syscall_kill(pid);
                if (res == 0) {
                    syscall_write("Process killed.\n");
                } else {
                    syscall_write("Error: Could not kill process.\n");
                }
            } else {
                syscall_write("Invalid PID.\n");
            }
        }
        else if (strcmp(command_buffer, "ps") == 0) {
            process_info_t procs[10];
            int count = syscall_get_process_list(procs, 10);
            
            syscall_write("\nPID  | Name\n");
            syscall_write("-----|--------------------\n");
            
            for (int i = 0; i < count; i++) {
                char pid_buf[10];
                itoa(procs[i].pid, pid_buf, 10);
                
                syscall_write(pid_buf);
                
                int pad = 5 - strlen(pid_buf);
                while(pad-- > 0) syscall_write(" ");
                
                syscall_write("| ");
                syscall_write(procs[i].name);
                syscall_write("\n");
            }
        }
        else if (strcmp(command_buffer, "clear") == 0) {
            syscall_clear();
        }
        else if (command_buffer[0] != '\0') {
             syscall_write("\nUnknown command: ");
             syscall_write(command_buffer);
             syscall_write("\n");
        }
    }
}
#ifndef SYSCALL_H
#define SYSCALL_H

void syscall_exit();
void syscall_write(char *msg);
char syscall_read_char();
void syscall_read_line(char *buffer, int max_len);

#endif
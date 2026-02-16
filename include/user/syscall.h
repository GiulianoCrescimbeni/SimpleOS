#ifndef SYSCALL_H
#define SYSCALL_H

void syscall_exit();
void syscall_write(char *msg);
char syscall_read_char();
void syscall_read_line(char *buffer, int max_len);
int syscall_list_files(int index, char *buffer);
int syscall_read_file(char *filename, char *buffer);
int syscall_create_file(char *filename);
int syscall_write_file(char *filename, char *content, int len);
int syscall_delete_file(char *filename);

#endif
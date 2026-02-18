#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

typedef struct {
    uint32_t pid;
    char name[32];
} process_info_t;

void syscall_exit();
void syscall_write(char *msg);
char syscall_read_char();
void syscall_read_line(char *buffer, int max_len);
int syscall_list_files(int index, char *buffer);
int syscall_read_file(char *filename, char *buffer);
int syscall_create_file(char *filename);
int syscall_write_file(char *filename, char *content, int len);
int syscall_delete_file(char *filename);
uint32_t syscall_get_ticks();
void syscall_sleep(uint32_t ticks);
int syscall_kill(uint32_t pid);
int syscall_get_process_list(process_info_t *buffer, int max_count);
void syscall_clear();

#endif
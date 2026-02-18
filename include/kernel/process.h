#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <kernel/interrupt.h>

typedef struct {
    uint32_t pid;
    char name[32];
} process_info_t;

typedef struct process {
    uint32_t pid;
    char name[32];
    uint32_t esp;
    uint32_t stack_start;
    struct process *next;
} process_t;

void init_multitasking();
void create_kernel_thread(void (*function)(void), char *name); 
void schedule();
int get_current_pid();
int kill_process(uint32_t pid);
void exit_process();
int get_process_list(process_info_t *buffer, int max_count);

#endif
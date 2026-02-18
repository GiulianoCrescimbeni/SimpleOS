#include <kernel/process.h>
#include <kernel/kheap.h>
#include <kernel/kprint.h>
#include <kernel/idt_load.h>
#include <kernel/lock.h>
#include <kernel/interrupt.h>

#include <debug.h>

extern void switch_to_task(uint32_t *old_esp_ptr, uint32_t new_esp);

process_t *current_process = 0;
process_t *ready_queue = 0;
uint32_t next_pid = 0;

void kthread_wrapper(void (*function)(void)) {
    enable_interrupt(); 
    function();

    DEBUG_LOG("[MULTI] Thread finished. Exiting...\n");
    exit_process();
    
    while(1);
}

void init_multitasking() {

    current_process = (process_t*)kmalloc(sizeof(process_t));
    current_process->pid = next_pid++;
    kstrncpy(current_process->name, "Kernel/Shell", 32);
    current_process->esp = 0;
    current_process->stack_start = 0;
    current_process->next = current_process;

    ready_queue = current_process;

}

void create_kernel_thread(void (*function)(void), char *name){
    uint32_t flags = interrupt_save_disable();

    process_t *new_proc = (process_t*)kmalloc(sizeof(process_t));
    new_proc->pid = next_pid++;
    kstrncpy(new_proc->name, name, 32);

    uint32_t stack_size = 4096;
    void *stack_addr = kmalloc(stack_size);
    new_proc->stack_start = (uint32_t)stack_addr;

    uint32_t *esp = (uint32_t*)(new_proc->stack_start + stack_size);

    *--esp = (uint32_t)function;
    *--esp = 0;
    *--esp = (uint32_t)kthread_wrapper;
    *--esp = 0; // EBP
    *--esp = 0; // EDI
    *--esp = 0; // ESI
    *--esp = 0; // EBX

    new_proc->esp = (uint32_t)esp;

    process_t *last = ready_queue;
    while (last->next != ready_queue) {
        last = last->next;
    }
    last->next = new_proc;
    new_proc->next = ready_queue;

    DEBUG_LOG("[MULTI] Created Thread PID %d at Stack 0x%x\n", new_proc->pid, new_proc->esp);
    interrupt_restore(flags);
}

void schedule() {
    if (!ready_queue) return; // Nothing to schedule

    // Simple Round-robin algorithm
    process_t *next_proc = current_process->next;

    if (next_proc == current_process) return; // There is only one process, no need to switch

    process_t *prev_proc = current_process;
    current_process = next_proc;

    // DEBUG_LOG("[MULTI] Switch: PID %d -> PID %d\n", prev_proc->pid, current_process->pid);

    // Context switch
    switch_to_task(&prev_proc->esp, current_process->esp);
}

int get_current_pid() {
    return current_process ? (int)current_process->pid : -1;
}

void exit_process() {
    disable_interrupt();

    process_t *victim = current_process;
    process_t *next_node = victim->next;

    process_t *prev = ready_queue;
    while (prev->next != victim) {
        prev = prev->next;
    }
    prev->next = next_node;

    if (ready_queue == victim) {
        ready_queue = prev;
    }

    DEBUG_LOG("[MULTI] Process PID %d exited.\n", victim->pid);

    kfree((void*)victim->stack_start);
    kfree(victim);
    
    current_process = next_node;

    uint32_t dummy_esp;
    switch_to_task(&dummy_esp, current_process->esp);

    while(1);
}

int kill_process(uint32_t pid) {
    if (pid == 0) {
        printf("Error: Cannot kill Kernel (PID 0).\n");
        return -1;
    }

    if (pid == (uint32_t)get_current_pid()) {
        exit_process();
        return 0;
    }

    uint32_t flags = interrupt_save_disable();

    process_t *victim = 0;
    process_t *prev = 0;
    process_t *iterator = ready_queue;
    
    do {
        if (iterator->next->pid == pid) {
            prev = iterator;
            victim = iterator->next;
            break;
        }
        iterator = iterator->next;
    } while (iterator != ready_queue);

    if (!victim) {
        interrupt_restore(flags);
        printf("Error: PID %d not found.\n", pid);
        return -1;
    } 
    
    prev->next = victim->next;
    if (ready_queue == victim) ready_queue = prev;

    kfree((void*)victim->stack_start);
    kfree(victim);

    DEBUG_LOG("[MULTI] Killed PID %d. Memory freed.\n", pid);

    interrupt_restore(flags);
    return 0;
}

int get_process_list(process_info_t *buffer, int max_count) {
    uint32_t flags = interrupt_save_disable();
    
    process_t *iterator = ready_queue;
    int count = 0;

    do {
        if (count >= max_count) break;

        buffer[count].pid = iterator->pid;
        kstrncpy(buffer[count].name, iterator->name, 32);
        
        count++;
        iterator = iterator->next;
    } while (iterator != ready_queue);

    interrupt_restore(flags);
    return count;
}

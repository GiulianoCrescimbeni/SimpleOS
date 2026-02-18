#include <stdint.h>
#include <kernel/lock.h>
#include <kernel/interrupt.h>

void acquire(spinlock_t *lock) {
    while (!__sync_bool_compare_and_swap(lock, 0, 1)) {
        asm volatile("pause"); 
    }
}

void release(spinlock_t *lock) {
    __sync_synchronize();
    *lock = 0;
}

uint32_t acquire_irqsave(spinlock_t *lock) {
    uint32_t flags = interrupt_save_disable();
    acquire(lock);
    return flags;
}

void release_irqrestore(spinlock_t *lock, uint32_t flags) {
    release(lock);
    interrupt_restore(flags);
}
#ifndef LOCK_H
#define LOCK_H

typedef volatile int spinlock_t;

void acquire(spinlock_t *lock);
void release(spinlock_t *lock);
uint32_t acquire_irqsave(spinlock_t *lock);
void release_irqrestore(spinlock_t *lock, uint32_t flags);

#endif
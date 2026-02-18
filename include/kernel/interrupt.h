#ifndef INCLUDE_INTERRUPT_H
#define INCLUDE_INTERRUPT_H

#include <stdint.h>

typedef struct {
    // 1. Pushed by 'common_interrupt_handler' (in irqs.s)
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp;
    uint32_t ebx, edx, ecx, eax;

    // 2. Pushed by the interrupt stub (macros in irqs.s)
    uint32_t int_no;
    uint32_t err_code;

    // 3. Pushed by the CPU automatically
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t useresp;
    uint32_t ss;
} registers_t;

void interrupt_handler(registers_t *regs);
static inline uint32_t interrupt_save_disable() {
    uint32_t flags;
    asm volatile("pushf; pop %0; cli" : "=r"(flags));
    return flags;
}

static inline void interrupt_restore(uint32_t flags) {
    asm volatile("push %0; popf" : : "r"(flags));
}

#endif
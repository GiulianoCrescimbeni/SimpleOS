#ifndef INCLUDE_INTERRUPT_H
#define INCLUDE_INTERRUPT_H

/* Interrupt info */
struct idt_info {
	uint32_t idt_index;
	uint32_t error_code;
} __attribute__((packed));
typedef struct idt_info idt_info_t;

/* State of the cpu before the interrupt */
struct cpu_state {
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t edx;
	uint32_t ecx;
	uint32_t ebx;
	uint32_t eax;
	uint32_t esp;
} __attribute__((packed));
typedef struct cpu_state cpu_state_t;

/* State of the stack before the interrupt */
struct stack_state {
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t user_esp;
    uint32_t user_ss;
} __attribute__((packed));
typedef struct stack_state stack_state_t;

void interrupt_handler(cpu_state_t state, idt_info_t info, stack_state_t exec);

#endif
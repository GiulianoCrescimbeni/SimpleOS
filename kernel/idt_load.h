#ifndef IDT_LOAD_H
#define IDT_LOAD_H

#include "stdint.h"

extern void idt_load(uint32_t idt_address);
extern void enable_interrupt();
extern void disable_interrupt();

#endif
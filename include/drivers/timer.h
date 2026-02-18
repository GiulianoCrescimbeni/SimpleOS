#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <kernel/interrupt.h>

void init_timer(uint32_t freq);
void timer_handler(registers_t *regs);
uint32_t get_tick_count();
void sleep(uint32_t ticks);

#endif
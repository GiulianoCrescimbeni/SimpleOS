#ifndef INCLUDE_IDT_H
#define INCLUDE_IDT_H

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void idt_init();

#endif
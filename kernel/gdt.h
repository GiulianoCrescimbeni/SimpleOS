#ifndef INCLUDE_GDT_H
#define INCLUDE_GDT_H

#define PL0 0x0
#define PL3 0x3

void gdt_create_entry(uint32_t n, uint8_t pl, uint8_t type);
void gdt_init();

#endif
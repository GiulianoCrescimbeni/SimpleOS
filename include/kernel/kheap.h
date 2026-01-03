#ifndef KHEAP_H
#define KHEAP_H

#include <stdint.h>

typedef struct heap_header {
    struct heap_header *next;
    uint32_t size;
    uint8_t is_free;
} heap_header_t;

void kheap_init(uint32_t start_address, uint32_t size);
void *kmalloc(uint32_t size);
void kfree(void *ptr);

#endif
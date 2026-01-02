#ifndef FRAME_ALLOCATOR_H
#define FRAME_ALLOCATOR_H

#include <stdint.h>
#include <kernel/multiboot.h>

#define MAX_FRAMES 32768 // 128 MB / 4KB

void init_frame_allocator(multiboot_info_t *mbinfo);
uint32_t alloc_frame();
void free_frame(uint32_t addr);

#endif

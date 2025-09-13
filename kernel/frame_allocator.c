#include <kernel/frame_allocator.h>
#include <kernel/multiboot.h>
#include <kernel/kprint.h>

#define FRAME_SIZE 4096 // 4KB each frame
#define BITMAP_SIZE (MAX_FRAMES / 8)

static uint8_t bitmap[BITMAP_SIZE];
static uint32_t total_frames = 0;
static uint32_t used_frames = 0;

static void set_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / FRAME_SIZE;
    bitmap[frame / 8] |= (1 << (frame % 8));
    used_frames++;
}

static void clear_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / FRAME_SIZE;
    bitmap[frame / 8] &= ~(1 << (frame % 8));
    used_frames--;
}

/** Check if a frame is available (returns 1 if occupied, 0 if available) */
static uint8_t test_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / FRAME_SIZE;
    return bitmap[frame / 8] & (1 << (frame % 8));
}

void init_frame_allocator(multiboot_info_t *mbinfo) {
    if (!(mbinfo->flags & MULTIBOOT_INFO_MEM_MAP)) {
        kprint("No memory map provided\n", 1);
        return;
    }

    multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)(mbinfo->mmap_addr + 0xC0000000);
    multiboot_memory_map_t* mmap_end = (multiboot_memory_map_t*)(mbinfo->mmap_addr + mbinfo->mmap_length + 0xC0000000);

    for (int i = 0; i < BITMAP_SIZE; i++) {
        bitmap[i] = 0xFF;
    }

    while (mmap < mmap_end) {
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            uint32_t addr = (uint32_t)(mmap->addr);
            uint32_t length = (uint32_t)(mmap->len);

            uint32_t start = (addr + FRAME_SIZE - 1) & ~(FRAME_SIZE - 1);
            uint32_t end = addr + length;

            for (uint32_t i = start; i + FRAME_SIZE <= end && i < MAX_FRAMES * FRAME_SIZE; i += FRAME_SIZE) {
                clear_frame(i);
                total_frames++;
            }
        }

        mmap = (multiboot_memory_map_t*)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
    }

    //kprint("Frame allocator initialized: %u frames available\n", total_frames);
}

uint32_t alloc_frame() {
    for (uint32_t i = 0; i < total_frames; i++) {
        uint32_t addr = i * FRAME_SIZE;
        if (!test_frame(addr)) {
            set_frame(addr);
            return addr;
        }
    }

    kprint("No free frames available\n", 1);
    return 0;
}

void free_frame(uint32_t addr) {
    clear_frame(addr);
}

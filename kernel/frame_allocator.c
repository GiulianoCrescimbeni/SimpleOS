#include <kernel/frame_allocator.h>
#include <kernel/multiboot.h>
#include <kernel/kprint.h>
#include <kernel/utils.h>

#include <debug.h>

#define FRAME_SIZE 4096 // 4KB
#define BITMAP_SIZE (MAX_FRAMES / 8)

extern char KERNEL_PHYSICAL_START[], KERNEL_PHYSICAL_END[];

static uint8_t bitmap[BITMAP_SIZE];
static uint32_t total_frames = 0;
static uint32_t used_frames = 0;

static inline uint64_t combine(uint32_t high, uint32_t low) {
    return ((uint64_t)high << 32) | low;
}

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

static uint8_t test_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / FRAME_SIZE;
    return bitmap[frame / 8] & (1 << (frame % 8));
}

void init_frame_allocator(multiboot_info_t *mbinfo) {

    total_frames = 0;
    used_frames = 0;

    if (!(mbinfo->flags & MULTIBOOT_INFO_MEM_MAP)) {
        kprint("No memory map provided\n", 1);
        return;
    }

    // Initialize all frames as occupied (0xFF)
    for (int i = 0; i < BITMAP_SIZE; i++) {
        bitmap[i] = 0xFF;
    }

    // First pass: mark available memory regions as free
    multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)(mbinfo->mmap_addr + 0xC0000000);
    multiboot_memory_map_t *mmap_end = (multiboot_memory_map_t *)(mbinfo->mmap_addr + mbinfo->mmap_length + 0xC0000000);

    while ((uint32_t)mmap < (uint32_t)mmap_end) {
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            uint64_t start64 = combine(mmap->addr_high, mmap->addr_low);
            uint64_t end64   = start64 + combine(mmap->len_high, mmap->len_low);

            // Exclude memory below 1 MiB
            if (end64 <= 0x100000) {
                mmap = (multiboot_memory_map_t *)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
                continue;
            }

            if (start64 < 0x100000) start64 = 0x100000;
            if (end64 > MAX_FRAMES * FRAME_SIZE) end64 = MAX_FRAMES * FRAME_SIZE;

            uint32_t start = (uint32_t)((start64 + FRAME_SIZE - 1) & ~(FRAME_SIZE - 1));
            uint32_t end   = (uint32_t)(end64 & ~(FRAME_SIZE - 1));

            DEBUG_LOG("[FRAME ALLOCATOR] Usable memory region: base = %x, end = %x\n", start, end);

            for (uint32_t addr = start; addr + FRAME_SIZE <= end; addr += FRAME_SIZE) {
                clear_frame(addr);
                total_frames++;
            }
        }

        mmap = (multiboot_memory_map_t *)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
    }

    // Second pass: re-mark unavailable memory as reserved
    mmap = (multiboot_memory_map_t *)((uint32_t)mbinfo->mmap_addr + 0xC0000000); // Explicit cast for safety
    while ((uint32_t)mmap < (uint32_t)mmap_end) {
        if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE) {
            uint64_t start64 = combine(mmap->addr_high, mmap->addr_low);
            uint64_t end64   = start64 + combine(mmap->len_high, mmap->len_low);

            if (start64 < 0x100000) start64 = 0x100000; // Low address protection already handled
            if (end64 > MAX_FRAMES * FRAME_SIZE) end64 = MAX_FRAMES * FRAME_SIZE;

            uint32_t start = (uint32_t)((start64 + FRAME_SIZE - 1) & ~(FRAME_SIZE - 1));
            uint32_t end   = (uint32_t)(end64 & ~(FRAME_SIZE - 1));

            for (uint32_t addr = start; addr + FRAME_SIZE <= end; addr += FRAME_SIZE) {
                
                // Only decrement if frame was previously marked free.
                // If already occupied (from init), just ensure it stays set.
                if (!test_frame(addr)) { 
                    set_frame(addr);
                    total_frames--;
                } else {
                    // Already marked as occupied (0xFF), ensure it stays that way
                    set_frame(addr);
                    // Do not decrement total_frames here
                }
            }
        }
        mmap = (multiboot_memory_map_t *)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
    }

    // Reserve frames used by the kernel code
    uint32_t kernel_start = (uint32_t)KERNEL_PHYSICAL_START;
    uint32_t kernel_end   = (uint32_t)KERNEL_PHYSICAL_END;

    // Convert virtual kernel addresses to physical addresses
    if (kernel_start >= 0xC0000000) kernel_start -= 0xC0000000;
    if (kernel_end >= 0xC0000000)   kernel_end   -= 0xC0000000;

    kernel_start = (kernel_start + FRAME_SIZE - 1) & ~(FRAME_SIZE - 1);
    kernel_end   = kernel_end & ~(FRAME_SIZE - 1);

    for (uint32_t addr = kernel_start; addr <= kernel_end; addr += FRAME_SIZE) {
        if (!test_frame(addr)) {
            set_frame(addr);
            total_frames--;
        }
    }
    
    DEBUG_LOG("[FRAME ALLOCATOR] Frame allocator initialized: %u frames available\n", total_frames);
}

uint32_t alloc_frame() {
    for (uint32_t i = 0; i < MAX_FRAMES; i++) {
        uint32_t addr = i * FRAME_SIZE;
        if (!test_frame(addr)) {
            set_frame(addr);
            return addr;
        }
    }

    DEBUG_LOG("[FRAME ALLOCATOR]No free frames available\n");
    return 0;
}
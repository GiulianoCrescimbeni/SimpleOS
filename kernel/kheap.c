#include <kernel/kheap.h>
#include <kernel/utils.h>
#include <kernel/frame_allocator.h>

static heap_header_t *head = 0;
static uint32_t heap_end_address = 0; // Track the end of the virtual heap

// External function from frame_allocator.c
extern uint32_t alloc_frame();

// Initialize the heap by creating a single block
void kheap_init(uint32_t start_address, uint32_t size) {
    head = (heap_header_t *)start_address;
    head->next = 0;
    head->size = size - sizeof(heap_header_t);
    head->is_free = 1;
    
    // Calculate the end address of the initial heap
    heap_end_address = start_address + size;
}

// Internal function to expand the heap by allocating new frames
void *kheap_extend(uint32_t size) {
    uint32_t total_needed = size + sizeof(heap_header_t);

    // Calculate how many 4KB pages we need (round up)
    uint32_t pages_needed = (total_needed + 0xFFF) / 0x1000;
    
    // Save the current end, this will be the start of the new block
    heap_header_t *new_block = (heap_header_t *)heap_end_address;

    for (uint32_t i = 0; i < pages_needed; i++) {
        // 1. Get a physical frame
        uint32_t phys_addr = alloc_frame();
        
        if (phys_addr == 0) {
            printf("Error: Physical Memory Exhausted!\n");
            return 0;
        }

        // 2. TODO: Map this physical frame to the virtual address 'heap_end_address'
        // For now it relies on the loader's initial 4MB mapping.
        // Once you implement VMM, it needs to call map_page(heap_end_address, phys_addr, flags) here.
        
        // Move the end pointer forward by one page (4096 bytes)
        heap_end_address += 0x1000;
    }

    // Initialize the new block header
    new_block->is_free = 0;
    new_block->size = (pages_needed * 0x1000) - sizeof(heap_header_t);
    new_block->next = 0;

    // Link the new block to the end of the existing list 
    heap_header_t *current = head;
    while (current->next != 0) {
        current = current->next;
    }
    current->next = new_block;

    // Return the usable area (after the header)
    return (void *)((uint32_t)new_block + sizeof(heap_header_t));
}

void *kmalloc(uint32_t size) {
    if (size == 0) return 0;

    // Word align the size (4 bytes) for performance
    if (size % 4 != 0) {
        size += 4 - (size % 4);
    }

    heap_header_t *current = head;

    // 1. Search for a free block (First Fit)
    while (current != 0) {
        if (current->is_free && current->size >= size) {

            // Try to split and create a new free block if there's enough space
            if (current->size > size + sizeof(heap_header_t)) {
                
                heap_header_t *new_block = (heap_header_t *)((uint32_t)current + sizeof(heap_header_t) + size);
                
                new_block->is_free = 1;
                new_block->size = current->size - size - sizeof(heap_header_t);
                new_block->next = current->next;

                current->size = size;
                current->next = new_block;
            }

            current->is_free = 0;
            
            // Writing address returned to user
            return (void *)((uint32_t)current + sizeof(heap_header_t));
        }
        
        current = current->next;
    }

    // Se arriviamo qui, non c'è spazio (Out of Memory)
    // In un OS avanzato, qui chiederemmo altri frame al Frame Allocator!
    
    // Attempt to extend the heap
    return kheap_extend(size);
}

void kfree(void *ptr) {
    if (ptr == 0) return;

    // The header is located just before the pointer provided by the user
    heap_header_t *header = (heap_header_t *)((uint32_t)ptr - sizeof(heap_header_t));
    
    header->is_free = 1;

    // If the next block is free, merge them into one
    heap_header_t *current = head;
    while (current != 0) {
        if (current->is_free && current->next != 0 && current->next->is_free) {
            current->size += current->next->size + sizeof(heap_header_t);
            current->next = current->next->next;
        }
        current = current->next;
    }
}
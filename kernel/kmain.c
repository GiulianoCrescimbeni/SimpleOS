#include <stdint.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/idt_load.h>
#include <kernel/interrupt.h>
#include <kernel/kprint.h>
#include <kernel/utils.h>
#include <kernel/paging.h>
#include <kernel/multiboot.h>
#include <kernel/frame_allocator.h>
#include <kernel/kheap.h>
#include <drivers/framebuffer.h>
#include <drivers/serial.h>

typedef void (*call_module_t)(void);

extern char KERNEL_PHYSICAL_END[];

static inline uint64_t combine(uint32_t high, uint32_t low) {
    return ((uint64_t)high << 32) | low;
}

void kmain(unsigned int ebx) {
    disable_interrupt();
    printf("Paging Enabled\n");
    gdt_init();
    printf("GDT Loaded\n");
    idt_init();
    printf("IDT Loaded\n");
    clear_screen();
    printf("\n\n .d8888b.  d8b                        888           .d88888b.   .d8888b.  888 \nd88P  Y88b Y8P                        888          d88P\" \"Y88b d88P  Y88b 888 \nY88b.                                 888          888     888 Y88b.      888 \n \"Y888b.   888 88888b.d88b.  88888b.  888  .d88b.  888     888  \"Y888b.   888 \n    \"Y88b. 888 888 \"888 \"88b 888 \"88b 888 d8P  Y8b 888     888     \"Y88b. 888 \n      \"888 888 888  888  888 888  888 888 88888888 888     888       \"888 Y8P \nY88b  d88P 888 888  888  888 888 d88P 888 Y8b.     Y88b. .d88P Y88b  d88P  \"  \n \"Y8888P\"  888 888  888  888 88888P\"  888  \"Y8888   \"Y88888P\"   \"Y8888P\"  888 \n                             888                                              \n                             888                                              \n                             888                                              \n");
    enable_interrupt();

    multiboot_info_t *mbinfo = (multiboot_info_t *) (ebx + 0xC0000000);

    init_frame_allocator(mbinfo);

    uint32_t heap_start = (uint32_t)KERNEL_PHYSICAL_END + 4096;
    uint32_t heap_size = 100 * 1024;
    printf("Heap Start: %x\n", heap_start);
    kheap_init(heap_start, heap_size);

    //** TEST ZONE **/

    //unsigned int address_of_module = mbinfo->mods_addr;
    //if(address_of_module) {

    //}

    //call_module_t start_program = (call_module_t) address_of_module;
    //start_program();

    /**  Kernel Test **/
    // Page Fault Test 
    //trigger_page_fault();

    //** Frame Allocation Test **/
    //uint32_t f1 = alloc_frame();
    //uint32_t f2 = alloc_frame();
    //uint32_t f3 = alloc_frame();
    //printf("Allocated frames: %x %x %x\n", f1, f2, f3);

    /** kmalloc Test **/
    // uint32_t *a = (uint32_t *)kmalloc(sizeof(uint32_t));
    // *a = 123;
    // printf("Allocated int at %x with value %d\n", a, *a);

    while (1) {
        // infinite loop to avoid return to loader
    }
}
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

//** USER MODE SUPPORT START **//
extern void enter_user_mode(uint32_t entry_point, uint32_t user_stack);
extern void gdt_flush_tss(uint16_t tss_selector);

void syscall_write(char *msg) {
    asm volatile(
        "mov $1, %%eax;"    // Syscall Number 1 (SYS_WRITE)
        "mov %0, %%ebx;"
        "int $0x80;"
        : 
        : "r" (msg)
        : "eax", "ebx"
    );
}

void general_protection_fault_test() {
    asm volatile("cli"); 
    while(1);
}

void enter_user_mode_test() {
    syscall_write("User mode enabled\n");
    while(1);
}

//** USER MODE SUPPORT END **//

static inline uint64_t combine(uint32_t high, uint32_t low) {
    return ((uint64_t)high << 32) | low;
}

void kmain(unsigned int ebx) {
    //** KERNEL SETUP **/
    disable_interrupt();
    printf("Paging Enabled\n");
    gdt_init();
    printf("GDT Loaded\n");
    idt_init();
    printf("IDT Loaded\n");
    clear_screen();
    printf("\n\n .d8888b.  d8b                         888           .d88888b.   .d8888b.  888 \nd88P  Y88b Y8P                         888          d88P\" \"Y88b d88P  Y88b 888 \nY88b.                                  888          888     888 Y88b.      888 \n \"Y888b.   888 88888b.d88b.  88888b.   888  .d88b.  888     888  \"Y888b.   888 \n    \"Y88b. 888 888 \"888 \"88b 888 \"88b 888 d8P  Y8b 888     888     \"Y88b. 888 \n      \"888 888 888  888  888 888  888 888 88888888 888     888       \"888 Y8P \nY88b  d88P 888 888  888  888 888 d88P 888 Y8b.     Y88b. .d88P Y88b  d88P  \"  \n \"Y8888P\"  888 888  888  888 88888P\"  888  \"Y8888   \"Y88888P\"   \"Y8888P\"  888 \n                                      888                                     \n                                      888                                     \n                                      888                                     \n");
    enable_interrupt();

    multiboot_info_t *mbinfo = (multiboot_info_t *) (ebx + 0xC0000000);

    init_frame_allocator(mbinfo);

    uint32_t heap_start = (uint32_t)KERNEL_PHYSICAL_END + 4096;
    uint32_t heap_size = 100 * 1024;
    printf("Heap Start: %x\n", heap_start);
    kheap_init(heap_start, heap_size);

    //** KERNEL SETUP END **/

    //** KERNEL TEST ZONE **/

    //unsigned int address_of_module = mbinfo->mods_addr;
    //if(address_of_module) {

    //}

    //call_module_t start_program = (call_module_t) address_of_module;
    //start_program();

    /** Kernel Test **/
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

    //** KERNEL TEST ZONE END **/

    //** USER MODE SETUP **//
    printf("Preparing User Mode Switch\n");

    uint32_t kernel_stack = (uint32_t)kmalloc(4096) + 4096;
    write_tss(5, 0x10, kernel_stack);
    gdt_flush_tss(0x28);
    printf("TSS Initialized.\n");

    uint32_t user_stack = (uint32_t)kmalloc(4096) + 4096;

    //** General Protection Fault Test **/
    //enter_user_mode((uint32_t)general_protection_fault_test, user_stack);

    enter_user_mode((uint32_t)enter_user_mode_test, user_stack);
    //** USER MODE SETUP END **/

    while (1) {
        // infinite loop to avoid return to loader
    }
}
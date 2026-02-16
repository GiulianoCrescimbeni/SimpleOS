#include <stdint.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/idt_load.h>
#include <kernel/kprint.h>
#include <kernel/utils.h>
#include <kernel/multiboot.h>
#include <kernel/frame_allocator.h>
#include <kernel/kheap.h>
#include <drivers/framebuffer.h>
#include <user/shell.h>

extern char KERNEL_PHYSICAL_END[];

extern void enter_user_mode(uint32_t entry_point, uint32_t user_stack);
extern void gdt_flush_tss(uint16_t tss_selector);

void kmain(unsigned int ebx) {
    disable_interrupt();
    clear_screen();
    printf("--- Kernel Booting ---\n\n");

    gdt_init();
    printf("[OK] GDT Loaded\n");

    idt_init();
    printf("[OK] IDT Loaded\n");

    multiboot_info_t *mbinfo = (multiboot_info_t *) (ebx + 0xC0000000);
    init_frame_allocator(mbinfo);
    printf("[OK] Frame Allocator Initialized\n");

    uint32_t heap_start = (uint32_t)KERNEL_PHYSICAL_END + 4096;
    uint32_t heap_size = 100 * 1024; // 100 KB Heap
    kheap_init(heap_start, heap_size);
    printf("[OK] Kernel Heap Initialized (Start: 0x%x)\n", heap_start);

    enable_interrupt();
    printf("[OK] Interrupts Enabled\n");

    uint32_t kernel_stack = (uint32_t)kmalloc(4096) + 4096;
    write_tss(5, 0x10, kernel_stack);
    gdt_flush_tss(0x28); 
    printf("[OK] TSS Initialized\n");

    uint32_t user_stack = (uint32_t)kmalloc(4096) + 4096;

    printf("Jumping to User Mode (Shell)\n");
    
    enter_user_mode((uint32_t)shell_main, user_stack);

    while (1);
}
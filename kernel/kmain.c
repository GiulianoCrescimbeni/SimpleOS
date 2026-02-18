#include <stdint.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/idt_load.h>
#include <kernel/kprint.h>
#include <kernel/utils.h>
#include <kernel/multiboot.h>
#include <kernel/frame_allocator.h>
#include <kernel/kheap.h>
#include <kernel/fs.h>
#include <kernel/tar.h>
#include <kernel/process.h>
#include <drivers/framebuffer.h>
#include <drivers/timer.h>
#include <user/shell.h>

#include <debug.h>

#define PAGE_ALIGN_UP(addr) (((addr) & 0xFFFFF000) + 0x1000)

extern char KERNEL_PHYSICAL_END[];
extern void enter_user_mode(uint32_t entry_point, uint32_t user_stack);
extern void gdt_flush_tss(uint16_t tss_selector);

void task_temp() {
    for(int i=0; i<5; i++) {
        printf("[Task Temp] %d\n", i);
        sleep(100);
    }
}

void task_loop() {
    while(1) {
        printf(".");
        sleep(200);
    }
}

void kmain(unsigned int ebx) {
    disable_interrupt();
    clear_screen();
    DEBUG_LOG("--- Kernel Booting ---\n\n");

    gdt_init();
    idt_init();
    
    // RAMDisk Setup
    multiboot_info_t *mbinfo = (multiboot_info_t *) (ebx + 0xC0000000);
    uint32_t ramdisk_start = 0, ramdisk_end = 0;

    if ((mbinfo->flags & MULTIBOOT_INFO_MODS) && mbinfo->mods_count > 0) {
        multiboot_module_t *mods = (multiboot_module_t *)(mbinfo->mods_addr + 0xC0000000);
        ramdisk_start = (uint32_t)mods[0].mod_start + 0xC0000000;
        ramdisk_end = (uint32_t)mods[0].mod_end + 0xC0000000;
        DEBUG_LOG("[FS] RAMDisk: 0x%x - 0x%x\n", ramdisk_start, ramdisk_end);
    }

    // Memory Manager Setup
    init_frame_allocator(mbinfo);

    uint32_t heap_start = (uint32_t)KERNEL_PHYSICAL_END + 4096;
    if (ramdisk_end > 0 && heap_start < ramdisk_end) {
        heap_start = ramdisk_end;
    }

    if ((heap_start & 0xFFF) != 0) {
        heap_start = PAGE_ALIGN_UP(heap_start);
    }

    kheap_init(heap_start, 100 * 1024); // 100 KB Heap
    DEBUG_LOG("[MEM] Heap Start: 0x%x\n", heap_start);

    // System Services Setup
    init_timer(100);
    init_multitasking();

    if (ramdisk_start != 0) {
        fs_root = parse_tar(ramdisk_start);
        DEBUG_LOG("[FS] TAR Filesystem Mounted\n");
    }

    // User Mode Setup
    uint32_t kernel_stack = (uint32_t)kmalloc(4096) + 4096;
    write_tss(5, 0x10, kernel_stack);
    gdt_flush_tss(0x28);

    //create_kernel_thread(task_temp, "Temp Task");
    //create_kernel_thread(task_loop, "Loop Task");

    DEBUG_LOG("[KERNEL] Jumping to Shell...\n");
    
    uint32_t user_stack = (uint32_t)kmalloc(4096) + 4096;
    enable_interrupt();
    enter_user_mode((uint32_t)shell_main, user_stack);

    while (1);
}
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
#include <drivers/framebuffer.h>
#include <user/shell.h>

#include <debug.h>

extern char KERNEL_PHYSICAL_END[];

extern void enter_user_mode(uint32_t entry_point, uint32_t user_stack);
extern void gdt_flush_tss(uint16_t tss_selector);
extern fs_node_t *parse_tar(uint32_t address);

void kmain(unsigned int ebx) {
    disable_interrupt();
    clear_screen();
    
    DEBUG_LOG("--- Kernel Booting ---\n\n");

    gdt_init();
    DEBUG_LOG("[OK] GDT Loaded\n");

    idt_init();
    DEBUG_LOG("[OK] IDT Loaded\n");

    multiboot_info_t *mbinfo = (multiboot_info_t *) (ebx + 0xC0000000);

    uint32_t ramdisk_location = 0;
    uint32_t ramdisk_end = 0;

    // Searching for RAMDisk in Multiboot Modules
    if (mbinfo->flags & MULTIBOOT_INFO_MODS) {
        if (mbinfo->mods_count > 0) {
            multiboot_module_t *mods = (multiboot_module_t *)(mbinfo->mods_addr + 0xC0000000);
            ramdisk_location = (uint32_t)mods[0].mod_start + 0xC0000000;
            ramdisk_end = (uint32_t)mods[0].mod_end + 0xC0000000;
            
            DEBUG_LOG("[FS] Found RAMDisk at 0x%x (Size: %d bytes)\n", 
                   ramdisk_location, ramdisk_end - ramdisk_location);
        }
    }

    init_frame_allocator(mbinfo);
    DEBUG_LOG("[OK] Frame Allocator Initialized\n");
 
    uint32_t heap_start = (uint32_t)KERNEL_PHYSICAL_END + 4096;

    if (ramdisk_end > 0 && heap_start < ramdisk_end) {
        heap_start = ramdisk_end;
    }

    if ((heap_start & 0xFFFFF000) != heap_start) {
        heap_start &= 0xFFFFF000; // Remove lower 12 bits to align to next page
        heap_start += 0x1000;     // Move to the next page
    }

    uint32_t heap_size = 100 * 1024; // 100 KB Heap
    kheap_init(heap_start, heap_size);
    
    DEBUG_LOG("[OK] Kernel Heap Initialized (Start: 0x%x)\n", heap_start);

    enable_interrupt();
    DEBUG_LOG("[OK] Interrupts Enabled\n");

    if (ramdisk_location != 0) {
        fs_root = parse_tar(ramdisk_location); 
        DEBUG_LOG("[FS] File System TAR Initialized!\n");
    } else {
        printf("[ERR] No RAMDisk found! Did you update grub.cfg?\n");
    }

    uint32_t kernel_stack = (uint32_t)kmalloc(4096) + 4096;
    write_tss(5, 0x10, kernel_stack);
    gdt_flush_tss(0x28); 
    DEBUG_LOG("[OK] TSS Initialized\n");

    uint32_t user_stack = (uint32_t)kmalloc(4096) + 4096;

    DEBUG_LOG("[KERNEL] Jumping to User Mode (Shell)\n");
    
    enter_user_mode((uint32_t)shell_main, user_stack);

    while (1);
}
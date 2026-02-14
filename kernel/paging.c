#include <stdint.h>
#include <kernel/paging.h>

// A page is 4KB
#define PAGE_SIZE 4096
#define PAGE_PRESENT 0x1
#define PAGE_RW      0x2
#define PAGE_USER    0x4    // User mode access

unsigned int page_directory[1024] __attribute__((aligned(PAGE_SIZE)));
unsigned int first_page_table[1024] __attribute__((aligned(PAGE_SIZE)));

void pag_init() {
    // Identity mapping
    for (int i = 0; i < 1024; i++) {
        // Added PAGE_USER to allow access from Ring 3
        first_page_table[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_RW | PAGE_USER;
    }

    for (int i = 1; i < 1024; i++) {
        page_directory[i] = 0;
    }


    // The first entry of the page directory points to the page table
    // Added PAGE_USER to allow access from Ring 3
    page_directory[0] = (unsigned int)first_page_table | PAGE_PRESENT | PAGE_RW | PAGE_USER;    // 0x00000000
    page_directory[768] = (unsigned int)first_page_table | PAGE_PRESENT | PAGE_RW | PAGE_USER;  // 0xC0000000    
    
    // Load page directory in CR3 and enable paging
    asm volatile("mov %0, %%cr3" :: "r"(page_directory));
    unsigned int cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
    asm volatile("jmp 1f\n1:"); // flush TLB / pipeline
}

void trigger_page_fault() {
    volatile uint32_t *ptr = (uint32_t *)0xDEADBEEF;
    *ptr = 42;
}
#include "stdint.h"
#include "idt.h"
#include "../drivers/io.h"
#include "pic.h"
#include "idt_load.h"

#define IDT_ENTRIES 256

// Struttura per una singola entry nella IDT (Interrupt Descriptor)
struct idt_entry {
    uint16_t base_low;     // Parte bassa dell'indirizzo della ISR
    uint16_t sel;          // Segment Selector (codice)
    uint8_t  always0;      // Sempre 0
    uint8_t  flags;        // Flag (tipo e privilegi)
    uint16_t base_high;    // Parte alta dell'indirizzo della ISR
} __attribute__((packed));

// Struttura per il puntatore all'IDT (utilizzato da lidt)
struct idt_ptr {
    uint16_t limit;        // Dimensione della IDT meno 1
    uint32_t base;         // Indirizzo della IDT
} __attribute__((packed));

// La IDT vera e propria e il puntatore all'IDT
struct idt_entry idt[IDT_ENTRIES];
struct idt_ptr idt_descriptor;

// Interrupt handler functions
extern void interrupt_handler_0();
extern void interrupt_handler_1();
extern void interrupt_handler_2();
extern void interrupt_handler_3();
extern void interrupt_handler_4();
extern void interrupt_handler_5();
extern void interrupt_handler_6();
extern void interrupt_handler_7();
extern void interrupt_handler_8();
extern void interrupt_handler_9();
extern void interrupt_handler_10();
extern void interrupt_handler_11();
extern void interrupt_handler_12();
extern void interrupt_handler_13();
extern void interrupt_handler_14();
extern void interrupt_handler_15();
extern void interrupt_handler_16();
extern void interrupt_handler_17();
extern void interrupt_handler_18();
extern void interrupt_handler_19();

extern void interrupt_handler_32(); // IRQ0 - Timer
extern void interrupt_handler_33(); // IRQ1 - Keyboard
extern void interrupt_handler_34(); // IRQ2 - PIC 2
extern void interrupt_handler_35(); // IRQ3
extern void interrupt_handler_36(); // IRQ4
extern void interrupt_handler_37(); // IRQ5
extern void interrupt_handler_38(); // IRQ6
extern void interrupt_handler_39(); // IRQ7
extern void interrupt_handler_40(); // IRQ8
extern void interrupt_handler_41(); // IRQ9
extern void interrupt_handler_42(); // IRQ10
extern void interrupt_handler_43(); // IRQ11
extern void interrupt_handler_44(); // IRQ12
extern void interrupt_handler_45(); // IRQ13
extern void interrupt_handler_46(); // IRQ14
extern void interrupt_handler_47(); // IRQ15

// Funzione per impostare un'entry nella IDT
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = (base & 0xFFFF);        // Parte bassa dell'indirizzo
    idt[num].base_high = (base >> 16) & 0xFFFF; // Parte alta dell'indirizzo
    idt[num].sel = sel;                         // Segment Selector (0x08 = Kernel Code)
    idt[num].always0 = 0;                       // Sempre 0
    idt[num].flags = flags;                     // Flag: presente, DPL, tipo
}

// Funzione per inizializzare la IDT
void idt_init() {
    // 1. Imposta il puntatore alla IDT
    idt_descriptor.limit = sizeof(struct idt_entry) * IDT_ENTRIES - 1;
    idt_descriptor.base = (uint32_t)&idt;

    // 2. Pulisce la IDT
    // TODO: Change with an implementation of memset
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt[i].base_low  = 0;
        idt[i].base_high = 0;
        idt[i].sel       = 0;
        idt[i].always0   = 0;
        idt[i].flags     = 0;
    }

    // 3. Installazione delle routine di gestione degli interrupt
    idt_set_gate(0, (uint32_t)interrupt_handler_0, 0x08, 0x8E);
    idt_set_gate(1, (uint32_t)interrupt_handler_1, 0x08, 0x8E);
    idt_set_gate(2, (uint32_t)interrupt_handler_2, 0x08, 0x8E);
    idt_set_gate(3, (uint32_t)interrupt_handler_3, 0x08, 0x8E);
    idt_set_gate(4, (uint32_t)interrupt_handler_4, 0x08, 0x8E);
    idt_set_gate(5, (uint32_t)interrupt_handler_5, 0x08, 0x8E);
    idt_set_gate(6, (uint32_t)interrupt_handler_6, 0x08, 0x8E);
    idt_set_gate(7, (uint32_t)interrupt_handler_7, 0x08, 0x8E);
    idt_set_gate(8, (uint32_t)interrupt_handler_8, 0x08, 0x8E);
    idt_set_gate(9, (uint32_t)interrupt_handler_9, 0x08, 0x8E);
    idt_set_gate(10, (uint32_t)interrupt_handler_10, 0x08, 0x8E);
    idt_set_gate(11, (uint32_t)interrupt_handler_11, 0x08, 0x8E);
    idt_set_gate(12, (uint32_t)interrupt_handler_12, 0x08, 0x8E);
    idt_set_gate(13, (uint32_t)interrupt_handler_13, 0x08, 0x8E);
    idt_set_gate(14, (uint32_t)interrupt_handler_14, 0x08, 0x8E);
    idt_set_gate(15, (uint32_t)interrupt_handler_15, 0x08, 0x8E);
    idt_set_gate(16, (uint32_t)interrupt_handler_16, 0x08, 0x8E);
    idt_set_gate(17, (uint32_t)interrupt_handler_17, 0x08, 0x8E);
    idt_set_gate(18, (uint32_t)interrupt_handler_18, 0x08, 0x8E);
    idt_set_gate(19, (uint32_t)interrupt_handler_19, 0x08, 0x8E);

    idt_set_gate(32, (uint32_t)interrupt_handler_32, 0x08, 0x8E); // Timer
    idt_set_gate(33, (uint32_t)interrupt_handler_33, 0x08, 0x8E); // Keyboard
    idt_set_gate(34, (uint32_t)interrupt_handler_34, 0x08, 0x8E);
    idt_set_gate(35, (uint32_t)interrupt_handler_35, 0x08, 0x8E);
    idt_set_gate(36, (uint32_t)interrupt_handler_36, 0x08, 0x8E);
    idt_set_gate(37, (uint32_t)interrupt_handler_37, 0x08, 0x8E);
    idt_set_gate(38, (uint32_t)interrupt_handler_38, 0x08, 0x8E);
    idt_set_gate(39, (uint32_t)interrupt_handler_39, 0x08, 0x8E);
    idt_set_gate(40, (uint32_t)interrupt_handler_40, 0x08, 0x8E);
    idt_set_gate(41, (uint32_t)interrupt_handler_41, 0x08, 0x8E);
    idt_set_gate(42, (uint32_t)interrupt_handler_42, 0x08, 0x8E);
    idt_set_gate(43, (uint32_t)interrupt_handler_43, 0x08, 0x8E);
    idt_set_gate(44, (uint32_t)interrupt_handler_44, 0x08, 0x8E);
    idt_set_gate(45, (uint32_t)interrupt_handler_45, 0x08, 0x8E);
    idt_set_gate(46, (uint32_t)interrupt_handler_46, 0x08, 0x8E);
    idt_set_gate(47, (uint32_t)interrupt_handler_47, 0x08, 0x8E);

    // 4. Carica la IDT
    idt_load((uint32_t)&idt_descriptor);

    // 5. Rimappa il PIC per evitare conflitti
    pic_remap();
}

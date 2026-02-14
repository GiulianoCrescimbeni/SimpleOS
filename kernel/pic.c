#include <stdint.h>
#include <kernel/pic.h>
#include <drivers/io.h>

#define PIC1_COMMAND  0x20   // PIC Master command port
#define PIC1_DATA     0x21   // PIC Master data port
#define PIC2_COMMAND  0xA0   // PIC Slave command  port
#define PIC2_DATA     0xA1   // PIC Slave data port
#define PIC_EOI       0x20   // End Of Interrupt (EOI)

static inline void io_wait(void) {
    outb(0x80, 0);
}

void pic_remap() {
    // 1. PIC initialization
    outb(PIC1_COMMAND, 0x11);
    io_wait();
    outb(PIC2_COMMAND, 0x11);
    io_wait();

    // 2. IRQ Remapping
    outb(PIC1_DATA, 0x20);     // IRQ offset PIC Master (0x20 - 0x27)
    io_wait();
    outb(PIC2_DATA, 0x28);     // IRQ offset PIC Slave (0x28 - 0x2F)
    io_wait();

    // 3. Master slave linking
    outb(PIC1_DATA, 0x04);     // PIC Master links Slave on IRQ2
    io_wait();
    outb(PIC2_DATA, 0x02);     // PIC Slave links Master on IRQ2
    io_wait();

    // 4. ICW4
    outb(PIC1_DATA, 0x01);
    io_wait();
    outb(PIC2_DATA, 0x01);
    io_wait(); 

    // 5. IRQ Masks (enables only Timer and keyboard)
    outb(PIC1_DATA, 0xFC);     // 11111100: enables only IRQ0 (timer) and IRQ1 (keyboard)
    outb(PIC2_DATA, 0xFF);     // 11111111: disables every other IRQ of the PIC Slave
}

void pic_acknowledge(uint32_t int_num) {
    if (int_num >= 40) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}

#include <drivers/timer.h>
#include <drivers/io.h>
#include <kernel/kprint.h>
#include <kernel/idt_load.h>
#include <kernel/process.h>
#include <kernel/pic.h>
#include <debug.h>

volatile uint32_t tick = 0;

void timer_handler(registers_t *regs) {
    (void)regs;
    tick++;
    pic_acknowledge(32);
    schedule();
    //if (tick % 100 == 0) DEBUG_LOG("Tick: %d\n", tick);
}

void init_timer(uint32_t freq) {
    uint32_t divisor = 1193180 / freq;

    outb(0x43, 0x36);

    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)((divisor >> 8) & 0xFF);

    outb(0x40, l);
    outb(0x40, h);
    
    DEBUG_LOG("[PIT] Timer Initialized at %d Hz\n", freq);
}

uint32_t get_tick_count() {
    return tick;
}

void sleep(uint32_t ticks) {
    enable_interrupt();
    uint32_t start_ticks = tick;
    DEBUG_LOG("[SLEEP] Start: %d, Target: %d\n", start_ticks, start_ticks + ticks);
    while (tick < start_ticks + ticks) {
        asm volatile("hlt");
    }
    DEBUG_LOG("[SLEEP] Woke up at: %d\n", tick);
}
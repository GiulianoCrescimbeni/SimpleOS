#include <stdint.h>
#include <kernel/interrupt.h>
#include <kernel/kprint.h>
#include <kernel/pic.h>
#include <drivers/io.h>
#include <drivers/framebuffer.h>
#include <drivers/keyboard.h>

char ir0[] = " - Division by 0 interrupt - ";
char irunknown[] = " - Unknown interrupt - ";

/** interrupt_handler:
    Function that manage an interrupt
    @param cpu_state    Structure with the values of all the registers before the interrupt
    @param stack_state  Contains the state of the stack
    @param interrupt    The index of the interrupt to be processed
 */
void interrupt_handler(cpu_state_t state, idt_info_t info, stack_state_t exec) {
    (void)state;
    (void)exec; 
    
    switch (info.idt_index) {

    case 0: // Division by zero
        kprint(ir0, 1);
        break;

    case 0x21: // Keyboard interrupt
        {
            keyboard_interrupt_handler();
        }
        break;

    case 0x20: // Timer interrupt (IRQ0)
        break;

    default:
        kprint(irunknown, 1);
        break;
    }
    pic_acknowledge();
}
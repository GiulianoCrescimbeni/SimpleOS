#include "stdint.h"
#include "interrupt.h"
#include "kprint.h"
#include "pic.h"
#include "../drivers/io.h"
#include "../drivers/framebuffer.h"
#include "../drivers/keyboard.h"

char ir0[] = " - Division by 0 interrupt - ";
char irunknown[] = " - Unknown interrupt - ";

char separator1[] = "(";
char separator2[] = ")";

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
        kprint(ir0, 29, 1);
        break;

    case 0x21: // Keyboard interrupt
        {
            keyboard_interrupt_handler();
        }
        break;

    case 0x20: // Timer interrupt (IRQ0)
        break;

    default:
        kprint(irunknown, 23, 1);
        break;
    }
    pic_acknowledge();
}
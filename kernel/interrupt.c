#include <stdint.h>
#include <kernel/interrupt.h>
#include <kernel/kprint.h>
#include <kernel/pic.h>
#include <kernel/utils.h>
#include <drivers/io.h>
#include <drivers/framebuffer.h>
#include <drivers/keyboard.h>

// System call identifiers
#define SYS_WRITE 1
#define SYS_READ  2
#define SYS_EXIT  0

char ir0[] = " - Division by 0 interrupt - \n";
char ir13[] = " - General Protection Fault - \n";
char ir14[] = " - Page Fault - \n";
char irunknown[] = " - Unknown interrupt - \n";

/** interrupt_handler:
    Function that manage an interrupt
    @param cpu_state    Structure with the values of all the registers before the interrupt
    @param stack_state  Contains the state of the stack
    @param interrupt    The index of the interrupt to be processed
 */
void interrupt_handler(registers_t *regs) {
    
    switch (regs->int_no) {

    case 0: // Division by zero
        kprint(ir0, 1);
        break;

    case 13: // General Protection Fault
        kprint(ir13, 0); 
        while(1); 
        break;

    case 14: // Page Fault
        kprint(ir14, 0);
        break;

    case 0x80: //System call
    {
        uint32_t syscall_num = regs->eax;

        switch (syscall_num) {
            case SYS_WRITE:

                printf((char*)regs->ebx);
                break;
            
            case SYS_READ:
                regs->eax = (uint32_t)keyboard_get_char();
                break;

            case SYS_EXIT:
                kprint("SYS_EXIT.\n", 0x0F);
                while(1); // Block the process
                break;

            default:
                kprint("Syscall Error\n", 4);
                break;
        }
    }
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
    
    if (regs->int_no >= 32 && regs->int_no <= 47) {
        pic_acknowledge(regs->int_no);
    }
}
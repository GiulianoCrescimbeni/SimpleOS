#include <stdint.h>
#include <kernel/interrupt.h>
#include <kernel/kprint.h>
#include <kernel/pic.h>
#include <kernel/utils.h>
#include <kernel/fs.h>
#include <kernel/tar.h>
#include <drivers/io.h>
#include <drivers/framebuffer.h>
#include <drivers/keyboard.h>

// System call identifiers
#define SYS_EXIT            0
#define SYS_WRITE           1
#define SYS_READ            2
#define SYS_LIST_FILES      3
#define SYS_READ_FILE       4
#define SYS_CREATE_FILE     5
#define SYS_WRITE_FILE      6
#define SYS_DELETE_FILE     7

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
            
            case SYS_LIST_FILES:
                {
                    fs_node_t *node = get_tar_file_by_index(regs->ebx);
                    char *usr_buf = (char*)regs->ecx;
                    
                    if (node != 0) {
                        int i = 0;
                        while(node->name[i] != 0) {
                            usr_buf[i] = node->name[i];
                            i++;
                        }
                        usr_buf[i] = 0;
                        regs->eax = node->flags;
                    } else {
                        regs->eax = -1;
                    }
                }
                break;

            case SYS_READ_FILE:
                {
                    char *filename = (char*)regs->ebx;
                    char *content_buf = (char*)regs->ecx;
                    
                    fs_node_t *node = get_tar_file_by_name(filename);
                    
                    if (node != 0) {
                        uint32_t read = read_fs(node, 0, 2048, (uint8_t*)content_buf);
                        content_buf[read] = 0;
                        regs->eax = read;
                    } else {
                        regs->eax = -1;
                    }
                }
                break;

            case SYS_CREATE_FILE:
                {
                    char *filename = (char*)regs->ebx;
                    regs->eax = tar_create_file(filename);
                }
                break;

            case SYS_WRITE_FILE:
                {
                    char *filename = (char*)regs->ebx;
                    char *content = (char*)regs->ecx;
                    int len = regs->edx;

                    fs_node_t *node = get_tar_file_by_name(filename);
                    if (node) {
                        regs->eax = write_fs(node, 0, len, (uint8_t*)content);
                    } else {
                        regs->eax = -1;
                    }
                }
                break;

            case SYS_DELETE_FILE:
                {
                    char *filename = (char*)regs->ebx;
                    regs->eax = tar_delete_file(filename);
                }
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
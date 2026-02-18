#include <stdint.h>
#include <kernel/interrupt.h>
#include <kernel/kprint.h>
#include <kernel/pic.h>
#include <kernel/utils.h>
#include <kernel/fs.h>
#include <kernel/tar.h>
#include <kernel/process.h>
#include <drivers/io.h>
#include <drivers/framebuffer.h>
#include <drivers/keyboard.h>
#include <drivers/timer.h>

// System call identifiers
#define SYS_EXIT                0
#define SYS_WRITE               1
#define SYS_READ                2
#define SYS_LIST_FILES          3
#define SYS_READ_FILE           4
#define SYS_CREATE_FILE         5
#define SYS_WRITE_FILE          6
#define SYS_DELETE_FILE         7
#define SYS_GET_TICKS           8
#define SYS_SLEEP               9
#define SYS_KILL                10
#define SYS_GET_PROCESS_LIST    11
#define SYS_CLEAR_SCREEN        12

void handle_syscall(registers_t *regs) {
    switch (regs->eax) {
        case SYS_WRITE:
            printf((char*)regs->ebx);
            break;
        case SYS_READ:
            regs->eax = (uint32_t)keyboard_get_char();
            break;
        case SYS_EXIT:
            exit_process();
            break;
        case SYS_LIST_FILES: {
            fs_node_t *node = get_tar_file_by_index(regs->ebx);
            char *usr_buf = (char*)regs->ecx;
            if (node) {
                int i = 0;
                while(node->name[i]) { usr_buf[i] = node->name[i]; i++; }
                usr_buf[i] = 0;
                regs->eax = node->flags;
            } else {
                regs->eax = -1;
            }
            break;
        }
        case SYS_READ_FILE: {
            fs_node_t *node = get_tar_file_by_name((char*)regs->ebx);
            if (node) {
                uint32_t read = read_fs(node, 0, 2048, (uint8_t*)regs->ecx);
                ((char*)regs->ecx)[read] = 0;
                regs->eax = read;
            } else regs->eax = -1;
            break;
        }
        case SYS_CREATE_FILE:
            regs->eax = tar_create_file((char*)regs->ebx);
            break;
        case SYS_WRITE_FILE: {
            fs_node_t *node = get_tar_file_by_name((char*)regs->ebx);
            if (node) regs->eax = write_fs(node, 0, regs->edx, (uint8_t*)regs->ecx);
            else regs->eax = -1;
            break;
        }
        case SYS_DELETE_FILE:
            regs->eax = tar_delete_file((char*)regs->ebx);
            break;
        case SYS_GET_TICKS:
            regs->eax = get_tick_count();
            break;
        case SYS_SLEEP:
            sleep(regs->ebx);
            regs->eax = 0;
            break;
        case SYS_KILL:
            regs->eax = kill_process(regs->ebx);
            break;
        case SYS_GET_PROCESS_LIST:
            regs->eax = get_process_list((process_info_t*)regs->ebx, regs->ecx);
            break;
        case SYS_CLEAR_SCREEN:
            clear_screen();
            break;
        default:
            kprint("Syscall Error: Unknown ID\n", 4);
    }
}

void interrupt_handler(registers_t *regs) {
    
    if (regs->int_no < 32) {
        switch (regs->int_no) {
            case 0: kprint(" - Division by 0 -\n", 4); break;
            case 13: kprint(" - GPF -\n", 4); while(1); break;
            case 14: kprint(" - Page Fault -\n", 4); break;
            default: kprint(" - CPU Exception -\n", 4); break;
        }
        return;
    }

    if (regs->int_no == 0x80) {
        handle_syscall(regs);
        return;
    }

    if (regs->int_no == 32) {
        timer_handler(regs);
        return; 
    }

    if (regs->int_no == 33) {
        keyboard_interrupt_handler();
    }

    if (regs->int_no >= 32 && regs->int_no <= 47) {
        pic_acknowledge(regs->int_no);
    }
}
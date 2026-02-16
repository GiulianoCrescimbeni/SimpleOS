# SimpleOS

## Project Overview
SimpleOS is a basic operating system designed as a learning project to understand low-level programming concepts such as bootloaders, kernel development, and system architecture.

---

## Features Implemented

### 1. Bootloader: GRUB
- **GRUB** (Grand Unified Bootloader) has been successfully implemented as the bootloader for SimpleOS.
- GRUB loads the kernel from the ISO file and transfers control to the kernel's entry point.
- The `menu.lst` configuration file provides instructions for GRUB to locate and load the kernel (`kernel.elf`).

### 2. Kernel Entry Function
- A basic **kernel entry function** has been implemented.
- The kernel entry function is defined in `loader.s` (written in assembly) and hands over control to `kmain`, a C function.

### **3. Memory Segmentation**
- A **Global Descriptor Table (GDT)** has been implemented to enable memory segmentation in protected mode.
- The GDT defines the following segments:

  | **Segment Name**        | **Selector** | **Base Address** | **Limit**       | **Type**           | **Privilege Level** |
  |-------------------------|--------------|------------------|-----------------|--------------------|---------------------|
  | **Null Segment**        | `0x00`       | `0x00000000`     | `0x00000000`    | Reserved           | N/A                 |
  | **Kernel Code Segment** | `0x08`       | `0x00000000`     | `0xFFFFFFFF`    | Read/Execute       | `0` (Kernel Mode)   |
  | **Kernel Data Segment** | `0x10`       | `0x00000000`     | `0xFFFFFFFF`    | Read/Write         | `0` (Kernel Mode)   |
  
- This segmentation setup ensures the separation of kernel code and data, providing a foundation for memory protection and management.

### **4. Interrupt Handling**
- A **Programmable Interrupt Controller (PIC)** is used to manage hardware interrupts, remapping them to avoid conflicts with CPU exceptions.
- A **Interrupt Descriptor Table (IDT)** has been implemented to define and manage interrupt vectors.
- Support for both hardware interrupts (e.g., keyboard, timer) and CPU exceptions has been added:
  - **Keyboard Interrupt (IRQ1)**: Converts scancodes into ASCII characters and prints them to the screen.
  - **Timer Interrupt (IRQ0)**: Currently acknowledges the interrupt but serves as a foundation for future scheduling and multitasking functionality.
- The interrupt handling system preserves CPU state, processes the interrupt, and restores state before returning.

### 5. Drivers for Framebuffer, I/O Ports, and Serial Port (COM1)
- **Framebuffer driver**:
  - Enables interaction with the video memory to write text directly to the screen.
  - Provides functions for clearing the screen and writing strings or characters at specific positions.
  - Implemented in `framebuffer.c` and `framebuffer.h` under the `drivers` directory.
- **I/O Ports driver**:
  - Implements low-level input and output operations for hardware communication via I/O ports.
  - Provides helper functions like `outb` and `inb` for sending and receiving data to/from specific ports.
  - Implemented in `io.s` and `io.h` under the `drivers` directory.
- **Serial Port driver (COM1)**:
  - Enables communication through the COM1 serial port.
  - Provides functions to initialize the serial port, send data, and receive data.
  - Useful for debugging and inter-process communication.
  - Implemented in `serial.c` and `serial.h` under the `drivers` directory.
  - Results of a serial_write() are printed in the com1_log.txt file
- **Keyboard driver**:
  - Simple keyboard driver that converts scancode into ascii
 
### **6. Basic User Mode**
- **Ring 3 Transition**: Successfully implemented the transition from Kernel Mode (Ring 0) to User Mode (Ring 3).
- **GDT Updates**: Extended the Global Descriptor Table to include User Code and User Data segments with specific DPL (Descriptor Privilege Level) set to 3.
- **Task State Segment (TSS)**:
  - Implemented and loaded the TSS to manage stack switching during interrupts.
  - Ensures the CPU can safely switch back to the Kernel Stack (`ESP0`) when an interrupt occurs while in User Mode.
- **Paging for User Mode**: Updated page tables to allow User Mode access (`User` bit set) to necessary memory regions (code and stack).

---

## Current Code Structure
- **loader.s**: Implements the Multiboot header and transfers control to the kernel entry function.
- **kernel/kmain.c**: Contains the kernel's entry point.
- **link.ld**: Linker script to organize sections of the kernel binary and ensure compatibility with the Multiboot protocol.
- **drivers**: Contains all the drivers implemented for the kernel
- **com1_log.txt**: Contains the results of the serial port communication on com1 (Used for debug information), you can comunicate on that port with serial_write()

---

## How to Build and Run

### Prerequisites
- **NASM**: Assembler for `loader.s`.
- **GCC**: Compiler for `kmain.c`.
- **genisoimage**: For generating the bootable ISO.
- **Bochs**: Emulator to test the OS.

### Build Steps
1. Compile the kernel source files:
```bash
make
```
2. Generate the bootable ISO:
```bash
make run
```
3. Run the OS in the Bochs emulator by choosing the configuration file in the repo (**bochsrc.txt**)

### Clean the build
To remove all generated files:
```bash
make clean
```

### Future plans
- Expand kernel functionality
- Implement a simple memory management system.
- Enrich shell for user interaction.

### Contributions
This project is open to contributions. Feel free to fork the repository and submit a pull request with your changes or improvements.

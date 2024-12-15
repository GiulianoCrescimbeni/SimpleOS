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
- The kernel writes a simple character to the screen as a basic demonstration of its functionality.

---

## Current Code Structure
- **loader.s**: Implements the Multiboot header and transfers control to the kernel entry function.
- **kmain.c**: Contains the kernel's entry point, which interacts with the hardware (e.g., video memory).
- **link.ld**: Linker script to organize sections of the kernel binary and ensure compatibility with the Multiboot protocol.

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
- Expand kernel functionality (e.g., interrupt handling, basic drivers)
- Implement a simple memory management system.
- Develop a basic shell for user interaction.

### Contributions
This project is open to contributions. Feel free to fork the repository and submit a pull request with your changes or improvements.

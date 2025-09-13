DRIVERS_DIR = drivers
KERNEL_DIR = kernel
OBJECTS = loader.o 					\
	$(KERNEL_DIR)/kmain.o 			\
	$(KERNEL_DIR)/gdt.o 			\
	$(KERNEL_DIR)/gdt_load.o 		\
	$(KERNEL_DIR)/idt.o 			\
	$(KERNEL_DIR)/idt_load.o 		\
	$(KERNEL_DIR)/interrupt.o 		\
	$(KERNEL_DIR)/irqs.o 			\
	$(KERNEL_DIR)/pic.o 			\
	$(KERNEL_DIR)/kprint.o 			\
	$(KERNEL_DIR)/paging.o 			\
	$(DRIVERS_DIR)/io.o 			\
	$(DRIVERS_DIR)/framebuffer.o 	\
	$(DRIVERS_DIR)/serial.o 		\
	$(DRIVERS_DIR)/keyboard.o 

CC = gcc
LD = ld
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c -Iinclude
LDFLAGS = -T link.ld -melf_i386
AS = nasm
ASFLAGS = -f elf

all: kernel.elf

kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o kernel.elf

os.iso: kernel.elf
	cp kernel.elf iso/boot/kernel.elf
	genisoimage -R                              \
	            -b boot/grub/stage2_eltorito    \
	            -no-emul-boot                   \
	            -boot-load-size 4               \
	            -A os                           \
	            -input-charset utf8             \
	            -quiet                          \
	            -boot-info-table                \
	            -o os.iso                       \
	            iso

run: os.iso
	@echo "Kernel compilation completed!"

$(KERNEL_DIR)/%.o: $(KERNEL_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL_DIR)/%.o: $(KERNEL_DIR)/%.s
	$(AS) $(ASFLAGS) $< -o $@

$(DRIVERS_DIR)/%.o: $(DRIVERS_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

$(DRIVERS_DIR)/%.o: $(DRIVERS_DIR)/%.s
	$(AS) $(ASFLAGS) $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf $(KERNEL_DIR)/*.o $(DRIVERS_DIR)/*.o *.o kernel.elf os.iso
	rm bochslog.txt
	rm com1_log.txt
	@echo "Cleared working space"

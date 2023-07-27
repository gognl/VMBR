# C COMPILER

C_COMPILER = gcc
C_FLAGS = -c -nostdlib -fno-builtin -nostdinc -fno-stack-protector -Wall -I./include

# LINKER

LINKER = ld
LINKER_FLAGS = -nostdlib --oformat elf64-x86-64 -n

# ASM ASSEMBLER

ASM_ASSEMBLER = nasm
ASM_FLAGS = -f elf64 -I./include -w-all

# -------------------------------------------------------- #


C_FILES = $(shell find src/ -name '*.c')
ASM_FILES = $(shell find src/ -name '*.asm')
OBJ_FILES = $(addprefix build/, $(notdir $(C_FILES:.c=.o))) $(addprefix build/, $(notdir $(ASM_FILES:.asm=.o)))

LINKER_SCRIPT := src/linker.ld
GRUB_CFG := src/boot/grub.cfg

default: run

.PHONY: default build run clean

build: build/vmbr.iso

run: build
	qemu-system-x86_64 -cdrom build/vmbr.iso -serial stdio

clean:
	rm -rf build

build/%.o: src/*/*%.c
	mkdir -p build
	$(C_COMPILER) $(C_FLAGS) -o $@ $<

build/%.o: src/*/*%.asm
	mkdir -p build
	$(ASM_ASSEMBLER) $(ASM_FLAGS) -o $@ $<

build/kernel.bin: $(OBJ_FILES) $(LINKER_SCRIPT)
	mkdir -p build
	$(LINKER) $(LINKER_FLAGS) -T $(LINKER_SCRIPT) $(OBJ_FILES) -o $@

build/vmbr.iso: build/kernel.bin $(GRUB_CFG)
	mkdir -p build/isofiles/boot/grub
	cp $(GRUB_CFG) build/isofiles/boot/grub
	cp build/kernel.bin build/isofiles/boot
	grub-mkrescue -o $@ build/isofiles

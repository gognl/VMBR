# C COMPILER

C_COMPILER = gcc
C_FLAGS = -c -nostdlib -fno-builtin -nostdinc -fno-stack-protector -I./include -g

# LINKER

LINKER = ld
LINKER_FLAGS = -nostdlib --oformat elf64-x86-64 -n

# ASM ASSEMBLER

ASM_ASSEMBLER = nasm
ASM_FLAGS = -f elf64 -w-all -g -F dwarf

# -------------------------------------------------------- #

ASM_MAIN_FILE := src/boot/boot.asm
ASM_MAINO_FILE := build/boot.o

C_FILES = $(shell find src/ -name '*.c')
ASM_FILES = $(shell find src/ -name '*.asm')
OBJ_FILES =  $(ASM_MAINO_FILE) $(addprefix build/, $(notdir $(C_FILES:.c=.o)))

LINKER_SCRIPT := src/linker.ld
GRUB_CFG := src/boot/grub.cfg

default: run

.PHONY: default build run clean scr gdb gef win

build: build/vmbr.iso

scr: build
	qemu-system-x86_64 -cdrom build/vmbr.iso -nographic --enable-kvm -serial mon:stdio -smp cores=1 -cpu host 

run: build
	qemu-system-x86_64 -boot d -cdrom build/vmbr.iso -drive file=win10.vhd,format=raw -serial stdio --enable-kvm -smp 4 -cpu host -m 1024,maxmem=8G

gdb: build
	qemu-system-x86_64 -cdrom build/vmbr.iso -s -S --enable-kvm -serial stdio -smp 4 -cpu host & gdb

gef: build
	qemu-system-x86_64 -cdrom build/vmbr.iso -s -S --enable-kvm -serial stdio -smp 4 -cpu host & gef

win: win10.vhd
	qemu-system-x86_64 -drive file=win10.vhd,format=raw --enable-kvm -smp 1 -cpu host -m 1024,maxmem=8G -vga std

clean:
	rm -rf build

build/%.o: src/*/*%.c
	mkdir -p build
	$(C_COMPILER) $(C_FLAGS) -o $@ $<

$(ASM_MAINO_FILE): $(ASM_FILES)
	mkdir -p build
	$(ASM_ASSEMBLER) $(ASM_FLAGS) $(ASM_MAIN_FILE) -o $@ 

build/vmbr.so: $(OBJ_FILES) $(LINKER_SCRIPT)
	mkdir -p build
	$(LINKER) $(LINKER_FLAGS) -T $(LINKER_SCRIPT) $(OBJ_FILES) -o $@

build/vmbr.iso: build/vmbr.so $(GRUB_CFG)
	mkdir -p build/isofiles/boot/grub
	cp $(GRUB_CFG) build/isofiles/boot/grub
	cp build/vmbr.so build/isofiles/boot
	grub-mkrescue -o $@ build/isofiles

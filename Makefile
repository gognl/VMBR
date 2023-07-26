default: run

.PHONY: default build run clean

build: build/vmbr.iso

run: build
	qemu-system-x86_64 -cdrom build/vmbr.iso

clean:
	rm -rf build

build/mb2_header.o: src/mb2_header.asm
	mkdir -p build
	nasm -f elf64 src/mb2_header.asm -o build/mb2_header.o

build/boot.o: src/boot.asm
	mkdir -p build
	nasm -f elf64 src/boot.asm -o build/boot.o

build/main.o: src/main.c
	mkdir -p build
	gcc -c -Wall -I./include -o build/main.o src/main.c

build/kernel.bin: build/mb2_header.o build/boot.o build/main.o src/linker.ld
	ld -n -o build/kernel.bin -T src/linker.ld build/mb2_header.o build/boot.o build/main.o

build/vmbr.iso: build/kernel.bin src/grub.cfg
	mkdir -p build/isofiles/boot/grub
	cp src/grub.cfg build/isofiles/boot/grub
	cp build/kernel.bin build/isofiles/boot
	grub-mkrescue -o build/vmbr.iso build/isofiles

default: run

.PHONY: default build run clean

build: build/vmbr.iso

run: build
	qemu-system-x86_64 -cdrom build/vmbr.iso

clean:
	rm -rf build

build/mb2_header.o: src/boot/mb2_header.asm
	mkdir -p build
	nasm -f elf64 src/boot/mb2_header.asm -o build/mb2_header.o

build/boot.o: src/boot/boot.asm
	mkdir -p build
	nasm -f elf64 src/boot/boot.asm -o build/boot.o

build/system.o: src/util/system.c
	mkdir -p build
	gcc -c -fno-builtin -nostdinc -Wall -I ./include -o build/system.o src/util/system.c

build/main.o: src/boot/main.c
	mkdir -p build
	gcc -c -fno-builtin -nostdinc -Wall -I ./include -o build/main.o src/boot/main.c

build/kernel.bin: build/mb2_header.o build/boot.o build/main.o build/system.o src/linker.ld
	ld -n -o build/kernel.bin -T src/linker.ld build/mb2_header.o build/boot.o build/main.o build/system.o

build/vmbr.iso: build/kernel.bin src/boot/grub.cfg
	mkdir -p build/isofiles/boot/grub
	cp src/boot/grub.cfg build/isofiles/boot/grub
	cp build/kernel.bin build/isofiles/boot
	grub-mkrescue -o build/vmbr.iso build/isofiles

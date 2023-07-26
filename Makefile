default: run

.PHONY: default build run clean

build: build/vmbr.iso

run: build
	qemu-system-x86_64 -cdrom build/vmbr.iso

clean:
	rm -rf build

build/mb2_header.o: mb2_header.asm
	mkdir -p build
	nasm -f elf64 mb2_header.asm -o build/mb2_header.o

build/boot.o: boot.asm
	mkdir -p build
	nasm -f elf64 boot.asm -o build/boot.o

build/kernel.bin: build/mb2_header.o build/boot.o linker.ld
	ld -n -o build/kernel.bin -T linker.ld build/mb2_header.o build/boot.o

build/vmbr.iso: build/kernel.bin grub.cfg
	mkdir -p build/isofiles/boot/grub
	cp grub.cfg build/isofiles/boot/grub
	cp build/kernel.bin build/isofiles/boot
	grub-mkrescue -o build/vmbr.iso build/isofiles

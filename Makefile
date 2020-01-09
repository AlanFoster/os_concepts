BOOTLOADER=bootloader.bin
DISK_IMG=disk.img

build: bootloader bootdisk

bootloader: boot.asm
	nasm -f bin boot.asm -O0 -o $(BOOTLOADER)

bootdisk: $(BOOTLOADER)
	dd if=/dev/zero of=$(DISK_IMG) bs=512 count=2880
	dd conv=notrunc if=$(BOOTLOADER) of=$(DISK_IMG) bs=512 count=1 seek=0

run: build
	qemu-system-x86_64 -fda $(DISK_IMG)

debug: build
	gdb -x debug.gdb

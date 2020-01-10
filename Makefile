BOOTLOADER=bootloader.bin
DISK_IMG=disk.img
DISK_DRIVE=f0

DISK_SECTOR_COUNT=3

build: bootloader bootdisk

bootloader: boot.asm
	nasm -f bin boot.asm -O0 -o $(BOOTLOADER)

bootdisk: $(BOOTLOADER)
	dd if=/dev/zero of=$(DISK_IMG) bs=512 count=2880
	dd conv=notrunc if=$(BOOTLOADER) of=$(DISK_IMG) bs=512 count=3 seek=0

run: build
	qemu-system-x86_64 -blockdev driver=file,node-name=$(DISK_DRIVE),filename=$(DISK_IMG) -device floppy,drive=$(DISK_DRIVE)


debug: build
	gdb -x debug.gdb

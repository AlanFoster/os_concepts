BOOTLOADER=bootloader.bin
DISK_IMG=disk.img
KERNEL=kernel.bin
KERNEL_IMG=kernel.img
KERNEL_OFFSET=0x1000
DISK_DRIVE=f0
DISK_SECTOR_COUNT=3

build: bootloader kernel link write_disk

bootloader: boot.asm
	nasm -f bin boot.asm -O0 -o $(BOOTLOADER)

write_disk: $(BOOTLOADER)
	dd if=/dev/zero of=$(DISK_IMG) bs=512 count=2880
	dd conv=notrunc if=$(KERNEL_IMG) of=$(DISK_IMG) bs=512 count=3 seek=0

run: build
	qemu-system-x86_64 -blockdev driver=file,node-name=$(DISK_DRIVE),filename=$(DISK_IMG) -device floppy,drive=$(DISK_DRIVE)

debug: new_setup
	gdb -x debug.gdb

kernel:
	nasm -f elf kernel_entry.asm -o kernel_entry.o

	~/opt/cross/bin/i686-elf-gcc -ffreestanding -c kernel.c -o kernel.o
	~/opt/cross/bin/i686-elf-ld -o $(KERNEL) -Ttext $(KERNEL_OFFSET) kernel_entry.o kernel.o --oformat binary

link: $(BOOTLOADER) $(KERNEL)
	cat $(BOOTLOADER) kernel.bin > $(KERNEL_IMG)

clean:
	rm *.bin *.img *.o

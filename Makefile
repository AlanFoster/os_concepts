# $@ = target file
# $< = first dependency
# $^ = all dependencies

CC = ~/opt/cross/bin/i686-elf-gcc
LD = ~/opt/cross/bin/i686-elf-ld

build: disk.img

kernel.bin: kernel_entry.o kernel.o
	$(LD) -o $@ -Ttext 0x1000 $^ --oformat binary

os.img: bootloader.bin kernel.bin
	cat $^ > $@

disk.img: os.img
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd conv=notrunc if=$< of=$@ bs=512 count=3 seek=0

%.o: %.c
	${CC} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm -f elf $< -o $@

%.bin: %.asm
	nasm -f bin $< -O0 -o $@

run: disk.img
	qemu-system-x86_64 -blockdev driver=file,node-name=f0,filename=$< -device floppy,drive=f0

debug: disk.img
	gdb -x debug.gdb

clean:
	rm -rf *.bin *.img *.o

# $@ = target file
# $< = first dependency
# $^ = all dependencies

C_SOURCES = $(wildcard kernel/*.c drivers/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h)
OBJ = ${C_SOURCES:.c=.o}
# Debugging symbols enabled
CFLAGS = -g

CC = ~/opt/cross/bin/i686-elf-gcc
LD = ~/opt/cross/bin/i686-elf-ld
GDB = ~/opt/cross/bin/i686-elf-gdb

build: disk.img

kernel.bin: kernel/kernel_entry.o ${OBJ}
	$(LD) -o $@ -Ttext 0x1000 $^ --oformat binary

kernel.elf: kernel/kernel_entry.o ${OBJ}
	$(LD) -o $@ -Ttext 0x1000 $^

os.img: boot/bootloader.bin kernel.bin
	cat $^ > $@

disk.img: os.img
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd conv=notrunc if=$< of=$@ seek=0

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm -f elf $< -o $@

%.bin: %.asm
	nasm -f bin $< -O0 -o $@

run: disk.img
	qemu-system-x86_64 -blockdev driver=file,node-name=f0,filename=$< -device floppy,drive=f0

debug: disk.img kernel.elf
	${GDB} -x debug.gdb

clean:
	rm -rf *.bin *.img *.o *.elf
	rm -rf kernel/*.o drivers/*.o boot/*.bin boot/*.o

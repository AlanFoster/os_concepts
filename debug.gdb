target remote | qemu-system-i386 -S -gdb stdio -fda disk.img

set disassembly-flavor intel
dashboard -layout asm, reg
symbol-file kernel.elf

b *0x7c00
b kernel.c:main
continue

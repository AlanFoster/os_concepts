set architecture i386:x86-64:intel
target remote | qemu-system-x86_64 -S -gdb stdio -m 16 -fda disk.img

set disassembly-flavor intel
dashboard -layout asm, reg
symbol-file kernel.elf

b *0x7c00
b kernel.c:main
continue

target remote | qemu-system-i386 -S -gdb stdio -fda disk.img

set disassembly-flavor intel
dashboard -layout asm, reg
symbol-file kernel.elf

b isr.c:8
b isr.c:23
continue

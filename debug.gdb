# For docker debugging, `-display none` is required
target remote | qemu-system-i386 -S -gdb stdio -fda disk.img

set disassembly-flavor intel
symbol-file kernel.elf

b *0x7c00
b kernel.c:main
b isr.c:handle_isr
b isr.c:handle_irq
continue

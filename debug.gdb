set architecture i386:x86-64:intel
target remote | qemu-system-x86_64 -S -gdb stdio -m 16 -fda disk.img

layout asm
layout reg

set disassembly-flavor intel

b *0x7c00
continue

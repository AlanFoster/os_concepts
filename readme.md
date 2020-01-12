# OS Concepts

Simple hello world OS concepts, starting with a simple bootloader

## Running

Ensure that qemu and nasm are installed:

```
apt-get install nasm
apt-get install qemu qemu-kvm
```

Build and start qemu:

```
make run
```

## Debugging

Ensure gdb is available and you have installed [gdb-dashboard](https://github.com/cyrus-and/gdb-dashboard):

```
make debug
```

## Notes

- The stack grows downwards in terms of memory
- The easiest way to see specific calling conventions between the `caller` and `callee` is with [godbolt](https://godbolt.org/), in conjunction with reading [Calling convention](https://en.wikipedia.org/wiki/Calling_convention)

## Resources

- [Writing a simple bootloader](https://www.alanfoster.me/posts/writing-a-bootloader/)
- [Writing a simple operating system - from scratch](https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf)
- [os-tutorial](https://github.com/cfenollosa/os-tutorial)

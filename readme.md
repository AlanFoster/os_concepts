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

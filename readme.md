# OS Concepts

A simple hello world "OS" - created purely for the purpose of learning more about OS concepts. Functionality includes a simple bootloader which switches to 32 bit protected mode, and a kernel which provides basic VGA graphics and PS/2 keyboard support.

![Running os with simple input/output](example.gif)

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

## Setting up the cross compiler

The cross compiler can either run within docker, or can be installed on your host machine:

```
./install_crosscompiler.sh binutils
./install_crosscompiler.sh gcc
```

This will enable access to both `i686-elf-gcc` and `i686-elf-ld`, which can be used to compile the OS.

## Using docker

It might be useful to run the cross compiler within docker. To run an interactive terminal within the current directory:

```
docker build -t osbuilder .
docker run --rm -it -v $(pwd):$(pwd) -w $(pwd) osbuilder
```

The OS image can then be created within docker via:

```
make
```

## Debugging

Ensure gdb is available and you have installed [gdb-dashboard](https://github.com/cyrus-and/gdb-dashboard):

```
make debug
```

## Notes

- The stack grows downwards in terms of memory
- The easiest way to see specific calling conventions between the `caller` and `callee` is with [godbolt](https://godbolt.org/), in conjunction with reading [Calling convention](https://en.wikipedia.org/wiki/Calling_convention)
- Useful commands: dumping stack `x/100w $esp+4`,`info registers`, `info frame`

## Resources

- [Writing a simple bootloader](https://www.alanfoster.me/posts/writing-a-bootloader/)
- [Writing a simple operating system - from scratch](https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf)
- [os-tutorial](https://github.com/cfenollosa/os-tutorial)
- [Brans's kernel development tutorial](http://www.osdever.net/bkerndev/Docs/intro.htm)
- [JamesM's kernel development tools](http://www.jamesmolloy.co.uk/tutorial_html/)
- [Intel® 64 and IA-32 Architectures Software Developer’s Manual](https://www.intel.co.uk/content/www/uk/en/architecture-and-technology/64-ia-32-architectures-software-developer-system-programming-manual-325384.html)
- [print memory address and value of c arguments](https://stackoverflow.com/questions/31972345/how-to-print-the-memory-address-and-the-value-of-the-arguments-of-a-c-c-functi)

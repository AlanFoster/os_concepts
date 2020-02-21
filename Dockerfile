FROM ubuntu:18.04

RUN apt update && apt install -y \
    build-essential \
    curl \
    libgmp3-dev \
    libmpc-dev \
    libmpfr-dev \
    nasm \
    texinfo \
    qemu-system-i386 \
    python3 \
    python3-dev \
    python3-distutils

COPY ./install_crosscompiler.sh ./
RUN ./install_crosscompiler.sh binutils
RUN ./install_crosscompiler.sh gcc
RUN ./install_crosscompiler.sh gdb

# Add the cross compiler to the path, to ensure 'i386-elf-gcc' is now available
ENV PATH="~/opt/cross/bin:${PATH}"

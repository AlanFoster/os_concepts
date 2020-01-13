FROM ubuntu:18.04

COPY ./install_crosscompiler.sh ./

RUN apt update
RUN apt install -y \
    build-essential \
    curl \
    libgmp3-dev \
    libmpc-dev \
    libmpfr-dev \
    nasm
run ./install_crosscompiler.sh binutils
run ./install_crosscompiler.sh gcc

# Add the cross compiler to the path, to ensure 'i686-elf-gcc' is now available
ENV PATH="~/opt/cross/bin:${PATH}"

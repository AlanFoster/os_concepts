FROM ubuntu:20.04

RUN apt update && \
    DEBIAN_FRONTEND=noninteractive apt install -y \
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
        python3-distutils \
        git

COPY ./install_crosscompiler.sh ./
RUN ./install_crosscompiler.sh all

# Add the cross compiler to the path, to ensure 'i386-elf-gcc' is now available
ENV PATH="~/opt/cross/bin:${PATH}"

RUN git clone https://github.com/pwndbg/pwndbg && \
    cd pwndbg && \
    ./setup.sh

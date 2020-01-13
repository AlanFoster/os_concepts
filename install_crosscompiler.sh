#!/bin/bash

set -euxo pipefail

export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

export BINUTILS=binutils-2.33.1
export GCC=gcc-7.4.0

compile_binutils() {
    mkdir -p /tmp/src
    cd /tmp/src

    BINUTILS_TAR=${BINUTILS}.tar.gz

    if ! [ -e "${BINUTILS_TAR}" ]; then
        curl -O https://ftp.gnu.org/gnu/binutils/${BINUTILS_TAR}
    fi
    tar xf ${BINUTILS_TAR}

    mkdir -p binutils-build
    cd binutils-build

    ../${BINUTILS}/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror

    make
    make install
}

compile_gcc() {
    mkdir -p /tmp/src
    cd /tmp/src

    GCC_TAR=${GCC}.tar.gz

    if ! [ -e "${GCC_TAR}" ]; then
        curl -O https://ftp.gnu.org/gnu/gcc/${GCC}/${GCC_TAR}
    fi
    tar xf ${GCC_TAR}

    mkdir -p gcc-build
    cd gcc-build
    ../${GCC}/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers

    make all-gcc
    make all-target-libgcc
    make install-gcc
    make install-target-libgcc
}

if [ $# -eq 0 ]; then
    echo "missing target"
    exit 1
fi

target="$1"

case $target in
    binutils)
        compile_binutils
    ;;
    gcc)
        compile_gcc
    ;;
    *)
        echo "unknown target, target must be binutils or gcc"
    ;;
esac

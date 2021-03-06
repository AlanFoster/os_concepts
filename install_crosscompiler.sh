#!/bin/bash

set -euxo pipefail

export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:/usr/local/bin:/usr/bin:/bin:$PATH"

export BINUTILS=binutils-2.33.1
export GCC=gcc-7.4.0
export GDB=gdb-9.1

if [[ "$(which python3)" = "" ]]; then
   echo "Missing python3 dependency"
   exit 1
fi

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

    ../${BINUTILS}/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-multilib --disable-werror

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

    cd ${GCC}
    ./contrib/download_prerequisites
    cd ..

    mkdir -p gcc-build
    cd gcc-build
    ../${GCC}/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers

    make all-gcc
    make all-target-libgcc
    make install-gcc
    make install-target-libgcc
}

compile_gdb() {
    mkdir -p /tmp/src
    cd /tmp/src

    GDB_TAR=${GDB}.tar.gz

    if ! [ -e "${GDB_TAR}" ]; then
        curl -O https://ftp.gnu.org/gnu/gdb/${GDB_TAR}
    fi
    tar xf ${GDB_TAR}

    mkdir -p gdb-build
    cd gdb-build
    ../${GDB}/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers --with-python=$(which python3)

    make
    make install
}

if [ $# -eq 0 ]; then
    echo "missing target"
    exit 1
fi

target="$1"

case $target in
    all)
        compile_binutils
        compile_gcc
        compile_gdb
    ;;
    binutils)
        compile_binutils
    ;;
    gcc)
        compile_gcc
    ;;
    gdb)
        compile_gdb
    ;;
    *)
        echo "unknown target, target must be binutils or gdb"
    ;;
esac

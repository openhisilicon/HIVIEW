#!/bin/bash

if [ n"$GSF_CPU_ARCH" = n3516d ];then
    echo "3516d"
    export MYCFG="--host arm-himix200-linux"
    export CC=arm-himix200-linux-gcc CXX=arm-himix200-linux-g++
elif [ n"$GSF_CPU_ARCH" = n3516e ];then
    echo "3516e"
    exit
elif [ n"$GSF_CPU_ARCH" = nt31 ];then
    echo "t31"
    export MYCFG="--host mips-linux-gnu"
    export CC=mips-linux-gnu-gcc CXX=mips-linux-gnu-g++
elif [ n"$GSF_CPU_ARCH" = nx86 ];then
    echo "x86"
    #export MYCFG="linux-generic32"
    export CC=gcc CXX=g++
    export CFLAGS="-m32" CPP_FLAGS="-m32"
else
    echo "no arch"
    exit -1
fi

if [ ! -d "sqlite" ];then
git clone https://github.com/sqlite/sqlite.git
fi
pushd sqlite
git checkout version-3.32.3
rm -rf *
git checkout *
./configure $MYCFG --prefix=$PWD/../tmp/
make
make install
popd

if [ ! -f "tmp/lib/libsqlite3.a" ];then
    echo "no tmp/lib/libsqlite3.a"
    exit -1
fi
cp tmp/lib/libsqlite3.a ../../lib/$GSF_CPU_ARCH/

echo build ok!!!
exit 0

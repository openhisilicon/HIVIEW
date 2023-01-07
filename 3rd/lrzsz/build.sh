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
    #export MYCFG="CFLAGS=-m32 CPPFLAGS=-m32"
    export CC=gcc CXX=g++
    #export CFLAGS="-m32" CPPFLAGS="-m32"
else
    echo "no arch"
    exit -1
fi

if [ ! -f "lrzsz-0.12.20.tar.gz" ];then
wget https://www.ohse.de/uwe/releases/lrzsz-0.12.20.tar.gz
fi

rm -rf lrzsz-0.12.20/ 
tar -zxvf lrzsz-0.12.20.tar.gz
pushd lrzsz-0.12.20/
./configure $MYCFG
make
mkdir -p  ../../usr/bin
popd

if [ ! -f "./lrzsz-0.12.20/src/lrz" ];then
    echo "no ./lrzsz-0.12.20/src/lrz"
    exit -1
fi

if [ ! -f "./lrzsz-0.12.20/src/lsz" ];then
    echo "no ./lrzsz-0.12.20/src/lsz"
    exit -1
fi

mkdir -p ../usr/bin
cp -raf ./lrzsz-0.12.20/src/lrz ../usr/bin/rz
cp -raf ./lrzsz-0.12.20/src/lsz ../usr/bin/sz

echo "build ok!!!"
exit 0;

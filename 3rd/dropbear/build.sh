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

if [ ! -f "dropbear-2022.82.tar.bz2" ];then
wget https://matt.ucc.asn.au/dropbear/releases/dropbear-2022.82.tar.bz2
fi

rm -rf dropbear-2022.82/
tar -jxvf dropbear-2022.82.tar.bz2
pushd dropbear-2022.82/
./configure --prefix=$PWD/../../usr/  $MYCFG --disable-zlib
make PROGRAMS="dropbear dbclient dropbearkey scp"
make PROGRAMS="dropbear dbclient dropbearkey scp" install
popd

if [ ! -f "./usr/sbin/dropbear" ];then
    echo "no ./usr/sbin/dropbear"
    exit -1
fi

echo "build ok!!!"
exit 0;

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
else
    echo "no arch"
    exit 1
fi

# 5ffa99933d01835de59bf1e1c849d851
if [ ! -f "zbar-0.10.tar.gz" ];then
wget http://downloads.sourceforge.net/project/zbar/zbar/0.10/zbar-0.10.tar.gz
fi
rm zbar-0.10
tar -zxvf zbar-0.10.tar.gz

pushd zbar-0.10
./configure CFLAGS="" $MYCFG --disable-video --without-python --without-qt \
            --without-gtk --without-qt --without-python --without-imagemagick

make 
popd

if [ ! -f "./zbar-0.10/zbar/.libs/libzbar.so" ];then
    echo "no ./zbar-0.10/zbar/.libs/libzbar.so"
    exit
fi
cp ./zbar-0.10/zbar/.libs/libzbar.so ../../lib/$GSF_CPU_ARCH/

echo build ok!!!

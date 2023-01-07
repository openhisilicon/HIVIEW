#!/bin/bash

if [ n"$GSF_CPU_ARCH" = n3516d ];then
    echo "3516d"
    export CC=arm-himix200-linux-gcc CXX=arm-himix200-linux-g++
elif [ n"$GSF_CPU_ARCH" = n3516e ];then
    echo "3516e"
    export CC=arm-himix100-linux-gcc CXX=arm-himix100-linux-g++
elif [ n"$GSF_CPU_ARCH" = nt31 ];then
    echo "t31"
    export CC=mips-linux-gnu-gcc CXX=mips-linux-gnu-g++
elif [ n"$GSF_CPU_ARCH" = nx86 ];then
    echo "x86"
    export CC=gcc CXX=g++
    export CFLAGS="-m32" CPP_FLAGS="-m32"
else
    echo "no arch"
    exit -1
fi

if [ ! -d "freetype" ];then
git clone https://gitlab.freedesktop.org/freetype/freetype.git
pushd freetype/
git checkout VER-2-12-0
popd
fi

rm -rf freetype/build
mkdir freetype/build
pushd freetype/build
cmake -DBUILD_SHARED_LIBS=true -DCMAKE_BUILD_TYPE=Release ../
make
popd

if [ ! -f "./freetype/build/libfreetype.so.6.18.2" ];then
    echo "no ./freetype/build/libfreetype.so.6.18.2"
    exit -1
fi

cp -raf ./freetype/include/* ./inc/
cp -raf ./freetype/build/libfreetype.so.6.18.2 ../../lib/$GSF_CPU_ARCH/libfreetype.so
cp -raf ./freetype/build/libfreetype.so.6.18.2 ../../lib/$GSF_CPU_ARCH/libfreetype.so.6

echo build ok!!!
exit 0;

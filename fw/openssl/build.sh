#!/bin/bash

if [ n"$GSF_CPU_ARCH" = n3516d ];then
    echo "3516d"
    export MYCFG="linux-armv4"
    export CC=arm-himix200-linux-gcc CXX=arm-himix200-linux-g++
elif [ n"$GSF_CPU_ARCH" = n3516e ];then
    echo "3516e"
    exit
elif [ n"$GSF_CPU_ARCH" = nt31 ];then
    echo "t31"
    export MYCFG="linux-mips32"
    export CC=mips-linux-gnu-gcc CXX=mips-linux-gnu-g++
elif [ n"$GSF_CPU_ARCH" = nx86 ];then
    echo "x86"
    export MYCFG="linux-generic32"
    export CC=gcc CXX=g++
else
    echo "no arch"
    exit -1
fi

# md5:48278a48ec9bedb84565f7e741612fb4
if [ ! -f "openssl-1.1.0l.tar.gz" ];then
wget https://www.openssl.org/source/old/1.1.0/openssl-1.1.0l.tar.gz
fi
rm -rf src
mkdir -p src
pushd src
tar -zxvf ../openssl-1.1.0l.tar.gz
popd

pushd src/openssl-1.1.0l/
#./Configure  $MYCFG no-asm no-async -fPIC no-shared no-dso
./Configure  $MYCFG no-asm no-async shared
make
if [ ! -f "libcrypto.a" ];then
    echo "no libcrypto.a"
    exit -1
fi
if [ ! -f "libssl.a" ];then
    echo "no libssl.a"
    exit -1
fi
cp libcrypto.so libssl.so ../../../../lib/$GSF_CPU_ARCH/
cp libcrypto.so ../../../../lib/$GSF_CPU_ARCH/libcrypto.so.1.1
cp libssl.so ../../../../lib/$GSF_CPU_ARCH/libssl.so.1.1
popd

echo build ok!!!
exit 0

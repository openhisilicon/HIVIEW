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

if [ ! -d "amazon-kinesis-video-streams-webrtc-sdk-c" ];then
git clone https://github.com/awslabs/amazon-kinesis-video-streams-webrtc-sdk-c.git
pushd amazon-kinesis-video-streams-webrtc-sdk-c/
git checkout v1.4.0
popd
fi

cp libsrtp-CMakeLists.txt amazon-kinesis-video-streams-webrtc-sdk-c/CMake/Dependencies/

rm -rf amazon-kinesis-video-streams-webrtc-sdk-c/build
mkdir amazon-kinesis-video-streams-webrtc-sdk-c/build
pushd amazon-kinesis-video-streams-webrtc-sdk-c/build

#cmake .. -DBUILD_SAMPLE=FALSE -DBUILD_STATIC_LIBS=TRUE -DBUILD_OPENSSL=TRUE -DCMAKE_C_FLAGS="-m32" -DCMAKE_CXX_FLAGS="-m32"
if [ n"$GSF_CPU_ARCH" = nx86 ];then
cmake .. -DBUILD_SAMPLE=FALSE -DBUILD_STATIC_LIBS=TRUE -DBUILD_OPENSSL=TRUE -DBUILD_OPENSSL_PLATFORM=linux-generic32
else
cmake .. -DBUILD_SAMPLE=FALSE -DBUILD_STATIC_LIBS=TRUE -DBUILD_OPENSSL=TRUE -DBUILD_OPENSSL_PLATFORM=linux-generic32 -DBUILD_LIBSRTP_HOST_PLATFORM=x86_64-unknown-linux-gnu -DBUILD_LIBSRTP_DESTINATION_PLATFORM=arm-unknown-linux-uclibcgnueabi
fi
make
popd

cp amazon-kinesis-video-streams-webrtc-sdk-c/src/include/com/amazonaws/kinesis/video/webrtcclient/* ./inc/com/amazonaws/kinesis/video/webrtcclient/
cp amazon-kinesis-video-streams-webrtc-sdk-c/open-source/include/com/amazonaws/kinesis/video/* ./inc/com/amazonaws/kinesis/video/ -rf

if [ ! -f "./amazon-kinesis-video-streams-webrtc-sdk-c/build/libkvsWebrtcClient.a" ];then
    echo "no ./amazon-kinesis-video-streams-webrtc-sdk-c/build/libkvsWebrtcClient.a"
    exit -1
fi

if [ ! -f "./amazon-kinesis-video-streams-webrtc-sdk-c/open-source/lib/libkvspic.a" ];then
    echo "no ./amazon-kinesis-video-streams-webrtc-sdk-c/open-source/lib/libkvspic.a"
    exit -1
fi

if [ ! -f "./amazon-kinesis-video-streams-webrtc-sdk-c/open-source/lib/libsrtp2.a" ];then
    echo "no ./amazon-kinesis-video-streams-webrtc-sdk-c/open-source/lib/libsrtp2.a"
    exit -1
fi

if [ ! -f "./amazon-kinesis-video-streams-webrtc-sdk-c/open-source/lib/libusrsctp.a" ];then
    echo "no ./amazon-kinesis-video-streams-webrtc-sdk-c/open-source/lib/libusrsctp.a"
    exit -1
fi

cp ./amazon-kinesis-video-streams-webrtc-sdk-c/build/libkvsWebrtcClient.a ../../lib/$GSF_CPU_ARCH/libkvsWebrtcClient.a
cp ./amazon-kinesis-video-streams-webrtc-sdk-c/open-source/lib/libkvspic.a ../../lib/$GSF_CPU_ARCH/libkvspic.a
cp ./amazon-kinesis-video-streams-webrtc-sdk-c/open-source/lib/libsrtp2.a ../../lib/$GSF_CPU_ARCH/libsrtp2.a
cp ./amazon-kinesis-video-streams-webrtc-sdk-c/open-source/lib/libusrsctp.a ../../lib/$GSF_CPU_ARCH/libusrsctp.a

echo "build ok !!!"
exit 0
#if [ n"$GSF_CPU_ARCH" = nx86 ];then
#cp ./amazon-kinesis-video-streams-webrtc-sdk-c/open-source/lib/libssl.a ../../lib/$GSF_CPU_ARCH/libssl.a
#cp ./amazon-kinesis-video-streams-webrtc-sdk-c/open-source/lib/libcrypto.a ../../lib/$GSF_CPU_ARCH/libcrypto.a
#fi

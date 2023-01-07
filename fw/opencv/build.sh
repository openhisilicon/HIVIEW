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
    export MYCFG="-D CMAKE_C_FLAGS=-m32 -D CMAKE_CXX_FLAGS=-m32"
    export CC=gcc CXX=g++
    #export CFLAGS="-m32" CPPFLAGS="-m32"
else
    echo "no arch"
    exit -1
fi

if [ ! -d "opencv" ];then
git clone https://github.com/opencv/opencv.git
fi
pushd opencv
#git checkout 3.4
git checkout 3.4.2
#git checkout 3.4.2-openvino
rm -rf build tmp
mkdir -p build
popd

if [ ! -d "opencv_contrib" ];then
git clone https://github.com/opencv/opencv_contrib.git
fi
pushd opencv_contrib
git checkout 3.4.2
popd

pushd opencv/build
#cmake -DCMAKE_INSTALL_PREFIX=$PWD/../tmp/ -DWITH_GTK=OFF -DWITH_GTK_2_X=OFF ../
#-DWITH_GPHOTO2=OFF \
#-DBUILD_opencv_fuzzy=ON -DBUILD_opencv_line_descriptor=ON \
cmake -DCMAKE_INSTALL_PREFIX=$PWD/../tmp/ -DWITH_GTK=OFF -DWITH_GTK_2_X=OFF \
        -DOPENCV_EXTRA_MODULES_PATH=$PWD/../../opencv_contrib/modules/ \
        -DBUILD_opencv_bioinspired=OFF -DBUILD_opencv_datasets=OFF -DBUILD_ITT=OFF \
        -DBUILD_opencv_img_hash=OFF -DBUILD_opencv_hfs=OFF -DBUILD_opencv_freetype=OFF \
        -DBUILD_opencv_phase_unwrapping=OFF -DBUILD_opencv_supface_matching=OFF -DBUILD_opencv_tracking=OFF \
        -DBUILD_opencv_plot=OFF -DBUILD_opencv_reg=OFF -DBUILD_opencv_saliency=OFF -DBUILD_opencv_xphoto=OFF \
        -DBUILD_opencv_dnn=OFF -DBUILD_opencv_shape=OFF -DBUILD_opencv_highgui=OFF -DBUILD_opencv_surface_matching=OFF \
        -DBUILD_opencv_superres=OFF -DBUILD_opencv_videoio=OFF -DBUILD_opencv_videostab=OFF \
        -DBUILD_opencv_calib3d=OFF -DBUILD_opencv_photo=OFF -DBUILD_opencv_objdetect=OFF\
        -DZLIB_INCLUDE_DIR=$PWD/../3rdparty/zlib $MYCFG ../

make 
make install
popd

if [ ! -f "./opencv/tmp/lib/libopencv_imgcodecs.so" ];then
    echo "no ./opencv/tmp/lib/libopencv_imgcodecs.so"
    exit -1
fi
if [ ! -f "./opencv/tmp/lib/libopencv_core.so" ];then
    echo "no ./opencv/tmp/lib/libopencv_core.so"
    exit -1
fi
if [ ! -f "./opencv/tmp/lib/libopencv_imgproc.so" ];then
    echo "no ./opencv/tmp/lib/libopencv_imgproc.so"
    exit -1
fi
cp ./opencv/tmp/lib/libopencv_imgproc.so ../../lib/$GSF_CPU_ARCH/
cp ./opencv/tmp/lib/libopencv_imgproc.so ../../lib/$GSF_CPU_ARCH/libopencv_imgproc.so.3.4
cp ./opencv/tmp/lib/libopencv_core.so ../../lib/$GSF_CPU_ARCH/
cp ./opencv/tmp/lib/libopencv_core.so ../../lib/$GSF_CPU_ARCH/libopencv_core.so.3.4
cp ./opencv/tmp/lib/libopencv_imgcodecs.so ../../lib/$GSF_CPU_ARCH/
cp ./opencv/tmp/lib/libopencv_imgcodecs.so ../../lib/$GSF_CPU_ARCH/libopencv_imgcodecs.so.3.4

echo build ok!!!
exit 0

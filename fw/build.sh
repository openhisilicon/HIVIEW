#!/bin/bash

run_check(){
pushd $1
$2
if [ ! $? -eq 0 ]; then
    echo exit build [$1 $2]
    exit -1
fi
echo ok build [$1 $2]
popd
}

run_check ./zbar/ ./build.sh
run_check ./freetype/ ./build.sh
run_check ./opencv/ ./build.sh
run_check ./openssl/ ./build.sh
run_check ./sqlite/ ./build.sh
run_check ./kvs/ ./build.sh

exit 0

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

run_check ./lrzsz/ ./build.sh
run_check ./dropbear/ ./build.sh

exit 0

#!/bin/bash
if [ $# -lt 4 ];then
echo $0 192.168.1.12 22 ./debugrv1126/ipcserverd /app/ipcserverd
echo $0 192.168.1.12 22 ./DebugRv1126/libipccamera.so /app/lib/libipccamera.so
exit 1
fi

if [ n"$GSF_CPU_ARCH" = n3516d ];then
    echo "3516d"
    arm-himix200-linux-strip $3
elif [ n"$GSF_CPU_ARCH" = n3516e ];then
    echo "3516e"
    arm-himix100-linux-strip $3
else
    echo "no arch"
    exit 1
fi

scp -P $2 $3 root@$1:/tmp/tttt
ssh -p $2 root@$1 "cp /tmp/tttt $4"
ssh -p $2 root@$1 "chmod 777 $4"
ssh -p $2 root@$1 "rm /tmp/tttt"

if [ -n "$5" ]; then
ssh -p $2 root@$1 "$5"
fi


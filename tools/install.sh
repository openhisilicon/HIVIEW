#!/bin/sh

basepath=$(cd `dirname $0`; pwd)                                                                                                                                                             
cd $basepath 

cpfunc() {
  [ -d "$2"  ] && k=${2%%/}/`basename $1` || k=$2

  fromsize=`ls -l $1| awk '{print $5}'`
  
  [ "$fromsize"x == 0x ] && echo -e "progress=-2" && return
  
  cp $1 $k &
  sleep 0.5
  while :;do
      tosize=`ls -l $k 2>/dev/null | awk '{print $5}'`

      if [ "$tosize"x != x ]; then
          x=`echo "$tosize $fromsize"| awk '{print int($1*100/$2)}'`
          echo -e "progress=$x"
          [ $x -ge 100  ] && break
      fi
      sleep 0.5
  done
}



[ -f "./runing" ] && echo -e "progress=-3" && exit;

touch ./runing

if [ -f "./app.xz" ]; then
  cpfunc "./app.xz" "/factory/app.xz";
else
  echo -e "progress=-1";
fi

rm ./runing



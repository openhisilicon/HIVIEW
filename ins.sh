#!/bin/sh


ins3516d()
{

INST=/nfs/16d
echo "ins3516d to [$INST]"

mkdir -p $INST/lib $INST/bin $INST/cfg $INST/ko $INST/model;

cp lib/3516d/*.so*  $INST/lib -arfv;
cp bin/3516d/*.exe $INST/bin -arfv;
arm-himix200-linux-strip $INST/lib/*;
arm-himix200-linux-strip $INST/bin/*;

cp mod/mpp/3516d/ko/* $INST/ko/ -arfv;
chmod +x $INST/ko/load3516*;

cp mod/svp/3516d/model/*.wk $INST/model/ -arfv;

cp mod/mpp/3516d/lib/hisisdk/*.so $INST/lib -arfv;
arm-himix200-linux-strip $INST/lib/*;

cp mod/mpp/3516d/src/scene_auto/param/*.ini $INST/cfg -arfv;

mkdir -p $INST/www;
cp mod/webs/www/* $INST/www -arfv;

#### packet .upg #####
echo "wait minizip ..."
mkdir -p $INST/upg;
cp $PROJ/tools/install.sh $INST/upg/ -arfv;
cd $INST; tar Jcf ./upg/app.xz ko lib bin cfg www model; cd -;
cd $INST/upg; $PROJ/tools/minizip -o -p 123 cam16d.upg app.xz install.sh;cp -vf ./cam16d.upg /mnt/hgfs/works/; cd -;
######################
echo "ok."
######################
}


ins3516a()
{

INST=/nfs/16a
echo "ins3516a to [$INST]"

mkdir -p $INST/lib $INST/bin $INST/cfg $INST/ko $INST/model;

cp lib/3516a/*.so*  $INST/lib -arfv;
cp bin/3516a/*.exe $INST/bin -arfv;
arm-himix200-linux-strip $INST/lib/*;
arm-himix200-linux-strip $INST/bin/*;

cp mod/mpp/3516a/ko/* $INST/ko/ -arfv;
chmod +x $INST/ko/load3516*;

cp mod/svp/3516a/model/*.wk $INST/model/ -arfv;

cp mod/mpp/3516a/lib/hisisdk/*.so $INST/lib -arfv;
arm-himix200-linux-strip $INST/lib/*;

cp mod/mpp/3516a/src/scene_auto/param/*.ini $INST/cfg -arfv;

mkdir -p $INST/www;
cp mod/webs/www/* $INST/www -arfv;

#### packet .upg #####
echo "wait minizip ..."
mkdir -p $INST/upg;
cp $PROJ/tools/install.sh $INST/upg/ -arfv;
cd $INST; tar Jcf ./upg/app.xz ko lib bin cfg www model; cd -;
cd $INST/upg; $PROJ/tools/minizip -o -p 123 cam16a.upg app.xz install.sh;cp -vf ./cam16a.upg /mnt/hgfs/works/; cd -;
######################
echo "ok."
######################
}

ins3559v200()
{

INST=/nfs/59v200
echo "ins3559v200 to [$INST]"

mkdir -p $INST/lib $INST/bin $INST/cfg $INST/ko $INST/model;

cp lib/3559/*.so*  $INST/lib -arfv;
cp bin/3559/*.exe $INST/bin -arfv;
arm-himix200-linux-strip $INST/lib/*;
arm-himix200-linux-strip $INST/bin/*;

cp mod/mpp/3559/ko/* $INST/ko/ -arfv;
chmod +x $INST/ko/load3559*;

cp mod/svp/3559/model/*.wk $INST/model/ -arfv;

cp mod/mpp/3559/lib/hisisdk/*.so $INST/lib -arfv;
arm-himix200-linux-strip $INST/lib/*;

cp mod/mpp/3559/src/scene_auto/param/*.ini $INST/cfg -arfv;

mkdir -p $INST/www;
cp mod/webs/www/* $INST/www -arfv;

#### packet .upg #####
echo "wait minizip ..."
mkdir -p $INST/upg;
cp $PROJ/tools/install.sh $INST/upg/ -arfv;
cd $INST; tar Jcf ./upg/app.xz ko lib bin cfg www model; cd -;
cd $INST/upg; $PROJ/tools/minizip -o -p 123 cam59v200.upg app.xz install.sh;cp -vf ./cam59v200.upg /mnt/hgfs/works/; cd -;
######################
echo "ok."
######################
}





ins3519()
{

INST=/nfs/3519
echo "ins3519 to [$INST]"

mkdir -p $INST/lib $INST/bin $INST/cfg $INST/ko;

cp lib/3519/*.so*  $INST/lib -arfv;
cp bin/3519/*.exe $INST/bin -arfv;
arm-hisiv500-linux-strip $INST/lib/*;
arm-hisiv500-linux-strip $INST/bin/*;

cp mod/mpp/3519/ko/* $INST/ko/ -arfv;
chmod +x $INST/ko/load3519*;

cp mod/mpp/3519/lib/hisisdk/*.so $INST/lib -arfv;
arm-hisiv500-linux-strip $INST/lib/*;

mkdir -p $INST/www;
cp mod/mpp/3519/src/scene_auto/ini/*.ini $INST/cfg -arfv;
cp mod/webs/www/* $INST/www -arfv;

#### packet .upg #####
echo "wait minizip ..."
mkdir -p $INST/upg;
cp $PROJ/tools/install.sh $INST/upg/ -arfv;
cd $INST; tar Jcf ./upg/app.xz ko lib bin cfg www; cd -;
cd $INST/upg; $PROJ/tools/minizip -o -p 123 cam19.upg app.xz install.sh;cp -vf ./cam19.upg /mnt/hgfs/works/; cd -;
######################
echo "ok."
######################
}

PROJ=`pwd`
echo "PROJ:[$PROJ]"

case $1 in
    16d)
        ins3516d
        ;;
    16a)
        ins3516a
        ;;
    19)
        ins3519
        ;;
    59)
        ins3559v200
        ;;
    *)
        echo "usage: $0 {16d|16a|19|59}"
        exit 1
        ;;
esac


1. modify the spi parameter, now support spi1 and spi3: smp/a55_linux/source/out/ko/load3519dv500
1.1  if use spi1, mux with vo GPIO1/I2C5:       insmod extdrv/ot_spi.ko spi=1
1.2  if use spi3, mux with mipitx:              insmod extdrv/ot_spi.ko spi=3

2. insert gyro ko and update the file: smp/a55_linux/source/out/ko/load3519dv500
2.1  insert_gyro
2.2  rmmod_gyro
2.3  SNS_TYPE0=os08a20;               # sensor type

3. modify the makefile parameter: smp/a55_linux/source/mpp/sample/Makefile.param.
3.1 SENSOR0_TYPE ?= OV_OS08A20_MIPI_8M_30FPS_12BIT
3.2 ################ open GYRO_DIS sample ########################
	GYRO_DIS ?= y

Rebuild the sample and get the sample_dis.

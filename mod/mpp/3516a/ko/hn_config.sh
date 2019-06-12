#!/bin/sh

####################Variables Definition##########################

insert_sns()
{
	case $SNS_TYPE in
		ar0130|9m034)
			himm 0x200f0050 0x2;                # i2c0_scl
			himm 0x200f0054 0x2;                # i2c0_sda
			;;
		mt9p006)
			himm 0x200f0050 0x2;                # i2c0_scl
			himm 0x200f0054 0x2;                # i2c0_sda

			himm 0x2003002c 0xE0007             # sensor unreset, clk 24MHz, VI 250MHz
			;;
		imx136)
			himm 0x200f0050 0x1;                # spi0_sclk
			himm 0x200f0054 0x1;                # spi0_sdo
			himm 0x200f0058 0x1;                # spi0_sdi
			himm 0x200f005c 0x1;                # spi0_csn
			;;			
		imx178)
			himm 0x200f0050 0x2;                # i2c0_scl
			himm 0x200f0054 0x2;                # i2c0_sda
			himm 0x2003002c 0xF0007             # sensor unreset, clk 25MHz, VI 250MHz
			#himm 0x2003002c 0x90007            # sensor unreset, clk 37.125MHz, VI 250MHz
			;;
		imx185)
			himm 0x200f0050 0x1;                # spi0_sclk
			himm 0x200f0054 0x1;                # spi0_sdo
			himm 0x200f0058 0x1;                # spi0_sdi
			himm 0x200f005c 0x1;                # spi0_csn
			
			himm 0x2003002c 0x90007             # sensor unreset, clk 37.125MHz, VI 250MHz

			#insmod extdrv/sensor_spi.ko
			;;
		mn34220)
			himm 0x200f0050 0x2;                # i2c0_scl
			himm 0x200f0054 0x2;                # i2c0_sda
			
			himm 0x2003002c 0x90007             # sensor unreset, clk 37.125MHz, VI 250MHz
			;;
		ar0330)
			himm 0x200f0050 0x2;                # i2c0_scl
			himm 0x200f0054 0x2;                # i2c0_sda

			himm 0x2003002c 0xE0007             # sensor unreset, clk 24MHz, VI 250MHz
			;;
		bt1120)
			;;
		*)
			echo "xxxx Invalid sensor type $SNS_TYPE xxxx"
			;;
	esac
}

remove_sns()
{
	rmmod ssp &> /dev/null
	rmmod sensor_spi &> /dev/null
	rmmod ssp_pana &> /dev/null
}

sys_restore()
{
	####################################################
	clkcfg_hi3516a.sh > /dev/null

	pinmux_hi3516a.sh > /dev/null

	# system configuration
	sysctl_hi3516a.sh $b_arg_online > /dev/null
	insert_sns;
}

######################parse arg###################################
b_arg_sensor=0
b_arg_online=1
b_arg_restore=0

for arg in $@
do
	if [ $b_arg_sensor -eq 1 ] ; then
		b_arg_sensor=0
		SNS_TYPE=$arg;
	fi

	case $arg in
		"-sensor")
			b_arg_sensor=1;
			;;
		"-restore")
			b_arg_restore=1;
			;;
		"-offline")
			b_arg_online=0;
			;;
	esac
done
#######################parse arg end########################

#######################Action###############################
if [ $b_arg_restore -eq 1 ]; then	
	sys_restore;
fi


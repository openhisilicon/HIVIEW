#!/bin/sh

####################Variables Definition##########################

spi0_4wire_pin_mux()
{
    #pinmux
    himm 0x1204018c 0x1;  #SPI0_SCLK
    himm 0x12040190 0x1;  #SPI0_SD0
    himm 0x12040194 0x1;  #SPI0_SDI
    himm 0x12040198 0x1;  #SPI0_CSN
    
    #drive capability   
    himm 0x12040998 0x150;  #SPI0_SCLK
    himm 0x1204099c 0x160;  #SPI0_SD0
    himm 0x120409a0 0x160;  #SPI0_SDI
    himm 0x120409a4 0x160;  #SPI0_CSN
}
spi0_3wire_pin_mux()
{
    #pinmux
    himm 0x1204018c 0x3;    #SPI0_3WIRE_CLK
    himm 0x12040190 0x3;    #SPI0_3WIRE_DATA
    himm 0x12040198 0x3;    #SPI0_3WIRE_CSN

    #drive capability   
    himm 0x12040998 0x150;  #SPI0_3WIRE_CLK
    himm 0x1204099c 0x160;  #SPI0_3WIRE_DATA
    himm 0x120409a4 0x160;  #SPI0_3WIRE_CSN
}
i2c0_pin_mux()
{
    #pinmux
    himm 0x12040190 0x2;  #I2C0_SDA
    himm 0x1204018c 0x2;  #I2C0_SCL
    
    #drive capability
    himm 0x1204099c 0x120; #I2C0_SDA
    himm 0x12040998 0x120; #I2C0_SCL    
}
i2c1_pin_mux()
{
    #pinmux
    himm 0x1204001c 0x2;  #I2C1_SDA
    himm 0x12040018 0x2;  #I2C1_SCL
    
    #drive capability
    himm 0x1204081c 0x120;  #I2C1_SDA
    himm 0x12040818 0x120;  #I2C1_SCL       
}

insert_sns()
{
	local tmp=0;
	local tmp1;
	local tmp2;
if [ ${SNS_TYPE0} != "NULL" ] ; then		
		case $SNS_TYPE0 in
        imx226)
            tmp=0x11;
            himm 0x12010040 0x11;           # sensor0 clk_en, 72MHz
			# SDK config:     IVE:396M,  GDC:475M,  VGS:500M,  VEDU:600M,   VPSS:300M 
			#imx226: viu0:600M,isp0:600M, viu1:300M,isp1:300M
			himm 0x1201004c 0x00094c23;
			himm 0x12010054 0x00024041;
            spi0_4wire_pin_mux;
            ;;
        imx274)
            tmp=0x11;
            himm 0x12010040 0x11;           # sensor0 clk_en, 72MHz
			# SDK config:     IVE:396M,  GDC:475M,  VGS:500M,  VEDU:600M,   VPSS:300M 
			#imx274:viu0: 600M,isp0:300M, viu1:300M,isp1:300M
			himm 0x1201004c 0x00094c23;
			himm 0x12010054 0x0004041;
            spi0_4wire_pin_mux;
            ;;
        imx274_mipi)
            tmp=0x14;
			# SDK config:     IVE:396M,  GDC:475M,  VGS:500M,  VEDU:600M,   VPSS:300M 
			#	viu0:300M,isp0:300M, viu1:300M,isp1:300M
			himm 0x1201004c 0x00094c21;
			himm 0x12010054 0x0004041;
            himm 0x12010040 0x14;           # sensor0 clk_en, 24MHz
            i2c0_pin_mux;
            ;;
        imx117)
            tmp=0x11;
			# SDK config:     IVE:396M,  GDC:475M,  VGS:500M,  VEDU:600M,   VPSS:300M 
			#	viu0:300M,isp0:300M, viu1:300M,isp1:300M
			himm 0x1201004c 0x00094c21;
			himm 0x12010054 0x0004041;
            himm 0x12010040 0x11;           # sensor0 clk_en, 72MHz
            spi0_4wire_pin_mux;
            ;;			
        imx317)
            tmp=0x11;
            himm 0x12010040 0x11;           # sensor0 clk_en, 72MHz
            spi0_4wire_pin_mux;
            ;;
        imx290)
            tmp=0x18;
			# SDK config:     IVE:396M,  GDC:475M,  VGS:500M,  VEDU:600M,   VPSS:300M 
			#imx290:viu0:340M,isp0:214M, viu1:340M,isp1:214M
			himm 0x1201004c 0x00094c24;
			himm 0x12010054 0x0004;
            himm 0x12010040 0x18;           # sensor0 clk_en, 37.125MHz
            i2c0_pin_mux;
            ;;
        mn34220)
            tmp=0x18;
            himm 0x12010040 0x18;           # sensor0 clk_en, 37.125MHz
            i2c0_pin_mux;
            ;;
        mn34120)
            tmp=0x12;
            himm 0x12010040 0x12;           # sensor0 clk_en, 54MHz
            spi0_3wire_pin_mux;
            ;;
        ov4689)
            tmp=0x14;
			# SDK config:     IVE:396M,  GDC:475M,  VGS:500M,  VEDU:600M,   VPSS:300M 
			#ov4689:	viu0:300M,isp0:300M, viu1:300M,isp1:300M
			himm 0x1201004c 0x00094c21;
			himm 0x12010054 0x0004041;
            himm 0x12010040 0x14;           # sensor0 clk_en, 24MHz
            i2c0_pin_mux;
            ;;
        bt1120)
            #himm 0x2003002c 0x23;
            #himm 0x20670018 0x1;
            #himm 0x20120004 0x501a00;
            #himm 0x20670000 0x0;
            ;;
        *)
            echo "xxxx Invalid sensor type $SNS_TYPE0 xxxx"
            report_error;;
    esac
fi

if [ ${SNS_TYPE1} != "NULL" ] ; then	
	case $SNS_TYPE1 in       
        imx290)
            tmp1=0x1800;
            tmp2=$((tmp+tmp1));
            himm 0x12010040 $tmp2;           # sensor0 clk_en, 37.125MHz
            i2c1_pin_mux;
            ;;
        ov4689)
            tmp1=0x1400;
            tmp2=$((tmp+tmp1));
            himm 0x12010040 $tmp2;           # sensor0 clk_en, 24MHz
            i2c1_pin_mux;
            ;;
        bt1120)
            #himm 0x2003002c 0x23;
            #himm 0x20670018 0x1;
            #himm 0x20120004 0x501a00;
            #himm 0x20670000 0x0;
            ;;
	    *)
            echo "xxxx Invalid sensor type $SNS_TYPE1 xxxx"
            report_error;;
    esac			
fi

if [ ${WORK_MODE} != "NULL" ] ; then	
	case $WORK_MODE in       
        stitching)
            himm 0x12040184 0x1;    # SENSOR0 HS from VI0 HS
            himm 0x12040188 0x1;    # SENSOR0 VS from VI0 VS
            himm 0x12040010 0x1;    # SENSOR1 HS from VI0 HS
            himm 0x12040014 0x1;    # SENSOR1 VS from VI0 VS
            ;;     
         single_pipe)
            himm 0x12040184 0x1;    # SENSOR0 HS from VI0 HS
            himm 0x12040188 0x1;    # SENSOR0 VS from VI0 VS
            himm 0x12040010 0x2;    # SENSOR1 HS from VI1 HS
            himm 0x12040014 0x2;    # SENSOR1 VS from VI1 VS
            ;;     
        double_pipe)
            tmp1=$((tmp<<8))
            tmp2=$((tmp+tmp1))
            himm 0x12010040 $tmp2;  # sensor0 clk + sensor1 clk
            himm 0x12040184 0x2;    # SENSOR0 HS from VI1 HS
            himm 0x12040188 0x2;    # SENSOR0 VS from VI1 VS
            #himm 0x12040010 0x1;    # SENSOR1 HS from VI0 HS
            #himm 0x12040014 0x1;    # SENSOR1 VS from VI0 VS
            ;;
        *)
            echo "xxxx Invalid work mode $WORK_MODE xxxx"
            report_error;;
    esac			
fi			
}


remove_sns()
{
	rmmod hi_ssp_sony &> /dev/null
	rmmod hi_sensor_spi &> /dev/null
}

vicap1_pwrdn()
{
	himm 0x120a012c 0x3; 
	himd.l 0x120a012c > /dev/null;
	himm 0x120a012c 0x2;
}

sys_restore()
{
	# pinmux configuration
    pinmux.sh -net -vi > /dev/null;

	# clock configuration
	clkcfg.sh > /dev/null;
	#vicap1_pwrdn;
	vicap1_pwrdn > /dev/null

	# system configuration
	sysctl.sh $b_arg_online > /dev/null;
	insert_sns > /dev/null;
	pinmux.sh -vo BT1120 > /dev/null;
}

######################parse arg###################################
b_arg_sensor0=0
b_arg_sensor1=0
b_arg_online=1
b_arg_restore=0
b_arg_mode=0

SNS_TYPE0=imx274;           	# sensor type
SNS_TYPE1=NULL;            	# sensor type
WORK_MODE=single_pipe

for arg in $@
do
	if [ $b_arg_sensor0 -eq 1 ] ; then
		b_arg_sensor0=0;
		SNS_TYPE0=$arg;		
	fi
	
	if [ $b_arg_sensor1 -eq 1 ] ; then
		b_arg_sensor1=0;
		SNS_TYPE1=$arg;		
	fi

	case $arg in
		"-sensor0")
			b_arg_sensor0=1;
			;;
		"-sensor1")
			b_arg_sensor1=1;
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


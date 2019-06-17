#!/bin/sh

# This is a sample, you should rewrite it according to your chip #
# You can configure your pinmux for the application here!

report_error()
{
	echo "******* Error: There's something wrong, please check! *****"
	exit 1
}

#VICAP default setting is VIU
vicap_pin_mux()
{
	#sensor0 pinmux
	himm 0x1204017c 0x1;  #SENSOR0_CLK
	himm 0x12040180 0x0;  #SENSOR0_RSTN
	himm 0x12040184 0x1;  #SENSOR0_HS,from vi0
	himm 0x12040188 0x1;  #SENSOR0_VS,from vi0
	#sensor0 drive capability
	himm 0x12040988 0x150;  #SENSOR0_CLK
	himm 0x1204098c 0x170;  #SENSOR0_RSTN
	himm 0x12040990 0x170;  #SENSOR0_HS
	himm 0x12040994 0x170;  #SENSOR0_VS 

	#sensor1 pinmux
	himm 0x12040008 0x1;  #SENSOR1_CLK
	himm 0x1204000c 0x0;  #SENSOR1_RSTN
	himm 0x12040010 0x2;  #SENSOR1_HS,from vi1
	himm 0x12040014 0x2;  #SENSOR1_VS,from vi1
	#sensor1 drive capability
	himm 0x12040808 0x150;  #SENSOR1_CLK
	himm 0x1204080c 0x170;  #SENSOR1_RSTN
	himm 0x12040810 0x170;  #SENSOR1_HS
	himm 0x12040814 0x170;  #SENSOR1_VS  
	
	#vi1 pinmux
	himm 0x12040030 0x1;  #VI1_CLK
	himm 0x12040028 0x1;  #VI1_HS
	himm 0x1204002c 0x1;  #VI1_VS
	himm 0x12040034 0x1;  #VI1_DATA0
	himm 0x12040038 0x1;  #VI1_DATA1
	himm 0x1204003c 0x1;  #VI1_DATA2
	himm 0x12040040 0x1;  #VI1_DATA3
	himm 0x12040044 0x1;  #VI1_DATA4
	himm 0x12040048 0x1;  #VI1_DATA5
	#vi1 drive capability
	himm 0x12040830 0x170;  #VI1_CLK
	himm 0x12040828 0x170;  #VI1_HS
	himm 0x1204082c 0x170;  #VI1_VS
	himm 0x12040834 0x170;  #VI1_DATA0
	himm 0x12040838 0x170;  #VI1_DATA1
	himm 0x1204083c 0x170;  #VI1_DATA2
	himm 0x12040840 0x170;  #VI1_DATA3
	himm 0x12040844 0x170;  #VI1_DATA4
	himm 0x12040848 0x170;  #VI1_DATA5	
	
}

#i2c2 -> sil9136 aic31
i2c2_pin_mux()
{
	#pinmux
	himm 0x1204005c 0x1;  #I2C2_SDA
	himm 0x12040060 0x1;  #I2C2_SCL
	
	#drive capability
	himm 0x1204085C 0x120; #I2C2_SDA
	himm 0x12040860 0x120; #I2C2_SCL	
}


#i2c3 -> adv7179
i2c3_pin_mux()
{
	#pinmux
	himm 0x12040178 0x3;  #I2C3_SDA
	himm 0x12040160 0x3;  #I2C3_SCL
	
	#drive capability
	himm 0x12040984 0x120  #I2C3_SDA
	himm 0x1204096C 0x120  #I2C3_SCL	
}


#spi1 -> vi
spi1_pin_mux()
{
	#pinmux
	himm 0x12040018 0x1;  #SPI1_SCLK
	himm 0x1204001c 0x1;  #SPI1_SD0
	himm 0x12040020 0x1;  #SPI1_SDI
	himm 0x12040024 0x1;  #SPI1_CSN
	
	#drive capability	
	himm 0x12040818 0x150;  #SPI1_SCLK
	himm 0x1204081C 0x160;  #SPI1_SD0
	himm 0x12040820 0x160;  #SPI1_SDI
	himm 0x12040824 0x160;  #SPI1_CSN
}

#spi3 -> LCD
spi3_pin_mux()
{
	#pinmux
	himm 0x12040160 0x1;  #SPI3_SCLK
	himm 0x12040178 0x1;  #SPI3_SD0
	himm 0x12040170 0x1;  #SPI3_SDI
	himm 0x12040174 0x1;  #SPI3_CSN
	#drive capability
	himm 0x1204096C 0x150; #SPI3_SCLK
	himm 0x12040984 0x160; #SPI3_SD0
	himm 0x1204097C 0x160; #SPI3_SDI
	himm 0x12040980 0x160; #SPI3_CSN
}

#vo pinmux for 16bit/24bit LCD
vo_lcd_16bit_24bit_mux()
{
	#pinmux
	himm 0x120400F8 0x1; 	#VO_CLK      muxctrl_reg62
	himm 0x120400FC 0x1; 	#VO_DATA0    muxctrl_reg63
	himm 0x12040100 0x1; 	#VO_DATA1    muxctrl_reg64
	himm 0x12040104 0x1; 	#VO_DATA2    muxctrl_reg65
	himm 0x12040108 0x1; 	#VO_DATA3    muxctrl_reg66
	himm 0x1204010C 0x1; 	#VO_DATA4    muxctrl_reg67
	himm 0x12040110 0x1; 	#VO_DATA5    muxctrl_reg68
	himm 0x12040114 0x1; 	#VO_DATA6    muxctrl_reg69
	himm 0x12040118 0x1; 	#VO_DATA7    muxctrl_reg70
	himm 0x1204011C 0x1;	#VO_DATA8    muxctrl_reg71
	himm 0x12040120 0x1;	#VO_DATA9    muxctrl_reg72
	himm 0x12040124 0x1;	#VO_DATA10   muxctrl_reg73
	himm 0x12040128 0x1;	#VO_DATA11	 muxctrl_reg74
	himm 0x1204012C 0x1;	#VO_DATA12	muxctrl_reg75
	himm 0x12040130 0x1; 	#VO_DATA13	muxctrl_reg76
	himm 0x12040134 0x1; 	#VO_DATA14	muxctrl_reg77
	himm 0x12040138 0x1; 	#VO_DATA15	muxctrl_reg78
	himm 0x1204013C 0x1; 	#VO_VS		muxctrl_reg79
	himm 0x12040140 0x1; 	#VO_DATA18	muxctrl_reg80
	himm 0x12040144 0x1; 	#VO_DATA16	muxctrl_reg81
	himm 0x12040148 0x1; 	#VO_DATA20	muxctrl_reg82
	himm 0x1204014C 0x1; 	#VO_DATA17	muxctrl_reg83
	himm 0x12040150 0x1; 	#VO_HS		muxctrl_reg84
	himm 0x12040154 0x1;	#VO_DATA19	muxctrl_reg85
	himm 0x12040158 0x1;	#VO_DATA23	muxctrl_reg86
	himm 0x1204015C 0x1;	#VO_DATA22	muxctrl_reg87
	himm 0x12040164 0x1;	#VO_DE		muxctrl_reg89
	himm 0x12040168 0x1;	#VO_DATA21	muxctrl_reg90

	#drive capability
	#himm 0x12040904 0x020;  #VO_CLK  
	himm 0x12040904 0x0A0;	#VO_CLK 
	himm 0x12040908 0x040; 	#VO_DATA0 
	himm 0x1204090C 0x040; 	#VO_DATA1 
	himm 0x12040910 0x040; 	#VO_DATA2 
	himm 0x12040914 0x040; 	#VO_DATA3 
	himm 0x12040918 0x040; 	#VO_DATA4 
	himm 0x1204091C 0x040; 	#VO_DATA5 
	himm 0x12040920 0x040; 	#VO_DATA6 
	himm 0x12040924 0x040; 	#VO_DATA7 
	himm 0x12040928 0x040;	#VO_DATA8 
	himm 0x1204092C 0x040;	#VO_DATA9 
	himm 0x12040930 0x040;	#VO_DATA10
	himm 0x12040934 0x040;	#VO_DATA11
	himm 0x12040938 0x040;	#VO_DATA12
	himm 0x1204093C 0x040; 	#VO_DATA13
	himm 0x12040940 0x040; 	#VO_DATA14
	himm 0x12040944 0x040; 	#VO_DATA15
	himm 0x12040948 0x1F0; 	#VO_VS		
	himm 0x1204094C 0x0C0; 	#VO_DATA18
	himm 0x12040950 0x0C0; 	#VO_DATA16
	himm 0x12040954 0x0C0; 	#VO_DATA20
	himm 0x12040958 0x0C0; 	#VO_DATA17
	himm 0x1204095C 0x1F0; 	#VO_HS		
	himm 0x12040960 0x000;	#VO_DATA19
	himm 0x12040964 0x000;	#VO_DATA23
	himm 0x12040968 0x000;	#VO_DATA22
	himm 0x12040970 0x130;	#VO_DE		
	himm 0x12040974 0x000; 	#VO_DATA21	
}

#vo pinmux for bt656 and 6bit/8bit LCD
vo_lcd_bt656_6bit_8bit_pin()
{
	#pinmux
	himm 0x120400F8 0x1; 	#VO_CLK      muxctrl_reg62
	himm 0x120400FC 0x1; 	#VO_DATA0    muxctrl_reg63
	himm 0x12040100 0x1; 	#VO_DATA1    muxctrl_reg64
	himm 0x12040104 0x1; 	#VO_DATA2    muxctrl_reg65
	himm 0x12040108 0x1; 	#VO_DATA3    muxctrl_reg66
	himm 0x1204010C 0x1; 	#VO_DATA4    muxctrl_reg67
	himm 0x12040110 0x1; 	#VO_DATA5    muxctrl_reg68
	himm 0x12040114 0x1; 	#VO_DATA6    muxctrl_reg69
	himm 0x12040118 0x1; 	#VO_DATA7    muxctrl_reg70
	himm 0x1204013C 0x1; 	#VO_VS		 muxctrl_reg79
	himm 0x12040150 0x1; 	#VO_HS		 muxctrl_reg84
	himm 0x12040164 0x1;	#VO_DE		 muxctrl_reg89
	

	#drive capability
	#himm 0x12040904 0x020;  #VO_CLK
	himm 0x12040904 0x0A0;	#VO_CLK
	himm 0x12040908 0x040; 	#VO_DATA0 
	himm 0x1204090C 0x040; 	#VO_DATA1 
	himm 0x12040910 0x040; 	#VO_DATA2 
	himm 0x12040914 0x040; 	#VO_DATA3 
	himm 0x12040918 0x040; 	#VO_DATA4 
	himm 0x1204091C 0x040; 	#VO_DATA5 
	himm 0x12040920 0x040; 	#VO_DATA6 
	himm 0x12040924 0x040; 	#VO_DATA7 
	himm 0x12040948 0x1F0; 	#VO_VS
	himm 0x1204095C 0x1F0; 	#VO_HS
	himm 0x12040970 0x130;	#VO_DE
}


vo_bt1120_pinmux()
{	
	#pinmux
	himm 0x120400F8 0x1; 	#VO_CLK   
	himm 0x120400FC 0x1; 	#VO_DATA0 
	himm 0x12040100 0x1; 	#VO_DATA1 
	himm 0x12040104 0x1; 	#VO_DATA2 
	himm 0x12040108 0x1; 	#VO_DATA3 
	himm 0x1204010C 0x1; 	#VO_DATA4 
	himm 0x12040110 0x1; 	#VO_DATA5 
	himm 0x12040114 0x1; 	#VO_DATA6 
	himm 0x12040118 0x1; 	#VO_DATA7 
	himm 0x1204011C 0x1;	#VO_DATA8 
	himm 0x12040120 0x1;	#VO_DATA9 
	himm 0x12040124 0x1;	#VO_DATA10 
	himm 0x12040128 0x1;	#VO_DATA11
	himm 0x1204012C 0x1;	#VO_DATA12
	himm 0x12040130 0x1; 	#VO_DATA13
	himm 0x12040134 0x1; 	#VO_DATA14
	himm 0x12040138 0x1; 	#VO_DATA15
	
	#drive capability
	#himm 0x12040904 0x020;  #VO_CLK  
	himm 0x12040904 0x0A0;	#VO_CLK
	himm 0x12040908 0x040; 	#VO_DATA0 
	himm 0x1204090C 0x040; 	#VO_DATA1 
	himm 0x12040910 0x040; 	#VO_DATA2 
	himm 0x12040914 0x040; 	#VO_DATA3 
	himm 0x12040918 0x040; 	#VO_DATA4 
	himm 0x1204091C 0x040; 	#VO_DATA5 
	himm 0x12040920 0x040; 	#VO_DATA6 
	himm 0x12040924 0x040; 	#VO_DATA7 
	himm 0x12040928 0x040;	#VO_DATA8 
	himm 0x1204092C 0x040;	#VO_DATA9 
	himm 0x12040930 0x040;	#VO_DATA10
	himm 0x12040934 0x040;	#VO_DATA11
	himm 0x12040938 0x040;	#VO_DATA12
	himm 0x1204093C 0x040; 	#VO_DATA13
	himm 0x12040940 0x040; 	#VO_DATA14
	himm 0x12040944 0x040;	#VO_DATA15	
}

#rgmii
net_rgmii_pinmux()
{  
	#pinmux
	himm 0x1204013C 0x2;
	himm 0x12040140 0x2;
	himm 0x12040144 0x2;
	himm 0x12040148 0x2;
	himm 0x1204014C 0x2;
	himm 0x12040150 0x2;
	himm 0x12040154 0x2;
	himm 0x12040158 0x2;
	himm 0x1204015C 0x2;
	himm 0x1204015C 0x2;
	himm 0x12040164 0x2;
	himm 0x12040168 0x2;
	himm 0x1204016C 0x2;
	himm 0x12040170 0x2;
	himm 0x12040174 0x2;
	himm 0x12040178 0x2;

	#drive capability
	himm 0x12040948	0x0d0;
	himm 0x1204094C	0x0c0;
	himm 0x12040950	0x0d0;
	himm 0x12040954	0x0d0;
	himm 0x12040958	0x0d0;
	himm 0x1204095C	0x0d0;	
	himm 0x12040960 0x130;
	himm 0x12040964 0x130;
	himm 0x12040968 0x130;
	himm 0x1204096C 0x130;
	himm 0x12040970 0x130;
	himm 0x12040974 0x130;
	himm 0x12040978 0x130;
	himm 0x1204097C	0x050;
	himm 0x12040980	0x120;
	himm 0x12040984	0x120;	
}
    

i2s_pin_mux()
{
	#i2s pinmux
	himm 0x1204004c 0x2;		#I2S_BCLK_RX
	himm 0x12040050 0x2;		#I2S_WS_RX
	himm 0x12040074 0x2;		#I2S_SD_RX
	himm 0x12040068 0x2;		#I2S_MCLK
	himm 0x12040070 0x2;		#I2S_SD_TX
	himm 0x1204006c 0x2;		#I2S_BCLK_TX
	himm 0x12040064 0x2;		#I2S_WS_TX
	
	#i2s drive capability
	himm 0x1204084c 0x120;		#I2S_BCLK_RX
	himm 0x12040850 0x120;		#I2S_WS_RX
	himm 0x12040874 0x130;		#I2S_SD_RX
	himm 0x12040868 0x120;		#I2S_MCLK
	himm 0x12040870 0x120;		#I2S_SD_TX
	himm 0x1204086c 0x120;		#I2S_BCLK_TX
	himm 0x12040864 0x120;		#I2S_WS_TX
}

######################parse arg###################################
b_arg_vo=0
b_arg_net=0
b_arg_vi=0
vo_type="CVBS"
for arg in $@
do
	if [ $b_arg_vo -eq 1 ] ; then
		if [ $vo_type = "LCD" ] ; then
			lcd_type=$arg;
			break;
		else
			vo_type=$arg;
		fi
	fi

	case $arg in
		"-vo")
			b_arg_vo=1;
			;;
		"-net")
			b_arg_net=1;
			;;
		"-vi")
			b_arg_vi=1;
			;;
	esac
done
#######################parse arg end########################

#######################Action###############################
if [ $b_arg_vo -eq 1 ]; then
	case $vo_type in
		"BT1120")
			vo_bt1120_pinmux;
			i2c2_pin_mux;
			echo "===========VO TYPE BT1120===========";			
			;;
		"CVBS")
			echo "===========VO TYPE CVBS=============";
			;;
		"BT656")
			vo_lcd_bt656_6bit_8bit_pin;
			i2c3_pin_mux;
			rmmod hi_adv7179 &> /dev/null;              
			insmod extdrv/hi_adv7179.ko;                #PAL in default, 
			#insmod extdrv/hi_adv7179.ko norm_mode=1;   #NTSC 
			echo "===========VO TYPE BT656============";			
			;;
		"LCD")
			case $lcd_type in
				"9342-6bit")
					vo_lcd_bt656_6bit_8bit_pin;
					spi3_pin_mux;
					rmmod hi_ssp_ili9342h_6bit &> /dev/null;
					insmod extdrv/hi_ssp_ili9342h_6bit.ko;
					echo "============VO TYPE LCD 9342-6bit============="					
					;;
				"9341-6bit")
					vo_lcd_bt656_6bit_8bit_pin;
					spi3_pin_mux;
					rmmod hi_ssp_ili9341v_6bit &> /dev/null;
					insmod extdrv/hi_ssp_ili9341v_6bit.ko;
					echo "============VO TYPE LCD 9341-6bit============="					
					;;
				"9341-16bit")
					vo_lcd_16bit_24bit_mux;
					spi3_pin_mux;
					rmmod hi_ssp_ili9341v_16bit &> /dev/null;
					insmod extdrv/hi_ssp_ili9341v_16bit.ko;
					echo "============VO TYPE LCD 9341-16bit============="
					;;
				"5182-8bit")
					vo_lcd_bt656_6bit_8bit_pin;
					spi3_pin_mux;
					rmmod hi_ssp_ota5182 &> /dev/null;
					insmod extdrv/hi_ssp_ota5182.ko;
					echo "============VO TYPE LCD 5182-8bit============="
					;;
				"24bit")
					vo_lcd_16bit_24bit_mux;
					echo "============VO TYPE LCD 24bit============="
					;;
				*)
					echo "xxxx Invalid LCD type $lcd_type xxxx"
					report_error;
					;;
			esac
			;;
		*)
			echo "xxxx Invalid vo type $vo_type xxxx"
			report_error;
			;;
	esac
fi

if [ $b_arg_net -eq 1 ]; then
	net_rgmii_pinmux;
	echo "==============NET MODE================";
fi

if [ $b_arg_vi -eq 1 ]; then
	vicap_pin_mux;
	echo "==============VI MODE================";
fi

#i2c2_pin_mux;
#i2s_pin_mux;

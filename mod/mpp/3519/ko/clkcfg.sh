#!/bin/sh

# This is a sample, you should rewrite it according to your chip #
# clock will be closed at uboot, so you needn't call it!

clk_close()
{
	# Below clock operation is all from Hi3516A, you should modify by datasheet!
	echo "clock close operation done!"
}

# open module clock while you need it!
clk_cfg()
{
	himm 0x120100e4 0x1ff70000; # I2C0-3/SSP0-3 unreset, enable clk gate
	himm 0x1201003c 0x31000100;	# MIPI VI ISP unreset
	himm 0x12010050 0x2;		# VEDU0 unreset	
	himm 0x12010058 0x2;		# VPSS0 unreset 
	himm 0x12010058 0x3;		# VPSS0 unreset 
	himm 0x12010058 0x2;		# VPSS0 unreset 
	himm 0x1201005c 0x2;		# VGS unreset 
	himm 0x12010060 0x2;		# JPGE unreset 
	himm 0x12010064 0x2;		# TDE unreset 
	himm 0x1201006c 0x2;		# IVE unreset      
	himm 0x12010070 0x2;		# FD unreset
	himm 0x12010074 0x2;		# GDC unreset 
	himm 0x1201007C 0x2a;		# HASH&SAR ADC&CIPHER unreset   
	himm 0x12010080 0x2;		# AIAO unreset,clock 1188M
	himm 0x12010084 0x2;		# GZIP unreset  
	himm 0x120100d8 0xa;    	# ddrt efuse enable clock, unreset
	himm 0x120100e0 0xa8;   	# rsa trng klad enable clock, unreset
	#himm 0x120100e0 0xaa       # rsa trng klad DMA enable clock, unreset
	himm 0x12010040 0x60;
	himm 0x12010040 0x0;		# sensor unreset,unreset the control module with slave-mode

	
	#VDP_OUT 
	
	#himm 0x12010044 0x00015ff4	# D1@30fps,BT656 CVBS
	
	#himm 0x12010044 0x00004ff0  # 1080p30 BT1120
	
	
	#                 IVE[21:19] GDC[18:16] VGS[15:13] VEDU [12:10] VPSS0[7:5] VI0[2:0]
	# SDK config:     IVE:396M,  GDC:475M,  VGS:500M,  VEDU:600M,   VPSS:300M  VI0:300M---0x00494841
	#himm 0x1201004c 0x00094c21;
	#                 ISP0 [18:14] ISP1[10:6] VI1[2:0]
	# SDK config:     ISP0:300M,   ISP1:300M, VI1:300M
	#himm 0x12010054 0x00004041;

	# configure with different sensor type
	#himm 0x12010040 0x11;   #226  8 lane sensor clock 72M


	# pcie clk enable
	himm 0x120100b0 0x000001f0

	echo "clock configure operation done!"
}

#clk_close
clk_cfg

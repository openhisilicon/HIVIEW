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
	himm 0x20030030 0x00004005      # AVC-300M VGS-300M VPSS-250M VEDU-300M mda1axi 250M mda0axi 300M DDR-250
    himm 0x20030104 0x3             # VICAP-198M VPSS-198M
	himm 0x2003002c 0x90007         # VICAP-250M, ISP unreset & clk en, Sensor clk en-37.125M, clk reverse
	#himm 0x20030034 0xffc           # VDP-1080p@60fps unreset & clk en
	#himm 0x20030034 0xef74          #VDP-PAL/NTSC 
	himm 0x20030040 0x2002          # VEDU0 AVC unreset & clk en
	himm 0x20030048 0x2             # VPSS0 unreset & clk en

	himm 0x20030058 0x2             # TDE unreset & clk en
	himm 0x2003005c 0x2             # VGS unreset & clk en
	himm 0x20030060 0x2             # JPGE  unreset & clk en

	himm 0x20030068 0x2             # MDU unreset & clk en
	himm 0x2003006c 0x2             # IVE-300MHz unreset & clk en
	#himm 0x20030070 0x2            # VOIE unreset & clk en

	himm 0x2003007c 0x2             # cipher unreset & clk en
	himm 0x2003008c 0xe             # aio MCLK PLL 1188M, unreset & clk en
	#himm 0x200300d8 0xa;           # ddrt

	# USB not set
	# SDIO not set
	# SFC not set 
	# NAND not set
	# RTC use external clk
	# PWM not set
	# DMAC not set
	# SPI not set
	# I2C not set
	# SENSE CLK not set
	# WDG not set

	echo "clock configure operation done!"
}

#clk_close
clk_cfg

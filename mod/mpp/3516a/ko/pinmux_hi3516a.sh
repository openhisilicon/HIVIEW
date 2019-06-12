#!/bin/sh

# This is a sample, you should rewrite it according to your chip #

# You can configure your pinmux for the application here!

#VICAP default setting is VIU
vicap_pin_mux()
{
	himm 0x200f0000 0x1		# 0: GPIO0_5, 		1: SENSOR_CLK
	himm 0x200f0004 0x1		# 1£ºFLASH_TRIG,	0: GPIO0_6,		2£ºSPI1_CSN1
	himm 0x200f0008 0x1		# 1£ºSHUTTER_TRIG,	0£ºGPIO0_7,		2£ºSPI1_CSN2
}

i2c_pin_mux()
{
	himm 0x200f0070 0x1;		# i2c2_sda
	himm 0x200f0074 0x1;		# i2c2_scl
}

#BT1120
vo_bt1120_mode()
{
	himm 0x200f000c 0x1;	# vo clk
	himm 0x200f0010 0x1;	# vo DAT
	himm 0x200f0014 0x1;	# vo DAT
	himm 0x200f0018 0x1;	# vo DAT
	himm 0x200f001c 0x1;	# vo DAT
	himm 0x200f0020 0x1;	# vo DAT
	himm 0x200f0024 0x1;	# vo DAT
	himm 0x200f0028 0x1;	# vo DAT
	himm 0x200f002c 0x1;	# vo DAT
	himm 0x200f0030 0x1;	# vo DAT
	himm 0x200f0034 0x1;	# vo DAT
	himm 0x200f0038 0x1;	# vo DAT
	himm 0x200f003c 0x1;	# vo DAT
	himm 0x200f0040 0x1;	# vo DAT
	himm 0x200f0044 0x1;	# vo DAT
	himm 0x200f0048 0x1;	# vo DAT
	himm 0x200f004c 0x1;	# vo DAT
}

# just for test
jtag_pin_mux()
{
	himm 0x200f0090 0x1;	# JTAG_TRSTN
	himm 0x200f0094 0x1;	# JTAG_TCK
	himm 0x200f0098 0x1;	# JTAG_TMS
	himm 0x200f009c 0x1;	# JTAG_TDO
	himm 0x200f00a0 0x1;	# JTAG_TDI
}

# TEMPER_DQ
temper_pin_mux()
{
	himm 0x200f00d8 0x0;	# 2£ºTEMPER_DQ
	himm 0x200f00dc 0x0;	# 2£ºTEMPER_DQ
	himm 0x200f00e0 0x0;	# 2£ºTEMPER_DQ
	himm 0x200f00e4 0x0;	# 2£ºTEMPER_DQ
}

i2s_pin_mux()
{
	himm 0x200f0088 0x2;	# i2s0_bclk_rx
	himm 0x200f008c 0x2;	# i2s0_ws_rx
	himm 0x200f0090 0x2;	# i2s0_mclk
	himm 0x200f0094 0x2;	# i2s0_bclk_tx
	himm 0x200f0098 0x2;	# i2s0_ws_tx
	himm 0x200f009c 0x2;	# i2s0_sd_rx
	himm 0x200f00a0 0x2;	# i2s0_sd_tx

	himm 0x200f00b0 0x2;	# i2s1_mclk
	himm 0x200f00b4 0x2;	# i2s1_bclk_tx
	himm 0x200f00b8 0x2;	# i2s1_ws_tx
	himm 0x200f00bc 0x2;	# i2s1_sd_tx
	himm 0x200f00c0 0x2;	# i2s1_bclk_rx
	himm 0x200f00c4 0x2;	# i2s1_ws_rx
	himm 0x200f00c8 0x2;	# i2s1_sd_rx

	himm 0x200f01ac 0x2;	# i2s2_mclk
	himm 0x200f01b0 0x2;	# i2s2_bclk_tx
	himm 0x200f01b4 0x2;	# i2s2_ws_tx
	himm 0x200f01b8 0x2;	# i2s2_sd_tx
	himm 0x200f01bc 0x2;	# i2s2_bclk_rx
	himm 0x200f01c0 0x2;	# i2s2_ws_rx
	himm 0x200f01c4 0x2;	# i2s2_sd_rx
}

i2c_pin_mux;
vicap_pin_mux;
#i2s_pin_mux;
vo_bt1120_mode;

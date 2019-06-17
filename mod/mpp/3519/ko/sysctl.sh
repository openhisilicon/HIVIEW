#!/bin/sh

# This is a sample, you should rewrite it according to your chip #

# mddrc pri&timeout setting

#########################################################################################
# param $1=1 --- online
# param $1=0 --- offline
vi_vpss_online_config()
{
	# -------------vi vpss online open
	if [ $b_vpss_online -eq 1 ]; then
		echo "==============vi_vpss_online==============";
		himm 0x12030000 0x00000204
		
		# write priority select
		himm 0x12030054 0x55552356     # each module 4bit  cci       ---        ddrt  ---    ---     gzip   ---    ---
		himm 0x12030058 0x16554411     # each module 4bit  vicap1    hash       ive   aio    jpge    tde   vicap0  vdp 
		himm 0x1203005c 0x33466314     # each module 4bit  mmc2      A17        fmc   sdio1  sdio0   A7    vpss0   vgs 
		himm 0x12030060 0x46266666     # each module 4bit  gdc       usb3/pcie  vedu  usb2   cipher  dma2  dma1    gsf

		# read priority select
		himm 0x12030064 0x55552356 	# each module 4bit  cci       ---         ddrt  ---    ---     gzip   ---    ---
		himm 0x12030068 0x06554401 	# each module 4bit  vicap1    hash        ive   aio    jpge    tde   vicap0  vdp
		himm 0x1203006c 0x33466304 	# each module 4bit  mmc2      A17         fmc   sdio1  sdio0    A7   vpss0   vgs 
		himm 0x12030070 0x46266666 	# each module 4bit  gdc       usb3/pcie   vedu  usb2   cipher  dma2  dma1    gsf

		himm 0x120641f0 0x1			# use pri_map
		# write timeout select
		himm 0x1206409c 0x00000040	#
		himm 0x120640a0 0x00000000	#
		
		#read timeout select
		himm 0x120640ac 0x00000040	#
		himm 0x120640b0 0x00000000	#
	else
		echo "==============vi_vpss_offline==============";
		himm 0x12030000 0x00000004

		# write priority select
		himm 0x12030054 0x55552366     # each module 4bit  cci       ---        ddrt  ---    ---     gzip   ---    ---
		himm 0x12030058 0x16556611     # each module 4bit  vicap1    hash       ive   aio    jpge    tde   vicap0  vdp
		himm 0x1203005c 0x43466445     # each module 4bit  mmc2      A17        fmc   sdio1  sdio0   A7    vpss0   vgs
		himm 0x12030060 0x56466666     # each module 4bit  gdc       usb3/pcie  vedu  usb2   cipher  dma2  dma1    gsf

		# read priority select
		himm 0x12030064 0x55552366 	# each module 4bit  cci       ---        ddrt  ---    ---     gzip   ---    ---
		himm 0x12030068 0x06556600 	# each module 4bit  vicap1    hash       ive   aio    jpge    tde   vicap0  vdp
		himm 0x1203006c 0x43466435 	# each module 4bit  mmc2      A17        fmc   sdio1  sdio0   A7    vpss0   vgs
		himm 0x12030070 0x56266666 	# each module 4bit  gdc       usb3/pcie  vedu  usb2   cipher  dma2  dma1    gsf

		himm 0x120641f0 0x1			# use pri_map
		# write timeout select
		himm 0x1206409c 0x00000040	# 
		himm 0x120640a0 0x00000000	# 
		
		# read timeout select
		himm 0x120640ac 0x00000040	# each module 8bit
		himm 0x120640b0 0x00000000	#
	fi
}
#########################################################################################


himm  0x120300e0 0xd				# internal codec: AIO MCLK out, CODEC AIO TX MCLK 
#himm 0x120300e0 0xe				# external codec: AIC31 AIO MCLK out, CODEC AIO TX MCLK

# mddrc pri&timeout setting
#himm 0x12060204 0x76543210     # ports0         
#himm 0x12060214 0x76543210     # ports1         
#himm 0x12060224 0x76543210     # ports2
#himm 0x12060234 0x76543210     # ports3
#himm 0x12060244 0x76543210     # ports4
#himm 0x12060254 0x76543210     # ports5
#himm 0x12060264 0x76543210     # ports6
#
##read command priority                 
#himm 0x12060208 0x76543210     # ports0         
#himm 0x12060218 0x76543210     # ports1         
#himm 0x12060228 0x76543210     # ports2
#himm 0x12060238 0x76543210     # ports3
#himm 0x12060248 0x76543210     # ports4
#himm 0x12060258 0x76543210     # ports5
#himm 0x12060268 0x76543210     # ports6


echo "++++++++++++++++++++++++++++++++++++++++++++++"
b_vpss_online=1

if [ $# -ge 1 ]; then
    b_vpss_online=$1
fi

vi_vpss_online_config;

#ive utili 
#himm 0x11040000	0x2		#Open utili statistic
#himm 0x11040080 0x11E1A300  	#Utili peri,default 0x11E1A300 cycle


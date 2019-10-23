/******************************************************************************
 Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : hi_defines.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2005/4/23
Last Modified :
Description   : The common data type defination
Function List :
History       :
******************************************************************************/
#ifndef __HI_DEFINES_H__
#define __HI_DEFINES_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
#define VENC_MAX_SSE_NUM             8
#define VENC_QP_HISGRM_NUM           52

#define HI3536_V100  0x35360100
#define HI3536C_V100 0x3536c100
#define HI3521A_V100 0x3521a100
#define HI3520D_V300 0x3520d300
#define HI3531A_V100 0x3531a100
#define HI35xx_Vxxx  0x35000000
#define HI3519_V100  0x35190100
#define HI3519_V101  0x35190101
#define HI3516C_V300 0x35160300
#define HI3536D_V100 0x3536d100

#ifndef HICHIP
    #define HICHIP HI3536D_V100
#endif

#if HICHIP==HI3536_V100
    #define CHIP_NAME    "Hi3536"
    #define MPP_VER_PRIX "_MPP_V"
#elif HICHIP==HI3536C_V100
    #define CHIP_NAME    "Hi3536C"
    #define MPP_VER_PRIX "_MPP_V"
#elif HICHIP==HI3536D_V100
    #define CHIP_NAME    "Hi3536D"
    #define MPP_VER_PRIX "_MPP_V"
#elif HICHIP==HI3521A_V100
    #define CHIP_NAME    "Hi3521A"
    #define MPP_VER_PRIX "_MPP_V"
#elif HICHIP==HI3531A_V100
    #define CHIP_NAME    "Hi3531A"
    #define MPP_VER_PRIX "_MPP_V"
#else
    #error HICHIP define may be error
#endif

#define LINE_LEN_BIT            5
#define LINE_LEN                (1<<LINE_LEN_BIT)
#define LINE_BASE_MASK          (~(LINE_LEN-1))
static inline void InvalidateDcache(unsigned long addr, unsigned long len)
{
    unsigned long end;
    //TODO: cache need rewrite when flush
    return ;

    addr &= LINE_BASE_MASK;
    len >>= LINE_LEN_BIT;
    end   = addr + len*LINE_LEN;

    while(addr != end)
    {
        asm("mcr p15, 0, %0, c7, c6, 1"::"r"(addr));
        addr += LINE_LEN;
    }
    return;
}

static inline  void FlushDcache(unsigned long addr, unsigned long len)
{
    unsigned long end;

    //TODO: cache need rewrite when flush
    return ;

    addr &= LINE_BASE_MASK;
    len >>= LINE_LEN_BIT;
    end   = addr + len*LINE_LEN;

    while(addr != end)
    {
        asm("mcr p15, 0, %0, c7, c10, 1"::"r"(addr));
        addr += LINE_LEN;
    }
    return;
}

#define DEFAULT_ALIGN           16
#define MAX_MMZ_NAME_LEN        16

#define MAX_NODE_NUM            16

/* For VDA */
#define VDA_MAX_NODE_NUM        32
#define VDA_MAX_INTERNAL        256
#define VDA_CHN_NUM_MAX         128
#define VDA_MAX_WIDTH           960
#define VDA_MAX_HEIGHT          960
#define VDA_MIN_WIDTH           32
#define VDA_MIN_HEIGHT          32


/* For VENC */
#define VENC_MAX_NAME_LEN       16
#define VENC_MAX_CHN_NUM        16
#define VENC_MAX_GRP_NUM        16
#define H264E_MAX_WIDTH         4096
#define H264E_MAX_HEIGHT        4096
#define H264E_MIN_WIDTH         256
#define H264E_MIN_HEIGHT        128
#define H265E_MAX_WIDTH         4096
#define H265E_MAX_HEIGHT        4096
#define H265E_MIN_WIDTH         128
#define H265E_MIN_HEIGHT        128
#define JPEGE_MAX_WIDTH         8192
#define JPEGE_MAX_HEIGHT        8192
#define JPEGE_MIN_WIDTH         32
#define JPEGE_MIN_HEIGHT        32
#define VENC_MAX_ROI_NUM        8               /* max roi region */
#define H264E_MIN_HW_INDEX      0
#define H264E_MAX_HW_INDEX      11
#define H264E_MIN_VW_INDEX      0
#define H264E_MAX_VW_INDEX      5
#define MPEG4E_MAX_HW_INDEX     1
#define MPEG4E_MAX_VW_INDEX     0

#define VENC_MAX_SSE_NUM        8
#define VENC_QP_HISGRM_NUM      52

/*For RC*/
#define RC_TEXTURE_THR_SIZE     16
#define RC_RQRATIO_SIZE         8



/* For VDEC */
#define VDEC_MAX_COMPRESS_WIDTH 2048
#define VDEC_MAX_CHN_NUM        32
#define VDH_VEDU_CHN		    32
#define VEDU_MAX_CHN_NUM	    0
#define MAX_VDEC_CHN            VDEC_MAX_CHN_NUM
#define BUF_RESERVE_LENTH       64          /*reserve 64byte for hardware*/
#define H264D_MAX_SLICENUM	    137
#define VEDU_H264D_ERRRATE	    10
#define VEDU_H264D_FULLERR      100
#define H265_WIDTH_ALIGN        64
#define H265_HEIGHT_ALIGN       64
#define JPEG_WIDTH_ALIGN        64
#define H264_WIDTH_ALIGN        16
#define H264_HEIGHT_ALIGN       32


/* for VDH-H264  */
#define VDH_H264D_MAX_WIDTH       3072
#define VDH_H264D_MAX_HEIGHT      3072
#define VDH_H264D_MIN_WIDTH       128
#define VDH_H264D_MIN_HEIGHT      128
#define VDH_H264D_MAX_SPS         32
#define VDH_H264D_MIN_SPS         1
#define VDH_H264D_MAX_PPS         256
#define VDH_H264D_MIN_PPS         1
#define VDH_H264D_MAX_SLICE       136
#define VDH_H264D_MIN_SLICE       1

/* for VDH-HEVC  */
#define VDH_H265D_MAX_WIDTH       3072
#define VDH_H265D_MAX_HEIGHT      3072
#define VDH_H265D_MIN_WIDTH       128
#define VDH_H265D_MIN_HEIGHT      128
#define VDH_H265D_MAX_VPS         16
#define VDH_H265D_MIN_VPS         1
#define VDH_H265D_MAX_SPS         16
#define VDH_H265D_MIN_SPS         1
#define VDH_H265D_MAX_PPS         64
#define VDH_H265D_MIN_PPS         1
#define VDH_H265D_MAX_SLICE       200
#define VDH_H265D_MIN_SLICE       1


/* for VDH-MPEG4 */
#define VDH_MPEG4D_MAX_WIDTH       3072
#define VDH_MPEG4D_MAX_HEIGHT      3072
#define VDH_MPEG4D_MIN_WIDTH       128
#define VDH_MPEG4D_MIN_HEIGHT      128


/* for VEDU-H264  */
#define VEDU_H264D_MAX_WIDTH       4096
#define VEDU_H264D_MAX_HEIGHT      4096
#define VEDU_H264D_MIN_WIDTH       80
#define VEDU_H264D_MIN_HEIGHT      64



/* for JPEG  */
#define JPEGD_MAX_WIDTH             8192
#define JPEGD_MAX_HEIGHT            8192
#define JPEGD_MIN_WIDTH             8
#define JPEGD_MIN_HEIGHT            8

/* for VPSS */
#define VPSS_MIN_WIDTH              64
#define VPSS_MIN_HEIGHT             64
#define VPSS_MAX_WIDTH              4096
#define VPSS_MAX_HEIGHT             4096
#define VPSS_MAX_DEI_WIDTH          2048
#define VPSS_MAX_DECMP_WIDTH        2048
#define VPSS_MAX_PRESCALE_WIDTH     4096
#define VPSS_MAX_GRP_NUM            32
/* VPSS send onlinetask to VGS group number */
#define VPSS_VGS_GRP_NO             0
#define VPSS_MAX_PHY_CHN_NUM        1
#define VPSS_MAX_EXT_CHN_NUM        0
#define VPSS_MAX_CHN_NUM            (VPSS_MAX_PHY_CHN_NUM + VPSS_MAX_EXT_CHN_NUM)
#define VPSS_CHN0                   0
#define VPSS_CHN1                   1
#define VPSS_CHN2                   2
#define VPSS_CHN3                   3
#define VPSS_INVALID_CHN            -1
#define VPSS_LUMA_STAT_NUM          64
#define VPSS_MAX_SPLIT_NODE_NUM     3

/*for VGS*/
#define VGS_MAX_COVER_W             COVEREX_MAX_WIDTH
#define VGS_MAX_COVER_H             COVEREX_MAX_HEIGHT
#define VGS_MIN_COVER_W             2
#define VGS_MIN_COVER_H             2
#define VGS_MAX_OSD_W               OVERLAYEX_MAX_WIDTH
#define VGS_MAX_OSD_H               OVERLAYEX_MAX_HEIGHT
#define VGS_MIN_OSD_W               2
#define VGS_MIN_OSD_H               2
#define VGS_MAX_LINE_POS            4096

#define OVERLAYEX_MAX_X             4096
#define OVERLAYEX_MAX_Y             4096
#define OVERLAYEX_MAX_WIDTH         4096
#define OVERLAYEX_MAX_HEIGHT        4096
#define OVERLAYEX_MAX_FGALPHA       255
#define OVERLAYEX_MAX_BGALPHA       255

#define COVEREX_MAX_X               4096
#define COVEREX_MAX_Y               4096
#define COVEREX_MAX_WIDTH           4096
#define COVEREX_MAX_HEIGHT          4096
#define COVEREX_MAX_XPOINT          4096
#define COVEREX_MAX_YPOINT          4096

#define LINE_MAX_X					4096
#define LINE_MAX_Y                  4096
#define LINEWIDTH_MAX               7

/* For PCIV */
#define PCIV_MAX_CHN_NUM            128       /* max pciv channel number in each pciv device */

/* for REGION */
#define RGN_HANDLE_MAX              1024
#define OVERLAY_MAX_NUM_VENC        8
#define OVERLAY_MAX_NUM_VPSS        8
#define OVERLAYEX_MAX_NUM_PCIV      1
#define COVER_MAX_NUM_VPSS          4
#define LINE_MAX_NUM_VPSS           4
#define MASAIC_MAX_NUM_VPSS         4
#define OVERLAYEX_MAX_NUM_VPSS      8
#define COVEREX_MAX_NUM_VPSS        4
#define COVEREX_MAX_NUM_VO          4
#define OVERLAYEX_MAX_NUM_VO        1
#define LINE_MAX_NUM_VO             4

/* for VENC */
#define OVERLAY_MAX_X_VENC           8190
#define OVERLAY_MAX_Y_VENC           8190
#define OVERLAY_MAX_WIDTH_VENC       8190
#define OVERLAY_MAX_HEIGHT_VENC      8190
#define OVERLAY_MAX_FGALPHA_VENC     128
#define OVERLAY_MAX_BGALPHA_VENC     128
#define OVERLAY_MAX_QPABS_VENC       51
#define OVERLAY_MIN_QPABS_VENC       0
#define OVERLAY_MAX_QPREL_VENC       51
#define OVERLAY_MIN_QPREL_VENC       (-51)
#define RGN_OVERLAY_MAX_WIDTH        OVERLAY_MAX_WIDTH_VENC
#define RGN_OVERLAY_MAX_HEIGHT       OVERLAY_MAX_HEIGHT_VENC
#define OVERLAYEX_MAX_NUM_VENC       8
#define OVERLAYEX_MAX_X_VENC         OVERLAYEX_MAX_X
#define OVERLAYEX_MAX_Y_VENC         OVERLAYEX_MAX_Y
#define RGN_OVERLAYEX_MAX_WIDTH      OVERLAYEX_MAX_WIDTH
#define RGN_OVERLAYEX_MAX_HEIGHT     OVERLAYEX_MAX_HEIGHT



/* number of channle and device on video input unit of chip
 * Note! VIU_MAX_CHN_NUM is NOT equal to VIU_MAX_DEV_NUM
 * multiplied by VIU_MAX_CHN_NUM, because all VI devices
 * can't work at mode of 4 channles at the same time.
 */
#define VIU_MAX_DEV_NUM               4
#define VIU_MAX_WAY_NUM_PER_DEV       4
#define VIU_MAX_CHN_NUM_PER_DEV       4
#define VIU_MAX_PHYCHN_NUM            (16+0)

#define VIU_EXT_CHN_START             (VIU_MAX_PHYCHN_NUM + VIU_MAX_CAS_CHN_NUM)
#define VIU_MAX_EXT_CHN_NUM           0
#define VIU_MAX_EXTCHN_BIND_PER_CHN   0

#define VIU_MAX_CAS_CHN_NUM           0
#define VIU_CAS_CHN_START             VIU_MAX_PHYCHN_NUM

#define VIU_MAX_CHN_NUM               (VIU_MAX_PHYCHN_NUM + VIU_MAX_CAS_CHN_NUM + VIU_MAX_EXT_CHN_NUM)
#define VIU_CHNID_DEV_FACTOR          2

/*get the subchn index by main chn */
#define  SUBCHN(ViChn)              (ViChn + 16)

/* define cascade chn */
#define VI_CAS_CHN_1                33
#define VI_CAS_CHN_2                34

#define COVER_MAX_NUM_VI            4
#define COVEREX_MAX_NUM_VI          0
#define OVERLAY_MAX_NUM_VI          0
#define OVERLAYEX_MAX_NUM_VI        0

/* max number of VBI region*/
#define VIU_MAX_VBI_NUM             0
/* max length of one VBI region (by word)*/
#define VIU_MAX_VBI_LEN             0x20


/* For VOU */
#define VO_MAX_DEV_NUM              1       /* max dev num, not vir dev */
#define VO_MAX_LAYER_NUM            2       /* max layer num, not vir dev */
#define VHD_MAX_CHN_NUM             16      /* max VHD chn num */
#define VO_MAX_CHN_NUM              VHD_MAX_CHN_NUM      /* max chn num */
#define VO_MAX_LAYER_IN_DEV         2       /* max layer num of each dev */

#define VO_MAX_CAS_DEV_NUM      2       /* max cascade dev num*/
#define VO_CAS_DEV_1            7       /* cascade display device 1 */
#define VO_CAS_DEV_2            8       /* cascade display device 2 */
#define VO_CAS_MAX_PAT          128     /* cascade pattern max number */
#define VO_CAS_MAX_POS_32RGN    32      /* cascade position max number */
#define VO_CAS_MAX_POS_64RGN    64      /* cascade position max number */

#define VO_MAX_VIRT_DEV_NUM     4       /* max virtual dev num*/
#define VO_VIRT_DEV_0           2       /* virtual display device 1 */
#define VO_VIRT_DEV_1           3       /* virtual display device 2 */
#define VO_VIRT_DEV_2           4       /* virtual display device 3 */
#define VO_VIRT_DEV_3           5       /* virtual display device 4 */

#define VO_MAX_GFX_LAYER_PER_DEV    2
#define VO_MAX_GRAPHICS_LAYER_NUM   2
#define MDDRC_ZONE_MAX_NUM          32

#define VO_MAX_WBC_NUM              1
#define VO_MAX_PRIORITY             2
/* the startup condition of video compress */
#define VIDEO_COMPRESS_WIDTH_THRESHOLD     352

#define VO_MIN_TOLERATE                 1       /* min play toleration 1ms */
#define VO_MAX_TOLERATE                 100000  /* max play toleration 100s */
//3531A
#define VO_MIN_CHN_WIDTH                32      /* channel minimal width */
#define VO_MIN_CHN_HEIGHT               32      /* channel minimal height */
#define VO_MAX_ZOOM_RATIO               1000    /* max zoom ratio, 1000 means 100% scale */


/* For AUDIO */
#define AI_DEV_MAX_NUM       1

#define AO_DEV_MIN_NUM       0
#define AO_DEV_MAX_NUM       2
#define AIO_MAX_NUM          2
#define AIO_MAX_CHN_NUM      2

#define AENC_MAX_CHN_NUM     32
#define ADEC_MAX_CHN_NUM     32

#define RGN_MIN_WIDTH           2
#define RGN_MIN_HEIGHT          2
#define RGN_MAX_WIDTH 	        8192
#define RGN_MAX_HEIGHT 	        8192
#define RGN_ALIGN               2

/* VB size calculate for compressed frame.
	[param input]
		w: 	width
		h: 	height
		fmt:	pixel format, 0: SP420, 1: SP422
		z:	compress mode, 0: no compress, 1: default compress
	[param output]
		size: vb blk size

 */
#define VB_W_ALIGN		16
#define VB_H_ALIGN		32
#define VB_ALIGN(x, a)	((a) * (((x) + (a) - 1) / (a)))



/* VB size calculate for compressed frame.
	[param input]
		w: 	width
		h: 	height
		fmt:	pixel format, 0: SP420, 1: SP422
		z:	compress mode, 0: no compress, 1: default compress
	[param output]
		size: vb blk size
 */
#define VB_HEADER_STRIDE    16
#define VB_ALIGN_LEN        256    

#define VB_PIC_Y_HEADER_SIZE(Width, Height, size)\
	do{\
            size = VB_HEADER_STRIDE * (Height);\
	}while(0)

#define VB_PIC_HEADER_SIZE(Width, Height, Type, size)\
	do{\
            if (PIXEL_FORMAT_YUV_SEMIPLANAR_422 == Type)\
            {\
		        size = VB_HEADER_STRIDE * (Height) * 2;\
            }\
            else\
            {\
                size = (VB_HEADER_STRIDE * (Height) * 3) >> 1;\
            }\
	}while(0)

#define VB_PIC_BLK_SIZE(Width, Height, Type, size)\
	do{\
			unsigned int u32AlignWidth;\
			unsigned int u32AlignHeight;\
			unsigned int u32HeaderSize;\
			if (Type==PT_H264 || Type==PT_MP4VIDEO  || Type==PT_MPEG2VIDEO)\
			{\
			    u32AlignWidth = VB_ALIGN(Width,64);\
			    u32AlignHeight= VB_ALIGN(Height,16);\
                u32HeaderSize = (VB_ALIGN(u32AlignWidth,2048)>>7) * (VB_ALIGN(u32AlignHeight,16)>>1);\
                size = (((u32AlignWidth * u32AlignHeight + u32HeaderSize) * 3) >> 1);\
            }\
			else if (Type==PT_H265)\
			{\
			    u32AlignWidth = VB_ALIGN(Width,64);\
			    u32AlignHeight= VB_ALIGN(Height,64);\
                u32HeaderSize = (VB_ALIGN(u32AlignWidth,2048)>>7) * (VB_ALIGN(u32AlignHeight,64)>>1);\
                size = (( (u32AlignWidth * u32AlignHeight + u32HeaderSize) * 3) >> 1);\
			}\
			else\
			{\
			    u32AlignWidth = VB_ALIGN(Width,64);\
			    u32AlignHeight= VB_ALIGN(Height,16);\
	            size = (u32AlignWidth * u32AlignHeight * 3) >> 1;\
			}\
			size = VB_ALIGN(size, VB_ALIGN_LEN); \
	}while(0)

#define VB_PMV_BLK_SIZE(Width, Height, enType, size)\
    do{\
            unsigned int WidthInMb, HeightInMb;\
            unsigned int ColMbSize;\
            WidthInMb  = (enType == PT_H265) ? ((Width   + 63) >> 4) : ((Width   + 15) >> 4);\
            HeightInMb = (enType == PT_H265) ? ((Height  + 63) >> 4) : ((Height  + 15) >> 4);\
            ColMbSize  = (enType == PT_H265) ? (4*4) : (16*4);\
            size       = VB_ALIGN((ColMbSize * WidthInMb * HeightInMb), VB_ALIGN_LEN);\
    }while(0)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_DEFINES_H__ */


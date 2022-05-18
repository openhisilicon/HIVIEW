/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2011-2019. All rights reserved.
* Description:
* Author: Hisilicon multimedia software group
* Create: 2011/06/28
*/

#ifndef __HI_DEFINES_H__
#define __HI_DEFINES_H__

#include "autoconf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#ifndef HICHIP
#define HICHIP                          CONFIG_HI_SUBCHIP_TYPE
#endif

#define HI3531D_V200                    0x3531D200
#define HI3521D_V200                    0x3521D200
#define HI3535A_V100                    0x3535A100

#define HI35xx_Vxxx                     0x35000000

#if HICHIP == HI3531D_V200
#define HI_CHIP_NAME                       "Hi3531DV200"
#else
#define HI_CHIP_NAME                       "Hi3535AV100"
#endif

#define HI_MAX_BIND_DST_NUM         64
#define HI_MPP_VER_PRIX                    "_MPP_V"

#define HI_MAX_COLOR_COMPONENT             2
#define HI_ALIGN_NUM                       8
#define HI_MAX_PRIVATE_DATA_NUM            1
#define HI_MAX_USER_DATA_NUM               2
#define HI_LUMA_PHYS_ALIGN                  32

#define HI_DEFAULT_ALIGN                   16
#define HI_MAX_ALIGN                       1024
#define HI_SEG_COMPRESS_LEN                256

#define HI_MAX_MMZ_NAME_LEN                32

#define HI_QUAD_POINT_NUM                  4

/* For VB */
#define HI_VB_MAX_POOLS                  768

/* For VENC */
#define HI_VENC_VEDU_IP_NUM              3
#define HI_VENC_JPGE_IP_NUM              1
#define HI_VENC_MAX_TILE_NUM             1
#define HI_VENC_MAX_CHN_NUM              128
#define HI_VENC_H264_MAX_WIDTH           4096
#define HI_VENC_H264_MAX_HEIGHT          4096
#define HI_VENC_H264_MIN_WIDTH           114
#define HI_VENC_H264_MIN_HEIGHT          114
#define HI_VENC_H265_MAX_WIDTH           4096
#define HI_VENC_H265_MAX_HEIGHT          4096
#define HI_VENC_H265_MIN_WIDTH           114
#define HI_VENC_H265_MIN_HEIGHT          114
#define HI_VENC_JPEG_MAX_WIDTH           16384
#define HI_VENC_JPEG_MAX_HEIGHT          16384
#define HI_VENC_JPEG_MIN_WIDTH           32
#define HI_VENC_JPEG_MIN_HEIGHT          32
#define HI_VENC_MAX_INPUT_FRAME_RATE     240
#define HI_VENC_MIN_INPUT_FRAME_RATE     1
#define HI_VENC_MAX_RECV_SRC             4
#define HI_VENC_H265_ADAPTIVE_FRAME_TYPE 4
#define HI_VENC_H265_ADAPTIVE_QP_TYPE    5
#define HI_VENC_H265E_LCU_SIZE           32

/* For RC */
#define HI_VENC_MAX_BITRATE              (60 * 1024)
#define HI_VENC_MIN_BITRATE              2

/* For VDEC */
#define HI_VDEC_MAX_CHN_NUM                128
#define HI_VDH_MAX_CHN_NUM                 128
#define HI_VEDU_CHN_START                  HI_VDH_MAX_CHN_NUM
#define HI_VEDU_H264D_ERRRATE              10
#define HI_VEDU_H264D_FULLERR              100

#define HI_H264D_ALIGN_W                   64
#define HI_H264D_ALIGN_H                   16
#define HI_H265D_ALIGN_W                   64
#define HI_H265D_ALIGN_H                   64
#define HI_JPEGD_ALIGN_W                   64
#define HI_JPEGD_ALIGN_H                   16
#define HI_JPEGD_RGB_ALIGN                 16

#define HI_H264D_MAX_SPS                   32
#define HI_H264D_MIN_SPS                   1
#define HI_H264D_MAX_PPS                   256
#define HI_H264D_MIN_PPS                   1
#define HI_H264D_MAX_SLICE                 300
#define HI_H264D_MIN_SLICE                 1

#define HI_H265D_MAX_VPS                   16
#define HI_H265D_MIN_VPS                   1
#define HI_H265D_MAX_SPS                   16
#define HI_H265D_MIN_SPS                   1
#define HI_H265D_MAX_PPS                   64
#define HI_H265D_MIN_PPS                   1
#define HI_H265D_MAX_SLICE                 600
#define HI_H265D_MIN_SLICE                 1

#define HI_VDH_H264D_MAX_WIDTH             8192
#define HI_VDH_H264D_MAX_HEIGHT            8192
#define HI_VDH_H264D_MIN_WIDTH             96
#define HI_VDH_H264D_MIN_HEIGHT            96
#define HI_VDH_H265D_MAX_WIDTH             8192
#define HI_VDH_H265D_MAX_HEIGHT            8192
#define HI_VDH_H265D_MIN_WIDTH             96
#define HI_VDH_H265D_MIN_HEIGHT            96
#define HI_VDH_MIN_MSG_NUM                 1
#define HI_VDH_MAX_MSG_NUM                 8

#define HI_JPEGD_IP_NUM                    1
#define HI_JPEGD_MAX_WIDTH                 16384
#define HI_JPEGD_MAX_HEIGHT                16384
#define HI_JPEGD_MIN_WIDTH                 8
#define HI_JPEGD_MIN_HEIGHT                8
#define HI_JPEGD_RGB_MAX_HEIGHT            8640

/* For VDA */
#define HI_VDA_MAX_NODE_NUM        32
#define HI_VDA_MAX_INTERNAL        256
#define HI_VDA_CHN_NUM_MAX         128
#define HI_VDA_MAX_WIDTH           960
#define HI_VDA_MAX_HEIGHT          960
#define HI_VDA_MIN_WIDTH           32
#define HI_VDA_MIN_HEIGHT          32

/* For Region */
#define HI_RGN_MIN_WIDTH                   2
#define HI_RGN_MIN_HEIGHT                  2

#define HI_RGN_COVER_MIN_X                 -16382
#define HI_RGN_COVER_MIN_Y                 -8190
#define HI_RGN_COVER_MAX_X                 16382
#define HI_RGN_COVER_MAX_Y                 8190
#define HI_RGN_COVER_MAX_WIDTH             16384
#define HI_RGN_COVER_MAX_HEIGHT            8192
#define HI_RGN_COVER_MIN_THICK             2
#define HI_RGN_COVER_MAX_THICK             8

#define HI_RGN_COVEREX_MIN_X               -16382
#define HI_RGN_COVEREX_MIN_Y               -8190
#define HI_RGN_COVEREX_MAX_X               16382
#define HI_RGN_COVEREX_MAX_Y               8190
#define HI_RGN_COVEREX_MAX_WIDTH           16384
#define HI_RGN_COVEREX_MAX_HEIGHT          8192
#define HI_RGN_COVEREX_MIN_THICK           2
#define HI_RGN_COVEREX_MAX_THICK           14
#define HI_RGN_COVEREX_THICK_ALIGN         2

#define HI_RGN_OVERLAY_MIN_X               0
#define HI_RGN_OVERLAY_MIN_Y               0
#define HI_RGN_OVERLAY_MAX_X               16382
#define HI_RGN_OVERLAY_MAX_Y               8190
#define HI_RGN_OVERLAY_MAX_WIDTH           16384
#define HI_RGN_OVERLAY_MAX_HEIGHT          8192
#define HI_RGN_OVERLAY_MIN_ALPHA           0
#define HI_RGN_OVERLAY_MAX_ALPHA           255
#define HI_RGN_OVERLAY_VENC_MAX_ALPHA      128

#define HI_RGN_OVERLAYEX_MIN_X             0
#define HI_RGN_OVERLAYEX_MIN_Y             0
#define HI_RGN_OVERLAYEX_MAX_X             16382
#define HI_RGN_OVERLAYEX_MAX_Y             8190
#define HI_RGN_OVERLAYEX_MAX_WIDTH         16384
#define HI_RGN_OVERLAYEX_MAX_HEIGHT        8192
#define HI_RGN_OVERLAYEX_MIN_ALPHA         0
#define HI_RGN_OVERLAYEX_MAX_ALPHA         255


#define HI_RGN_LINE_MIN_X                   0
#define HI_RGN_LINE_MIN_Y                   0
#define HI_RGN_LINE_MAX_X                   16382
#define HI_RGN_LINE_MAX_Y                   8190
#define HI_RGN_LINE_MIN_THICK               1
#define HI_RGN_LINE_MAX_THICK               7

#define HI_RGN_MOSAIC_X_ALIGN              4
#define HI_RGN_MOSAIC_Y_ALIGN              2
#define HI_RGN_MOSAIC_WIDTH_ALIGN          4
#define HI_RGN_MOSAIC_HEIGHT_ALIGN         2

#define HI_RGN_MOSAIC_MIN_X                0
#define HI_RGN_MOSAIC_MIN_Y                0
#define HI_RGN_MOSAIC_MAX_X                16380
#define HI_RGN_MOSAIC_MAX_Y                8190
#define HI_RGN_MOSAIC_MIN_WIDTH            8
#define HI_RGN_MOSAIC_MIN_HEIGHT           8
#define HI_RGN_MOSAIC_MAX_WIDTH            16384
#define HI_RGN_MOSAIC_MAX_HEIGHT           8192

#define HI_RGN_MOSAICEX_MIN_X              0
#define HI_RGN_MOSAICEX_MIN_Y              0
#define HI_RGN_MOSAICEX_MAX_X              16380
#define HI_RGN_MOSAICEX_MAX_Y              8190
#define HI_RGN_MOSAICEX_MIN_WIDTH          8
#define HI_RGN_MOSAICEX_MIN_HEIGHT         8
#define HI_RGN_MOSAICEX_MAX_WIDTH          16384
#define HI_RGN_MOSAICEX_MAX_HEIGHT         8192

#define HI_RGN_ALIGN                       2

#define HI_RGN_HANDLE_MAX                  1024
#define HI_RGN_MAX_BUF_NUM                 2

#define HI_RGN_VI_MAX_COVER_NUM            4

#define HI_RGN_VENC_MAX_OVERLAY_NUM        8

#define HI_RGN_VPSS_MAX_COVER_NUM          4
#define HI_RGN_VPSS_MAX_COVEREX_NUM        8
#define HI_RGN_VPSS_MAX_OVERLAY_NUM        8
#define HI_RGN_VPSS_MAX_OVERLAYEX_NUM      8
#define HI_RGN_VPSS_MAX_LINE_NUM           40
#define HI_RGN_VPSS_MAX_MOSAIC_NUM         4
#define HI_RGN_VPSS_MAX_MOSAICEX_NUM       4

#define HI_RGN_VO_MAX_COVEREX_NUM          4
#define HI_RGN_VO_MAX_OVERLAYEX_NUM        1
#define HI_RGN_VO_MAX_LINE_NUM             4

#define HI_RGN_PCIV_MAX_OVERLAYEX_NUM      1

#define HI_RGN_VGS_TASK_WIDTH_MAX         16384

/* For VI */
/* number of channle and device on video input unit of chip
 * Note! HI_VI_MAX_CHN_NUM is NOT equal to HI_VI_MAX_DEV_NUM
 * multiplied by HI_VI_MAX_CHN_NUM, because all VI devices
 * can't work at mode of 4 channles at the same time.
 */
#define HI_VI_MAX_NORMAL_DEV_NUM        8
#define HI_VI_MAX_CAS_DEV_NUM           1
#define HI_VI_MAX_DEV_NUM               ((HI_VI_MAX_NORMAL_DEV_NUM) + (HI_VI_MAX_CAS_DEV_NUM))
#define HI_VI_MAX_WAY_NUM_PER_DEV       4
#define HI_VI_MAX_CHN_NUM_PER_DEV       4
#define HI_VI_MAX_NORMAL_WAY_NUM        32
#define HI_VI_MAX_CAS_WAY_NUM           1
#define HI_VI_MAX_WAY_NUM               ((HI_VI_MAX_NORMAL_WAY_NUM) + (HI_VI_MAX_CAS_WAY_NUM))
#define HI_VI_MAX_NORMAL_CHN_NUM        32
#define HI_VI_MAX_CAS_PHYS_CHN_NUM      1
#define HI_VI_MAX_PHYS_CHN_NUM          ((HI_VI_MAX_NORMAL_CHN_NUM) + (HI_VI_MAX_CAS_PHYS_CHN_NUM))

#define HI_VI_MAX_CAS_VIRT_CHN_NUM      2
#define HI_VI_CAS_VIRT_CHN_START        HI_VI_MAX_PHYS_CHN_NUM

#define HI_VI_MAX_CHN_NUM               (HI_VI_MAX_PHYS_CHN_NUM + HI_VI_MAX_CAS_VIRT_CHN_NUM)

/* define cascade chn */
#define HI_VI_CAS_PHYS_CHN              32
#define HI_VI_CAS_VIRT_CHN_0            33
#define HI_VI_CAS_VIRT_CHN_1            34

/* max number of VBI region */
#define HI_VI_MAX_VBI_NUM               2
/* max length of one VBI region (by word) */
#define HI_VI_MAX_VBI_LEN               0x20

/* For VO */
#define HI_VO_MAX_PHYS_DEV_NUM             3   /* max physical dev num */
#define HI_VO_MAX_VIRT_DEV_NUM            32  /* max virtual dev num */
#define HI_VO_MAX_CAS_DEV_NUM             2   /* max cascade dev num */
#define HI_VO_MAX_DEV_NUM                  (HI_VO_MAX_PHYS_DEV_NUM + HI_VO_MAX_VIRT_DEV_NUM + HI_VO_MAX_CAS_DEV_NUM) /* max dev num */

#define HI_VO_VIRT_DEV_0                   3  /* virtual device 0 */
#define HI_VO_VIRT_DEV_1                   4  /* virtual device 1 */
#define HI_VO_VIRT_DEV_2                   5  /* virtual device 2 */
#define HI_VO_VIRT_DEV_3                   6  /* virtual device 3 */
#define HI_VO_CAS_DEV_1                    35  /* cascade device 1 */
#define HI_VO_CAS_DEV_2                    36  /* cascade device 2 */

#define HI_VO_MAX_PHYS_VIDEO_LAYER_NUM     4  /* max physical video layer num */
#define HI_VO_MAX_GFX_LAYER_NUM             4  /* max graphic layer num */
#define HI_VO_MAX_PHYS_LAYER_NUM           (HI_VO_MAX_PHYS_VIDEO_LAYER_NUM + HI_VO_MAX_GFX_LAYER_NUM) /* max physical layer num */
#define HI_VO_MAX_LAYER_NUM                (HI_VO_MAX_PHYS_LAYER_NUM + HI_VO_MAX_VIRT_DEV_NUM + HI_VO_MAX_CAS_DEV_NUM) /* max layer num */
#define HI_VO_MAX_LAYER_IN_DEV             2 /* max video layer num of each dev */

#define HI_VO_LAYER_V0                     0  /* video layer 0 */
#define HI_VO_LAYER_V1                     1  /* video layer 1 */
#define HI_VO_LAYER_V2                     2  /* video layer 2 */
#define HI_VO_LAYER_V3                     3  /* video layer 3 */
#define HI_VO_LAYER_G0                     4  /* graphics layer 0 */
#define HI_VO_LAYER_G1                     5  /* graphics layer 1 */
#define HI_VO_LAYER_G2                     6  /* graphics layer 2 */
#define HI_VO_LAYER_G3                     7  /* graphics layer 3 */
#define hi_vo_get_virt_layer(vo_virt_dev) ((vo_virt_dev) + 5)  /* get virtual layer of virtual dev */
#define hi_vo_get_cas_layer(vo_cas_dev)   ((vo_cas_dev) + 5)   /* get cascade layer of cascade dev */

#define HI_VO_MAX_PRIORITY                 4 /* max layer priority */
#define HI_VO_MIN_TOLERATE                 1 /* min play toleration 1ms */
#define HI_VO_MAX_TOLERATE                 100000 /* max play toleration 100s */

#define HI_VO_MAX_CHN_NUM                  64 /* max chn num */
#define HI_VO_MIN_CHN_WIDTH                32 /* channel minimal width */
#define HI_VO_MIN_CHN_HEIGHT               32 /* channel minimal height */
#define HI_VO_MAX_ZOOM_RATIO               1000 /* max zoom ratio, 1000 means 100% scale */
#define HI_VO_MAX_NODE_NUM                 16 /* max node num */

#define HI_VO_MAX_WBC_NUM                  1 /* max wbc num */

#define HI_VO_MAX_CAS_PATTERN             128     /* cascade pattern max number */
#define HI_VO_MAX_CAS_POS_32RGN           32      /* cascade position max number */
#define HI_VO_MAX_CAS_POS_64RGN           64      /* cascade position max number */

/* For AUDIO */
#define HI_AI_DEV_MAX_NUM                  2
#define HI_AO_DEV_MIN_NUM                  0
#define HI_AO_DEV_MAX_NUM                  2
#define HI_AIO_MAX_NUM                     ((HI_AO_DEV_MAX_NUM > HI_AI_DEV_MAX_NUM) ? HI_AO_DEV_MAX_NUM : HI_AI_DEV_MAX_NUM)
#define HI_AENC_MAX_CHN_NUM                49
#define HI_ADEC_MAX_CHN_NUM                32

#define HI_AI_MAX_CHN_NUM                  20
#define HI_AO_MAX_CHN_NUM                  9
#define HI_AO_SYS_CHN_ID                  (HI_AO_MAX_CHN_NUM - 1)

#define HI_AIO_MAX_CHN_NUM                 ((HI_AO_MAX_CHN_NUM > HI_AI_MAX_CHN_NUM) ? HI_AO_MAX_CHN_NUM : HI_AI_MAX_CHN_NUM)

/* For VPSS */
#define HI_VPSS_MAX_GRP_NUM                256
#define HI_VPSS_VGS_GRP_NO                 240
#define HI_VPSS_IP_NUM                     3
#define HI_VPSS0                           0
#define HI_VPSS1                           1
#define HI_VPSS2                           2
#define HI_VPSS_MAX_PHYS_CHN_NUM            4
#define HI_VPSS_MAX_CHN_NUM                HI_VPSS_MAX_PHYS_CHN_NUM
#define HI_VPSS_MIN_IMG_WIDTH            64
#define HI_VPSS_MIN_IMG_HEIGHT           64
#define HI_VPSS_MAX_IMG_WIDTH            16384
#define HI_VPSS_MAX_DEI_WIDTH            960
#define HI_VPSS_MAX_SEG_COMPACT_WIDTH    2688
#define HI_VPSS_MAX_IMG_HEIGHT           8192
#define HI_VPSS_MAX_ZOOMIN                 16
#define HI_VPSS_MAX_ZOOMOUT                15

/* For PCIV */
#define HI_PCIV_MAX_CHN_NUM                128 /* max pciv channel number in each pciv device */

/* For IVS_MD */
#define HI_MD_MAX_CHN                      64
#define HI_MD_MAX_WIDTH                    1920
#define HI_MD_MAX_HEIGHT                   1080
#define HI_MD_MIN_WIDTH                    64
#define HI_MD_MIN_HEIGHT                   64

/* For VGS */
#define HI_VGS_IP_NUM                      1
#define HI_VGS0                            0
#define HI_VGS_MAX_COVER_NUM               4
#define HI_VGS_MAX_OSD_NUM                 1

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_DEFINES_H__ */


/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __OT_COMMON_H__
#define __OT_COMMON_H__

#include "autoconf.h"
#include "ot_type.h"
#include "ot_defines.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

#ifndef VER_X
#define VER_X                  1
#endif

#ifndef VER_Y
#define VER_Y                  0
#endif

#ifndef VER_Z
#define VER_Z                  0
#endif

#ifndef VER_P
#define VER_P                  0
#endif

#ifndef VER_B
#define VER_B                  0
#endif

#ifdef OT_DEBUG
#define VER_D                  " Debug"
#else
#define VER_D                  " Release"
#endif

#define ATTRIBUTE              __attribute__((aligned(OT_ALIGN_NUM)))
#define ot_unused(x)             ((td_void)(x))

#define OT_COMPAT_POINTER(ptr, type) \
    do { \
        td_u32 dest_addr_ = (td_u32)(td_uintptr_t)ptr; \
        ptr = (type)(td_uintptr_t)dest_addr_; \
    } while (0)

#define __MK_VERSION(x, y, z, p, b) #x "." #y "." #z "." #p " B0" #b
#define MK_VERSION(x, y, z, p, b) __MK_VERSION(x, y, z, p, b)
#define OT_MPP_VERSION            OT_CHIP_NAME OT_MPP_VER_PRIX MK_VERSION(VER_X, VER_Y, VER_Z, VER_P, VER_B) VER_D

#define OT_MAX_VERSION_NAME_LEN    64
#define OT_INVALID_CHN         (-1)
#define OT_INVALID_WAY         (-1)
#define OT_INVALID_LAYER       (-1)
#define OT_INVALID_DEV         (-1)
#define OT_INVALID_HANDLE      (-1)
#define OT_INVALID_VALUE       (-1)
#define OT_INVALID_TYPE        (-1)

#define OT_MPP_MOD_VI             "vi"
#define OT_MPP_MOD_VO             "vo"
#define OT_MPP_MOD_AVS            "avs"
#define OT_MPP_MOD_HDMI           "hdmi"
#define OT_MPP_MOD_VGS            "vgs"
#define OT_MPP_MOD_GDC            "gdc"
#define OT_MPP_MOD_DIS            "dis"
#define OT_MPP_MOD_GYRODIS        "gyrodis"
#define OT_MPP_MOD_CHNL           "chnl"
#define OT_MPP_MOD_VENC           "venc"
#define OT_MPP_MOD_VPSS           "vpss"
#define OT_MPP_MOD_RGN            "rgn"
#define OT_MPP_MOD_IVE            "ive"
#define OT_MPP_MOD_FD             "fd"
#define OT_MPP_MOD_MD             "md"
#define OT_MPP_MOD_IVP            "ivp"
#define OT_MPP_MOD_SVP            "svp"
#define OT_MPP_MOD_SVP_NNIE       "nnie"
#define OT_MPP_MOD_SVP_DSP        "dsp"
#define OT_MPP_MOD_SVP_ALG        "svp_alg"
#define OT_MPP_MOD_DPU_RECT       "rect"
#define OT_MPP_MOD_DPU_MATCH      "match"
#define OT_MPP_MOD_SVP_MAU        "mau"
#define OT_MPP_MOD_H264E          "h264e"
#define OT_MPP_MOD_H265E          "h265e"
#define OT_MPP_MOD_JPEGE          "jpege"
#define OT_MPP_MOD_MPEG4E         "mpeg4e"
#define OT_MPP_MOD_VEDU           "vedu"
#define OT_MPP_MOD_PRORES         "prores"
#define OT_MPP_MOD_VDEC           "vdec"
#define OT_MPP_MOD_H264D          "h264d"
#define OT_MPP_MOD_JPEGD          "jpegd"
#define OT_MPP_MOD_AI             "ai"
#define OT_MPP_MOD_AO             "ao"
#define OT_MPP_MOD_AENC           "aenc"
#define OT_MPP_MOD_ADEC           "adec"
#define OT_MPP_MOD_AIO            "aio"
#define OT_MPP_MOD_ACODEC         "acodec"
#define OT_MPP_MOD_VB             "vb"
#define OT_MPP_MOD_SYS            "sys"
#define OT_MPP_MOD_CMPI           "cmpi"
#define OT_MPP_MOD_PCIV           "pciv"
#define OT_MPP_MOD_PCIVFMW        "pcivfmw"
#define OT_MPP_MOD_PROC           "proc"
#define OT_MPP_MOD_LOG            "logmpp"
#define OT_MPP_MOD_LOG_MDC        "logmpp_mdc"
#define OT_MPP_MOD_FB             "fb"
#define OT_MPP_MOD_RC             "rc"
#define OT_MPP_MOD_TDE            "tde"
#define OT_MPP_MOD_ISP            "isp"
#define OT_MPP_MOD_USR            "usr"
#define OT_MPP_MOD_MCF            "mcf"
#define OT_MPP_MOD_PM             "pm"
#define OT_MPP_MOD_MFUSION        "motionfusion"
#define OT_MPP_MOD_VDA            "vda"
#define OT_MPP_MOD_VPP            "vpp"
#define OT_MPP_MOD_KCF            "kcf"
#define OT_MPP_MOD_PQP            "pqp"
#define OT_MPP_MOD_SVP_NPU        "svp_npu"
#define OT_MPP_MOD_NPU_DEV        "npudev"
#define OT_MPP_MOD_NPU_AICPU      "aicpu"
#define OT_MPP_MOD_NPU_DFX        "npudfx"
#define OT_MPP_MOD_NPU_TSFW       "tsfw"
#define OT_MPP_MOD_CIPHER         "cipher"
#define OT_MPP_MOD_KLAD           "klad"
#define OT_MPP_MOD_OTP            "otp"
#define OT_MPP_MOD_DCC            "dcc"
#define OT_MPP_MOD_VDEC_SERVERS   "vdec_server"
#define OT_MPP_MOD_VDEC_ADAPT     "vdec_adapt"
#define OT_MPP_MOD_UVC            "uvc"
typedef struct {
    td_char version[OT_MAX_VERSION_NAME_LEN];
} ot_mpp_version;

typedef td_s32 ot_ai_chn;
typedef td_s32 ot_ao_chn;
typedef td_s32 ot_aenc_chn;
typedef td_s32 ot_adec_chn;
typedef td_s32 ot_audio_dev;
typedef td_s32 ot_vi_dev;
typedef td_s32 ot_vi_pipe;
typedef td_s32 ot_vi_way;
typedef td_s32 ot_vi_chn;
typedef td_s32 ot_vi_grp;
typedef td_s32 ot_vo_dev;
typedef td_s32 ot_vo_layer;
typedef td_s32 ot_vo_chn;
typedef td_s32 ot_vo_wbc;
typedef td_s32 ot_gfx_layer;
typedef td_s32 ot_venc_chn;
typedef td_s32 ot_vdec_chn;
typedef td_s32 ot_ive_handle;
typedef td_s32 ot_fd_chn;
typedef td_s32 ot_md_chn;
typedef td_s32 ot_isp_dev;
typedef td_s32 ot_blk_dev;
typedef td_s32 ot_sensor_id;
typedef td_s32 ot_mipi_dev;
typedef td_s32 ot_slave_dev;
typedef td_s32 ot_svp_nnie_handle;
typedef td_s32 ot_svp_dsp_handle;
typedef td_s32 ot_svp_alg_chn;
typedef td_s32 ot_svp_mau_handle;
typedef td_s32 ot_vpss_grp;
typedef td_s32 ot_vpss_grp_pipe;
typedef td_s32 ot_vpss_chn;
typedef td_s32 ot_avs_grp;
typedef td_s32 ot_avs_pipe;
typedef td_s32 ot_avs_chn;
typedef td_s32 ot_mcf_grp;
typedef td_s32 ot_mcf_pipe;
typedef td_s32 ot_mcf_chn;
typedef td_s32 ot_vda_chn;
typedef td_s32 ot_pciv_chn;
typedef td_s32 ot_uvc_chn;

typedef enum {
    OT_ID_CMPI    = 0,
    OT_ID_VB      = 1,
    OT_ID_SYS     = 2,
    OT_ID_RGN     = 3,
    OT_ID_CHNL    = 4,
    OT_ID_VDEC    = 5,
    OT_ID_AVS     = 6,
    OT_ID_VPSS    = 7,
    OT_ID_VENC    = 8,
    OT_ID_SVP     = 9,
    OT_ID_H264E   = 10,
    OT_ID_JPEGE   = 11,
    OT_ID_H265E   = 13,
    OT_ID_JPEGD   = 14,
    OT_ID_VO      = 15,
    OT_ID_VI      = 16,
    OT_ID_DIS     = 17,
    OT_ID_VALG    = 18,
    OT_ID_RC      = 19,
    OT_ID_AIO     = 20,
    OT_ID_AI      = 21,
    OT_ID_AO      = 22,
    OT_ID_AENC    = 23,
    OT_ID_ADEC    = 24,
    OT_ID_VPU     = 25,
    OT_ID_PCIV    = 26,
    OT_ID_PCIVFMW = 27,
    OT_ID_ISP     = 28,
    OT_ID_IVE     = 29,
    OT_ID_USER    = 30,
    OT_ID_PROC    = 33,
    OT_ID_LOG     = 34,
    OT_ID_VFMW    = 35,
    OT_ID_GDC     = 37,
    OT_ID_PHOTO   = 38,
    OT_ID_FB      = 39,
    OT_ID_HDMI    = 40,
    OT_ID_VOIE    = 41,
    OT_ID_TDE     = 42,
    OT_ID_HDR     = 43,
    OT_ID_PRORES  = 44,
    OT_ID_VGS     = 45,
    OT_ID_FD      = 47,
    OT_ID_OD      = 48,
    OT_ID_LPR     = 50,
    OT_ID_SVP_NNIE     = 51,
    OT_ID_SVP_DSP      = 52,
    OT_ID_DPU_RECT     = 53,
    OT_ID_DPU_MATCH    = 54,

    OT_ID_MOTIONSENSOR = 55,
    OT_ID_MOTIONFUSION = 56,

    OT_ID_GYRODIS      = 57,
    OT_ID_PM           = 58,
    OT_ID_SVP_ALG      = 59,
    OT_ID_IVP          = 60,
    OT_ID_MCF          = 61,
    OT_ID_SVP_MAU      = 62,
    OT_ID_VDA          = 63,
    OT_ID_VPP          = 64,
    OT_ID_KCF          = 65,
    OT_ID_PQP          = 66,

    OT_ID_NPUDEV       = 67,
    OT_ID_AICPU        = 68,
    OT_ID_NPUDFX       = 69,
    OT_ID_TSFW         = 70,

    OT_ID_CIPHER       = 71,
    OT_ID_KLAD         = 72,
    OT_ID_KEYSLOT      = 73,
    OT_ID_OTP          = 74,
    OT_ID_VDEC_ADAPT   = 75,
    OT_ID_DCC          = 76,
    OT_ID_VDEC_SERVER  = 77,
    OT_ID_VFMW_MDC     = 78,
    OT_ID_VB_LOG       = 79,
    OT_ID_MCF_CALIBRATION = 80,
    OT_ID_SVP_NPU      = 81,
    OT_ID_HNR          = 82,
    OT_ID_SNAP         = 83,
    OT_ID_LOG_MDC      = 84,
    OT_ID_UVC          = 85,
    OT_ID_FISHEYE_CALIBRATION = 86,
    OT_ID_BUTT,
} ot_mod_id;

typedef struct {
    ot_mod_id    mod_id;
    td_s32      dev_id;
    td_s32      chn_id;
} ot_mpp_chn;

/* We just copy this value of payload type from RTP/RTSP definition */
typedef enum {
    OT_PT_PCMU          = 0,
    OT_PT_1016          = 1,
    OT_PT_G721          = 2,
    OT_PT_GSM           = 3,
    OT_PT_G723          = 4,
    OT_PT_DVI4_8K       = 5,
    OT_PT_DVI4_16K      = 6,
    OT_PT_LPC           = 7,
    OT_PT_PCMA          = 8,
    OT_PT_G722          = 9,
    OT_PT_S16BE_STEREO  = 10,
    OT_PT_S16BE_MONO    = 11,
    OT_PT_QCELP         = 12,
    OT_PT_CN            = 13,
    OT_PT_MPEGAUDIO     = 14,
    OT_PT_G728          = 15,
    OT_PT_DVI4_3        = 16,
    OT_PT_DVI4_4        = 17,
    OT_PT_G729          = 18,
    OT_PT_G711A         = 19,
    OT_PT_G711U         = 20,
    OT_PT_G726          = 21,
    OT_PT_G729A         = 22,
    OT_PT_LPCM          = 23,
    OT_PT_CelB          = 25,
    OT_PT_JPEG          = 26,
    OT_PT_CUSM          = 27,
    OT_PT_NV            = 28,
    OT_PT_PICW          = 29,
    OT_PT_CPV           = 30,
    OT_PT_H261          = 31,
    OT_PT_MPEGVIDEO     = 32,
    OT_PT_MPEG2TS       = 33,
    OT_PT_H263          = 34,
    OT_PT_SPEG          = 35,
    OT_PT_MPEG2VIDEO    = 36,
    OT_PT_AAC           = 37,
    OT_PT_WMA9STD       = 38,
    OT_PT_HEAAC         = 39,
    OT_PT_PCM_VOICE     = 40,
    OT_PT_PCM_AUDIO     = 41,
    OT_PT_MP3           = 43,
    OT_PT_ADPCMA        = 49,
    OT_PT_AEC           = 50,
    OT_PT_X_LD          = 95,
    OT_PT_H264          = 96,
    OT_PT_D_GSM_HR      = 200,
    OT_PT_D_GSM_EFR     = 201,
    OT_PT_D_L8          = 202,
    OT_PT_D_RED         = 203,
    OT_PT_D_VDVI        = 204,
    OT_PT_D_BT656       = 220,
    OT_PT_D_H263_1998   = 221,
    OT_PT_D_MP1S        = 222,
    OT_PT_D_MP2P        = 223,
    OT_PT_D_BMPEG       = 224,
    OT_PT_MP4VIDEO      = 230,
    OT_PT_MP4AUDIO      = 237,
    OT_PT_VC1           = 238,
    OT_PT_JVC_ASF       = 255,
    OT_PT_D_AVI         = 256,
    OT_PT_DIVX3         = 257,
    OT_PT_AVS           = 258,
    OT_PT_REAL8         = 259,
    OT_PT_REAL9         = 260,
    OT_PT_VP6           = 261,
    OT_PT_VP6F          = 262,
    OT_PT_VP6A          = 263,
    OT_PT_SORENSON      = 264,
    OT_PT_H265          = 265,
    OT_PT_VP8           = 266,
    OT_PT_MVC           = 267,
    OT_PT_PNG           = 268,
    /* add by vendor */
    OT_PT_AMR           = 1001,
    OT_PT_MJPEG         = 1002,
    OT_PT_AMRWB         = 1003,
    OT_PT_PRORES        = 1006,
    OT_PT_OPUS          = 1007,
    OT_PT_BUTT
} ot_payload_type;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif /* __OT_COMMON_H__ */

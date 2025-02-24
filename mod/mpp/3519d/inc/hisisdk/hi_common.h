/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_COMMON_H
#define HI_COMMON_H

#include "autoconf.h"
#include "hi_type.h"
#include "hi_defines.h"
#include "ot_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_COMPAT_POINTER OT_COMPAT_POINTER
#define HI_MPP_VERSION OT_MPP_VERSION
#define HI_MAX_VERSION_NAME_LEN OT_MAX_VERSION_NAME_LEN
#define HI_INVALID_CHN OT_INVALID_CHN
#define HI_INVALID_WAY OT_INVALID_WAY
#define HI_INVALID_LAYER OT_INVALID_LAYER
#define HI_INVALID_DEV OT_INVALID_DEV
#define HI_INVALID_HANDLE OT_INVALID_HANDLE
#define HI_INVALID_VALUE OT_INVALID_VALUE
#define HI_INVALID_TYPE OT_INVALID_TYPE
#define HI_MPP_MOD_VI OT_MPP_MOD_VI
#define HI_MPP_MOD_VO OT_MPP_MOD_VO
#define HI_MPP_MOD_VO_DEV OT_MPP_MOD_VO_DEV
#define HI_MPP_MOD_AVS OT_MPP_MOD_AVS
#define HI_MPP_MOD_HDMI OT_MPP_MOD_HDMI
#define HI_MPP_MOD_VGS OT_MPP_MOD_VGS
#define HI_MPP_MOD_GDC OT_MPP_MOD_GDC
#define HI_MPP_MOD_DIS OT_MPP_MOD_DIS
#define HI_MPP_MOD_GYRODIS OT_MPP_MOD_GYRODIS
#define HI_MPP_MOD_CHNL OT_MPP_MOD_CHNL
#define HI_MPP_MOD_VENC OT_MPP_MOD_VENC
#define HI_MPP_MOD_VENC_MSG OT_MPP_MOD_VENC_MSG
#define HI_MPP_MOD_VPSS OT_MPP_MOD_VPSS
#define HI_MPP_MOD_RGN OT_MPP_MOD_RGN
#define HI_MPP_MOD_IVE OT_MPP_MOD_IVE
#define HI_MPP_MOD_FD OT_MPP_MOD_FD
#define HI_MPP_MOD_MD OT_MPP_MOD_MD
#define HI_MPP_MOD_IVP OT_MPP_MOD_IVP
#define HI_MPP_MOD_SVP OT_MPP_MOD_SVP
#define HI_MPP_MOD_SVP_NNIE OT_MPP_MOD_SVP_NNIE
#define HI_MPP_MOD_SVP_DSP OT_MPP_MOD_SVP_DSP
#define HI_MPP_MOD_SVP_ALG OT_MPP_MOD_SVP_ALG
#define HI_MPP_MOD_DPU_RECT OT_MPP_MOD_DPU_RECT
#define HI_MPP_MOD_DPU_MATCH OT_MPP_MOD_DPU_MATCH
#define HI_MPP_MOD_SVP_MAU OT_MPP_MOD_SVP_MAU
#define HI_MPP_MOD_H264E OT_MPP_MOD_H264E
#define HI_MPP_MOD_H265E OT_MPP_MOD_H265E
#define HI_MPP_MOD_JPEGE OT_MPP_MOD_JPEGE
#define HI_MPP_MOD_MPEG4E OT_MPP_MOD_MPEG4E
#define HI_MPP_MOD_VEDU OT_MPP_MOD_VEDU
#define HI_MPP_MOD_PRORES OT_MPP_MOD_PRORES
#define HI_MPP_MOD_VDEC OT_MPP_MOD_VDEC
#define HI_MPP_MOD_H264D OT_MPP_MOD_H264D
#define HI_MPP_MOD_JPEGD OT_MPP_MOD_JPEGD
#define HI_MPP_MOD_AI OT_MPP_MOD_AI
#define HI_MPP_MOD_AO OT_MPP_MOD_AO
#define HI_MPP_MOD_AENC OT_MPP_MOD_AENC
#define HI_MPP_MOD_ADEC OT_MPP_MOD_ADEC
#define HI_MPP_MOD_AIO OT_MPP_MOD_AIO
#define HI_MPP_MOD_ACODEC OT_MPP_MOD_ACODEC
#define HI_MPP_MOD_AB OT_MPP_MOD_AB
#define HI_MPP_MOD_VB OT_MPP_MOD_VB
#define HI_MPP_MOD_VB_USER OT_MPP_MOD_VB_USER
#define HI_MPP_MOD_SYS OT_MPP_MOD_SYS
#define HI_MPP_MOD_SYS_USER OT_MPP_MOD_SYS_USER
#define HI_MPP_MOD_CMPI OT_MPP_MOD_CMPI
#define HI_MPP_MOD_PCIV OT_MPP_MOD_PCIV
#define HI_MPP_MOD_PCIVFMW OT_MPP_MOD_PCIVFMW
#define HI_MPP_MOD_PROC OT_MPP_MOD_PROC
#define HI_MPP_MOD_LOG OT_MPP_MOD_LOG
#define HI_MPP_MOD_LOG_USER OT_MPP_MOD_LOG_USER
#define HI_MPP_MOD_LOG_MDC OT_MPP_MOD_LOG_MDC
#define HI_MPP_MOD_FB OT_MPP_MOD_FB
#define HI_MPP_MOD_RC OT_MPP_MOD_RC
#define HI_MPP_MOD_TDE OT_MPP_MOD_TDE
#define HI_MPP_MOD_ISP OT_MPP_MOD_ISP
#define HI_MPP_MOD_USR OT_MPP_MOD_USR
#define HI_MPP_MOD_MCF OT_MPP_MOD_MCF
#define HI_MPP_MOD_PM OT_MPP_MOD_PM
#define HI_MPP_MOD_MFUSION OT_MPP_MOD_MFUSION
#define HI_MPP_MOD_VDA OT_MPP_MOD_VDA
#define HI_MPP_MOD_VPP OT_MPP_MOD_VPP
#define HI_MPP_MOD_KCF OT_MPP_MOD_KCF
#define HI_MPP_MOD_PQP OT_MPP_MOD_PQP
#define HI_MPP_MOD_SVP_NPU OT_MPP_MOD_SVP_NPU
#define HI_MPP_MOD_NPU_DEV OT_MPP_MOD_NPU_DEV
#define HI_MPP_MOD_NPU_AICPU OT_MPP_MOD_NPU_AICPU
#define HI_MPP_MOD_NPU_DFX OT_MPP_MOD_NPU_DFX
#define HI_MPP_MOD_NPU_TSFW OT_MPP_MOD_NPU_TSFW
#define HI_MPP_MOD_CIPHER OT_MPP_MOD_CIPHER
#define HI_MPP_MOD_KLAD OT_MPP_MOD_KLAD
#define HI_MPP_MOD_OTP OT_MPP_MOD_OTP
#define HI_MPP_MOD_DCC OT_MPP_MOD_DCC
#define HI_MPP_MOD_VDEC_SERVERS OT_MPP_MOD_VDEC_SERVERS
#define HI_MPP_MOD_VDEC_ADAPT OT_MPP_MOD_VDEC_ADAPT
#define HI_MPP_MOD_VCA OT_MPP_MOD_VCA
#define HI_MPP_MOD_UVC OT_MPP_MOD_UVC
#define HI_MPP_MOD_VI_RGBW OT_MPP_MOD_VI_RGBW
typedef ot_mpp_version hi_mpp_version;
typedef ot_ai_chn hi_ai_chn;
typedef ot_ao_chn hi_ao_chn;
typedef ot_aenc_chn hi_aenc_chn;
typedef ot_adec_chn hi_adec_chn;
typedef ot_audio_dev hi_audio_dev;
typedef ot_vi_dev hi_vi_dev;
typedef ot_vi_pipe hi_vi_pipe;
typedef ot_vi_way hi_vi_way;
typedef ot_vi_chn hi_vi_chn;
typedef ot_vi_grp hi_vi_grp;
typedef ot_vo_dev hi_vo_dev;
typedef ot_vo_layer hi_vo_layer;
typedef ot_vo_chn hi_vo_chn;
typedef ot_vo_wbc hi_vo_wbc;
typedef ot_gfx_layer hi_gfx_layer;
typedef ot_venc_chn hi_venc_chn;
typedef ot_vdec_chn hi_vdec_chn;
typedef ot_ive_handle hi_ive_handle;
typedef ot_fd_chn hi_fd_chn;
typedef ot_md_chn hi_md_chn;
typedef ot_isp_dev hi_isp_dev;
typedef ot_blk_dev hi_blk_dev;
typedef ot_sensor_id hi_sensor_id;
typedef ot_mipi_dev hi_mipi_dev;
typedef ot_slave_dev hi_slave_dev;
typedef ot_svp_nnie_handle hi_svp_nnie_handle;
typedef ot_svp_dsp_handle hi_svp_dsp_handle;
typedef ot_svp_alg_chn hi_svp_alg_chn;
typedef ot_svp_mau_handle hi_svp_mau_handle;
typedef ot_vpss_grp hi_vpss_grp;
typedef ot_vpss_grp_pipe hi_vpss_grp_pipe;
typedef ot_vpss_chn hi_vpss_chn;
typedef ot_avs_grp hi_avs_grp;
typedef ot_avs_pipe hi_avs_pipe;
typedef ot_avs_chn hi_avs_chn;
typedef ot_mcf_grp hi_mcf_grp;
typedef ot_mcf_pipe hi_mcf_pipe;
typedef ot_mcf_chn hi_mcf_chn;
typedef ot_vda_chn hi_vda_chn;
typedef ot_pciv_chn hi_pciv_chn;
typedef ot_uvc_chn hi_uvc_chn;
#define HI_ID_MEM OT_ID_MEM
#define HI_ID_VB OT_ID_VB
#define HI_ID_SYS OT_ID_SYS
#define HI_ID_RGN OT_ID_RGN
#define HI_ID_BASE OT_ID_BASE
#define HI_ID_VDEC OT_ID_VDEC
#define HI_ID_AVS OT_ID_AVS
#define HI_ID_VPSS OT_ID_VPSS
#define HI_ID_VENC OT_ID_VENC
#define HI_ID_SVP OT_ID_SVP
#define HI_ID_H264E OT_ID_H264E
#define HI_ID_JPEGE OT_ID_JPEGE
#define HI_ID_H265E OT_ID_H265E
#define HI_ID_JPEGD OT_ID_JPEGD
#define HI_ID_VO OT_ID_VO
#define HI_ID_VI OT_ID_VI
#define HI_ID_DIS OT_ID_DIS
#define HI_ID_CHNL OT_ID_CHNL
#define HI_ID_RC OT_ID_RC
#define HI_ID_AIO OT_ID_AIO
#define HI_ID_AI OT_ID_AI
#define HI_ID_AO OT_ID_AO
#define HI_ID_AENC OT_ID_AENC
#define HI_ID_ADEC OT_ID_ADEC
#define HI_ID_VPU OT_ID_VPU
#define HI_ID_PCIV OT_ID_PCIV
#define HI_ID_PCIVFMW OT_ID_PCIVFMW
#define HI_ID_ISP OT_ID_ISP
#define HI_ID_IVE OT_ID_IVE
#define HI_ID_USER OT_ID_USER
#define HI_ID_PROC OT_ID_PROC
#define HI_ID_LOG OT_ID_LOG
#define HI_ID_VFMW OT_ID_VFMW
#define HI_ID_GDC OT_ID_GDC
#define HI_ID_PHOTO OT_ID_PHOTO
#define HI_ID_FB OT_ID_FB
#define HI_ID_HDMI OT_ID_HDMI
#define HI_ID_VOIE OT_ID_VOIE
#define HI_ID_TDE OT_ID_TDE
#define HI_ID_HDR OT_ID_HDR
#define HI_ID_PRORES OT_ID_PRORES
#define HI_ID_VGS OT_ID_VGS
#define HI_ID_VCA OT_ID_VCA
#define HI_ID_FD OT_ID_FD
#define HI_ID_OD OT_ID_OD
#define HI_ID_LPR OT_ID_LPR
#define HI_ID_SVP_NNIE OT_ID_SVP_NNIE
#define HI_ID_SVP_DSP OT_ID_SVP_DSP
#define HI_ID_DPU_RECT OT_ID_DPU_RECT
#define HI_ID_DPU_MATCH OT_ID_DPU_MATCH
#define HI_ID_MOTIONSENSOR OT_ID_MOTIONSENSOR
#define HI_ID_MOTIONFUSION OT_ID_MOTIONFUSION
#define HI_ID_GYRODIS OT_ID_GYRODIS
#define HI_ID_PM OT_ID_PM
#define HI_ID_SVP_ALG OT_ID_SVP_ALG
#define HI_ID_IVP OT_ID_IVP
#define HI_ID_MCF OT_ID_MCF
#define HI_ID_SVP_MAU OT_ID_SVP_MAU
#define HI_ID_VDA OT_ID_VDA
#define HI_ID_VPP OT_ID_VPP
#define HI_ID_KCF OT_ID_KCF
#define HI_ID_PQP OT_ID_PQP
#define HI_ID_NPUDEV OT_ID_NPUDEV
#define HI_ID_AICPU OT_ID_AICPU
#define HI_ID_NPUDFX OT_ID_NPUDFX
#define HI_ID_TSFW OT_ID_TSFW
#define HI_ID_CIPHER OT_ID_CIPHER
#define HI_ID_KLAD OT_ID_KLAD
#define HI_ID_KEYSLOT OT_ID_KEYSLOT
#define HI_ID_OTP OT_ID_OTP
#define HI_ID_VDEC_ADAPT OT_ID_VDEC_ADAPT
#define HI_ID_DCC OT_ID_DCC
#define HI_ID_VDEC_SERVER OT_ID_VDEC_SERVER
#define HI_ID_VFMW_MDC OT_ID_VFMW_MDC
#define HI_ID_VB_LOG OT_ID_VB_LOG
#define HI_ID_MCF_CALIBRATION OT_ID_MCF_CALIBRATION
#define HI_ID_SVP_NPU OT_ID_SVP_NPU
#define HI_ID_HNR OT_ID_HNR
#define HI_ID_SNAP OT_ID_SNAP
#define HI_ID_LOG_MDC OT_ID_LOG_MDC
#define HI_ID_VENC_SVR OT_ID_VENC_SVR
#define HI_ID_AB OT_ID_AB
#define HI_ID_VO_DEV OT_ID_VO_DEV
#define HI_ID_IRQ OT_ID_IRQ
#define HI_ID_AIISP OT_ID_AIISP
#define HI_ID_UVC OT_ID_UVC
#define HI_ID_VI_RGBW OT_ID_VI_RGBW
#define HI_ID_ACODEC OT_ID_ACODEC
#define HI_ID_BUTT OT_ID_BUTT
typedef ot_mod_id hi_mod_id;
typedef ot_mpp_chn hi_mpp_chn;
#define HI_PT_PCMU OT_PT_PCMU
#define HI_PT_1016 OT_PT_1016
#define HI_PT_G721 OT_PT_G721
#define HI_PT_GSM OT_PT_GSM
#define HI_PT_G723 OT_PT_G723
#define HI_PT_DVI4_8K OT_PT_DVI4_8K
#define HI_PT_DVI4_16K OT_PT_DVI4_16K
#define HI_PT_LPC OT_PT_LPC
#define HI_PT_PCMA OT_PT_PCMA
#define HI_PT_G722 OT_PT_G722
#define HI_PT_S16BE_STEREO OT_PT_S16BE_STEREO
#define HI_PT_S16BE_MONO OT_PT_S16BE_MONO
#define HI_PT_QCELP OT_PT_QCELP
#define HI_PT_CN OT_PT_CN
#define HI_PT_MPEGAUDIO OT_PT_MPEGAUDIO
#define HI_PT_G728 OT_PT_G728
#define HI_PT_DVI4_3 OT_PT_DVI4_3
#define HI_PT_DVI4_4 OT_PT_DVI4_4
#define HI_PT_G729 OT_PT_G729
#define HI_PT_G711A OT_PT_G711A
#define HI_PT_G711U OT_PT_G711U
#define HI_PT_G726 OT_PT_G726
#define HI_PT_G729A OT_PT_G729A
#define HI_PT_LPCM OT_PT_LPCM
#define HI_PT_CelB OT_PT_CelB
#define HI_PT_JPEG OT_PT_JPEG
#define HI_PT_CUSM OT_PT_CUSM
#define HI_PT_NV OT_PT_NV
#define HI_PT_PICW OT_PT_PICW
#define HI_PT_CPV OT_PT_CPV
#define HI_PT_H261 OT_PT_H261
#define HI_PT_MPEGVIDEO OT_PT_MPEGVIDEO
#define HI_PT_MPEG2TS OT_PT_MPEG2TS
#define HI_PT_H263 OT_PT_H263
#define HI_PT_SPEG OT_PT_SPEG
#define HI_PT_MPEG2VIDEO OT_PT_MPEG2VIDEO
#define HI_PT_AAC OT_PT_AAC
#define HI_PT_WMA9STD OT_PT_WMA9STD
#define HI_PT_HEAAC OT_PT_HEAAC
#define HI_PT_PCM_VOICE OT_PT_PCM_VOICE
#define HI_PT_PCM_AUDIO OT_PT_PCM_AUDIO
#define HI_PT_MP3 OT_PT_MP3
#define HI_PT_ADPCMA OT_PT_ADPCMA
#define HI_PT_AEC OT_PT_AEC
#define HI_PT_X_LD OT_PT_X_LD
#define HI_PT_H264 OT_PT_H264
#define HI_PT_D_GSM_HR OT_PT_D_GSM_HR
#define HI_PT_D_GSM_EFR OT_PT_D_GSM_EFR
#define HI_PT_D_L8 OT_PT_D_L8
#define HI_PT_D_RED OT_PT_D_RED
#define HI_PT_D_VDVI OT_PT_D_VDVI
#define HI_PT_D_BT656 OT_PT_D_BT656
#define HI_PT_D_H263_1998 OT_PT_D_H263_1998
#define HI_PT_D_MP1S OT_PT_D_MP1S
#define HI_PT_D_MP2P OT_PT_D_MP2P
#define HI_PT_D_BMPEG OT_PT_D_BMPEG
#define HI_PT_MP4VIDEO OT_PT_MP4VIDEO
#define HI_PT_MP4AUDIO OT_PT_MP4AUDIO
#define HI_PT_VC1 OT_PT_VC1
#define HI_PT_JVC_ASF OT_PT_JVC_ASF
#define HI_PT_D_AVI OT_PT_D_AVI
#define HI_PT_DIVX3 OT_PT_DIVX3
#define HI_PT_AVS OT_PT_AVS
#define HI_PT_REAL8 OT_PT_REAL8
#define HI_PT_REAL9 OT_PT_REAL9
#define HI_PT_VP6 OT_PT_VP6
#define HI_PT_VP6F OT_PT_VP6F
#define HI_PT_VP6A OT_PT_VP6A
#define HI_PT_SORENSON OT_PT_SORENSON
#define HI_PT_H265 OT_PT_H265
#define HI_PT_VP8 OT_PT_VP8
#define HI_PT_MVC OT_PT_MVC
#define HI_PT_PNG OT_PT_PNG
#define HI_PT_AMR OT_PT_AMR
#define HI_PT_MJPEG OT_PT_MJPEG
#define HI_PT_AMRWB OT_PT_AMRWB
#define HI_PT_PRORES OT_PT_PRORES
#define HI_PT_OPUS OT_PT_OPUS
#define HI_PT_BUTT OT_PT_BUTT
typedef ot_payload_type hi_payload_type;
#define hi_unused ot_unused

#ifdef __cplusplus
}
#endif
#endif /* HI_COMMON_H */

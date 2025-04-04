/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef PROC_EXT_H
#define PROC_EXT_H

#include "ot_osal.h"

#include "ot_type.h"
#include "ot_common.h"

#define PROC_ENTRY_VI        OT_MPP_MOD_VI
#define PROC_ENTRY_VO        OT_MPP_MOD_VO
#define PROC_ENTRY_VO_DEV    OT_MPP_MOD_VO_DEV
#define PROC_ENTRY_VGS       OT_MPP_MOD_VGS
#define PROC_ENTRY_VENC      OT_MPP_MOD_VENC
#define PROC_ENTRY_VDEC      OT_MPP_MOD_VDEC
#define PROC_ENTRY_H264E     OT_MPP_MOD_H264E
#define PROC_ENTRY_H265E     OT_MPP_MOD_H265E
#define PROC_ENTRY_SVAC3E    OT_MPP_MOD_SVAC3E
#define PROC_ENTRY_H264D     OT_MPP_MOD_H264D
#define PROC_ENTRY_JPEGE     OT_MPP_MOD_JPEGE
#define PROC_ENTRY_JPEGD     OT_MPP_MOD_JPEGD
#define PROC_ENTRY_PRORES    OT_MPP_MOD_PRORES
#define PROC_ENTRY_MPEG4E    OT_MPP_MOD_MPEG4E
#define PROC_ENTRY_CHNL      OT_MPP_MOD_CHNL
#define PROC_ENTRY_AIO       OT_MPP_MOD_AIO
#define PROC_ENTRY_AI        OT_MPP_MOD_AI
#define PROC_ENTRY_AO        OT_MPP_MOD_AO
#define PROC_ENTRY_AENC      OT_MPP_MOD_AENC
#define PROC_ENTRY_ADEC      OT_MPP_MOD_ADEC
#define PROC_ENTRY_ACODEC    OT_MPP_MOD_ACODEC
#define PROC_ENTRY_AB        OT_MPP_MOD_AB
#define PROC_ENTRY_VB        OT_MPP_MOD_VB
#define PROC_ENTRY_VB_USER   OT_MPP_MOD_VB_USER
#define PROC_ENTRY_GRP       OT_MPP_MOD_GRP
#define PROC_ENTRY_VPSS      OT_MPP_MOD_VPSS
#define PROC_ENTRY_RGN       OT_MPP_MOD_RGN
#define PROC_ENTRY_SYS       OT_MPP_MOD_SYS
#define PROC_ENTRY_SYS_USER  OT_MPP_MOD_SYS_USER
#define PROC_ENTRY_PCIV      OT_MPP_MOD_PCIV
#define PROC_ENTRY_LOG       OT_MPP_MOD_LOG
#define PROC_ENTRY_LOG_USER  OT_MPP_MOD_LOG_USER
#define PROC_ENTRY_DCCM      OT_MPP_MOD_DCCM
#define PROC_ENTRY_DCCS      OT_MPP_MOD_DCCS
#define PROC_ENTRY_MST_LOG   OT_MPP_MOD_MST_LOG
#define PROC_ENTRY_PCIVFMW   OT_MPP_MOD_PCIVFMW
#define PROC_ENTRY_RC        OT_MPP_MOD_RC
#define PROC_ENTRY_IVE       OT_MPP_MOD_IVE
#define PROC_ENTRY_FD        OT_MPP_MOD_FD
#define PROC_ENTRY_SVP       OT_MPP_MOD_SVP
#define PROC_ENTRY_SVP_NNIE  OT_MPP_MOD_SVP_NNIE
#define PROC_ENTRY_SVP_ALG   OT_MPP_MOD_SVP_ALG
#define PROC_ENTRY_SVP_DSP   OT_MPP_MOD_SVP_DSP
#define PROC_ENTRY_SVP_MAU   OT_MPP_MOD_SVP_MAU
#define PROC_ENTRY_DPU_RECT  OT_MPP_MOD_DPU_RECT
#define PROC_ENTRY_DPU_MATCH OT_MPP_MOD_DPU_MATCH
#define PROC_ENTRY_IVP       OT_MPP_MOD_IVP
#define PROC_ENTRY_VDA       OT_MPP_MOD_VDA
#define PROC_ENTRY_PQP       OT_MPP_MOD_PQP


#define PROC_ENTRY_MD        OT_MPP_MOD_MD
#define PROC_ENTRY_HDMI      OT_MPP_MOD_HDMI
#define PROC_ENTRY_ISP       OT_MPP_MOD_ISP
#define PROC_ENTRY_DIS       OT_MPP_MOD_DIS
#define PROC_ENTRY_GYRODIS   OT_MPP_MOD_GYRODIS
#define PROC_ENTRY_GDC       OT_MPP_MOD_GDC
#define PROC_ENTRY_AVS       OT_MPP_MOD_AVS
#define PROC_ENTRY_MCF       OT_MPP_MOD_MCF
#define PROC_ENTRY_PM        OT_MPP_MOD_PM
#define PROC_ENTRY_MFUSION   OT_MPP_MOD_MFUSION

#define PROC_ENTRY_SVP_NPU   OT_MPP_MOD_SVP_NPU
#define PROC_ENTRY_NPU_DEV   OT_MPP_MOD_NPU_DEV
#define PROC_ENTRY_NPU_AICPU OT_MPP_MOD_NPU_AICPU
#define PROC_ENTRY_NPU_DFX   OT_MPP_MOD_NPU_DFX
#define PROC_ENTRY_NPU_TSFW  OT_MPP_MOD_NPU_TSFW

#define PROC_ENTRY_DCC       OT_MPP_MOD_DCC
#define PROC_ENTRY_VDEC_SERVER  OT_MPP_MOD_VDEC_SERVERS
#define PROC_ENTRY_VDEC_ADAPT   OT_MPP_MOD_VDEC_ADAPT

#define PROC_ENTRY_CIPHER    OT_MPP_MOD_CIPHER
#define PROC_ENTRY_KLAD      OT_MPP_MOD_KLAD
#define PROC_ENTRY_OTP       OT_MPP_MOD_OTP
#define PROC_ENTRY_UVC       OT_MPP_MOD_UVC

#define CMPI_PROC_SHOW       OT_CMPI_PROC_READ
typedef td_s32 (*CMPI_PROC_READ)(struct osal_proc_dir_entry *entry);
typedef td_s32 (*CMPI_PROC_WRITE)(char *buf, int count);

td_s32 cmpi_log_init(td_u32 log_buf_len);
td_void cmpi_log_exit(td_void);
td_s32 cmpi_log_sys_init(td_void);
td_s32 cmpi_log_sys_deinit(td_void);

#endif

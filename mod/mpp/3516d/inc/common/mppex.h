#ifndef __mppex_h__
#define __mppex_h__

/*
 * NOTICE:
 *  This file only for hiview-tech
 * DESCRIBE:
 *  Support yuv input:
 *  v MIPI_YUV_2M_60FPS_8BIT
 *  v MIPI_YUV_8M_30FPS_8BIT
 *  v MIPI_YUVPKG_2M_60FPS_8BIT
 *  v BT1120_YUV_2M_60FPS_8BIT
 *  v BT656_YUV_0M_60FPS_8BIT
 *  v BT656_YUV_512P_60FPS_8BIT
 *  v BT656N_YUV_0M_60FPS_8BIT
 *  v BT656_YUV_288P_60FPS_8BIT
 */

#include "mpp.h"

int mppex_GetComboAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, combo_dev_t MipiDev, combo_dev_attr_t* pstComboAttr);
int mppex_GetDevAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, VI_DEV_ATTR_S* pstViDevAttr);
int mppex_GetPipeAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, VI_PIPE_ATTR_S* pstPipeAttr);
int mppex_GetChnAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, VI_CHN_ATTR_S* pstChnAttr);
int mppex_GetSizeBySensor(SAMPLE_SNS_TYPE_E enMode, PIC_SIZE_E* penSize);
int mppex_GetFrameRateBySensor(SAMPLE_SNS_TYPE_E enMode, HI_U32* pu32FrameRate);
int mppex_GetComboDevBySensor(SAMPLE_SNS_TYPE_E enMode, HI_S32 s32SnsIdx, combo_dev_t* dev);
int mppex_SetPipeAttr(VI_PIPE ViPipe, VI_PIPE_ATTR_S *pstPipeAttr;);

int mppex_hook_register();
int mppex_hook_sns(gsf_mpp_cfg_t *cfg);
int mppex_hook_vi(SAMPLE_VI_CONFIG_S *pstViConfig, int viLowDelay);
int mppex_hook_vpss_bb(gsf_mpp_vpss_t *vpss);
int mppex_hook_vpss_ee(gsf_mpp_vpss_t *vpss);
int mppex_hook_venc_bb(gsf_mpp_venc_t *venc);
int mppex_hook_venc_ee(gsf_mpp_venc_t *venc);
int mppex_hook_vo(VO_INTF_SYNC_E sync);
int mppex_hook_destroy();

#endif //__mppex_h__
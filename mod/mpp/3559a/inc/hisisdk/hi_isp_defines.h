
/******************************************************************************

  Copyright (C), 2016, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_isp_defines.h

  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/11/07
  Description   :
  History       :
  1.Date        : 2013/11/07
    Author      :
    Modification: Created file

******************************************************************************/
#ifndef __HI_ISP_DEFINES_H__
#define __HI_ISP_DEFINES_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/* isp sys part */
#define ISP_STRIPING_MAX_NUM                 (3)
#define ISP_NORMAL_BLOCK_NUM                 (1)
#define ISP_DEFAULT_BLOCK_NUM                (2)
#define ISP_SBS_BLOCK_NUM                    (2)

#define ISP_BE0_PIPE_ID                      (0)
#define ISP_BE1_PIPE_ID                      (4)
#define ISP_MAX_BE_NUM                       (2)
#define ISP_MAX_STITCH_NUM                   (8)

#define ISP_MAX_PIPE_NUM                     (8)
#define ISP_STRIPING_OVERLAP_DEFAULT         (128)

#define ISP_WDR_CHN_MAX                      (4)
#define ISP_SUPPORT_DE_MODULE                (0)

#define ISP_RES_WIDTH_MAX  8192,8192,8192,8192,8192,8192,8192,8192
#define ISP_RES_HEIGHT_MAX 8192,8192,8192,8192,8192,8192,8192,8192

extern HI_U16 g_au16ResWMax[ISP_MAX_PIPE_NUM];
extern HI_U16 g_au16ResHMax[ISP_MAX_PIPE_NUM];

/* isp alg part */
#define HI_ISP_CLUT_LUT_LENGTH               (22167)
#define HI_ISP_CA_YRATIO_LUT_LENGTH          (256)

#define STATIC_DP_COUNT_NORMAL               (4096)
#define STATIC_DP_COUNT_MAX                  (STATIC_DP_COUNT_NORMAL * ISP_SBS_BLOCK_NUM)
#define ISP_SUPPORT_OFFLINE_DPC_CALIBRATION  (0)

#define HI_ISP_SPECAWB_FACT_ELEMENT_LUT0     (4096)
#define HI_ISP_SPECAWB_FACT_ELEMENT_LUT1     (4096)
#define HI_ISP_SPECAWB_FACT_ELEMENT_LUT2     (4096)
#define HI_ISP_SPECAWB_FACT_ELEMENT_LUT3     (4096)
#define HI_ISP_SPECAWB_FACT_ELEMENT_LUT4     (4096)
#define HI_ISP_SPECAWB_FACT_ELEMENT_LUT5     (4096)
#define HI_ISP_SPECAWB_FACT_ELEMENT_LUT6     (4096)
#define HI_ISP_SPECAWB_WB_CNVTBL_LUT         (16384)
#define HI_ISP_SPECAWB_KELDBB_LUT            (16384)
#define HI_ISP_SPECAWB_BBL_LUT               (240)
#define HI_ISP_SPECAWB_CAA_LUT1              (16384)
#define HI_ISP_SPECAWB_CAA_LUT2              (16384)
#define HI_ISP_SPECAWB_CAA_LUT3              (16384)
#define ISP_SPECAWB_BUF_SIZE                 (110832)

#define AWB_ZONE_BIN                         (4)
#define AWB_LIB_NUM                          (8)
#define AE_LIB_NUM                           (8)

#define HI_ISP_SHARPEN_RGAIN                 (31)
#define HI_ISP_SHARPEN_BGAIN                 (31)
#define HI_ISP_SHARPEN_RGAIN_BIT             (31)
#define HI_ISP_SHARPEN_BGAIN_BIT             (31)

#define HI_ISP_DRC_STRENGTH_MAX              (255)
#define HI_ISP_DRC_SPA_FLT_COEF_MAX          (10)

#define HI_ISP_DEMOSAIC_DETAIL_SMOOTH_RANGE_MAX (8)

#define HI_ISP_LDCI_HEPOSWGT_MAX             (0x80)
#define HI_ISP_LDCI_HENEGWGT_MAX             (0x80)
#define HI_ISP_PREGAMMA_LUT_MAX              (0x100000)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __HI_ISP_DEFINES_H__ */

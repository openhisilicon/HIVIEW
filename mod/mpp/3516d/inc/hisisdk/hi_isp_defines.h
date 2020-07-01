/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author: Hisilicon multimedia software group
* Create: 2011/06/28
*/

#ifndef __HI_ISP_DEFINES_H__
#define __HI_ISP_DEFINES_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/* isp sys part */
#define ISP_STRIPING_MAX_NUM                      (3)
#define ISP_NORMAL_BLOCK_NUM                      (1)
#define ISP_DEFAULT_BLOCK_NUM                     (2)
#define ISP_SBS_BLOCK_NUM                         (2)

#define ISP_BE0_PIPE_ID                           (0)
#define ISP_BE1_PIPE_ID                           (8)
#define ISP_MAX_BE_NUM                            (1)
#define ISP_MAX_STITCH_NUM                        (1)

#define ISP_MAX_PHY_PIPE_NUM                      (4)
#define ISP_MAX_VIR_PIPE_NUM                      (0)
#define ISP_MAX_PIPE_NUM                          (ISP_MAX_PHY_PIPE_NUM + ISP_MAX_VIR_PIPE_NUM)

#define ISP_STRIPING_OVERLAP_DEFAULT              (256)

#define ISP_WDR_CHN_MAX                           (2)
#define ISP_SUPPORT_DE_MODULE                     (1)

#define SENSOR_RES_WIDTH_MAX                      4608
#define SENSOR_RES_HEIGHT_MAX                     4608
#define ISP_RES_WIDTH_MAX                         4608,3840,2048,2048 // need change when pipe num is change
#define ISP_RES_HEIGHT_MAX                        4608,3840,2048,2048 // need change when pipe num is change

extern HI_U16 g_au16ResWMax[ISP_MAX_PIPE_NUM];
extern HI_U16 g_au16ResHMax[ISP_MAX_PIPE_NUM];

/* isp alg part */
#define HI_ISP_CLUT_LUT_LENGTH                    (5508)
#define HI_ISP_CA_YRATIO_LUT_LENGTH               (128)


#define STATIC_DP_COUNT_NORMAL                    (2048)
#define STATIC_DP_COUNT_MAX                       (STATIC_DP_COUNT_NORMAL * ISP_STRIPING_MAX_NUM)
#define ISP_SUPPORT_OFFLINE_DPC_CALIBRATION       (1)

#define HI_ISP_SPECAWB_FACT_ELEMENT_LUT0          (4096)
#define HI_ISP_SPECAWB_FACT_ELEMENT_LUT1          (4096)
#define HI_ISP_SPECAWB_FACT_ELEMENT_LUT2          (4096)
#define HI_ISP_SPECAWB_FACT_ELEMENT_LUT3          (4096)
#define HI_ISP_SPECAWB_FACT_ELEMENT_LUT4          (4096)
#define HI_ISP_SPECAWB_FACT_ELEMENT_LUT5          (4096)
#define HI_ISP_SPECAWB_FACT_ELEMENT_LUT6          (4096)
#define HI_ISP_SPECAWB_WB_CNVTBL_LUT              (16384)
#define HI_ISP_SPECAWB_KELDBB_LUT                 (16384)
#define HI_ISP_SPECAWB_BBL_LUT                    (240)
#define HI_ISP_SPECAWB_CAA_LUT1                   (16384)
#define HI_ISP_SPECAWB_CAA_LUT2                   (16384)
#define HI_ISP_SPECAWB_CAA_LUT3                   (16384)
#define ISP_SPECAWB_BUF_SIZE                      (110832)

#define AWB_ZONE_BIN                              (1)
#define AWB_LIB_NUM                               (ISP_MAX_PIPE_NUM)
#define AE_LIB_NUM                                (ISP_MAX_PIPE_NUM)

#define HI_ISP_SHARPEN_RGAIN                      (31)
#define HI_ISP_SHARPEN_RGAIN_MAX                  (31)
#define HI_ISP_SHARPEN_RGAIN_BIT                  (31)
#define HI_ISP_SHARPEN_GGAIN                      (32)
#define HI_ISP_SHARPEN_GGAIN_MAX                  (255)
#define HI_ISP_SHARPEN_GGAIN_BIT                  (255)
#define HI_ISP_SHARPEN_BGAIN                      (31)
#define HI_ISP_SHARPEN_BGAIN_MAX                  (31)
#define HI_ISP_SHARPEN_BGAIN_BIT                  (31)
#define HI_ISP_SHARPEN_LUMAWGT_MAX                (127)
#define HI_ISP_SHARPEN_LUMAWGT                    (127)
#define HI_ISP_SHARPEN_LUMAWGT_BIT                (127)
#define HI_ISP_SHARPEN_EdgeFiltMaxCap        (18)
#define HI_ISP_SHARPEN_EdgeFiltMaxCap_MAX    (47)
#define HI_ISP_SHARPEN_EdgeFiltMaxCap_BIT    (63)


#define GAMMA_REG_NODE_NUM                        (257)
#define GAMMA_OUTSEG_NUM                          (8)

#define HI_ISP_DRC_STRENGTH_MAX                   (1023)
#define HI_ISP_DRC_SPA_FLT_COEF_MAX               (5)

#define HI_ISP_DEMOSAIC_DETAIL_SMOOTH_RANGE_MAX   (7)
#define HI_ISP_DEMOSAIC_DETAIL_SMOOTH_RANGE_MIN   (1)
#define HI_ISP_DEMOSAIC_NONDIR_MFDETALEHC_STR_MAX (127)
#define HI_ISP_DEMOSAIC_COLOR_NOISE_THDY_MAX       (255)
#define HI_ISP_DEMOSAIC_COLOR_NOISE_STRY_MAX       (255)

#define HI_ISP_LDCI_HEPOSWGT_MAX                  (255)
#define HI_ISP_LDCI_HENEGWGT_MAX                  (255)
#define HI_ISP_PREGAMMA_LUT_MAX                   (0xFFFFF)

#define EXPANDER_NODE_NUM                         (257)

#define DEFOG_ZONE_ROW                            (32)
#define DEFOG_ZONE_COLUMN                         (32)
#define DEFOG_ZONE_NUM                            (1024)
#define DEFOG_MAX_ZONE_NUM                        (1024)

#define HI_ISP_LSC_MESHSTR_MAX                    (65535)
#define HI_ISP_LSC_DEFAULT_MESH_STRENGTH          (4096)
#define HI_ISP_LSC_DEFAULT_WEIGHT                 (256)
#define HI_ISP_LSC_GRID_COL                       (33)
#define HI_ISP_LSC_GRID_ROW                       (33)
#define HI_ISP_LSC_GRID_POINTS                    (HI_ISP_LSC_GRID_COL * HI_ISP_LSC_GRID_ROW)
#define HI_ISP_LSC_DEFAULT_MESH_SCALE             (4)
#define HI_ISP_LSC_XGRID_WIDTH_MAX(x)        ((x) / 4 - 60)
#define HI_ISP_LSC_YGRID_WIDTH_MAX(x)        ((x) / 4 - 60)

#define AF_GAMMA_VALUE_MAX                        (6)
#define HI_EXT_SYSTEM_MANUAL_DEHAZE_HBLK_DEFAULT  (32)
#define HI_EXT_SYSTEM_MANUAL_DEHAZE_VBLK_DEFAULT  (32)

#define HI_ISP_CR_SLOPE_MAX                       (14)
#define HI_ISP_CR_THR_MAX                         (16383)
#define HI_ISP_CR_NPOFFSET_MAX                    (16383)
#define HI_ISP_CR_NPOFFSET_MIN                    (512)

#define HI_ISP_FE_AF_SUPPORT                      0
#define HI_ISP_BE_AF_SUPPORT                      1
#define HI_ISP_AF_ENABLE_MASK                     ((HI_ISP_BE_AF_SUPPORT << 1) + HI_ISP_FE_AF_SUPPORT)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __HI_ISP_DEFINES_H__ */

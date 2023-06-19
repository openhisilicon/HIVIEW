/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_ISP_DEFINE_H
#define OT_ISP_DEFINE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/* isp sys part */
#define OT_ISP_BE_BUF_NUM_MAX                     20
#define OT_ISP_STRIPING_MAX_NUM                   3
#define OT_ISP_SBS_BLOCK_NUM                      2

#define OT_ISP_MAX_PHY_PIPE_NUM                   4
#define OT_ISP_MAX_VIR_PIPE_NUM                   12
#define OT_ISP_MAX_PIPE_NUM                       (OT_ISP_MAX_PHY_PIPE_NUM + OT_ISP_MAX_VIR_PIPE_NUM)
#define OT_ISP_MAX_STITCH_NUM                     OT_ISP_MAX_PHY_PIPE_NUM

#define OT_ISP_SENSOR_WIDTH_MAX                   8192
#define OT_ISP_SENSOR_HEIGHT_MAX                  8192
#define OT_ISP_RES_WIDTH_MAX                      8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, \
                                                  8192, 8192, 8192, 8192, 8192, 8192
#define OT_ISP_RES_HEIGHT_MAX                     8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, \
                                                  8192, 8192, 8192, 8192, 8192, 8192

/* isp alg part */
#define OT_ISP_CLUT_LUT_LENGTH                    5508
#define OT_ISP_CA_YRATIO_LUT_LENGTH               256

#define OT_ISP_STATIC_DP_COUNT_NORMAL              2048
#define OT_ISP_STATIC_DP_COUNT_MAX                 (OT_ISP_STATIC_DP_COUNT_NORMAL * OT_ISP_STRIPING_MAX_NUM)
#define OT_ISP_SUPPORT_OFFLINE_DPC_CALIBRATION      1

#define OT_ISP_AWB_ZONE_ORIG_ROW                    32
#define OT_ISP_AWB_ZONE_ORIG_COLUMN                 32

#define OT_ISP_SPECAWB_FACTTBL_ELMNUM             7
#define OT_ISP_SPECAWB_FACTTBL_SIZE               64
#define OT_ISP_SPECAWB_BBL_SIZE                   40

#define OT_ISP_SPECAWB_FACT_ELEMENT_LUT           4096
#define OT_ISP_SPECAWB_WB_CNVTBL_LUT              16384
#define OT_ISP_SPECAWB_KELDBB_LUT                 16384
#define OT_ISP_SPECAWB_BBL_LUT                    240
#define OT_ISP_SPECAWB_CAA_LUT                    16384
#define OT_ISP_SPECAWB_BUF_SIZE                   110832
#define OT_ISP_SPECAWB_WB_CNVTBL_LUT_OFFSET       (OT_ISP_SPECAWB_FACT_ELEMENT_LUT * OT_ISP_SPECAWB_FACTTBL_ELMNUM)
#define OT_ISP_SPECAWB_KELDBB_LUT_OFFSET          (OT_ISP_SPECAWB_WB_CNVTBL_LUT_OFFSET + OT_ISP_SPECAWB_WB_CNVTBL_LUT)
#define OT_ISP_SPECAWB_BBL_LUT_OFFSET             (OT_ISP_SPECAWB_KELDBB_LUT_OFFSET + OT_ISP_SPECAWB_KELDBB_LUT)
#define OT_ISP_SPECAWB_CAA_LUT_OFFSET             (OT_ISP_SPECAWB_BBL_LUT_OFFSET + OT_ISP_SPECAWB_BBL_LUT)

#define OT_ISP_AWB_LIB_NUM                        (OT_ISP_MAX_PIPE_NUM)
#define OT_ISP_AE_LIB_NUM                         (OT_ISP_MAX_PIPE_NUM)

#define OT_ISP_SHARPEN_RGAIN                      31
#define OT_ISP_SHARPEN_RGAIN_MAX                  31
#define OT_ISP_SHARPEN_RGAIN_BIT                  31
#define OT_ISP_SHARPEN_GGAIN                      32
#define OT_ISP_SHARPEN_GGAIN_MAX                  255
#define OT_ISP_SHARPEN_GGAIN_BIT                  255
#define OT_ISP_SHARPEN_BGAIN                      31
#define OT_ISP_SHARPEN_BGAIN_MAX                  31
#define OT_ISP_SHARPEN_BGAIN_BIT                  31
#define OT_ISP_SHARPEN_LUMAWGT_MAX                31
#define OT_ISP_SHARPEN_LUMAWGT                    31
#define OT_ISP_SHARPEN_LUMAWGT_BIT                31
#define OT_ISP_SHARPEN_EDGE_FILT_MAX_CAP          18
#define OT_ISP_SHARPEN_EDGE_FILT_MAX_CAP_MAX      47
#define OT_ISP_SHARPEN_EDGE_FILT_MAX_CAP_BIT      63

#define OT_ISP_DRC_STRENGTH_MAX                   1023
#define OT_ISP_DRC_SPA_FLT_COEF_MAX               5
#define OT_ISP_DRC_CC_NODE_NUM                    33
#define OT_ISP_DRC_TM_NODE_NUM                    200
#define OT_ISP_DRC_CC_COEF_MAX                    0x400
#define OT_ISP_DRC_SHP_LOG_MIN                    0

#define OT_ISP_DEMOSAIC_DETAIL_SMOOTH_RANGE_MAX   7
#define OT_ISP_DEMOSAIC_DETAIL_SMOOTH_RANGE_MIN   1
#define OT_ISP_DEMOSAIC_NONDIR_MFDETALEHC_STR_MAX 127
#define OT_ISP_DEMOSAIC_COLOR_NOISE_THDY_MAX      15
#define OT_ISP_DEMOSAIC_COLOR_NOISE_STRY_MAX      63

#define OT_ISP_LDCI_HEPOSWGT_MAX                  255
#define OT_ISP_LDCI_HENEGWGT_MAX                  255

#define OT_ISP_PREGAMMA_LUT_MAX                   0xFFFFF
#define OT_ISP_AGAMMA_LUT_MAX                     0xFFFFF
#define OT_ISP_ADGAMMA_LUT_MAX                    0xFFFFF

#define OT_ISP_LSC_MESHSTR_MAX                    65535
#define OT_ISP_LSC_DEFAULT_MESH_STRENGTH          4096
#define OT_ISP_LSC_DEFAULT_WEIGHT                 256
#define OT_ISP_LSC_GRID_COL                       33
#define OT_ISP_LSC_GRID_ROW                       33
#define OT_ISP_LSC_GRID_POINTS                    (OT_ISP_LSC_GRID_COL * OT_ISP_LSC_GRID_ROW)
#define OT_ISP_LSC_DEFAULT_MESH_SCALE             3
#define ot_isp_lsc_xgrid_width_max(x)             ((x) / 4 - 60)
#define ot_isp_lsc_ygrid_width_max(x)             ((x) / 4 - 60)
#define OT_ISP_LSC_MESHSCALE_NUM                  8
#define OT_ISP_LSC_MESHSCALE0_DEF_GAIN            512
#define OT_ISP_LSC_MESHSCALE1_DEF_GAIN            256
#define OT_ISP_LSC_MESHSCALE2_DEF_GAIN            128
#define OT_ISP_LSC_MESHSCALE3_DEF_GAIN            64
#define OT_ISP_LSC_MAX_GAIN                       1023
#define OT_ISP_LSC_GAIN_BITWIDTH                  10

#define OT_ISP_AF_GAMMA_VALUE_MAX                 6

#define OT_ISP_CR_SLOPE_MAX                       12
#define OT_ISP_CR_THR_MAX                         4095
#define OT_ISP_CR_NPOFFSET_MAX                    4095
#define OT_ISP_CR_NPOFFSET_MIN                    512

#define OT_ISP_FE_AF_SUPPORT                      1
#define OT_ISP_BE_AF_SUPPORT                      1
#define OT_ISP_AF_ENABLE_MASK                     ((OT_ISP_BE_AF_SUPPORT << 1) + OT_ISP_FE_AF_SUPPORT)

#define OT_ISP_RGBIR_CVTMAT_NUM 12
#define OT_ISP_RGBIR_CTRL_NUM 2
#define OT_ISP_RGBIR_CROSSTALK_NUM 3
#define OT_ISP_RGBIR_LUT_NUM 33
#define OT_ISP_CRB_LUT_LENGTH                     1024
#define OT_ISP_CRB_GAIN_MAX                       2047
#define OT_ISP_CRB_GAIN_MIN                       511

#define OT_ISP_CA_RATIO_MAX                       2047
#define OT_ISP_CLUT_GAIN_MAX                      4095

#define OT_ISP_CCM_LUMA_COEFR_DEFAULT             8
#define OT_ISP_CCM_LUMA_COEFB_DEFAULT             4
#define OT_ISP_CCM_DARKPREV_DEFAULT               14
#define OT_ISP_CCM_LUMA_COEFR_UP_DEFAULT          10
#define OT_ISP_CCM_LUMA_COEFB_UP_DEFAULT          10
#define OT_ISP_CCM_SOFT_CLIP0_STEP_DEFAULT        10
#define OT_ISP_CCM_SOFT_CLIP1_STEP_DEFAULT        14
#define OT_ISP_CCM_PEAKSUPP_SAT_DEFAULT           50
#define OT_ISP_CCM_PEAKSUPP_MAX_DEFAULT           255
#define OT_ISP_CCM_DARK_RANGE_MAX                 15

#define OT_ISP_EXPANDER_NODE_NUM               257

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* OT_ISP_DEFINE_H */

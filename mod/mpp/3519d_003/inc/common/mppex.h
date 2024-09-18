#ifndef __mppex_h__
#define __mppex_h__

/*
 * NOTICE:
 *  This file only for hiview-tech
 * DESCRIBE:
 *  Support yuv input:
 *  v SONY_IMX378_MIPI_8M_30FPS_10BIT
 *  v SONY_IMX585_MIPI_8M_30FPS_12BIT
 *  v SONY_IMX586_MIPI_48M_5FPS_12BIT
 *  v SONY_IMX586_MIPI_8M_30FPS_12BIT
 *  v SONY_IMX482_MIPI_2M_30FPS_12BIT
 *  v OV_OS04A10_2L_MIPI_4M_30FPS_10BIT
 *  v OV_OS08A20_2L_MIPI_2M_30FPS_10BIT
 *  v MIPI_YUV422_2M_60FPS_8BIT
 *  v BT1120_YUV422_2M_60FPS_8BIT
 *  v BT656_YUV422_0M_60FPS_8BIT
 *  v BT601_YUV422_0M_60FPS_8BIT
 *  v SONY_IMX664_MIPI_4M_30FPS_12BIT
 *  v SONY_IMX335_MIPI_5M_30FPS_12BIT
 *  v SONY_IMX335_2L_MIPI_5M_30FPS_10BIT
 *  v SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1
 *  v MIPI_YUV422_2M_30FPS_8BIT_6CH
 *  v SONY_IMX327_MIPI_2M_30FPS_12BIT
 *  v SONY_IMX327_2L_MIPI_2M_30FPS_12BIT
 *  v SONY_IMX678_MIPI_8M_30FPS_12BIT
 */

#include "mpp.h"

int mppex_comm_vi_get_mipi_attr(sample_sns_type sns_type, combo_dev_attr_t *combo_attr, hi_u32 *ob_height);

int mppex_comm_vi_get_mipi_ext_data_attr(sample_sns_type sns_type, ext_data_type_t *ext_data_attr);

int mppex_comm_vi_get_mipi_attr_by_dev_id(sample_sns_type sns_type, hi_vi_dev vi_dev,
                                                      combo_dev_attr_t *combo_attr, hi_u32 *ob_height);
                                                      
int mppex_comm_vi_get_dev_attr_by_intf_mode(hi_vi_intf_mode intf_mode, hi_vi_dev_attr *dev_attr);

int mppex_comm_vi_get_size_by_sns_type(sample_sns_type sns_type, hi_size *size);

int mppex_comm_vi_get_obheight_by_sns_type(sample_sns_type sns_type, hi_u32 *ob_height);

int mppex_comm_vi_get_pipe_num_by_sns_type(sample_sns_type sns_type);

int mppex_comm_vi_get_default_dev_info(sample_sns_type sns_type, sample_vi_dev_info *dev_info);

int mppex_comm_vi_get_default_pipe_info(sample_sns_type sns_type, int i,
                                             sample_vi_pipe_info pipe_info[], const hi_size *size);
                                             
int mppex_comm_vi_bypass_isp(const sample_vi_cfg *vi_cfg);

int mppex_comm_isp_get_pub_attr_by_sns(sample_sns_type sns_type, hi_isp_pub_attr *pub_attr);

hi_wdr_mode mppex_comm_vi_get_wdr_mode_by_sns_type(sample_sns_type sns_type);

int mppex_comm_vi_bb(int snscnt, sample_vi_cfg vi_cfg[HI_VI_MAX_DEV_NUM]);
int mppex_comm_vi_ee(int snscnt, sample_vi_cfg vi_cfg[HI_VI_MAX_DEV_NUM]);
int mppex_comm_vi_tt(int snscnt, sample_vi_cfg vi_cfg[HI_VI_MAX_DEV_NUM]);

int mppex_comm_vpss_bb(gsf_mpp_vpss_t *vpss, sample_venc_vpss_chn_attr *vpss_param);
int mppex_comm_vpss_ee(gsf_mpp_vpss_t *vpss, sample_venc_vpss_chn_attr *vpss_param);

hi_video_frame_info* mppex_comm_vpss_send_bb(int VpssGrp, int VpssGrpPipe, hi_video_frame_info *pstVideoFrame);

int mppex_comm_uvc_get(int idx, ot_video_frame_info *pstFrameInfo, int s32MilliSec);
int mppex_comm_uvc_rel(int idx, ot_video_frame_info *pstFrameInfo);


int mppex_hook_sns(gsf_mpp_cfg_t *cfg);
int mppex_hook_destroy(void);

typedef struct
{
  int data_seq;     // hi_vi_data_seq;
  int width, height;// sns_size;
  hi_rect crop;     // {x, y, width, height};
}mppex_bt656_cfg_t;
int mppex_comm_bt656_cfg(mppex_bt656_cfg_t *bt656);

#endif //__mppex_h__
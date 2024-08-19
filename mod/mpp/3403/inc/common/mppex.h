#ifndef __mppex_h__
#define __mppex_h__

/*
 * NOTICE:
 *  This file only for hiview-tech
 * DESCRIBE:
 *  Support yuv input:
 *  v SONY_IMX378_MIPI_8M_30FPS_10BIT
 *  v SONY_IMX585_MIPI_8M_30FPS_12BIT
 *  v SONY_IMX585_MIPI_8M_60FPS_10BIT
 *  v SONY_IMX482_MIPI_2M_30FPS_12BIT
 *  v MIPI_YUV422_2M_60FPS_8BIT
 *  v MIPI_YUV422_half8M_60FPS_8BIT
 *  v BT1120_YUV422_2M_60FPS_8BIT
 *  v BT656_YUV422_0M_60FPS_8BIT
 *  v BT601_YUV422_0M_60FPS_8BIT
 *  v SONY_IMX586_MIPI_48M_5FPS_12BIT
 *  v SONY_IMX415_MIPI_8M_30FPS_12BIT
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

hi_video_frame_info* mppex_comm_vpss_send_bb(int VpssGrp, int VpssGrpPipe, hi_video_frame_info *pstVideoFrame);

int mppex_comm_uvc_get(int idx, ot_video_frame_info *pstFrameInfo, int s32MilliSec);
int mppex_comm_uvc_rel(int idx, ot_video_frame_info *pstFrameInfo);

int mppex_hook_sns(gsf_mpp_cfg_t *cfg);
int mppex_hook_destroy(void);

#endif //__mppex_h__
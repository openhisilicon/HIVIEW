#ifndef __VPSS_CAPTURE_H
#define __VPSS_CAPTURE_H

#include <stdio.h>
#include <stdbool.h>

#include "sample_comm.h"
#include "sample_common_svp.h"
#include "sample_common_ive.h"
#include <opencv2/opencv.hpp>

typedef struct
{
  hi_vb_blk vb_blk;
  hi_vb_pool vb_pool;
  hi_u32 pool_id;
  hi_phys_addr_t phys_addr;
  hi_void *virt_addr;
} vpss_dump_mem;

class VpssCapture
{
public:

	int init(int vpss_grp, int vpss_chn);
	int get_frame(cv::Mat &cv_mat);
	int get_frame_lock(cv::Mat &cv_mat, hi_video_frame_info **hi_frame);
  int get_frame_unlock(hi_video_frame_info *hi_frame);
	int destroy();

private:
  int yuv2mat(hi_video_frame *frame, cv::Mat &cv_mat);
  hi_void vpss_restore_depth(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn);
	hi_void vpss_restore(hi_vpss_grp hi_vpss_grp, hi_vpss_chn hi_vpss_chn);
	
private:
	hi_vpss_grp g_vpss_grp   = 0;
	hi_vpss_chn g_vpss_chn   = 0;
	hi_u32 milli_sec         = -1;
	hi_u32 g_orig_depth      = 0;
	hi_vpss_chn_mode g_orig_chn_mode = HI_VPSS_CHN_MODE_USER;
	hi_u32 g_vpss_depth_flag = 0;
	hi_video_frame_info g_frame = {0};
	
	//vgs;
	hi_vb_pool g_pool  = HI_VB_INVALID_POOL_ID;
	vpss_dump_mem g_dump_mem = {0};
	hi_vgs_handle g_handle = -1;
	hi_u32 g_blk_size = 0;
	
  //memcpy;
  hi_u32 g_size = 0;
  hi_u32 g_c_size = 0;
	hi_char* g_user_page_addr[2] = {HI_NULL, HI_NULL};
	
	//lock;
	hi_u32 frame_id = 0;
	hi_u32 frame_lock = 0;
	
	//csc;
	hi_svp_dst_img dst_img = {0};
	cv::Mat dst_mat;
};


#endif


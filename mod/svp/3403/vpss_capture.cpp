#include "vpss_capture.h"

hi_void VpssCapture::vpss_restore_depth(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn)
{
    hi_s32 ret;
    hi_vpss_chn_attr chn_attr;
    hi_vpss_ext_chn_attr ext_chn_attr;

    if (vpss_chn >= HI_VPSS_MAX_PHYS_CHN_NUM) {
        ret = hi_mpi_vpss_get_ext_chn_attr(vpss_grp, vpss_chn, &ext_chn_attr);
        if (ret != HI_SUCCESS) {
            printf("get ext chn attr error!!!\n");
        }

        ext_chn_attr.depth = g_orig_depth;
        ret = hi_mpi_vpss_set_ext_chn_attr(vpss_grp, vpss_chn, &ext_chn_attr);
        if (ret != HI_SUCCESS) {
            printf("set ext chn attr error!!!\n");
        }
    } else {
        ret = hi_mpi_vpss_get_chn_attr(vpss_grp, vpss_chn, &chn_attr);
        if (ret != HI_SUCCESS) {
            printf("get chn attr error!!!\n");
        }

        chn_attr.depth = g_orig_depth;
        chn_attr.chn_mode = g_orig_chn_mode;
        ret = hi_mpi_vpss_set_chn_attr(vpss_grp, vpss_chn, &chn_attr);
        if (ret != HI_SUCCESS) {
            printf("set chn attr error!!!\n");
        }
    }
}

hi_void VpssCapture::vpss_restore(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn)
{
    hi_s32 ret;

    if (g_frame.pool_id != HI_VB_INVALID_POOL_ID) {
        ret = hi_mpi_vpss_release_chn_frame(vpss_grp, vpss_chn, &g_frame);
        if (ret != HI_SUCCESS) {
            printf("Release Chn Frame error!!!\n");
        }

        g_frame.pool_id = HI_VB_INVALID_POOL_ID;
    }

    if (g_handle != -1) {
        hi_mpi_vgs_cancel_job(g_handle);
        g_handle = -1;
    }

    if (g_dump_mem.vb_pool != HI_VB_INVALID_POOL_ID) {
        hi_mpi_vb_release_blk(g_dump_mem.vb_blk);
        g_dump_mem.vb_pool = HI_VB_INVALID_POOL_ID;
    }

    if (g_pool != HI_VB_INVALID_POOL_ID) {
        hi_mpi_vb_destroy_pool(g_pool);
        g_pool = HI_VB_INVALID_POOL_ID;
    }

    if (g_user_page_addr[0] != HI_NULL) {
        hi_mpi_sys_munmap(g_user_page_addr[0], g_size);
        g_user_page_addr[0] = HI_NULL;
    }
    if (g_user_page_addr[1] != HI_NULL) {
        hi_mpi_sys_munmap(g_user_page_addr[1], g_size);
        g_user_page_addr[1] = HI_NULL;
    }
    
    if (g_vpss_depth_flag) {
        vpss_restore_depth(vpss_grp, vpss_chn);
        g_vpss_depth_flag = 0;
    }
    return;
}

int VpssCapture::yuv2mat(hi_video_frame *frame, cv::Mat &cv_mat)
{
	hi_s32 ret;
	hi_svp_src_img src;
	hi_ive_handle handle;
	hi_ive_csc_ctrl csc_ctrl;
	hi_bool is_instant = HI_TRUE;
	unsigned char *p_image;

	src.phys_addr[0] = (hi_u64)frame->phys_addr[0];
	src.phys_addr[1] = (hi_u64)frame->phys_addr[1];
	src.phys_addr[2] = (hi_u64)frame->phys_addr[2];
	src.virt_addr[0] = (hi_u64)frame->virt_addr[0];
	src.virt_addr[1] = (hi_u64)frame->virt_addr[1];
	src.virt_addr[2] = (hi_u64)frame->virt_addr[2];
	src.stride[0]  = frame->stride[0];
	src.stride[1]  = frame->stride[1];
	src.stride[2]  = frame->stride[2];
	src.width  = frame->width;
	src.height = frame->height;

	switch (frame->pixel_format)
	{
    case HI_PIXEL_FORMAT_YVU_PLANAR_420:
			src.type = HI_SVP_IMG_TYPE_YUV420P;
			break ;
		case HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420:
			src.type = HI_SVP_IMG_TYPE_YUV420SP;
			break ;
		default:
			printf("unsupported pixel_format yet, %d\n", frame->pixel_format);
			break ;
	}

	dst_img.stride[0]  = frame->width;
	dst_img.stride[1]  = frame->width;
	dst_img.stride[2]  = frame->width;
	dst_img.width  = frame->width;
	dst_img.height = frame->height;
	dst_img.type = HI_SVP_IMG_TYPE_U8C3_PACKAGE;

	if(!dst_img.virt_addr[0] ||
	  dst_mat.cols != frame->width || dst_mat.rows != frame->height)
	{
		ret = hi_mpi_sys_mmz_alloc_cached(&dst_img.phys_addr[0],
				(hi_void **)&dst_img.virt_addr[0],
				NULL,HI_NULL,
				dst_img.height*dst_img.width*3);
				
		if (HI_FAILURE == ret)
		{
			printf("MmzAlloc_Cached failed!\n");
			return HI_FAILURE;
		}
		printf("MmzAlloc_Cached OK!\n");
		dst_mat = cv::Mat(frame->height, frame->width, CV_8UC3, (void*)dst_img.virt_addr[0]);
		cv_mat = cv::Mat(dst_mat);
	}

	dst_img.phys_addr[1] = dst_img.phys_addr[0] + dst_img.stride[0];
	dst_img.phys_addr[2] = dst_img.phys_addr[1] + dst_img.stride[1];
	dst_img.virt_addr[1] = dst_img.virt_addr[0] + dst_img.stride[0];
	dst_img.virt_addr[2] = dst_img.virt_addr[1] + dst_img.stride[1]; 

	csc_ctrl.mode = HI_IVE_CSC_MODE_PIC_BT601_YUV_TO_RGB;
	
	ret = hi_mpi_ive_csc(&handle, &src, &dst_img, &csc_ctrl, is_instant); 
	if (HI_FAILURE == ret)
	{
		printf("YUV Convert to RGB failed!\n");
		return HI_FAILURE;
	}

	p_image = (unsigned char *)dst_img.virt_addr[0];
	if(NULL == p_image)
	{
		printf( "dst_img.virt_addr[0] is null!\n");
		return HI_FAILURE;    
	}
	
	return 0;
}

int VpssCapture::init(int grp, int chn)
{
  const hi_u32 depth = 2;
  hi_s32 ret;
  hi_vpss_chn_attr chn_attr;
  hi_vpss_ext_chn_attr ext_chn_attr;

  if (chn >= HI_VPSS_MAX_PHYS_CHN_NUM) {
      ret = hi_mpi_vpss_get_ext_chn_attr(grp, chn, &ext_chn_attr);
      if (ret != HI_SUCCESS) {
          printf("get ext chn attr error!!!\n");
          return -1;
      }

      g_orig_depth = ext_chn_attr.depth;
      ext_chn_attr.depth = depth;

      if (hi_mpi_vpss_set_ext_chn_attr(grp, chn, &ext_chn_attr) != HI_SUCCESS) {
          printf("set ext chn attr error!!!\n");
          goto exit;
      }
  } else {
      ret = hi_mpi_vpss_get_chn_attr(grp, chn, &chn_attr);
      if (ret != HI_SUCCESS) {
          printf("get chn attr error!!!\n");
          return -1;
      }

      g_orig_depth = chn_attr.depth;
      g_orig_chn_mode = chn_attr.chn_mode;
      chn_attr.depth = depth;
      chn_attr.chn_mode = HI_VPSS_CHN_MODE_USER;

      if (hi_mpi_vpss_set_chn_attr(grp, chn, &chn_attr) != HI_SUCCESS) {
          printf("set chn attr error!!!\n");
          goto exit;
      }
  }

	g_vpss_depth_flag = 1;

	memset(&g_frame, 0, sizeof(g_frame));
	g_frame.pool_id = HI_VB_INVALID_POOL_ID;
	
	this->g_vpss_grp = grp;
	this->g_vpss_chn = chn;
	return hi_mpi_vpss_get_chn_fd(grp, chn);

exit:
  vpss_restore(grp, chn);
  return -1;  
}

int VpssCapture::get_frame(cv::Mat &cv_mat)
{
  if (hi_mpi_vpss_get_chn_frame(g_vpss_grp, g_vpss_chn, &g_frame, milli_sec) != HI_SUCCESS) {
      printf("Get frame failed!\n");
      usleep(1000); /* 1000 sleep */
      return -1;
  }
  //printf("hi_mpi_vpss_get_chn_frame hi_frame:%p\n", &g_frame);
  
	if (HI_DYNAMIC_RANGE_SDR8 == g_frame.video_frame.dynamic_range)
	{
		int ret = yuv2mat(&g_frame.video_frame, cv_mat);
		if(ret < 0) return -1;
	}
	
	if(!frame_lock)
	{
	  hi_mpi_vpss_release_chn_frame(g_vpss_grp, g_vpss_chn, &g_frame);
  }

	frame_id++;
	return 0;
}

int VpssCapture::get_frame_lock(cv::Mat &cv_mat, hi_video_frame_info **hi_frame)
{
  frame_lock = 1;
  int ret = get_frame(cv_mat);
  *hi_frame = &g_frame;
  return ret;
}

int VpssCapture::get_frame_unlock(hi_video_frame_info *hi_frame)
{
  if(frame_lock)
  {
    frame_lock = 0;
    //printf("hi_mpi_vpss_release_chn_frame hi_frame:%p\n", hi_frame);
    hi_mpi_vpss_release_chn_frame(g_vpss_grp, g_vpss_chn, hi_frame);
  }
  return 0;
}


int VpssCapture::destroy()
{
	g_frame.pool_id = HI_VB_INVALID_POOL_ID;
	vpss_restore(g_vpss_grp, g_vpss_chn);
	hi_mpi_sys_mmz_free(dst_img.phys_addr[0], (hi_void *)dst_img.virt_addr[0]);
	dst_img.virt_addr[0] = 0;
	return 0;
}

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
        printf("set chn attr grp:%d, chn:%d, depth:%d\n", vpss_grp, vpss_chn, g_orig_depth);
    }
}

hi_void VpssCapture::vpss_restore(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn)
{
    hi_s32 ret;

    for(int i = 0; i < 2; i++)
    {
      if (g_frame[i].pool_id != HI_VB_INVALID_POOL_ID) {
          ret = hi_mpi_vpss_release_chn_frame(vpss_grp, vpss_chn, &g_frame[i]);
          printf("vpss_release_chn_frame(grp:%d, chn:%d) ret:%d\n", vpss_grp, vpss_chn, ret);
          g_frame[i].pool_id = HI_VB_INVALID_POOL_ID;
      }
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
        if(g_both)
          vpss_restore_depth(vpss_grp, !vpss_chn);
        g_vpss_depth_flag = 0;
    }
    return;
}

int VpssCapture::yuv2mat(hi_video_frame *frame, cv::Mat *cv_mat)
{
	hi_s32 ret;
	hi_svp_src_img src;
	hi_ive_handle handle;
	hi_ive_csc_ctrl csc_ctrl;
	hi_bool is_instant = HI_TRUE;
	unsigned char *p_image;

  if(frame == NULL || cv_mat == NULL)
  {
    return 0;
  }  

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
		dst_mat = cv::Mat(frame->height, frame->width, CV_8UC3, (void*)dst_img.virt_addr[0]);
		*cv_mat = cv::Mat(dst_mat);
		printf("MmzAlloc_Cached OK! dst_mat:%p, => cv_mat:%p\n", &dst_mat, cv_mat);
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

int VpssCapture::init(int grp, int chn, int vgsW, int vgsH, int both)
{
  const hi_u32 depth = 2;
  hi_s32 ret;
  hi_vpss_chn_attr chn_attr;
  hi_vpss_ext_chn_attr ext_chn_attr;
  
  g_both = both;
  g_vpss_grp = grp;
	g_vpss_chn = chn;
    
  do{
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
        printf("set chn attr grp:%d, chn:%d, depth:%d\n", grp, chn, depth);
    }
    chn = !chn;
  }while(both--);

	g_vpss_depth_flag = 1;

	memset(&g_frame, 0, sizeof(g_frame));
	g_frame[0].pool_id = HI_VB_INVALID_POOL_ID;
	g_frame[1].pool_id = HI_VB_INVALID_POOL_ID;
	
  this->vgsW = vgsW;
	this->vgsH = vgsH;
  g_dump_mem.vb_blk = -1;
  g_dump_mem.vb_pool = HI_VB_INVALID_POOL_ID;
  g_dump_mem.phys_addr = 0;
  g_dump_mem.virt_addr = HI_NULL;
	
	return hi_mpi_vpss_get_chn_fd(grp, g_vpss_chn);

exit:
  vpss_restore(grp, g_vpss_chn);
  return -1;  
}


hi_s32 VpssCapture::vpss_chn_dump_init_vgs_pool(vpss_dump_mem *dump_mem, hi_vb_calc_cfg *vb_calc_cfg)
{
    hi_u32 width = vgsW;//g_frame[0].video_frame.width;
    hi_u32 height = vgsH;//g_frame[0].video_frame.height;
    hi_pixel_format pixel_format = g_frame[0].video_frame.pixel_format;
    
    hi_pic_buf_attr buf_attr = {0};
    hi_vb_pool_cfg vb_pool_cfg = {0};

    buf_attr.width = width;
    buf_attr.height = height;
    buf_attr.pixel_format = pixel_format;
    buf_attr.bit_width = HI_DATA_BIT_WIDTH_8;
    buf_attr.compress_mode = HI_COMPRESS_MODE_NONE;
    buf_attr.align = 0;
    hi_common_get_pic_buf_cfg(&buf_attr, vb_calc_cfg);

    g_blk_size = vb_calc_cfg->vb_size;

    vb_pool_cfg.blk_size = g_blk_size;
    vb_pool_cfg.blk_cnt = 1;
    vb_pool_cfg.remap_mode = HI_VB_REMAP_MODE_NONE;

    g_pool = hi_mpi_vb_create_pool(&vb_pool_cfg);
    if (g_pool == HI_VB_INVALID_POOL_ID) {
        printf("HI_MPI_VB_CreatePool failed! \n");
        return HI_FAILURE;
    }

    dump_mem->vb_pool = g_pool;
    dump_mem->vb_blk = hi_mpi_vb_get_blk(dump_mem->vb_pool, g_blk_size, HI_NULL);
    if (dump_mem->vb_blk == HI_VB_INVALID_HANDLE) {
        printf("get vb blk failed!\n");
        return HI_FAILURE;
    }

    dump_mem->phys_addr = hi_mpi_vb_handle_to_phys_addr(dump_mem->vb_blk);
    return HI_SUCCESS;
}

hi_void VpssCapture::vpss_chn_dump_set_vgs_frame_info(hi_video_frame_info *vgs_frame_info, const vpss_dump_mem *dump_mem,
    const hi_vb_calc_cfg *vb_calc_cfg, const hi_video_frame_info *vpss_frame_info)
{
    vgs_frame_info->video_frame.phys_addr[0] = dump_mem->phys_addr;
    vgs_frame_info->video_frame.phys_addr[1] = vgs_frame_info->video_frame.phys_addr[0] + vb_calc_cfg->main_y_size;
    vgs_frame_info->video_frame.width = vgsW;//vpss_frame_info->video_frame.width;
    vgs_frame_info->video_frame.height = vgsH;//vpss_frame_info->video_frame.height;
    vgs_frame_info->video_frame.stride[0] = vb_calc_cfg->main_stride;
    vgs_frame_info->video_frame.stride[1] = vb_calc_cfg->main_stride;
    vgs_frame_info->video_frame.compress_mode = HI_COMPRESS_MODE_NONE;
    vgs_frame_info->video_frame.pixel_format = vpss_frame_info->video_frame.pixel_format;
    vgs_frame_info->video_frame.video_format = HI_VIDEO_FORMAT_LINEAR;
    vgs_frame_info->video_frame.dynamic_range = vpss_frame_info->video_frame.dynamic_range;
    vgs_frame_info->video_frame.pts = 0;
    vgs_frame_info->video_frame.time_ref = 0;
    vgs_frame_info->pool_id = dump_mem->vb_pool;
    vgs_frame_info->mod_id = HI_ID_VGS;
}

int VpssCapture::get_frame(cv::Mat *cv_mat)
{
  if (hi_mpi_vpss_get_chn_frame(g_vpss_grp, g_vpss_chn, &g_frame[0], milli_sec) != HI_SUCCESS) {
      printf("Get vpss %d frame failed!\n", g_vpss_chn);
      usleep(1000); /* 1000 usleep */
      return -1;
  }
  
	bool send_to_vgs = ((HI_COMPRESS_MODE_NONE != g_frame[0].video_frame.compress_mode) 
	                  || (HI_VIDEO_FORMAT_LINEAR != g_frame[0].video_frame.video_format)
	                  || (this->vgsW != 0 && this->vgsH != 0));
  
  if(send_to_vgs)
  {
    //vgs;
    if(g_pool == HI_VB_INVALID_POOL_ID)
    {  
      if (vpss_chn_dump_init_vgs_pool(&g_dump_mem, &vb_calc_cfg) != HI_SUCCESS) {
          printf("init vgs pool failed\n");
          return HI_FAILURE;
      }
      vpss_chn_dump_set_vgs_frame_info(&vgs_frame, &g_dump_mem, &vb_calc_cfg, &g_frame[0]);
    }
    
    hi_vgs_task_attr vgs_task_attr;
       
    if (hi_mpi_vgs_begin_job(&g_handle) != HI_SUCCESS) {
        printf("hi_mpi_vgs_begin_job failed\n");
        return HI_FAILURE;
    }

    if (memcpy_s(&vgs_task_attr.img_in, sizeof(hi_video_frame_info),
        &g_frame[0], sizeof(hi_video_frame_info)) != EOK) {
        printf("memcpy_s img_in failed\n");
        hi_mpi_vgs_cancel_job(g_handle);
        g_handle = -1;
        return HI_FAILURE;
    }
    if (memcpy_s(&vgs_task_attr.img_out, sizeof(hi_video_frame_info),
        &vgs_frame, sizeof(hi_video_frame_info)) != EOK) {
        printf("memcpy_s img_out failed\n");
        hi_mpi_vgs_cancel_job(g_handle);
        g_handle = -1;
        return HI_FAILURE;
    }
    if (hi_mpi_vgs_add_scale_task(g_handle, &vgs_task_attr, HI_VGS_SCALE_COEF_NORM) != HI_SUCCESS) {
        printf("hi_mpi_vgs_add_scale_task failed\n");
        hi_mpi_vgs_cancel_job(g_handle);
        g_handle = -1;
        return HI_FAILURE;
    }
    if (hi_mpi_vgs_end_job(g_handle) != HI_SUCCESS) {
        printf("hi_mpi_vgs_end_job failed\n");
        hi_mpi_vgs_cancel_job(g_handle);
        g_handle = -1;
        return HI_FAILURE;
    }
    g_handle = -1;
      	
    if (HI_DYNAMIC_RANGE_SDR8 == g_frame[0].video_frame.dynamic_range)
  	{
  		int ret = yuv2mat(&vgs_frame.video_frame, cv_mat);
  		if(ret < 0) return -1;
  	}
  }  
	else if (HI_DYNAMIC_RANGE_SDR8 == g_frame[0].video_frame.dynamic_range)
	{
		int ret = yuv2mat(&g_frame[0].video_frame, cv_mat);
		if(ret < 0) return -1;
	}
	
	if(!frame_lock)
	{
	  hi_mpi_vpss_release_chn_frame(g_vpss_grp, g_vpss_chn, &g_frame[0]);
	  g_frame[0].pool_id = HI_VB_INVALID_POOL_ID;
  }

	frame_id++;
	return 0;
}

int VpssCapture::get_frame_lock(cv::Mat *cv_mat, hi_video_frame_info **hi_frame, hi_video_frame_info **other_frame)
{
  frame_lock = 1;
  
  if(g_both && other_frame)
  {
    if (hi_mpi_vpss_get_chn_frame(g_vpss_grp, !g_vpss_chn, &g_frame[1], milli_sec) != HI_SUCCESS) {
        printf("Get vpss %d frame failed!\n", !g_vpss_chn);
        usleep(1000); /* 1000 usleep */
        return -1;
    }
    *other_frame = &g_frame[1];
  }
   
  int ret = get_frame(cv_mat);
  if(hi_frame)
  {  
    *hi_frame = &g_frame[0];
  }
  return ret;
}

int VpssCapture::get_frame_unlock(hi_video_frame_info *hi_frame, hi_video_frame_info *other_frame)
{
  if(frame_lock)
  {
    frame_lock = 0;
    
    if(hi_frame)
    {  
      //printf("hi_mpi_vpss_release_chn_frame hi_frame:%p\n", hi_frame);
      hi_mpi_vpss_release_chn_frame(g_vpss_grp, g_vpss_chn, hi_frame);
      hi_frame->pool_id = HI_VB_INVALID_POOL_ID;
    }
    
    if(g_both && other_frame)
    {  
      hi_mpi_vpss_release_chn_frame(g_vpss_grp, !g_vpss_chn, other_frame);
      other_frame->pool_id = HI_VB_INVALID_POOL_ID;
    }  
  }
  return 0;
}


int VpssCapture::destroy()
{
	vpss_restore(g_vpss_grp, g_vpss_chn);
	
	if(dst_img.virt_addr[0])
	{
  	hi_mpi_sys_mmz_free(dst_img.phys_addr[0], (hi_void *)dst_img.virt_addr[0]);
  	dst_img.virt_addr[0] = 0;
  }
	return 0;
}

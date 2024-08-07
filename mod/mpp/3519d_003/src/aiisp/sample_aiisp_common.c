/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <signal.h>
#include <limits.h>
#include "sample_aiisp_common.h"


#define VO_WIDTH 1920
#define VO_HEIGTH 1080

#define VENC_WIDTH 3840
#define VENC_HEIGTH 2160

static sample_vo_cfg g_vo_cfg = {
    .vo_dev            = SAMPLE_VO_DEV_UHD,
    .vo_layer          = SAMPLE_VO_LAYER_VHD0,
    .vo_intf_type      = HI_VO_INTF_BT1120,
    .intf_sync         = HI_VO_OUT_1080P60,
    .bg_color          = COLOR_RGB_BLACK,
    .pix_format        = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    .disp_rect         = {0, 0, VO_WIDTH, VO_HEIGTH},
    .image_size        = {VO_WIDTH, VO_HEIGTH},
    .vo_part_mode      = HI_VO_PARTITION_MODE_SINGLE,
    .dis_buf_len       = 3, /* 3: def buf len for single */
    .dst_dynamic_range = HI_DYNAMIC_RANGE_SDR8,
    .vo_mode           = VO_MODE_1MUX,
    .compress_mode     = HI_COMPRESS_MODE_NONE,
};

static sample_comm_venc_chn_param g_venc_chn_param = {
    .frame_rate           = 30, /* 30 is a number */
    .stats_time           = 2,  /* 2 is a number */
    .gop                  = 60, /* 60 is a number */
    .venc_size            = {VENC_WIDTH, VENC_HEIGTH},
    .size                 = -1,
    .profile              = 0,
    .is_rcn_ref_share_buf = HI_FALSE,
    .gop_attr             = {
        .gop_mode = HI_VENC_GOP_MODE_NORMAL_P,
        .normal_p = {2},
    },
    .type                 = HI_PT_H265,
    .rc_mode              = SAMPLE_RC_CBR,
};

volatile sig_atomic_t g_sig_flag = 0;

hi_void sample_aiisp_handle_sig(hi_s32 signo)
{
    if (signo == SIGINT || signo == SIGTERM) {
        g_sig_flag = 1;
    }
}

hi_void sample_get_char(hi_char *s)
{
    if (g_sig_flag == 1) {
        return;
    }

    printf("---------------press any key to %s!---------------\n", s);
    (hi_void)getchar();
}

sample_sns_type sample_aiisp_get_wdr_sns_type(sample_sns_type sns_type)
{
    if (sns_type == OV_OS08A20_MIPI_8M_30FPS_12BIT) {
        return OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1;
    } else if (sns_type == OV_OS04A10_MIPI_4M_30FPS_12BIT) {
        return OV_OS04A10_MIPI_4M_30FPS_12BIT_WDR2TO1;
    }

    sample_print("sns_type: %d, unsupport wdr mode!\n", sns_type);
    return SNS_TYPE_BUTT;
}
static hi_s32 sample_aiisp_check_fp(FILE *fp, hi_char *model_file)
{
    if (fp == HI_NULL) {
        sample_print("open file err!\n");
        return HI_FAILURE;
    }
    printf("open %s success\n", model_file);
    return HI_SUCCESS;
}

hi_s32 sample_aiisp_load_mem(hi_aiisp_mem_info *mem, hi_char *model_file)
{
    hi_s32 ret;
    FILE *fp = HI_NULL;
    hi_char path[PATH_MAX + 1] = {0};

    /* Get model file size */
    sample_aiisp_check_exps_return((strlen(model_file) > PATH_MAX) || realpath(model_file, path) == HI_NULL);
    fp = fopen(model_file, "rb");
    if (sample_aiisp_check_fp(fp, model_file) != HI_SUCCESS) {
        return HI_FAILURE;
    }
    ret = fseek(fp, 0L, SEEK_END);
    if (ret != HI_SUCCESS) {
        sample_print("fseek end failed!\n");
        goto fail_0;
    }

    mem->size = ftell(fp);
    if (mem->size <= 0) {
        sample_print("ftell failed!\n");
        goto fail_0;
    }

    ret = fseek(fp, 0L, SEEK_SET);
    if (ret != HI_SUCCESS) {
        sample_print("fseek set failed!\n");
        goto fail_0;
    }

    /* malloc model file mem */
    ret = hi_mpi_sys_mmz_alloc(&(mem->phys_addr), &(mem->virt_addr), "aibnr_cfg", HI_NULL, mem->size);
    if (ret != HI_SUCCESS) {
        sample_print("malloc mmz failed!\n");
        goto fail_0;
    }

    ret = fread(mem->virt_addr, mem->size, 1, fp);
    if (ret != 1) {
        sample_print("read model file failed!\n");
        goto fail_1;
    }

    ret = fclose(fp);
    if (ret != 0) {
        sample_print("close file error\n");
    }
    return HI_SUCCESS;

fail_1:
    hi_mpi_sys_mmz_free(mem->phys_addr, mem->virt_addr);
    mem->phys_addr = 0;
    mem->virt_addr = HI_NULL;
fail_0:
    if (fp != HI_NULL) {
        fclose(fp);
    }
    return HI_FAILURE;
}

hi_void sample_aiisp_unload_mem(hi_aiisp_mem_info *param_mem)
{
    if ((param_mem->phys_addr != 0) && (param_mem->virt_addr != HI_NULL)) {
        (hi_void)hi_mpi_sys_mmz_free(param_mem->phys_addr, param_mem->virt_addr);
    }
}

static hi_void sample_aiisp_get_default_vb_config(const hi_size *size, hi_vb_cfg *vb_cfg, hi_u32 vb_cnt)
{
    hi_vb_calc_cfg calc_cfg = {0};
    hi_pic_buf_attr buf_attr = {0};

    (hi_void)memset_s(vb_cfg, sizeof(hi_vb_cfg), 0, sizeof(hi_vb_cfg));
    vb_cfg->max_pool_cnt = 128; /* 128 blks */

    buf_attr.width         = size->width;
    buf_attr.height        = size->height;
    buf_attr.align         = HI_DEFAULT_ALIGN;
    buf_attr.bit_width     = HI_DATA_BIT_WIDTH_8;
    buf_attr.pixel_format  = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_422;
    buf_attr.compress_mode = HI_COMPRESS_MODE_NONE;
    buf_attr.video_format  = HI_VIDEO_FORMAT_LINEAR;
    hi_common_get_pic_buf_cfg(&buf_attr, &calc_cfg);

    vb_cfg->common_pool[0].blk_size = calc_cfg.vb_size;
    vb_cfg->common_pool[0].blk_cnt  = vb_cnt;
}

#ifdef SAMPLE_MEM_SHARE_ENABLE
/*maohw static */hi_void sample_aiisp_init_mem_share(hi_void)
{
    hi_u32 i;
    hi_vb_common_pools_id pools_id = {0};

    if (hi_mpi_vb_get_common_pool_id(&pools_id) != HI_SUCCESS) {
        sample_print("get common pool_id failed!\n");
        return;
    }
    for (i = 0; i < pools_id.pool_cnt; ++i) {
        hi_mpi_vb_pool_share_all(pools_id.pool[i]);
    }
}
#endif


static hi_s32 __skip_vb = 0; //maohw
hi_s32 sample_aiisp_skip_vb(hi_s32 en)
{
  __skip_vb = en;
}

hi_s32 sample_aiisp_sys_init(hi_size *in_size, hi_u32 vb_cnt)
{
    hi_s32 ret;
    hi_vb_cfg vb_cfg = {0};
    hi_u32 supplement_config;
    hi_vi_vpss_mode_type mode_type = HI_VI_OFFLINE_VPSS_OFFLINE;

    if(!__skip_vb) //maohw
    {
      sample_aiisp_get_default_vb_config(in_size, &vb_cfg, vb_cnt);

      supplement_config = HI_VB_SUPPLEMENT_BNR_MOT_MASK;
      ret = sample_comm_sys_init_with_vb_supplement(&vb_cfg, supplement_config);
      if (ret != HI_SUCCESS) {
          return HI_FAILURE;
      }
    }

#ifdef SAMPLE_MEM_SHARE_ENABLE
    sample_aiisp_init_mem_share();
#endif

    ret = sample_comm_vi_set_vi_vpss_mode(mode_type, HI_VI_AIISP_MODE_DEFAULT);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 sample_aiisp_start_vpss(hi_vpss_grp grp, hi_size *in_size)
{
    hi_s32 ret;
    hi_vpss_grp_attr grp_attr = {0};
    sample_vpss_chn_attr vpss_chn_attr = {0};

    vpss_chn_attr.chn_enable[0] = HI_TRUE;
    vpss_chn_attr.chn_array_size = HI_VPSS_MAX_PHYS_CHN_NUM;
    sample_comm_vpss_get_default_grp_attr(&grp_attr);
    grp_attr.max_width  = in_size->width;
    grp_attr.max_height = in_size->height;
    sample_comm_vpss_get_default_chn_attr(&vpss_chn_attr.chn_attr[0]);
    vpss_chn_attr.chn_attr[0].width  = in_size->width;
    vpss_chn_attr.chn_attr[0].height = in_size->height;

    ret = sample_common_vpss_start(grp, &grp_attr, &vpss_chn_attr);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 sample_aiisp_start_vo(sample_vo_mode vo_mode)
{
    g_vo_cfg.vo_mode = vo_mode;

    return sample_comm_vo_start_vo(&g_vo_cfg);
}

hi_void sample_aiisp_stop_vo(hi_void)
{
    sample_comm_vo_stop_vo(&g_vo_cfg);
}

hi_s32 sample_aiisp_start_venc(hi_venc_chn venc_chn[], hi_u32 venc_chn_len, hi_u32 chn_num, hi_size *size)
{
    hi_s32 i;
    hi_s32 ret;

    g_venc_chn_param.venc_size.width        = size->width;
    g_venc_chn_param.venc_size.height       = size->height;
    g_venc_chn_param.size = sample_comm_sys_get_pic_enum(size);

    for (i = 0; i < (hi_s32)chn_num && i < venc_chn_len; i++) {
        ret = sample_comm_venc_start(venc_chn[i], &g_venc_chn_param);
        if (ret != HI_SUCCESS) {
            goto exit;
        }
    }

    ret = sample_comm_venc_start_get_stream(venc_chn, chn_num);
    if (ret != HI_SUCCESS) {
        goto exit;
    }

    return HI_SUCCESS;

exit:
    for (i = i - 1; i >= 0; i--) {
        sample_comm_venc_stop(venc_chn[i]);
    }
    return HI_FAILURE;
}

hi_s32 sample_aiisp_start_venc_and_vo(hi_vpss_grp vpss_grp[], hi_u32 grp_num, hi_size *in_size)
{
    hi_u32 i;
    hi_s32 ret;
    sample_vo_mode vo_mode = VO_MODE_1MUX;
    const hi_vpss_chn vpss_chn = 0;
    const hi_vo_layer vo_layer = 0;
    hi_vo_chn vo_chn[4] = {0, 1, 2, 3};     /* 4: max chn num, 0/1/2/3 chn id */
    hi_venc_chn venc_chn[4] = {0, 1, 2, 3}; /* 4: max chn num, 0/1/2/3 chn id */

    if (grp_num > 1) {
        vo_mode = VO_MODE_4MUX;
    }

    ret = sample_aiisp_start_vo(vo_mode);
    if (ret != HI_SUCCESS) {
        goto start_vo_failed;
    }

    ret = sample_aiisp_start_venc(venc_chn, sizeof(venc_chn) / sizeof(hi_venc_chn), grp_num, in_size);
    if (ret != HI_SUCCESS) {
        goto start_venc_failed;
    }

    for (i = 0; i < grp_num; i++) {
        sample_comm_vpss_bind_vo(vpss_grp[i], vpss_chn, vo_layer, vo_chn[i]);
        sample_comm_vpss_bind_venc(vpss_grp[i], vpss_chn, venc_chn[i]);
    }

    return HI_SUCCESS;

start_venc_failed:
    sample_aiisp_stop_vo();
start_vo_failed:
    return HI_FAILURE;
}

hi_void sample_aiisp_stop_vpss(hi_vpss_grp grp)
{
    hi_bool chn_enable[HI_VPSS_MAX_PHYS_CHN_NUM] = {HI_TRUE, HI_FALSE, HI_FALSE, HI_FALSE};

    sample_common_vpss_stop(grp, chn_enable, HI_VPSS_MAX_PHYS_CHN_NUM);
}

hi_void sample_aiisp_stop_venc(hi_venc_chn venc_chn[], hi_u32 venc_chn_len, hi_u32 chn_num)
{
    hi_u32 i;

    sample_comm_venc_stop_get_stream(chn_num);

    for (i = 0; i < chn_num && i < venc_chn_len; i++) {
        sample_comm_venc_stop(venc_chn[i]);
    }
}

hi_void sample_aiisp_stop_venc_and_vo(hi_vpss_grp vpss_grp[], hi_u32 grp_num)
{
    hi_u32 i;
    const hi_vpss_chn vpss_chn = 0;
    const hi_vo_layer vo_layer = 0;
    hi_vo_chn vo_chn[4] = {0, 1, 2, 3};     /* 4: max chn num, 0/1/2/3 chn id */
    hi_venc_chn venc_chn[4] = {0, 1, 2, 3}; /* 4: max chn num, 0/1/2/3 chn id */

    for (i = 0; i < grp_num; i++) {
        sample_comm_vpss_un_bind_vo(vpss_grp[i], vpss_chn, vo_layer, vo_chn[i]);
        sample_comm_vpss_un_bind_venc(vpss_grp[i], vpss_chn, venc_chn[i]);
    }

    sample_aiisp_stop_venc(venc_chn, sizeof(venc_chn) / sizeof(hi_venc_chn), grp_num);
    sample_aiisp_stop_vo();
}

hi_s32 sample_aiisp_set_long_frame_mode(hi_vi_pipe vi_pipe, hi_bool is_wdr_mode)
{
    hi_s32 ret;
    hi_isp_exposure_attr exp_attr = {0};
    hi_isp_wdr_fs_attr fswdr_attr = {0};

    if (is_wdr_mode == HI_FALSE) {
        return HI_SUCCESS;
    }

    sample_get_char("set long frame mode");

    ret = hi_mpi_isp_get_exposure_attr(vi_pipe, &exp_attr);
    if (ret != HI_SUCCESS) {
        printf("hi_mpi_isp_get_exposure_attr error[0x%x]\n", ret);
        return HI_FAILURE;
    }

    exp_attr.auto_attr.fswdr_mode = HI_ISP_FSWDR_LONG_FRAME_MODE;
    ret = hi_mpi_isp_set_exposure_attr(vi_pipe, &exp_attr);
    if (ret != HI_SUCCESS) {
        printf("hi_mpi_isp_set_exposure_attr error[0x%x]\n", ret);
        return HI_FAILURE;
    }

    ret = hi_mpi_isp_get_fswdr_attr(vi_pipe, &fswdr_attr);
    if (ret != HI_SUCCESS) {
        printf("hi_mpi_isp_get_fswdr_attr error[0x%x]\n", ret);
        return HI_FAILURE;
    }

    fswdr_attr.wdr_merge_mode = HI_ISP_MERGE_WDR_MODE;
    ret = hi_mpi_isp_set_fswdr_attr(vi_pipe, &fswdr_attr);
    if (ret != HI_SUCCESS) {
        printf("hi_mpi_isp_set_fswdr_attr error[0x%x]\n", ret);
        return HI_FAILURE;
    }

    return ret;
}

hi_void sample_aiisp_get_default_cfg(sample_sns_type sns_type, hi_vi_pipe vi_pipe,
    hi_size *size, sample_vi_cfg *vi_cfg)
{
    sample_comm_vi_get_size_by_sns_type(sns_type, size);
    sample_comm_vi_get_default_vi_cfg(sns_type, vi_cfg);

#ifdef OT_FPGA
    vi_cfg->pipe_info[vi_pipe].pipe_attr.frame_rate_ctrl.src_frame_rate = 30; /* 30fps */
    vi_cfg->pipe_info[vi_pipe].pipe_attr.frame_rate_ctrl.dst_frame_rate = 5; /* 5fps */
#endif
}

hi_void sample_aiisp_sensor_cfg(hi_vi_pipe vi_pipe, sample_sns_type sns_type)
{
    switch (sns_type) {
        case SONY_IMX347_SLAVE_MIPI_4M_30FPS_12BIT:
            //maohw g_sns_imx347_slave_obj.pfn_mirror_flip(vi_pipe, ISP_SNS_MIRROR);
            break;

        default:
            break;
    }

    return;
}


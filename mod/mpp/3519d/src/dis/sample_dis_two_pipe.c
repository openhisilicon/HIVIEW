/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#include "sample_comm.h"
#ifdef OT_GYRODIS_SUPPORT
#include "sample_gyro_dis.h"
#include "sample_dis_two_pipe.h"
#endif

static sample_vo_cfg g_vo_4mux_cfg = {
    .vo_dev            = SAMPLE_VO_DEV_UHD,
    .vo_intf_type      = HI_VO_INTF_BT1120,
    .intf_sync         = HI_VO_OUT_1080P60,
    .bg_color          = COLOR_RGB_BLACK,
    .pix_format        = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    .disp_rect         = {0, 0, 1920, 1080},
    .image_size        = {1920, 1080},
    .vo_part_mode      = HI_VO_PARTITION_MODE_SINGLE,
    .dis_buf_len       = 3, /* 3: def buf len for single */
    .dst_dynamic_range = HI_DYNAMIC_RANGE_SDR8,
    .vo_mode           = VO_MODE_4MUX,
    .compress_mode     = HI_COMPRESS_MODE_NONE,
};

static sample_comm_venc_chn_param g_venc_chn_param = {
    .frame_rate           = 30, /* 30 is a number */
    .stats_time           = 2,  /* 2 is a number */
    .gop                  = 60, /* 60 is a number */
    .venc_size            = {1920, 1080},
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

static hi_void sample_dis_get_default_vb_config(hi_size *size, hi_vb_cfg *vb_cfg)
{
    hi_vb_calc_cfg calc_cfg;
    hi_pic_buf_attr buf_attr;

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
    vb_cfg->common_pool[0].blk_cnt  = 42; /* 42 blks */
}

static hi_s32 sample_dis_sys_init(hi_vi_vpss_mode_type mode_type, hi_vi_aiisp_mode aiisp_mode)
{
    hi_s32 ret;
    hi_size size;
    hi_vb_cfg vb_cfg;
    hi_u32 supplement_config;

    size.width  = 3840; /* 3840 pixel */
    size.height = 2160; /* 2160 line */
    sample_dis_get_default_vb_config(&size, &vb_cfg);

    supplement_config = HI_VB_SUPPLEMENT_BNR_MOT_MASK;
    ret = sample_comm_sys_init_with_vb_supplement(&vb_cfg, supplement_config);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    ret = sample_comm_vi_set_vi_vpss_mode(mode_type, aiisp_mode);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void sample_dis_get_two_sensor_vi_cfg(sample_sns_type sns_type,
    sample_vi_cfg *vi_cfg0, sample_vi_cfg *vi_cfg1)
{
    const hi_vi_dev vi_dev = 2; /* dev2 for sensor1 */
    const hi_vi_pipe vi_pipe = 1; /* dev2 bind pipe1 */

    sample_comm_vi_get_default_vi_cfg(sns_type, vi_cfg0);
    sample_comm_vi_get_default_vi_cfg(sns_type, vi_cfg1);

    vi_cfg0->mipi_info.divide_mode = LANE_DIVIDE_MODE_1;

    vi_cfg1->sns_info.bus_id = 3; /* i2c3 */
    vi_cfg1->sns_info.sns_clk_src = 1;
    vi_cfg1->sns_info.sns_rst_src = 1;

    sample_comm_vi_get_mipi_info_by_dev_id(sns_type, vi_dev, &vi_cfg1->mipi_info);
    vi_cfg1->dev_info.vi_dev = vi_dev;
    vi_cfg1->bind_pipe.pipe_id[0] = vi_pipe;
    vi_cfg1->grp_info.grp_num = 1;
    vi_cfg1->grp_info.fusion_grp[0] = 1;
    vi_cfg1->grp_info.fusion_grp_attr[0].pipe_id[0] = vi_pipe;
}

static hi_s32 sample_dis_start_vpss_grp(hi_vpss_grp grp, hi_size *in_size)
{
    hi_s32 ret;
    hi_low_delay_info low_delay_info;
    hi_vpss_grp_attr grp_attr;
    sample_vpss_chn_attr vpss_chn_attr = {0};

    sample_comm_vpss_get_default_grp_attr(&grp_attr);
    grp_attr.max_width  = in_size->width;
    grp_attr.max_height = in_size->height;
    sample_comm_vpss_get_default_chn_attr(&vpss_chn_attr.chn_attr[0]);
    vpss_chn_attr.chn_attr[0].width  = in_size->width;
    vpss_chn_attr.chn_attr[0].height = in_size->height;
    vpss_chn_attr.chn_enable[0] = HI_TRUE;
    vpss_chn_attr.chn_array_size = HI_VPSS_MAX_PHYS_CHN_NUM;

    ret = sample_common_vpss_start(grp, &grp_attr, &vpss_chn_attr);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    low_delay_info.enable     = HI_TRUE;
    low_delay_info.line_cnt   = 200; /* 200: lowdelay line */
    low_delay_info.one_buf_en = HI_FALSE;
    ret = hi_mpi_vi_set_chn_low_delay(grp, 0, &low_delay_info);
    if (ret != HI_SUCCESS) {
        sample_common_vpss_stop(grp, vpss_chn_attr.chn_enable, HI_VPSS_MAX_PHYS_CHN_NUM);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_void sample_dis_stop_vpss(hi_vpss_grp grp)
{
    hi_bool chn_enable[HI_VPSS_MAX_PHYS_CHN_NUM] = {HI_TRUE, HI_FALSE, HI_FALSE, HI_FALSE};

    sample_common_vpss_stop(grp, chn_enable, HI_VPSS_MAX_PHYS_CHN_NUM);
}

static hi_s32 sample_dis_start_multi_vi_vpss(sample_vi_cfg *vi_cfg, hi_vpss_grp *vpss_grp,
                                             hi_s32 dev_num, hi_s32 grp_num)
{
    hi_s32 ret;
    hi_s32 i, j;
    hi_size in_size;
    sample_sns_type sns_type = SENSOR0_TYPE; /* default: use same sensors */

    if (dev_num != grp_num) {
        return HI_FAILURE;
    }

    sample_comm_vi_get_size_by_sns_type(sns_type, &in_size);

    for (i = 0; i < dev_num; i++) {
        ret = sample_comm_vi_start_vi(&vi_cfg[i]);
        if (ret != HI_SUCCESS) {
        goto start_vi_failed;
        }
    }

    for (i = 0; i < grp_num; i++) {
        sample_comm_vi_bind_vpss(i, 0, vpss_grp[i], 0);
    }

    for (i = 0; i < grp_num; i++) {
        ret = sample_dis_start_vpss_grp(vpss_grp[i], &in_size);
        if (ret != HI_SUCCESS) {
            goto start_vpss_failed;
        }
    }

    return HI_SUCCESS;

start_vpss_failed:
    for (j = i - 1; j >= 0; j--) {
        sample_dis_stop_vpss(vpss_grp[j]);
    }

    for (i = 0; i < grp_num; i++) {
        sample_comm_vi_un_bind_vpss(i, 0, vpss_grp[i], 0);
    }

start_vi_failed:
    for (j = i - 1; j >= 0; j--) {
        sample_comm_vi_stop_vi(&vi_cfg[j]);
    }

    return HI_FAILURE;
}

static hi_s32 sample_dis_start_vo(hi_vo_intf_type vo_intf_type)
{
    g_vo_4mux_cfg.vo_intf_type = vo_intf_type;

    return sample_comm_vo_start_vo(&g_vo_4mux_cfg);
}

static hi_void sample_dis_stop_vo(hi_void)
{
    sample_comm_vo_stop_vo(&g_vo_4mux_cfg);
}

static hi_s32 sample_dis_start_venc_grp(hi_venc_chn venc_chn[], hi_u32 chn_num)
{
    hi_s32 i;
    hi_s32 ret;

    for (i = 0; i < (hi_s32)chn_num; i++) {
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

static hi_s32 sample_dis_start_venc_and_vo(hi_vpss_grp vpss_grp[], hi_u32 grp_num, hi_vo_intf_type vo_intf_type)
{
    hi_u32 i;
    hi_s32 ret;
    const hi_vpss_chn vpss_chn = 0;
    const hi_vo_layer vo_layer = 0;
    hi_vo_chn vo_chn[4] = {0, 1, 2, 3};     /* 4: max chn num, 0/1/2/3 chn id */
    hi_venc_chn venc_chn[4] = {0, 1, 2, 3}; /* 4: max chn num, 0/1/2/3 chn id */

    ret = sample_dis_start_vo(vo_intf_type);
    if (ret != HI_SUCCESS) {
        goto start_vo_failed;
    }

    ret = sample_dis_start_venc_grp(venc_chn, grp_num);
    if (ret != HI_SUCCESS) {
        goto start_venc_failed;
    }

    for (i = 0; i < grp_num; i++) {
        sample_comm_vpss_bind_vo(vpss_grp[i], vpss_chn, vo_layer, vo_chn[i]);
        sample_comm_vpss_bind_venc(vpss_grp[i], vpss_chn, venc_chn[i]);
    }
    return HI_SUCCESS;

start_venc_failed:
    sample_dis_stop_vo();
start_vo_failed:
    return HI_FAILURE;
}

static hi_void sample_dis_stop_venc(hi_venc_chn venc_chn[], hi_u32 chn_num)
{
    hi_u32 i;

    sample_comm_venc_stop_get_stream(chn_num);

    for (i = 0; i < chn_num; i++) {
        sample_comm_venc_stop(venc_chn[i]);
    }
}

static hi_void sample_dis_stop_venc_and_vo(hi_vpss_grp vpss_grp[], hi_u32 grp_num)
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

    sample_dis_stop_venc(venc_chn, grp_num);
    sample_dis_stop_vo();
}

/* Set pin_mux i2c2 & i2c3 & sensor0 & sensor1 & MIPI0 & MIPI1 before using this sample! */
hi_s32 sample_dis_gyro_two_sensor(hi_vo_intf_type vo_intf_type)
{
    hi_s32 ret;
    hi_vi_vpss_mode_type mode_type = HI_VI_OFFLINE_VPSS_OFFLINE;
    hi_vi_aiisp_mode aiisp_mode = HI_VI_AIISP_MODE_DEFAULT;
    hi_vi_pipe vi_pipe[2] = {0, 1}; /* 2 pipe */
    const hi_vi_chn vi_chn = 0;
    hi_vpss_grp vpss_grp[2] = {0, 1}; /* 2 vpss grp */
    const hi_u32 grp_num = 2; /* 2 vpss grp */
    const hi_vpss_chn vpss_chn = 0;
    sample_vi_cfg vi_cfg[2];
    sample_sns_type sns_type = SENSOR0_TYPE;
    hi_size size;

    ret = sample_dis_sys_init(mode_type, aiisp_mode);
    if (ret != HI_SUCCESS) {
        goto sys_init_failed;
    }

    sample_dis_get_two_sensor_vi_cfg(sns_type, &vi_cfg[0], &vi_cfg[1]);
    sample_comm_vi_get_size_by_sns_type(sns_type, &size);
    ret = sample_dis_start_multi_vi_vpss(vi_cfg, vpss_grp, 2, 2);  /* start 2 route */
    if (ret != HI_SUCCESS) {
        sample_print("start multi vi vpss failed! \n");
        goto start_vi_vpss_failed;
    }

    ret = sample_dis_start_venc_and_vo(vpss_grp, grp_num, vo_intf_type);
    if (ret != HI_SUCCESS) {
        sample_print("start venc and vo failed! \n");
        goto start_venc_and_vo_failed;
    }

    ret = sample_dis_ipc_gyro_two_sensor(&size);
    if (ret != HI_SUCCESS) {
        sample_print("sample_dis_ipc_gyro_two_sensor failed! \n");
        goto stop_sample;
    }

stop_sample:
    sample_dis_stop_venc_and_vo(vpss_grp, grp_num);

start_venc_and_vo_failed:
    sample_dis_stop_vpss(vpss_grp[0]);
    sample_dis_stop_vpss(vpss_grp[1]);
    sample_comm_vi_un_bind_vpss(vi_pipe[0], vi_chn, vpss_grp[0], vpss_chn);
    sample_comm_vi_un_bind_vpss(vi_pipe[1], vi_chn, vpss_grp[1], vpss_chn);
    sample_comm_vi_stop_vi(&vi_cfg[0]);
    sample_comm_vi_stop_vi(&vi_cfg[1]);

    sample_dis_stop_gyro();
start_vi_vpss_failed:
    sample_comm_sys_exit();
sys_init_failed:
    return ret;
}


/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description   : vdec sample
 * Author        : Hisilicon multimedia software group
 * Created       : 2019/07/16
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>

#include "sample_comm.h"
#ifdef HI_FPGA
    #define PIC_SIZE   PIC_1080P
#else
    #define PIC_SIZE   PIC_3840x2160
#endif

#define SAMPLE_STREAM_PATH "./source_file"
#define STREAN_WIDTH 3840
#define STREAM_HEIGHT 2160
#define REF_NUM 2
#define DISPLAY_NUM 2

hi_vo_intf_sync g_intf_sync = HI_VO_OUT_3840x2160_30;

static vdec_display_cfg g_vdec_display_cfg = {
    .pic_size = PIC_3840x2160,
    .intf_sync = HI_VO_OUT_3840x2160_30,
    .intf_type = HI_VO_INTF_HDMI,
};

hi_size g_disp_size;

#ifndef __huawei_lite__
hi_void sample_vdec_handle_sig(hi_s32 signo)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    if ((signo == SIGINT) || (signo == SIGTSTP) || (signo == SIGTERM)) {
        sample_comm_sys_exit();
        printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
    }

    exit(0);
}
#endif

hi_void sample_vdec_usage(const char *s_prg_nm)
{
    printf("\n/************************************/\n");
    printf("usage : %s <index> <intf_sync >\n", s_prg_nm);
    printf("index:\n");
    printf("\t0:  VDEC(H265 PLAYBACK)-VPSS-VO\n");
    printf("\t1:  VDEC(H264 PLAYBACK)-VPSS-VO\n");
    printf("\t2:  VDEC(H264 PREVIEW)-VPSS-VOPIP\n");
    printf("\t3:  VDEC(JPEG PLAYBACK)-VPSS-VO\n");
    printf("\t4:  VDEC(JPEG PROGRESSIVE PLAYBACK)-VPSS-VO\n");

    printf("\nintf_sync :\n");
#ifdef HI_FPGA
    printf("\t0: VO BT1120 \n");
#else
    printf("\t0: VO HDMI   \n");
    printf("\t1: VO BT1120 \n");
#endif
    printf("/************************************/\n\n");
}

hi_void sample_vdec_get_diplay_cfg()
{
#ifdef HI_FPGA
    g_vdec_display_cfg.pic_size   = PIC_720P;
    g_vdec_display_cfg.intf_sync  = HI_VO_OUT_720P60;
    g_vdec_display_cfg.intf_type   = HI_VO_INTF_BT1120;
#else
    if (g_intf_sync == HI_VO_OUT_3840x2160_30) {
        g_vdec_display_cfg.pic_size   = PIC_3840x2160;
        g_vdec_display_cfg.intf_sync  = HI_VO_OUT_3840x2160_30;
        g_vdec_display_cfg.intf_type   = HI_VO_INTF_HDMI;
    } else {
        g_vdec_display_cfg.pic_size   = PIC_1080P;
        g_vdec_display_cfg.intf_sync  = HI_VO_OUT_1080P60;
        g_vdec_display_cfg.intf_type   = HI_VO_INTF_BT1120;
    }
#endif
}

hi_s32 sample_init_module_vb(sample_vdec_attr *sample_vdec, hi_u32 vdec_chn_num, hi_payload_type type,
    hi_u32 len)
{
    hi_s32 i;
    hi_s32 ret;
    for (i = 0; (i < vdec_chn_num) && (i < len); i++) {
        sample_vdec[i].type                           = type;
        sample_vdec[i].width                         =
            sample_comm_vdec_get_progressive_en() ? 1024 : STREAN_WIDTH; /* 1024:image width */
        sample_vdec[i].height                        =
            sample_comm_vdec_get_progressive_en() ? 768 : STREAM_HEIGHT; /* 768:image height */
        sample_vdec[i].mode                           = HI_VDEC_SEND_MODE_FRAME;
        sample_vdec[i].sapmle_vdec_video.dec_mode      = HI_VIDEO_DEC_MODE_IP;
        sample_vdec[i].sapmle_vdec_video.bit_width     = HI_DATA_BIT_WIDTH_8;
        if (type == HI_PT_JPEG) {
            sample_vdec[i].sapmle_vdec_video.ref_frame_num = 0;
        } else {
            sample_vdec[i].sapmle_vdec_video.ref_frame_num = REF_NUM;
        }
        sample_vdec[i].display_frame_num               = DISPLAY_NUM;
        sample_vdec[i].frame_buf_cnt = (type == HI_PT_JPEG) ? sample_vdec[i].display_frame_num + 1 :
            sample_vdec[i].sapmle_vdec_video.ref_frame_num + sample_vdec[i].display_frame_num + 1;
        if (type == HI_PT_JPEG) {
            sample_vdec[i].sapmle_vdec_picture.pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
            sample_vdec[i].sapmle_vdec_picture.alpha      = 255; /* 255:pic alpha value */
        }
    }
    ret = sample_comm_vdec_init_vb_pool(vdec_chn_num, &sample_vdec[0], len);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("init mod common vb fail for %#x!\n", ret);
        return ret;
    }
    return ret;
}

hi_s32 sample_init_sys_and_vb(sample_vdec_attr *sample_vdec, hi_u32 vdec_chn_num, hi_payload_type type,
    hi_u32 len)
{
    hi_vb_cfg vb_cfg;
    hi_pic_buf_attr buf_attr = {0};
    hi_s32 ret;
    sample_vdec_get_diplay_cfg();
    ret = sample_comm_sys_get_pic_size(g_vdec_display_cfg.pic_size, &g_disp_size);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("sys get pic size fail for %#x!\n", ret);
        return ret;
    }
    buf_attr.align = 0;
    buf_attr.bit_width = HI_DATA_BIT_WIDTH_8;
    buf_attr.compress_mode = HI_COMPRESS_MODE_SEG;
    buf_attr.height = g_disp_size.height;
    buf_attr.width = g_disp_size.width;
    buf_attr.pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;

    (hi_void)memset_s(&vb_cfg, sizeof(hi_vb_cfg), 0, sizeof(hi_vb_cfg));
    vb_cfg.max_pool_cnt             = 1;
    vb_cfg.common_pool[0].blk_cnt  = 10 * vdec_chn_num; /* 10:common vb cnt */
    vb_cfg.common_pool[0].blk_size = hi_common_get_pic_buf_size(&buf_attr);
    ret = sample_comm_sys_init(&vb_cfg);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("init sys fail for %#x!\n", ret);
        sample_comm_sys_exit();
        return ret;
    }
    ret = sample_init_module_vb(&sample_vdec[0], vdec_chn_num, type, len);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("init mod vb fail for %#x!\n", ret);
        sample_comm_vdec_exit_vb_pool();
        sample_comm_sys_exit();
        return ret;
    }
    return ret;
}

hi_s32 get_vb_attr_vdec(hi_u32 vdec_chn_num, sample_venc_vb_attr *vb_attr)
{
    hi_pic_buf_attr buf_attr = {0};
    hi_s32 ret;
    sample_vdec_get_diplay_cfg();
    ret = sample_comm_sys_get_pic_size(g_vdec_display_cfg.pic_size, &g_disp_size);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("sys get pic size fail for %#x!\n", ret);
        return ret;
    }
    buf_attr.align = 0;
    buf_attr.bit_width = HI_DATA_BIT_WIDTH_8;
    buf_attr.compress_mode = HI_COMPRESS_MODE_SEG;
    buf_attr.height = g_disp_size.height;
    buf_attr.width = g_disp_size.width;
    buf_attr.pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;

    vb_attr->blk_cnt[vb_attr->valid_num]  = 4 * vdec_chn_num; /* 4:common vb cnt */
    vb_attr->blk_size[vb_attr->valid_num] = hi_common_get_pic_buf_size(&buf_attr);
    vb_attr->valid_num++;

    return ret;
}



hi_s32 sample_start_vpss_crop(hi_vpss_grp *vpss_grp)
{
    hi_vpss_crop_info crop_info;
    hi_s32 ret;
    *vpss_grp = 1;
    crop_info.enable = HI_TRUE;
    crop_info.crop_mode = HI_COORD_ABS;
    crop_info.crop_rect.x = HI_ALIGN_UP(STREAN_WIDTH / 4, 2); /* 4,2:align */
    crop_info.crop_rect.y = HI_ALIGN_UP(STREAM_HEIGHT / 4, 2); /* 4,2:align */
    crop_info.crop_rect.width = HI_ALIGN_UP(STREAN_WIDTH / 2, 2); /* 2,2:align */
    crop_info.crop_rect.height = HI_ALIGN_UP(STREAM_HEIGHT / 2, 2); /* 2,2:align */
    ret = hi_mpi_vpss_set_grp_crop(*vpss_grp, &crop_info);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("start VPSS fail for %#x!\n", ret);
        return ret;
    }
    return ret;
}

hi_s32 sample_vdec_bind_vpss(hi_u32 vpss_grp_num, hi_bool is_pip)
{
    hi_s32 i;
    hi_s32 ret = HI_SUCCESS;
    for (i = 0; i < vpss_grp_num; i++) {
        ret = sample_comm_vdec_bind_vpss(is_pip ? 0 : i, i);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("vdec bind vpss fail for %#x!\n", ret);
            return ret;
        }
    }
    return ret;
}

hi_void sample_stop_vpss(hi_vpss_grp vpss_grp, hi_bool *vpss_chn_enable, hi_u32 chn_array_size)
{
    hi_s32 i;
    for (i = vpss_grp; i >= 0; i--) {
        vpss_grp = i;
        sample_common_vpss_stop(vpss_grp, &vpss_chn_enable[0], chn_array_size);
    }
}

hi_s32 sample_vdec_unbind_vpss(hi_u32 vpss_grp_num, hi_bool is_pip)
{
    hi_s32 i;
    hi_s32 ret = HI_SUCCESS;
    for (i = 0; i < vpss_grp_num; i++) {
        ret = sample_comm_vdec_un_bind_vpss(is_pip ? 0 : i, i);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("vdec unbind vpss fail for %#x!\n", ret);
        }
    }
    return ret;
}

hi_void sample_config_vpss_grp_attr(hi_vpss_grp_attr *vpss_grp_attr)
{
    vpss_grp_attr->max_width = sample_comm_vdec_get_progressive_en() ? 1024 : STREAN_WIDTH; /* 1024:image width */
    vpss_grp_attr->max_height = sample_comm_vdec_get_progressive_en() ? 768 : STREAM_HEIGHT; /* 768:image height */
    vpss_grp_attr->frame_rate.src_frame_rate = -1;
    vpss_grp_attr->frame_rate.dst_frame_rate = -1;
    vpss_grp_attr->pixel_format  = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    vpss_grp_attr->nr_en   = HI_FALSE;
    vpss_grp_attr->ie_en   = HI_FALSE;
    vpss_grp_attr->dci_en   = HI_FALSE;
    vpss_grp_attr->dei_mode = HI_VPSS_DEI_MODE_OFF;
    vpss_grp_attr->buf_share_en   = HI_FALSE;
}

hi_s32 sample_start_vpss(hi_vpss_grp *vpss_grp, hi_u32 vpss_grp_num, hi_bool *vpss_chn_enable,
    hi_bool is_pip, hi_u32 arr_len)
{
    hi_s32 i;
    hi_vpss_chn_attr vpss_chn_attr[HI_VPSS_MAX_CHN_NUM];
    hi_vpss_grp_attr vpss_grp_attr = {0};
    hi_s32 ret = HI_SUCCESS;
    sample_config_vpss_grp_attr(&vpss_grp_attr);
    (hi_void)memset_s(vpss_chn_enable, arr_len * sizeof(hi_bool), 0, arr_len * sizeof(hi_bool));
    if (arr_len > 1) {
        vpss_chn_enable[1] = HI_TRUE;
        vpss_chn_attr[1].width                    = is_pip ? g_disp_size.width / 4 : g_disp_size.width; /* 4:crop */
        vpss_chn_attr[1].height                   = is_pip ? g_disp_size.height / 4 : g_disp_size.height; /* 4:crop */
        vpss_chn_attr[1].chn_mode                   = HI_VPSS_CHN_MODE_AUTO;
        vpss_chn_attr[1].compress_mode              = is_pip ? HI_COMPRESS_MODE_NONE : HI_COMPRESS_MODE_SEG;
        vpss_chn_attr[1].pixel_format               = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
        vpss_chn_attr[1].frame_rate.src_frame_rate = -1;
        vpss_chn_attr[1].frame_rate.dst_frame_rate = -1;
        vpss_chn_attr[1].depth                    = 0;
        vpss_chn_attr[1].mirror_en                     = HI_FALSE;
        vpss_chn_attr[1].flip_en                       = HI_FALSE;
        vpss_chn_attr[1].border_en                       = HI_FALSE;
        vpss_chn_attr[1].aspect_ratio.mode        = HI_ASPECT_RATIO_NONE;
    }

    for (i = 0; i < vpss_grp_num; i++) {
        *vpss_grp = i;
        ret = sample_common_vpss_start(*vpss_grp, &vpss_chn_enable[0],
            &vpss_grp_attr, vpss_chn_attr, HI_VPSS_MAX_CHN_NUM);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("start VPSS fail for %#x!\n", ret);
            goto VPSS_FAIL;
        }
    }
    if (is_pip) {
        ret = sample_start_vpss_crop(vpss_grp);
        if (ret != HI_SUCCESS) {
            goto VPSS_FAIL;
        }
    }
    ret = sample_vdec_bind_vpss(vpss_grp_num, is_pip);
    if (ret != HI_SUCCESS) {
        sample_vdec_unbind_vpss(vpss_grp_num, is_pip);
        goto VPSS_FAIL;
    }
    return ret;

VPSS_FAIL:
    sample_stop_vpss(*vpss_grp, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
    return ret;
}

hi_s32 sample_vpss_unbind_vo(hi_u32 vpss_grp_num, sample_vo_config vo_config)
{
    hi_s32 i;
    hi_vo_layer vo_layer = vo_config.vo_dev;
    hi_s32 ret = HI_SUCCESS;
    for (i = 0; i < vpss_grp_num; i++) {
        ret = sample_comm_vpss_un_bind_vo(i, 1, vo_layer, i);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("vpss unbind vo fail for %#x!\n", ret);
        }
    }
    return ret;
}

hi_s32 sample_vpss_bind_vo(sample_vo_config vo_config, hi_u32 vpss_grp_num, hi_bool is_pip)
{
    hi_s32 i;
    hi_vo_layer vo_layer;
    hi_s32 ret = HI_SUCCESS;
    vo_layer = vo_config.vo_dev;
    for (i = 0; i < vpss_grp_num; i++) {
        if (is_pip) {
            vo_layer = (i == 0) ? 0 : 2; /* 2:layer id */
        }
        ret = sample_comm_vpss_bind_vo(i, 1, vo_layer, is_pip ? 0 : i);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("vpss bind vo fail for %#x!\n", ret);
            return ret;
        }
    }
    return ret;
}

hi_s32 sample_start_vo(sample_vo_config *vo_config, hi_u32 vpss_grp_num, hi_bool is_pip)
{
    hi_s32 ret;
    vo_config->vo_dev                 = SAMPLE_VO_DEV_UHD;
    vo_config->vo_intf_type          = g_vdec_display_cfg.intf_type;
    vo_config->intf_sync            = g_vdec_display_cfg.intf_sync;
    vo_config->pic_size             = g_vdec_display_cfg.pic_size;
    vo_config->bg_color            = COLOR_RGB_BLUE;
    vo_config->dis_buf_len          = 3; /* 3:buf length */
    vo_config->dst_dynamic_range     = HI_DYNAMIC_RANGE_SDR8;
    vo_config->vo_mode              = VO_MODE_1MUX;
    vo_config->pix_format           = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    vo_config->disp_rect.x       = 0;
    vo_config->disp_rect.y       = 0;
    vo_config->disp_rect.width   = g_disp_size.width;
    vo_config->disp_rect.height  = g_disp_size.height;
    vo_config->image_size.width  = g_disp_size.width;
    vo_config->image_size.height = g_disp_size.height;
    vo_config->vo_part_mode          = HI_VO_PARTITION_MODE_SINGLE;
    vo_config->compress_mode         = HI_COMPRESS_MODE_NONE;

    ret = sample_comm_vo_start_vo(vo_config);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("start VO fail for %#x!\n", ret);
        goto VO_FAIL;
    }

    if (is_pip == HI_TRUE) {
        ret = sample_comm_vo_start_pip(vo_config);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("start VO PIP fail for %#x!\n", ret);
            sample_comm_vo_stop_pip(vo_config);
            goto VO_FAIL;
        }
    }

    ret = sample_vpss_bind_vo(*vo_config, vpss_grp_num, is_pip);
    if (ret != HI_SUCCESS) {
        sample_vpss_unbind_vo(vpss_grp_num, *vo_config);
        if (is_pip == HI_TRUE) {
            sample_comm_vo_stop_pip(vo_config);
        }
        goto VO_FAIL;
    }

    return ret;

VO_FAIL:
    sample_comm_vo_stop_vo(vo_config);
    return ret;
}

hi_void sample_send_stream_to_vdec(sample_vdec_attr *sample_vdec, hi_u32 arr_len, hi_u32 vdec_chn_num,
    const char *stream_name, hi_bool is_pip)
{
    hi_s32 i;
    vdec_thread_param vdec_send[HI_VDEC_MAX_CHN_NUM];
    pthread_t   vdec_thread[HI_VDEC_MAX_CHN_NUM] = {0}; /* 2:thread */
    if (arr_len > HI_VDEC_MAX_CHN_NUM) {
        SAMPLE_PRT("array size(%u) of vdec_send need < %u!\n", arr_len, HI_VDEC_MAX_CHN_NUM);
        return;
    }
    for (i = 0; (i < vdec_chn_num) && (i < arr_len); i++) {
        if (snprintf_s(vdec_send[i].c_file_name, sizeof(vdec_send[i].c_file_name), sizeof(vdec_send[i].c_file_name) - 1,
            stream_name) < 0) {
            return;
        }
        if (snprintf_s(vdec_send[i].c_file_path, sizeof(vdec_send[i].c_file_path), sizeof(vdec_send[i].c_file_path) - 1,
            "%s", SAMPLE_STREAM_PATH) < 0) {
            return;
        }
        vdec_send[i].type          = sample_vdec[i].type;
        vdec_send[i].stream_mode   = sample_vdec[i].mode;
        vdec_send[i].chn_id        = i;
        vdec_send[i].interval_time = 1000; /* 1000: interval time */
        vdec_send[i].pts_init      = 0;
        vdec_send[i].pts_increase  = 0;
        vdec_send[i].e_thread_ctrl     = THREAD_CTRL_START;
        vdec_send[i].circle_send     = HI_TRUE;
        vdec_send[i].milli_sec     = 0;
        vdec_send[i].min_buf_size   = (sample_vdec[i].width * sample_vdec[i].height * 3) >> 1; /* 3:yuv */
    }
    sample_comm_vdec_start_send_stream(vdec_chn_num, &vdec_send[0], &vdec_thread[0],
        HI_VDEC_MAX_CHN_NUM, HI_VDEC_MAX_CHN_NUM);
    if (is_pip) {
        printf("\n_sample_test:press any key to exit!\n");
        getchar();
    } else {
        sample_comm_vdec_cmd_ctrl(vdec_chn_num, &vdec_send[0], &vdec_thread[0],
            HI_VDEC_MAX_CHN_NUM, HI_VDEC_MAX_CHN_NUM);
    }

    sample_comm_vdec_stop_send_stream(vdec_chn_num, &vdec_send[0], &vdec_thread[0],
        HI_VDEC_MAX_CHN_NUM, HI_VDEC_MAX_CHN_NUM);
}

hi_s32 sample_start_vdec(sample_vdec_attr *sample_vdec, hi_u32 vdec_chn_num, hi_u32 len, hi_bool is_pip)
{
    hi_s32 i, ret;

    ret = sample_comm_vdec_start(vdec_chn_num, &sample_vdec[0], len);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("start VDEC fail for %#x!\n", ret);
        goto VDEC_FAIL;
    }
    if (is_pip == HI_TRUE) {
        for (i = 0; i < vdec_chn_num; i++) {
            ret = hi_mpi_vdec_set_display_mode(i, HI_VIDEO_DISPLAY_MODE_PREVIEW);
            if (ret != HI_SUCCESS) {
                SAMPLE_PRT("set VDEC display_mode fail for %#x!\n", ret);
                goto VDEC_FAIL;
            }
        }
    }

    return ret;
VDEC_FAIL:
    sample_comm_vdec_stop(vdec_chn_num);
    return ret;
}

hi_s32 sample_h265_vdec_vpss_vo(hi_void)
{
    hi_s32 ret;
    hi_u32 vdec_chn_num, vpss_grp_num;
    sample_vdec_attr sample_vdec[HI_VDEC_MAX_CHN_NUM];
    hi_bool vpss_chn_enable[HI_VPSS_MAX_CHN_NUM];
    sample_vo_config vo_config;
    hi_vpss_grp vpss_grp;

    vdec_chn_num = 1;
    vpss_grp_num = vdec_chn_num;
    /************************************************
    step1:  init SYS, init common VB(for VPSS and VO), init module VB(for VDEC)
    *************************************************/
    ret = sample_init_sys_and_vb(&sample_vdec[0], vdec_chn_num, HI_PT_H265, HI_VDEC_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    /************************************************
    step2:  init VDEC
    *************************************************/
    ret = sample_start_vdec(&sample_vdec[0], vdec_chn_num, HI_VDEC_MAX_CHN_NUM, HI_FALSE);
    if (ret != HI_SUCCESS) {
        goto END1;
    }
    /************************************************
    step3:  start VPSS
    *************************************************/
    ret = sample_start_vpss(&vpss_grp, vpss_grp_num, &vpss_chn_enable[0], HI_FALSE, HI_VPSS_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        goto END2;
    }
    /************************************************
    step4:  start VO
    *************************************************/
    ret = sample_start_vo(&vo_config, vpss_grp_num, HI_FALSE);
    if (ret != HI_SUCCESS) {
        goto END3;
    }

    /************************************************
    step5:  send stream to VDEC
    *************************************************/
    sample_send_stream_to_vdec(&sample_vdec[0], HI_VDEC_MAX_CHN_NUM, vdec_chn_num, "3840x2160_8bit.h265", HI_FALSE);

    ret = sample_vpss_unbind_vo(vpss_grp_num, vo_config);
    sample_comm_vo_stop_vo(&vo_config);
END3:
    ret = sample_vdec_unbind_vpss(vpss_grp_num, HI_FALSE);
    sample_stop_vpss(vpss_grp, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
END2:
    sample_comm_vdec_stop(vdec_chn_num);
    sample_comm_vdec_exit_vb_pool();
END1:
    sample_comm_sys_exit();

    return ret;
}

hi_s32 sample_h264_vdec_vpss_vo(hi_void)
{
    hi_s32 ret;
    hi_u32 vdec_chn_num, vpss_grp_num;
    hi_vpss_grp vpss_grp;
    sample_vdec_attr sample_vdec[HI_VDEC_MAX_CHN_NUM];
    sample_vo_config vo_config;
    hi_bool vpss_chn_enable[HI_VPSS_MAX_CHN_NUM];

    vdec_chn_num = 1;
    vpss_grp_num = vdec_chn_num;

    /************************************************
    step1:  init SYS, init common VB(for VPSS and VO), init module VB(for VDEC)
    *************************************************/
    ret = sample_init_sys_and_vb(&sample_vdec[0], vdec_chn_num, HI_PT_H264, HI_VDEC_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    /************************************************
    step2:  init VDEC
    *************************************************/
    ret = sample_start_vdec(&sample_vdec[0], vdec_chn_num, HI_VDEC_MAX_CHN_NUM, HI_FALSE);
    if (ret != HI_SUCCESS) {
        goto END1;
    }
    /************************************************
    step3:  start VPSS
    *************************************************/
    ret = sample_start_vpss(&vpss_grp, vpss_grp_num, &vpss_chn_enable[0], HI_FALSE, HI_VPSS_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        goto END2;
    }
    /************************************************
    step4:  start VO
    *************************************************/
    ret = sample_start_vo(&vo_config, vpss_grp_num, HI_FALSE);
    if (ret != HI_SUCCESS) {
        goto END3;
    }

    /************************************************
    step5:  send stream to VDEC
    *************************************************/
    sample_send_stream_to_vdec(&sample_vdec[0], HI_VDEC_MAX_CHN_NUM, vdec_chn_num, "3840x2160_8bit.h264", HI_FALSE);

    ret = sample_vpss_unbind_vo(vpss_grp_num, vo_config);
    sample_comm_vo_stop_vo(&vo_config);
END3:
    ret = sample_vdec_unbind_vpss(vpss_grp_num, HI_FALSE);
    sample_stop_vpss(vpss_grp, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
END2:
    sample_comm_vdec_stop(vdec_chn_num);
    sample_comm_vdec_exit_vb_pool();
END1:
    sample_comm_sys_exit();

    return ret;
}

hi_s32 sample_h264_vdec_vpss_vo_pip(hi_void)
{
    hi_s32 ret;
    hi_u32 vdec_chn_num, vpss_grp_num;
    hi_vpss_grp vpss_grp;
    sample_vdec_attr sample_vdec[HI_VDEC_MAX_CHN_NUM];
    sample_vo_config vo_config;
    hi_bool vpss_chn_enable[HI_VPSS_MAX_CHN_NUM];

    vdec_chn_num = 1;
    vpss_grp_num = 2; /* 2:vpss group cnt */

    /************************************************
    step1:  init SYS, init common VB(for VPSS and VO), init module VB(for VDEC)
    *************************************************/
    ret = sample_init_sys_and_vb(&sample_vdec[0], vdec_chn_num, HI_PT_H264, HI_VDEC_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    /************************************************
    step2:  init VDEC
    *************************************************/
    ret = sample_start_vdec(&sample_vdec[0], vdec_chn_num, HI_VDEC_MAX_CHN_NUM, HI_TRUE);
    if (ret != HI_SUCCESS) {
        goto END1;
    }
    /************************************************
    step3:  start VPSS
    *************************************************/
    ret = sample_start_vpss(&vpss_grp, vpss_grp_num, &vpss_chn_enable[0], HI_TRUE, HI_VPSS_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        goto END2;
    }
    /************************************************
    step4:  start VO
    *************************************************/
    ret = sample_start_vo(&vo_config, vpss_grp_num, HI_TRUE);
    if (ret != HI_SUCCESS) {
        goto END3;
    }

    /************************************************
    step5:  send stream to VDEC
    *************************************************/
    sample_send_stream_to_vdec(&sample_vdec[0], HI_VDEC_MAX_CHN_NUM, vdec_chn_num, "3840x2160_8bit.h264", HI_TRUE);

    ret = sample_vpss_unbind_vo(vpss_grp_num, vo_config);
    sample_comm_vo_stop_pip(&vo_config);
    sample_comm_vo_stop_vo(&vo_config);
END3:
    usleep(150000); /* 150000:make sure all frames are decoded before unbind */
    ret = sample_vdec_unbind_vpss(vpss_grp_num, HI_TRUE);
    sample_stop_vpss(vpss_grp, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
END2:
    sample_comm_vdec_stop(vdec_chn_num);
    sample_comm_vdec_exit_vb_pool();
END1:
    sample_comm_sys_exit();

    return ret;
}

hi_s32 sample_jpeg_vdec_vpss_vo(hi_void)
{
    hi_s32 ret;
    hi_u32 vdec_chn_num, vpss_grp_num;
    hi_vpss_grp vpss_grp;
    sample_vdec_attr sample_vdec[HI_VDEC_MAX_CHN_NUM];
    sample_vo_config vo_config;
    hi_bool vpss_chn_enable[HI_VPSS_MAX_CHN_NUM];

    vdec_chn_num = 1;
    vpss_grp_num = vdec_chn_num;

    /************************************************
    step1:  init SYS, init common VB(for VPSS and VO), init module VB(for VDEC)
    *************************************************/
    ret = sample_init_sys_and_vb(&sample_vdec[0], vdec_chn_num, HI_PT_JPEG, HI_VDEC_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    /************************************************
    step2:  init VDEC
    *************************************************/
    ret = sample_start_vdec(&sample_vdec[0], vdec_chn_num, HI_VDEC_MAX_CHN_NUM, HI_FALSE);
    if (ret != HI_SUCCESS) {
        goto END1;
    }
    /************************************************
    step3:  start VPSS
    *************************************************/
    ret = sample_start_vpss(&vpss_grp, vpss_grp_num, &vpss_chn_enable[0], HI_FALSE, HI_VPSS_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        goto END2;
    }
    /************************************************
    step4:  start VO
    *************************************************/
    ret = sample_start_vo(&vo_config, vpss_grp_num, HI_FALSE);
    if (ret != HI_SUCCESS) {
        goto END3;
    }

    /************************************************
    step5:  send stream to VDEC
    *************************************************/
    sample_send_stream_to_vdec(&sample_vdec[0], HI_VDEC_MAX_CHN_NUM, vdec_chn_num, "3840x2160.jpg", HI_FALSE);

    ret = sample_vpss_unbind_vo(vpss_grp_num, vo_config);
    sample_comm_vo_stop_vo(&vo_config);
END3:
    ret = sample_vdec_unbind_vpss(vpss_grp_num, HI_FALSE);
    sample_stop_vpss(vpss_grp, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
END2:
    sample_comm_vdec_stop(vdec_chn_num);
    sample_comm_vdec_exit_vb_pool();
END1:
    sample_comm_sys_exit();

    return ret;
}

hi_s32 sample_jpeg_progressive_vdec_vpss_vo(hi_void)
{
    hi_s32 ret;
    hi_u32 vdec_chn_num, vpss_grp_num;
    hi_vpss_grp vpss_grp;
    sample_vdec_attr sample_vdec[HI_VDEC_MAX_CHN_NUM];
    sample_vo_config vo_config;
    hi_bool vpss_chn_enable[HI_VPSS_MAX_CHN_NUM];

    vdec_chn_num = 1;
    vpss_grp_num = vdec_chn_num;

    sample_comm_vdec_set_progressive_en(HI_TRUE);
    /************************************************
    step1:  init SYS, init common VB(for VPSS and VO), init module VB(for VDEC)
    *************************************************/
    ret = sample_init_sys_and_vb(&sample_vdec[0], vdec_chn_num, HI_PT_JPEG, HI_VDEC_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    /************************************************
    step2:  init VDEC
    *************************************************/
    ret = sample_start_vdec(&sample_vdec[0], vdec_chn_num, HI_VDEC_MAX_CHN_NUM, HI_FALSE);
    if (ret != HI_SUCCESS) {
        goto END1;
    }
    /************************************************
    step3:  start VPSS
    *************************************************/
    ret = sample_start_vpss(&vpss_grp, vpss_grp_num, &vpss_chn_enable[0], HI_FALSE, HI_VPSS_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        goto END2;
    }
    /************************************************
    step4:  start VO
    *************************************************/
    ret = sample_start_vo(&vo_config, vpss_grp_num, HI_FALSE);
    if (ret != HI_SUCCESS) {
        goto END3;
    }

    /************************************************
    step5:  send stream to VDEC
    *************************************************/
    sample_send_stream_to_vdec(&sample_vdec[0], HI_VDEC_MAX_CHN_NUM, vdec_chn_num,
        "1024x768_progressive.jpg", HI_FALSE);

    ret = sample_vpss_unbind_vo(vpss_grp_num, vo_config);
    sample_comm_vo_stop_vo(&vo_config);
END3:
    ret = sample_vdec_unbind_vpss(vpss_grp_num, HI_FALSE);
    sample_stop_vpss(vpss_grp, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
END2:
    sample_comm_vdec_stop(vdec_chn_num);
    sample_comm_vdec_exit_vb_pool();
END1:
    sample_comm_sys_exit();

    return ret;
}

hi_s32 sample_check_parameter(int argc, const char *argv0, const char *argv1, const char *argv2)
{
    if ((argc != 3) || (strlen(argv1) != 1) || (strlen(argv2) != 1)) { /* 3:arg num */
        printf("\n_invaild input!  for examples:\n");
        sample_vdec_usage(argv0);
        return HI_FAILURE;
    }

    if (!strncmp(argv2, "1", 1)) {
        g_intf_sync = HI_VO_OUT_1080P60;
    } else if (!strncmp(argv2, "0", 1)) {
        g_intf_sync = HI_VO_OUT_3840x2160_30;
    } else {
        printf("\n_invaild input!  for examples:\n");
        printf("\n argv2 is %s\n", argv2);
        sample_vdec_usage(argv0);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 sample_choose_case(const char argv1, const char *argv0)
{
    hi_s32 ret = HI_SUCCESS;
    switch (argv1) {
        case '0': {
            ret = sample_h265_vdec_vpss_vo();
            break;
        }
        case '1': {
            ret = sample_h264_vdec_vpss_vo();
            break;
        }
        case '2': {
            ret = sample_h264_vdec_vpss_vo_pip();
            break;
        }
        case '3': {
            ret = sample_jpeg_vdec_vpss_vo();
            break;
        }
        case '4': {
            ret = sample_jpeg_progressive_vdec_vpss_vo();
            break;
        }
        default: {
            SAMPLE_PRT("the index is invaild!\n");
            sample_vdec_usage(argv0);
            ret = HI_FAILURE;
            break;
        }
    }
    return ret;
}

/******************************************************************************
* function    : main()
* description : video vdec sample
******************************************************************************/
#ifdef __huawei_lite__
    int app_main(int argc, char *argv[])
#else
    int sample_vdec_main(int argc, char *argv[])
#endif
{
    hi_s32 ret;

    ret = sample_check_parameter(argc, argv[0], argv[1], argv[2]); /* 2:array index */
    if (ret != HI_SUCCESS) {
        return ret;
    }

#ifndef __huawei_lite__
    signal(SIGINT, sample_vdec_handle_sig);
    signal(SIGTERM, sample_vdec_handle_sig);
#endif

    /******************************************
     choose the case
    ******************************************/
    ret = sample_choose_case(*argv[1], argv[0]);
    if (ret == HI_SUCCESS) {
        SAMPLE_PRT("program exit normally!\n");
    } else {
        SAMPLE_PRT("program exit abnormally!\n");
    }

    return ret;
}

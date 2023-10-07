/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <poll.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include "sample_comm.h"
#include "heif_format.h"

#define PIC_SIZE   PIC_3840X2160
#define SAMPLE_STREAM_PATH "./source_file"
#define UHD_STREAN_WIDTH 3840
#define UHD_STREAM_HEIGHT 2160
#define FHD_STREAN_WIDTH 1920
#define FHD_STREAN_HEIGHT 1080
#define REF_NUM 2
#define DISPLAY_NUM 1
//maohw
#define SAMPLE_VDEC_COMM_VB_CNT (4*1)
#define SAMPLE_VDEC_VPSS_LOW_DELAY_LINE_CNT 16

static hi_payload_type g_cur_type = HI_PT_H265;

static vdec_display_cfg g_vdec_display_cfg = {
    .pic_size = PIC_1080P,
    .intf_sync = HI_VO_OUT_1080P60,
    .intf_type = HI_VO_INTF_BT1120,
};

static hi_size g_disp_size;
static hi_s32 g_sample_exit = 0;

#ifndef __LITEOS__
static hi_void sample_vdec_handle_sig(hi_s32 signo)
{
    if ((signo == SIGINT) || (signo == SIGTERM)) {
        g_sample_exit = 1;
    }
}
#endif

static hi_s32 sample_getchar()
{
    int c;
    if (g_sample_exit == 1) {
        return 'e';
    }

    c = getchar();

    if (g_sample_exit == 1) {
        return 'e';
    }
    return c;
}

static hi_void sample_vdec_usage(const char *s_prg_nm)
{
    printf("\n/************************************/\n");
    printf("usage : %s <index>\n", s_prg_nm);
    printf("index:\n");
    printf("\t0:  VDEC(H265 PREVIEW)-VPSS-VO\n");
    printf("\t1:  VDEC(H264 PREVIEW)-VPSS-VO\n");
    printf("\t2:  VDEC(JPEG PREVIEW)-VPSS-VO\n");
    printf("\t3:  VDEC(H265 LOWDELAY PREVIEW)-VPSS-VO\n");
    printf("\t4:  VDEC(HEIC H265 PREVIEW)-VPSS-VO\n");
    printf("/************************************/\n\n");
}

static hi_u32 sample_vdec_get_chn_width(hi_void)
{
    switch (g_cur_type) {
        case HI_PT_H264:
        case HI_PT_H265:
            return UHD_STREAN_WIDTH;
        case HI_PT_JPEG:
        case HI_PT_MJPEG:
            return UHD_STREAN_WIDTH;
        default:
            sample_print("invalid type %d!\n", g_cur_type);
            return UHD_STREAN_WIDTH;
    }
}

static hi_u32 sample_vdec_get_chn_height(hi_void)
{
    switch (g_cur_type) {
        case HI_PT_H264:
        case HI_PT_H265:
            return UHD_STREAM_HEIGHT;
        case HI_PT_JPEG:
        case HI_PT_MJPEG:
            return UHD_STREAM_HEIGHT;
        default:
            sample_print("invalid type %d!\n", g_cur_type);
            return UHD_STREAM_HEIGHT;
    }
}

#ifdef SAMPLE_MEM_SHARE_ENABLE
static hi_void sample_init_mem_share(hi_void)
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

    if (hi_mpi_vb_get_mod_common_pool_id(HI_VB_UID_VDEC, &pools_id) != HI_SUCCESS) {
        sample_print("get mod(vb_uid %d) common pool_id failed!\n", HI_VB_UID_VDEC);
        return;
    }
    for (i = 0; i < pools_id.pool_cnt; ++i) {
        hi_mpi_vb_pool_share_all(pools_id.pool[i]);
    }
}
#endif

/*maohw static*/ hi_s32 sample_init_module_vb(sample_vdec_attr *sample_vdec, hi_u32 vdec_chn_num, hi_payload_type type,
    hi_u32 len)
{
    hi_u32 i;
    hi_s32 ret;
    for (i = 0; (i < vdec_chn_num) && (i < len); i++) {
        sample_vdec[i].type                        = type;
        sample_vdec[i].width                       = sample_vdec_get_chn_width();
        sample_vdec[i].height                      = sample_vdec_get_chn_height();
        sample_vdec[i].mode                        = HI_VDEC_SEND_MODE_FRAME;
        sample_vdec[i].sample_vdec_video.dec_mode  = HI_VIDEO_DEC_MODE_IP;
        sample_vdec[i].sample_vdec_video.bit_width = HI_DATA_BIT_WIDTH_8;
        if (type == HI_PT_JPEG) {
            sample_vdec[i].sample_vdec_video.ref_frame_num = 0;
        } else {
            sample_vdec[i].sample_vdec_video.ref_frame_num = REF_NUM;
        }
        sample_vdec[i].display_frame_num               = DISPLAY_NUM;
        sample_vdec[i].frame_buf_cnt = (type == HI_PT_JPEG) ? (sample_vdec[i].display_frame_num + 1) :
            (sample_vdec[i].sample_vdec_video.ref_frame_num + sample_vdec[i].display_frame_num + 1);
        if (type == HI_PT_JPEG) {
            sample_vdec[i].sample_vdec_picture.pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
            sample_vdec[i].sample_vdec_picture.alpha      = 255; /* 255:pic alpha value */
        }
    }
    ret = sample_comm_vdec_init_vb_pool(vdec_chn_num, &sample_vdec[0], len);
    if (ret != HI_SUCCESS) {
        sample_print("init mod common vb fail for %#x!\n", ret);
        return ret;
    }
#ifdef SAMPLE_MEM_SHARE_ENABLE
    sample_init_mem_share();
#endif
    return ret;
}

static hi_s32 sample_init_sys_and_vb(sample_vdec_attr *sample_vdec, hi_u32 vdec_chn_num, hi_payload_type type,
    hi_u32 len)
{
    hi_vb_cfg vb_cfg;
    hi_pic_buf_attr buf_attr = {0};
    hi_s32 ret;

    ret = sample_comm_sys_get_pic_size(g_vdec_display_cfg.pic_size, &g_disp_size);
    if (ret != HI_SUCCESS) {
        sample_print("sys get pic size fail for %#x!\n", ret);
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
    ret = sample_comm_sys_vb_init(&vb_cfg);
    if (ret != HI_SUCCESS) {
        sample_print("init sys fail for %#x!\n", ret);
        sample_comm_sys_exit();
        return ret;
    }
    ret = sample_init_module_vb(&sample_vdec[0], vdec_chn_num, type, len);
    if (ret != HI_SUCCESS) {
        sample_print("init mod vb fail for %#x!\n", ret);
        sample_comm_vdec_exit_vb_pool();
        sample_comm_sys_exit();
        return ret;
    }
    return ret;
}

//maohw;
hi_s32 get_vb_attr_vdec(hi_u32 vdec_chn_num, sample_venc_vb_attr *vb_attr)
{
    hi_pic_buf_attr buf_attr = {0};
    hi_s32 ret;
    //sample_vdec_get_diplay_cfg();
    ret = sample_comm_sys_get_pic_size(g_vdec_display_cfg.pic_size, &g_disp_size);
    if (ret != HI_SUCCESS) {
        sample_print("sys get pic size fail for %#x!\n", ret);
        return ret;
    }
    buf_attr.align = HI_DEFAULT_ALIGN;
    buf_attr.bit_width = HI_DATA_BIT_WIDTH_8;
    buf_attr.compress_mode = HI_COMPRESS_MODE_SEG;
    buf_attr.height = g_disp_size.height;
    buf_attr.width = g_disp_size.width;
    buf_attr.pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;

    vb_attr->blk_cnt[vb_attr->valid_num]  = SAMPLE_VDEC_COMM_VB_CNT * vdec_chn_num; /* 4:common vb cnt */
    vb_attr->blk_size[vb_attr->valid_num] = hi_common_get_pic_buf_size(&buf_attr);
    vb_attr->valid_num++;

    return ret;
}

static hi_s32 sample_vdec_bind_vpss(hi_u32 vpss_grp_num)
{
    hi_u32 i;
    hi_s32 ret = HI_SUCCESS;
    for (i = 0; i < vpss_grp_num; i++) {
        ret = sample_comm_vdec_bind_vpss(i, i);
        if (ret != HI_SUCCESS) {
            sample_print("vdec bind vpss fail for %#x!\n", ret);
            return ret;
        }
    }
    return ret;
}

static hi_void sample_stop_vpss(hi_vpss_grp vpss_grp, hi_bool *vpss_chn_enable, hi_u32 chn_array_size)
{
    hi_s32 i;
    for (i = vpss_grp; i >= 0; i--) {
        vpss_grp = i;
        sample_common_vpss_stop(vpss_grp, &vpss_chn_enable[0], chn_array_size);
    }
}

static hi_void sample_vdec_unbind_vpss(hi_u32 vpss_grp_num)
{
    hi_u32 i;
    hi_s32 ret = HI_SUCCESS;
    for (i = 0; i < vpss_grp_num; i++) {
        ret = sample_comm_vdec_un_bind_vpss(i, i);
        if (ret != HI_SUCCESS) {
            sample_print("vdec unbind vpss fail for %#x!\n", ret);
        }
    }
    return;
}

static hi_void sample_config_vpss_grp_attr(hi_vpss_grp_attr *vpss_grp_attr)
{
    vpss_grp_attr->max_width = sample_vdec_get_chn_width();
    vpss_grp_attr->max_height = sample_vdec_get_chn_height();
    vpss_grp_attr->frame_rate.src_frame_rate = -1;
    vpss_grp_attr->frame_rate.dst_frame_rate = -1;
    vpss_grp_attr->pixel_format  = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    vpss_grp_attr->ie_en   = HI_FALSE;
    vpss_grp_attr->dci_en   = HI_FALSE;
    vpss_grp_attr->dei_mode = HI_VPSS_DEI_MODE_OFF;
    vpss_grp_attr->buf_share_en   = HI_FALSE;
}

static hi_s32 sample_config_vpss_ldy_attr(hi_u32 vpss_grp_num)
{
    hi_s32 i;
    hi_low_delay_info vpss_ldy_info;

    if (sample_comm_vdec_get_lowdelay_en() == HI_FALSE) {
        return HI_SUCCESS;
    }

    for (i = 0; i < vpss_grp_num; i++) {
        check_chn_return(hi_mpi_vpss_get_chn_low_delay(i, 0, &vpss_ldy_info), i, "hi_mpi_vpss_get_chn_low_delay");
        vpss_ldy_info.enable = HI_TRUE;
        vpss_ldy_info.line_cnt = 16; /* 16:VDEC_LOW_DELAY_MIN_LINE */
        check_chn_return(hi_mpi_vpss_set_chn_low_delay(i, 0, &vpss_ldy_info), i, "hi_mpi_vpss_set_chn_low_delay");
    }

    return HI_SUCCESS;
}

static hi_s32 sample_start_vpss(hi_vpss_grp *vpss_grp, hi_u32 vpss_grp_num, hi_bool *vpss_chn_enable, hi_u32 arr_len)
{
    hi_u32 i;
    hi_s32 ret;
    sample_vpss_chn_attr vpss_chn_attr = {0};
    hi_vpss_grp_attr vpss_grp_attr = {0};

    sample_config_vpss_grp_attr(&vpss_grp_attr);
    (hi_void)memset_s(vpss_chn_enable, arr_len * sizeof(hi_bool), 0, arr_len * sizeof(hi_bool));
    sample_comm_vpss_get_default_chn_attr(&vpss_chn_attr.chn_attr[0]);
    vpss_chn_enable[0]      = HI_TRUE;
    vpss_chn_attr.chn_enable[0] = HI_TRUE;
    vpss_chn_attr.chn_array_size = HI_VPSS_MAX_CHN_NUM;
    vpss_chn_attr.chn_attr[0].width  = g_disp_size.width;
    vpss_chn_attr.chn_attr[0].height = g_disp_size.height;
    vpss_chn_attr.chn_attr[0].chn_mode = HI_VPSS_CHN_MODE_AUTO;

    for (i = 0; i < vpss_grp_num; i++) {
        *vpss_grp = i;
        ret = sample_common_vpss_start(*vpss_grp, &vpss_grp_attr, &vpss_chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("start VPSS fail for %#x!\n", ret);
            sample_stop_vpss(*vpss_grp, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
            return ret;
        }
    }

    ret = sample_config_vpss_ldy_attr(vpss_grp_num);
    if (ret != HI_SUCCESS) {
        sample_stop_vpss(*vpss_grp, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
        return ret;
    }

    ret = sample_vdec_bind_vpss(vpss_grp_num);
    if (ret != HI_SUCCESS) {
        sample_vdec_unbind_vpss(vpss_grp_num);
        sample_stop_vpss(*vpss_grp, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
    }
    return ret;
}

static hi_void sample_vpss_unbind_vo(hi_u32 vpss_grp_num, const sample_vo_cfg *vo_config)
{
    hi_u32 i;
    hi_s32 ret = HI_SUCCESS;
    for (i = 0; i < vpss_grp_num; i++) {
        ret = sample_comm_vpss_un_bind_vo(i, 0, vo_config->vo_layer, i);
        if (ret != HI_SUCCESS) {
            sample_print("vpss unbind vo fail for %#x!\n", ret);
        }
    }
    return;
}

static hi_s32 sample_vpss_bind_vo(const sample_vo_cfg *vo_config, hi_u32 vpss_grp_num)
{
    hi_u32 i;
    hi_s32 ret = HI_SUCCESS;

    for (i = 0; i < vpss_grp_num; i++) {
        ret = sample_comm_vpss_bind_vo(i, 0, vo_config->vo_layer, i);
        if (ret != HI_SUCCESS) {
            sample_print("vpss bind vo fail for %#x!\n", ret);
            return ret;
        }
    }
    return ret;
}

#ifdef SAMPLE_MEM_SHARE_ENABLE
static hi_void sample_init_vo_mem_share(hi_vo_layer vo_layer)
{
    hi_s32 ret;
    hi_video_frame_info frame_info;
    hi_s32 milli_sec = 5000;
    hi_sys_mem_info mem_info;

    ret = hi_mpi_vo_get_screen_frame(vo_layer, &frame_info, milli_sec);
    if (ret != HI_SUCCESS) {
        sample_print("layer %d hi_mpi_vo_get_screen_frame fail for %#x!\n", vo_layer, ret);
        return;
    }

    ret = hi_mpi_sys_get_mem_info_by_phys(frame_info.video_frame.phys_addr[0], &mem_info);
    if (ret != HI_SUCCESS) {
        hi_mpi_vo_release_screen_frame(vo_layer, &frame_info);
        sample_print("layer %d hi_mpi_sys_get_mem_info_by_phys fail for %#x!\n", vo_layer, ret);
        return;
    }

    ret = hi_mpi_sys_mem_share_all(mem_info.mem_handle);
    if (ret != HI_SUCCESS) {
        hi_mpi_vo_release_screen_frame(vo_layer, &frame_info);
        sample_print("layer %d hi_mpi_sys_mem_share_all fail for %#x!\n", vo_layer, ret);
        return;
    }

    hi_mpi_vo_release_screen_frame(vo_layer, &frame_info);
}
#endif

/*maohw static*/ hi_s32 sample_start_vo(sample_vo_cfg *vo_config, hi_u32 vpss_grp_num)
{
    hi_s32 ret;
    vo_config->vo_dev            = SAMPLE_VO_DEV_UHD;
    vo_config->vo_layer          = SAMPLE_VO_LAYER_VHD0;
    #if 0 //maohw
    vo_config->vo_intf_type      = g_vdec_display_cfg.intf_type;
    vo_config->intf_sync         = g_vdec_display_cfg.intf_sync;
    vo_config->pic_size          = g_vdec_display_cfg.pic_size;
    #endif
    vo_config->bg_color          = COLOR_RGB_BLACK;//maohw COLOR_RGB_BLUE;
    vo_config->dis_buf_len       = 3; /* 3:buf length */
    vo_config->dst_dynamic_range = HI_DYNAMIC_RANGE_SDR8;
    vo_config->vo_mode           = VO_MODE_1MUX;
    vo_config->pix_format        = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    vo_config->disp_rect.x       = 0;
    vo_config->disp_rect.y       = 0;
    
    #if 0 //maohw
    vo_config->disp_rect.width   = g_disp_size.width;
    vo_config->disp_rect.height  = g_disp_size.height;
    vo_config->image_size.width  = g_disp_size.width;
    vo_config->image_size.height = g_disp_size.height;
    #else
    hi_size disp_size;
    ret = sample_comm_sys_get_pic_size(vo_config->pic_size, &disp_size);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_sys_get_pic_size failed!\n");
        return ret;
    }
    vo_config->disp_rect.width   = disp_size.width;
    vo_config->disp_rect.height  = disp_size.height;
    vo_config->image_size.width  = disp_size.width;
    vo_config->image_size.height = disp_size.height;
    #endif
    
    vo_config->vo_part_mode      = HI_VO_PARTITION_MODE_SINGLE;
    vo_config->compress_mode     = HI_COMPRESS_MODE_NONE;

    ret = sample_comm_vo_start_vo(vo_config);
    if (ret != HI_SUCCESS) {
        sample_print("start VO fail for %#x!\n", ret);
        sample_comm_vo_stop_vo(vo_config);
        return ret;
    }

#ifdef SAMPLE_MEM_SHARE_ENABLE
    sample_init_vo_mem_share(vo_config->vo_layer);
#endif

    ret = sample_vpss_bind_vo(vo_config, vpss_grp_num);
    if (ret != HI_SUCCESS) {
        sample_vpss_unbind_vo(vpss_grp_num, vo_config);
        sample_comm_vo_stop_vo(vo_config);
    }

    return ret;
}

static hi_void sample_vdec_cmd_ctrl(hi_u32 chn_num, vdec_thread_param *vdec_send, pthread_t *vdec_thread,
    hi_u32 send_arr_len, hi_u32 thread_arr_len)
{
    hi_u32 i;
    hi_vdec_chn_status status;
    hi_s32 c;

    for (i = 0; (i < chn_num) && (i < send_arr_len) && (i < thread_arr_len); i++) {
        if (vdec_send[i].circle_send == HI_TRUE) {
            goto circle_send;
        }
    }

    sample_comm_vdec_cmd_not_circle_send(chn_num, vdec_send, vdec_thread, send_arr_len, thread_arr_len);
    return;

circle_send:
    while (1) {
        printf("\n_sample_test:press 'e' to exit; 'q' to query!;\n");
        c = sample_getchar();
        if (c == 'e') {
            break;
        } else if (c == 'q') {
            for (i = 0; (i < chn_num) && (i < send_arr_len) && (i < thread_arr_len); i++) {
                hi_mpi_vdec_query_status(vdec_send[i].chn_id, &status);
                sample_comm_vdec_print_chn_status(vdec_send[i].chn_id, status);
            }
        }
    }
    return;
}
static hi_void sample_send_stream_to_vdec(sample_vdec_attr *sample_vdec, hi_u32 arr_len, hi_u32 vdec_chn_num,
    const char *stream_name)
{
    hi_u32 i;
    vdec_thread_param vdec_send[HI_VDEC_MAX_CHN_NUM];
    pthread_t   vdec_thread[HI_VDEC_MAX_CHN_NUM] = {0}; /* 2:thread */
    if (arr_len > HI_VDEC_MAX_CHN_NUM) {
        sample_print("array size(%u) of vdec_send need < %u!\n", arr_len, HI_VDEC_MAX_CHN_NUM);
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
        vdec_send[i].e_thread_ctrl = THREAD_CTRL_START;
        vdec_send[i].circle_send   = HI_TRUE;
        vdec_send[i].milli_sec     = 0;
        vdec_send[i].min_buf_size  = (sample_vdec[i].width * sample_vdec[i].height * 3) >> 1; /* 3:yuv */
        vdec_send[i].fps           = 30; /* 30:frame rate */
    }
    sample_comm_vdec_start_send_stream(vdec_chn_num, &vdec_send[0], &vdec_thread[0],
        HI_VDEC_MAX_CHN_NUM, HI_VDEC_MAX_CHN_NUM);

    sample_vdec_cmd_ctrl(vdec_chn_num, &vdec_send[0], &vdec_thread[0],
        HI_VDEC_MAX_CHN_NUM, HI_VDEC_MAX_CHN_NUM);

    sample_comm_vdec_stop_send_stream(vdec_chn_num, &vdec_send[0], &vdec_thread[0],
        HI_VDEC_MAX_CHN_NUM, HI_VDEC_MAX_CHN_NUM);
}

static hi_s32 sample_start_vdec(sample_vdec_attr *sample_vdec, hi_u32 vdec_chn_num, hi_u32 len)
{
    hi_s32 ret;

    ret = sample_comm_vdec_start(vdec_chn_num, &sample_vdec[0], len);
    if (ret != HI_SUCCESS) {
        sample_print("start VDEC fail for %#x!\n", ret);
        sample_comm_vdec_stop(vdec_chn_num);
    }

    return ret;
}

static hi_s32 sample_h265_vdec_vpss_vo(hi_void)
{
    hi_s32 ret;
    hi_u32 vdec_chn_num, vpss_grp_num;
    sample_vdec_attr sample_vdec[HI_VDEC_MAX_CHN_NUM];
    hi_bool vpss_chn_enable[HI_VPSS_MAX_CHN_NUM];
    sample_vo_cfg vo_config;
    hi_vpss_grp vpss_grp;

    vdec_chn_num = 1;
    vpss_grp_num = vdec_chn_num;
    g_cur_type = HI_PT_H265;
    /************************************************
    step1:  init SYS, init common VB(for VPSS and VO), init module VB(for VDEC)
    *************************************************/
    ret = sample_init_sys_and_vb(&sample_vdec[0], vdec_chn_num, g_cur_type, HI_VDEC_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    /************************************************
    step2:  init VDEC
    *************************************************/
    ret = sample_start_vdec(&sample_vdec[0], vdec_chn_num, HI_VDEC_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        goto stop_sys;
    }
    /************************************************
    step3:  start VPSS
    *************************************************/
    ret = sample_start_vpss(&vpss_grp, vpss_grp_num, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        goto stop_vdec;
    }
    /************************************************
    step4:  start VO
    *************************************************/
    ret = sample_start_vo(&vo_config, vpss_grp_num);
    if (ret != HI_SUCCESS) {
        goto stop_vpss;
    }

    /************************************************
    step5:  send stream to VDEC
    *************************************************/
    sample_send_stream_to_vdec(&sample_vdec[0], HI_VDEC_MAX_CHN_NUM, vdec_chn_num, "3840x2160_8bit.h265");

    sample_vpss_unbind_vo(vpss_grp_num, &vo_config);
    sample_comm_vo_stop_vo(&vo_config);
stop_vpss:
    sample_vdec_unbind_vpss(vpss_grp_num);
    sample_stop_vpss(vpss_grp, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
stop_vdec:
    sample_comm_vdec_stop(vdec_chn_num);
    sample_comm_vdec_exit_vb_pool();
stop_sys:
    sample_comm_sys_exit();

    return ret;
}

static hi_s32 sample_h264_vdec_vpss_vo(hi_void)
{
    hi_s32 ret;
    hi_u32 vdec_chn_num, vpss_grp_num;
    hi_vpss_grp vpss_grp;
    sample_vdec_attr sample_vdec[HI_VDEC_MAX_CHN_NUM];
    sample_vo_cfg vo_config;
    hi_bool vpss_chn_enable[HI_VPSS_MAX_CHN_NUM];

    vdec_chn_num = 1;
    vpss_grp_num = vdec_chn_num;
    g_cur_type = HI_PT_H264;
    /************************************************
    step1:  init SYS, init common VB(for VPSS and VO), init module VB(for VDEC)
    *************************************************/
    ret = sample_init_sys_and_vb(&sample_vdec[0], vdec_chn_num, g_cur_type, HI_VDEC_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    /************************************************
    step2:  init VDEC
    *************************************************/
    ret = sample_start_vdec(&sample_vdec[0], vdec_chn_num, HI_VDEC_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        goto stop_sys;
    }
    /************************************************
    step3:  start VPSS
    *************************************************/
    ret = sample_start_vpss(&vpss_grp, vpss_grp_num, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        goto stop_vdec;
    }
    /************************************************
    step4:  start VO
    *************************************************/
    ret = sample_start_vo(&vo_config, vpss_grp_num);
    if (ret != HI_SUCCESS) {
        goto stop_vpss;
    }

    /************************************************
    step5:  send stream to VDEC
    *************************************************/
    sample_send_stream_to_vdec(&sample_vdec[0], HI_VDEC_MAX_CHN_NUM, vdec_chn_num, "3840x2160_8bit.h264");

    sample_vpss_unbind_vo(vpss_grp_num, &vo_config);
    sample_comm_vo_stop_vo(&vo_config);
stop_vpss:
    sample_vdec_unbind_vpss(vpss_grp_num);
    sample_stop_vpss(vpss_grp, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
stop_vdec:
    sample_comm_vdec_stop(vdec_chn_num);
    sample_comm_vdec_exit_vb_pool();
stop_sys:
    sample_comm_sys_exit();

    return ret;
}

static hi_s32 sample_jpeg_vdec_vpss_vo(hi_void)
{
    hi_s32 ret;
    hi_u32 vdec_chn_num, vpss_grp_num;
    hi_vpss_grp vpss_grp;
    sample_vdec_attr sample_vdec[HI_VDEC_MAX_CHN_NUM];
    sample_vo_cfg vo_config;
    hi_bool vpss_chn_enable[HI_VPSS_MAX_CHN_NUM];

    vdec_chn_num = 1;
    vpss_grp_num = vdec_chn_num;
    g_cur_type = HI_PT_JPEG;
    /************************************************
    step1:  init SYS, init common VB(for VPSS and VO), init module VB(for VDEC)
    *************************************************/
    ret = sample_init_sys_and_vb(&sample_vdec[0], vdec_chn_num, g_cur_type, HI_VDEC_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    /************************************************
    step2:  init VDEC
    *************************************************/
    ret = sample_start_vdec(&sample_vdec[0], vdec_chn_num, HI_VDEC_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        goto stop_sys;
    }
    /************************************************
    step3:  start VPSS
    *************************************************/
    ret = sample_start_vpss(&vpss_grp, vpss_grp_num, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        goto stop_vdec;
    }
    /************************************************
    step4:  start VO
    *************************************************/
    ret = sample_start_vo(&vo_config, vpss_grp_num);
    if (ret != HI_SUCCESS) {
        goto stop_vpss;
    }

    /************************************************
    step5:  send stream to VDEC
    *************************************************/
    sample_send_stream_to_vdec(&sample_vdec[0], HI_VDEC_MAX_CHN_NUM, vdec_chn_num, "3840x2160.jpg");

    sample_vpss_unbind_vo(vpss_grp_num, &vo_config);
    sample_comm_vo_stop_vo(&vo_config);
stop_vpss:
    sample_vdec_unbind_vpss(vpss_grp_num);
    sample_stop_vpss(vpss_grp, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
stop_vdec:
    sample_comm_vdec_stop(vdec_chn_num);
    sample_comm_vdec_exit_vb_pool();
stop_sys:
    sample_comm_sys_exit();

    return ret;
}

static hi_s32 sample_h265_lowdelay_vdec_vpss_vo(hi_void)
{
    hi_s32 ret;
    hi_u32 vdec_chn_num, vpss_grp_num;
    sample_vdec_attr sample_vdec[HI_VDEC_MAX_CHN_NUM];
    hi_bool vpss_chn_enable[HI_VPSS_MAX_CHN_NUM];
    sample_vo_cfg vo_config;
    hi_vpss_grp vpss_grp;

    vdec_chn_num = 1;
    vpss_grp_num = vdec_chn_num;
    g_cur_type = HI_PT_H265;
    sample_comm_vdec_set_lowdelay_en(HI_TRUE);
    /************************************************
    step1:  init SYS, init common VB(for VPSS and VO), init module VB(for VDEC)
    *************************************************/
    ret = sample_init_sys_and_vb(&sample_vdec[0], vdec_chn_num, g_cur_type, HI_VDEC_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    /************************************************
    step2:  init VDEC
    *************************************************/
    ret = sample_start_vdec(&sample_vdec[0], vdec_chn_num, HI_VDEC_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        goto stop_sys;
    }
    /************************************************
    step3:  start VPSS
    *************************************************/
    ret = sample_start_vpss(&vpss_grp, vpss_grp_num, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        goto stop_vdec;
    }
    /************************************************
    step4:  start VO
    *************************************************/
    ret = sample_start_vo(&vo_config, vpss_grp_num);
    if (ret != HI_SUCCESS) {
        goto stop_vpss;
    }

    /************************************************
    step5:  send stream to VDEC
    *************************************************/
    sample_send_stream_to_vdec(&sample_vdec[0], HI_VDEC_MAX_CHN_NUM, vdec_chn_num, "3840x2160_8bit.h265");

    sample_vpss_unbind_vo(vpss_grp_num, &vo_config);
    sample_comm_vo_stop_vo(&vo_config);
stop_vpss:
    sample_vdec_unbind_vpss(vpss_grp_num);
    sample_stop_vpss(vpss_grp, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
stop_vdec:
    sample_comm_vdec_stop(vdec_chn_num);
    sample_comm_vdec_exit_vb_pool();
stop_sys:
    sample_comm_sys_exit();

    return ret;
}

static hi_void sample_send_heic_to_vdec(const char *stream_name)
{
    heif_config config = { 0 };
    config.file_desc.file_type = HEIF_FILE_TYPE_URL;
    if (snprintf_s(config.file_desc.input.url, HEIF_MAX_URL_PATH_LEN, HEIF_MAX_URL_PATH_LEN - 1, "%s/%s",
        SAMPLE_STREAM_PATH, stream_name) < 0) {
        return;
    }
    config.config_type = HEIF_CONFIG_DEMUXER;
    heif_handle handle = NULL;

    if (heif_create(&handle, &config) != HI_SUCCESS) {
        return;
    }

    heif_file_info file_info = { 0 };
    hi_s32 ret = heif_get_file_info(handle, &file_info);
    if (ret != HI_SUCCESS) {
        sample_print("heif_get_file_info failed ret:%d\n", ret);
        goto FAIL;
    }

    heif_image_item item = { 0 };
    hi_u32 item_count = 1;
    ret = heif_get_master_image(handle, file_info.track[0].track_id, &item, &item_count);
    if (ret != HI_SUCCESS) {
        sample_print("heif_get_master_image fail\n");
        goto FAIL;
    }

    hi_vdec_stream stream;
    stream.pts = item.timestamp;
    stream.addr = item.data;
    stream.len = item.length;
    stream.end_of_frame = HI_TRUE;
    stream.end_of_stream = HI_TRUE;
    stream.need_display = 1;
    hi_mpi_vdec_send_stream(0, &stream, -1);

    hi_vdec_chn_status status;
    hi_s32 c;
    while (1) {
        sample_print("press 'e' to exit; 'q' to query!;\n");
        c = sample_getchar();
        if (c == 'e') {
            break;
        } else if (c == 'q') {
                hi_mpi_vdec_query_status(0, &status);
                sample_comm_vdec_print_chn_status(0, status);
        }
    }

FAIL:

    hi_mpi_vdec_stop_recv_stream(0);
    heif_destroy(handle);
}

static hi_s32 sample_heic_h265_vdec_vpss_vo(hi_void)
{
    hi_s32 ret;
    hi_u32 vdec_chn_num, vpss_grp_num;
    sample_vdec_attr sample_vdec[HI_VDEC_MAX_CHN_NUM];
    hi_bool vpss_chn_enable[HI_VPSS_MAX_CHN_NUM];
    sample_vo_cfg vo_config;
    hi_vpss_grp vpss_grp;

    vdec_chn_num = 1;
    vpss_grp_num = vdec_chn_num;
    g_cur_type = HI_PT_H265;
    /************************************************
    step1:  init SYS, init common VB(for VPSS and VO), init module VB(for VDEC)
    *************************************************/
    ret = sample_init_sys_and_vb(&sample_vdec[0], vdec_chn_num, g_cur_type, HI_VDEC_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    /************************************************
    step2:  init VDEC
    *************************************************/
    ret = sample_start_vdec(&sample_vdec[0], vdec_chn_num, HI_VDEC_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        goto stop_sys;
    }
    /************************************************
    step3:  start VPSS
    *************************************************/
    ret = sample_start_vpss(&vpss_grp, vpss_grp_num, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        goto stop_vdec;
    }
    /************************************************
    step4:  start VO
    *************************************************/
    ret = sample_start_vo(&vo_config, vpss_grp_num);
    if (ret != HI_SUCCESS) {
        goto stop_vpss;
    }

    /************************************************
    step5:  send stream to VDEC
    *************************************************/
    sample_send_heic_to_vdec("3840x2160_8bit.heic");

    sample_vpss_unbind_vo(vpss_grp_num, &vo_config);
    sample_comm_vo_stop_vo(&vo_config);
stop_vpss:
    sample_vdec_unbind_vpss(vpss_grp_num);
    sample_stop_vpss(vpss_grp, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
stop_vdec:
    sample_comm_vdec_stop(vdec_chn_num);
    sample_comm_vdec_exit_vb_pool();
stop_sys:
    sample_comm_sys_exit();

    return ret;
}

static hi_s32 sample_check_parameter(int argc, const char *argv0, const char *argv1)
{
    if ((argc != 2) || (strlen(argv1) != 1)) { /* 2:arg num */
        printf("\n invalid input!  for examples:\n");
        sample_vdec_usage(argv0);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_choose_case(const char argv1, const char *argv0)
{
    hi_s32 ret;
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
            ret = sample_jpeg_vdec_vpss_vo();
            break;
        }
        case '3': {
            ret = sample_h265_lowdelay_vdec_vpss_vo();
            break;
        }
        case '4': {
            ret = sample_heic_h265_vdec_vpss_vo();
            break;
        }
        default: {
            sample_print("the index is invalid!\n");
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
#ifdef __LITEOS__
    int app_main(int argc, char *argv[])
#else
    int sample_vdec_main(int argc, char *argv[])
#endif
{
    hi_s32 ret;

    ret = sample_check_parameter(argc, argv[0], argv[1]);
    if (ret != HI_SUCCESS) {
        return ret;
    }

#ifndef __LITEOS__
    sample_sys_signal(sample_vdec_handle_sig);
#endif

    /******************************************
     choose the case
    ******************************************/
    ret = sample_choose_case(*argv[1], argv[0]);
    if (ret == HI_SUCCESS && g_sample_exit == 0) {
        sample_print("program exit normally!\n");
    } else {
        sample_print("program exit abnormally!\n");
    }

    return ret;
}

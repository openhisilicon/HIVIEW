/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: vdec common function for sample.
 * Author: Hisilicon multimedia software group
 * Create: 2019/11/29
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
#include <sys/prctl.h>
#include <limits.h>
#include "sample_comm.h"


hi_vb_src g_vdec_vb_src = HI_VB_SRC_MOD;
hi_bool g_progressive_en = HI_FALSE;

hi_vb_pool g_pic_vb_pool[HI_VB_MAX_POOLS] = { [0 ... (HI_VB_MAX_POOLS - 1)] = HI_VB_INVALID_POOL_ID };
hi_vb_pool g_tmv_vb_pool[HI_VB_MAX_POOLS] = { [0 ... (HI_VB_MAX_POOLS - 1)] = HI_VB_INVALID_POOL_ID };

hi_void printf_vdec_chn_status(hi_s32 chn, hi_vdec_chn_status status)
{
    printf("\033[0;33m ---------------------------------------------------------------\033[0;39m\n");
    printf("\033[0;33m chn:%d, type:%d, start:%d, decode_frames:%d, left_pics:%d, left_bytes:%d, "
        "left_frames:%d, recv_frames:%d  \033[0;39m\n",
        chn, (status).type, (status).is_started,  (status).dec_stream_frames, (status).left_decoded_frames,
        (status).left_stream_bytes, (status).left_stream_frames, (status).recv_stream_frames);
    printf("\033[0;33m format_err:%d,    pic_size_err_set:%d,  stream_unsprt:%d,  pack_err:%d, "
        "set_pic_size_err:%d,  ref_err_set:%d,  pic_buf_size_err_set:%d  \033[0;39m\n",
        (status).dec_err.format_err, (status).dec_err.set_pic_size_err, (status).dec_err.stream_unsupport,
        (status).dec_err.pack_err, (status).dec_err.set_protocol_num_err, (status).dec_err.set_ref_num_err,
        (status).dec_err.set_pic_buf_size_err);
    printf("\033[0;33m -----------------------------------------------------------------\033[0;39m\n");
    return;
}

hi_bool sample_comm_vdec_get_progressive_en(hi_void)
{
    return g_progressive_en;
}
hi_void sample_comm_vdec_set_progressive_en(hi_bool enable)
{
    g_progressive_en = enable;
}

hi_s32 sample_comm_vdec_init_user_vb_pool(hi_u32 chn_num, sample_vdec_buf *vdec_buf,
    sample_vdec_attr *sample_vdec, hi_u32 array_len)
{
    hi_s32 i;
    hi_vb_pool_cfg vb_pool_cfg;
    for (i = 0; (i < chn_num) && (i < array_len) && (i < HI_VB_MAX_POOLS); i++) {
        if ((vdec_buf[i].pic_buf_size != 0) && (sample_vdec[i].frame_buf_cnt != 0)) {
            (hi_void)memset_s(&vb_pool_cfg, sizeof(hi_vb_pool_cfg), 0, sizeof(hi_vb_pool_cfg));
            vb_pool_cfg.blk_size = vdec_buf[i].pic_buf_size;
            vb_pool_cfg.blk_cnt = sample_vdec[i].frame_buf_cnt;
            vb_pool_cfg.remap_mode = HI_VB_REMAP_MODE_NONE;
            g_pic_vb_pool[i] = hi_mpi_vb_create_pool(&vb_pool_cfg);
            if (g_pic_vb_pool[i] == HI_VB_INVALID_POOL_ID) {
                return HI_FAILURE;
            }
        }
        if (vdec_buf[i].tmv_buf_size != 0) {
            (hi_void)memset_s(&vb_pool_cfg, sizeof(hi_vb_pool_cfg), 0, sizeof(hi_vb_pool_cfg));
            vb_pool_cfg.blk_size = vdec_buf[i].tmv_buf_size;
            vb_pool_cfg.blk_cnt = sample_vdec[i].sapmle_vdec_video.ref_frame_num + 1;
            vb_pool_cfg.remap_mode = HI_VB_REMAP_MODE_NONE;
            g_tmv_vb_pool[i] = hi_mpi_vb_create_pool(&vb_pool_cfg);
            if (g_tmv_vb_pool[i] == HI_VB_INVALID_POOL_ID) {
                return HI_FAILURE;
            }
        }
    }
    return HI_SUCCESS;
}

hi_void sample_comm_handle_init_vb_fail(hi_s32 idx)
{
    hi_s32 ret;
    hi_s32 i = idx;
    for (; (i >= 0) && (i < HI_VB_MAX_POOLS); i--) {
        if (g_pic_vb_pool[i] != HI_VB_INVALID_POOL_ID) {
            ret = hi_mpi_vb_destroy_pool(g_pic_vb_pool[i]);
            if (ret != HI_SUCCESS) {
                printf("hi_mpi_vb_destroy_pool %d fail!\n", g_pic_vb_pool[i]);
            }
            g_pic_vb_pool[i] = HI_VB_INVALID_POOL_ID;
        }
        if (g_tmv_vb_pool[i] != HI_VB_INVALID_POOL_ID) {
            ret = hi_mpi_vb_destroy_pool(g_tmv_vb_pool[i]);
            if (ret != HI_SUCCESS) {
                printf("hi_mpi_vb_destroy_pool %d fail!\n", g_tmv_vb_pool[i]);
            }
            g_tmv_vb_pool[i] = HI_VB_INVALID_POOL_ID;
        }
    }
    return;
}

hi_void sample_comm_vdec_cal_vb_size(hi_u32 chn_num, sample_vdec_attr *sample_vdec, hi_u32 sample_vdec_arr_len,
    sample_vdec_buf *vdec_buf)
{
    hi_s32 i;
    hi_pic_buf_attr buf_attr = { 0 };
    for (i = 0; (i < chn_num) && (i < sample_vdec_arr_len); i++) {
        buf_attr.align = 0;
        buf_attr.height = sample_vdec[i].height;
        buf_attr.width = sample_vdec[i].width;
        if (sample_vdec[i].type == HI_PT_H265) {
            buf_attr.bit_width = sample_vdec[i].sapmle_vdec_video.bit_width;
            buf_attr.pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
            vdec_buf[i].pic_buf_size = hi_vdec_get_pic_buf_size(sample_vdec[i].type, &buf_attr);
            vdec_buf[i].tmv_buf_size =
                hi_vdec_get_tmv_buf_size(sample_vdec[i].type, sample_vdec[i].width, sample_vdec[i].height);
        } else if (sample_vdec[i].type == HI_PT_H264) {
            buf_attr.bit_width = sample_vdec[i].sapmle_vdec_video.bit_width;
            buf_attr.pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
            vdec_buf[i].pic_buf_size = hi_vdec_get_pic_buf_size(sample_vdec[i].type, &buf_attr);
            if (sample_vdec[i].sapmle_vdec_video.dec_mode == HI_VIDEO_DEC_MODE_IPB) {
                vdec_buf[i].tmv_buf_size =
                    hi_vdec_get_tmv_buf_size(sample_vdec[i].type, sample_vdec[i].width, sample_vdec[i].height);
            }
        } else {
            buf_attr.bit_width = HI_DATA_BIT_WIDTH_8;
            buf_attr.pixel_format = sample_vdec[i].sapmle_vdec_picture.pixel_format;
            vdec_buf[i].pic_buf_size = hi_vdec_get_pic_buf_size(sample_vdec[i].type, &buf_attr);
        }
    }
    return;
}

hi_s32 sample_comm_vdec_config_vb_pool(hi_u32 chn_num, sample_vdec_attr *sample_vdec, hi_u32 arr_len,
    sample_vdec_buf *vdec_buf, hi_vb_cfg *vb_conf)
{
    hi_s32 i, j;
    hi_bool find_flag;
    hi_s32 pos = 0;
    /* pic_buffer */
    for (j = 0; j < HI_VB_MAX_COMMON_POOLS; j++) {
        find_flag = HI_FALSE;
        for (i = 0; (i < chn_num) && (i < arr_len); i++) {
            if ((find_flag == HI_FALSE) && (vdec_buf[i].pic_buf_size != 0) && (vdec_buf[i].pic_buf_alloc == HI_FALSE)) {
                vb_conf->common_pool[j].blk_size = vdec_buf[i].pic_buf_size;
                vb_conf->common_pool[j].blk_cnt = sample_vdec[i].frame_buf_cnt;
                vdec_buf[i].pic_buf_alloc = HI_TRUE;
                find_flag = HI_TRUE;
                pos = j;
            }

            if ((find_flag == HI_TRUE) && (vdec_buf[i].pic_buf_alloc == HI_FALSE) &&
                (vb_conf->common_pool[j].blk_size == vdec_buf[i].pic_buf_size)) {
                vb_conf->common_pool[j].blk_cnt += sample_vdec[i].frame_buf_cnt;
                vdec_buf[i].pic_buf_alloc = HI_TRUE;
            }
        }
    }

    /* tmv_buffer */
    for (j = pos + 1; j < HI_VB_MAX_COMMON_POOLS; j++) {
        find_flag = HI_FALSE;
        for (i = 0; (i < chn_num) && (i < arr_len); i++) {
            if ((find_flag == HI_FALSE) && (vdec_buf[i].tmv_buf_size != 0) && (vdec_buf[i].tmv_buf_alloc == HI_FALSE)) {
                vb_conf->common_pool[j].blk_size = vdec_buf[i].tmv_buf_size;
                vb_conf->common_pool[j].blk_cnt = sample_vdec[i].sapmle_vdec_video.ref_frame_num + 1;
                vdec_buf[i].tmv_buf_alloc = HI_TRUE;
                find_flag = HI_TRUE;
                pos = j;
            }

            if ((find_flag == HI_TRUE) && (vdec_buf[i].tmv_buf_alloc == HI_FALSE) &&
                (vb_conf->common_pool[j].blk_size == vdec_buf[i].tmv_buf_size)) {
                vb_conf->common_pool[j].blk_cnt += sample_vdec[i].sapmle_vdec_video.ref_frame_num + 1;
                vdec_buf[i].tmv_buf_alloc = HI_TRUE;
            }
        }
    }
    vb_conf->max_pool_cnt = pos + 1;
    return i - 1;
}

hi_s32 sample_comm_vdec_init_vb_pool(hi_u32 chn_num, sample_vdec_attr *sample_vdec, hi_u32 arr_len)
{
    hi_vb_cfg vb_conf;
    hi_s32 i, ret;
    sample_vdec_buf vdec_buf[HI_VDEC_MAX_CHN_NUM];

    check_null_ptr_return(sample_vdec);
    if (arr_len > HI_VDEC_MAX_CHN_NUM) {
        printf("sample_vdec_attr array len Invalid \n");
        return HI_FAILURE;
    }
    (hi_void)memset_s(vdec_buf, sizeof(vdec_buf), 0, sizeof(sample_vdec_buf) * HI_VDEC_MAX_CHN_NUM);
    (hi_void)memset_s(&vb_conf, sizeof(hi_vb_cfg), 0, sizeof(hi_vb_cfg));

    sample_comm_vdec_cal_vb_size(chn_num, sample_vdec, arr_len, vdec_buf);

    i = sample_comm_vdec_config_vb_pool(chn_num, sample_vdec, arr_len, vdec_buf, &vb_conf);

    if (g_vdec_vb_src == HI_VB_SRC_MOD) {
        hi_mpi_vb_exit_mod_common_pool(HI_VB_UID_VDEC);
        CHECK_RET(hi_mpi_vb_set_mod_pool_cfg(HI_VB_UID_VDEC, &vb_conf), "hi_mpi_vb_set_mod_pool_configig");
        ret = hi_mpi_vb_init_mod_common_pool(HI_VB_UID_VDEC);
        if (ret != HI_SUCCESS) {
            printf("hi_mpi_vb_exit_mod_common_pool fail for 0x%x\n", ret);
            hi_mpi_vb_exit_mod_common_pool(HI_VB_UID_VDEC);
            return HI_FAILURE;
        }
    } else if (g_vdec_vb_src == HI_VB_SRC_USER) {
        if (sample_comm_vdec_init_user_vb_pool(chn_num, &vdec_buf[0], &sample_vdec[0], HI_VDEC_MAX_CHN_NUM) !=
            HI_SUCCESS) {
            goto fail;
        }
    }

    return HI_SUCCESS;

fail:
    sample_comm_handle_init_vb_fail(i);
    return HI_FAILURE;
}

hi_void sample_comm_vdec_exit_user_vb_pool(hi_void)
{
    hi_s32 i, ret;
    for (i = HI_VB_MAX_POOLS - 1; i >= 0; i--) {
        if (g_pic_vb_pool[i] != HI_VB_INVALID_POOL_ID) {
            ret = hi_mpi_vb_destroy_pool(g_pic_vb_pool[i]);
            if (ret != HI_SUCCESS) {
                printf("hi_mpi_vb_destroy_pool %d fail!\n", g_pic_vb_pool[i]);
            }
            g_pic_vb_pool[i] = HI_VB_INVALID_POOL_ID;
        }
        if (g_tmv_vb_pool[i] != HI_VB_INVALID_POOL_ID) {
            ret = hi_mpi_vb_destroy_pool(g_tmv_vb_pool[i]);
            if (ret != HI_SUCCESS) {
                printf("hi_mpi_vb_destroy_pool %d fail!\n", g_tmv_vb_pool[i]);
            }
            g_tmv_vb_pool[i] = HI_VB_INVALID_POOL_ID;
        }
    }
    return;
}

hi_void sample_comm_vdec_exit_vb_pool(hi_void)
{
    if (g_vdec_vb_src == HI_VB_SRC_MOD) {
        hi_mpi_vb_exit_mod_common_pool(HI_VB_UID_VDEC);
    } else if (g_vdec_vb_src == HI_VB_SRC_USER) {
        sample_comm_vdec_exit_user_vb_pool();
    }

    return;
}

hi_void sample_comm_vdec_send_h264_frame_process(hi_s32 *read_len, hi_u8 *buf, vdec_thread_param *thread_param,
    hi_s32 used_bytes)
{
    hi_s32 i;
    hi_bool find_start = HI_FALSE;
    hi_bool find_end = HI_FALSE;
    /* H264 frame start marker */
    if (*read_len > thread_param->min_buf_size) {
        SAMPLE_PRT("chn %d read_len %d is bigger than buf_size %u!\n", thread_param->chn_id, *read_len,
            thread_param->min_buf_size);
        return;
    }
    for (i = 0; i < *read_len - 8; i++) { /* 8:h264 frame start code length */
        int tmp = buf[i + 3] & 0x1F; /* 3:index  0x1F:frame start marker */
        if (buf[i] == 0 && buf[i + 1] == 0 && buf[i + 2] == 1 && /* 1 2:index */
            (((tmp == 0x5 || tmp == 0x1) && ((buf[i + 4] & 0x80) == 0x80)) || /* 4:index 0x5 0x80:frame start marker */
            (tmp == 20 && (buf[i + 7] & 0x80) == 0x80))) { /* 20 0x1 0x80:frame start marker  7:index */
            find_start = HI_TRUE;
            i += 8; /* 8:h264 frame start code length */
            break;
        }
    }

    for (; i < *read_len - 8; i++) { /* 8:h264 frame start code length */
        int tmp = buf[i + 3] & 0x1F; /* 3:index  0x1F:frame start marker */
        if (buf[i] == 0 && buf[i + 1] == 0 && buf[i + 2] == 1 && /* 1 2:index */
            (tmp == 15 || tmp == 7 || tmp == 8 || tmp == 6 || /* 15 7 8 6:frame start marker */
            ((tmp == 5 || tmp == 1) && ((buf[i + 4] & 0x80) == 0x80)) || /* 4:index 5 0x80:frame start marker */
            (tmp == 20 && (buf[i + 7] & 0x80) == 0x80))) { /* 7:index 20 0x80:frame start marker */
            find_end = HI_TRUE;
            break;
        }
    }

    if (i > 0) {
        *read_len = i;
    }
    if (find_start == HI_FALSE) {
        SAMPLE_PRT("chn %d can not find H264 start code!read_len %d, used_bytes %d.!\n", thread_param->chn_id,
            *read_len, used_bytes);
    }
    if (find_end == HI_FALSE) {
        *read_len = i + 8; /* 8:h264 frame start code length */
    }
    return;
}

hi_void sample_comm_vdec_send_h265_frame_process(hi_s32 *read_len, hi_u8 *buf, vdec_thread_param *thread_param,
    hi_s32 used_bytes)
{
    hi_s32 i;
    hi_bool find_start = HI_FALSE;
    hi_bool find_end = HI_FALSE;
    /* H265 frame start marker */
    hi_bool new_pic = HI_FALSE;
    if (*read_len > thread_param->min_buf_size) {
        SAMPLE_PRT("chn %d read_len %d is bigger than buf_size %u!\n", thread_param->chn_id, *read_len,
            thread_param->min_buf_size);
        return;
    }
    for (i = 0; i < *read_len - 6; i++) { /* 6:h265 frame start code length */
        hi_u32 tmp = (buf[i + 3] & 0x7E) >> 1; /* 0x7E:frame start marker 3:index */
        new_pic = (buf[i + 0] == 0 && buf[i + 1] == 0 && buf[i + 2] == 1 && /* 1 2:index */
            (tmp >= 0 && tmp <= 21) && ((buf[i + 5] & 0x80) == 0x80)); /* 5:index 21 0x80:frame start marker */

        if (new_pic) {
            find_start = HI_TRUE;
            i += 6; /* 6:h265 frame start code length */
            break;
        }
    }

    for (; i < *read_len - 6; i++) { /* 6:h265 frame start code length */
        hi_u32 tmp = (buf[i + 3] & 0x7E) >> 1; /* 0x7E:frame start marker 3:index */
        new_pic = (buf[i + 0] == 0 && buf[i + 1] == 0 && buf[i + 2] == 1 && /* 1 2:index */
            (tmp == 32 || tmp == 33 || tmp == 34 || tmp == 39 || tmp == 40 || /* 32 33 34 39 40:frame start marker */
            ((tmp >= 0 && tmp <= 21) && (buf[i + 5] & 0x80) == 0x80))); /* 5:index 21 0x80:frame start marker */

        if (new_pic) {
            find_end = HI_TRUE;
            break;
        }
    }
    if (i > 0)
        *read_len = i;

    if (find_start == HI_FALSE) {
        SAMPLE_PRT("chn %d can not find H265 start code!read_len %d, used_bytes %d.!\n", thread_param->chn_id,
            *read_len, used_bytes);
    }
    if (find_end == HI_FALSE) {
        *read_len = i + 6; /* 6:h265 frame start code length */
    }
    return;
}

hi_u32 sample_comm_vdec_send_jpeg_frame_process(hi_s32 *read_len, hi_u8 *buf, vdec_thread_param *thread_param,
    hi_s32 used_bytes)
{
    hi_s32 i;
    hi_u32 len = 0;
    hi_u32 start = 0;
    hi_bool find_start = HI_FALSE;
    if (*read_len > thread_param->min_buf_size) {
        SAMPLE_PRT("chn %d read_len %d is bigger than buf_size %u!\n", thread_param->chn_id, *read_len,
            thread_param->min_buf_size);
        return start;
    }
    /* JPEG frame start marker */
    for (i = 0; i < *read_len - 1; i++) {
        if (buf[i] == 0xFF && buf[i + 1] == 0xD8) { /* 0xFF 0xD8:frame start marker */
            start = i;
            find_start = HI_TRUE;
            i = i + 2; /* 2:offset */
            break;
        }
    }

    for (; i < *read_len - 3; i++) { /* 3:jpeg frame start code length */
        if ((buf[i] == 0xFF) && (buf[i + 1] & 0xF0) == 0xE0) { /* 0xFF 0xF0 0xE0:frame start marker */
            len = (buf[i + 2] << 8) + buf[i + 3]; /* 2 3:index  8:left shift length */
            i += 1 + len;
        } else {
            break;
        }
    }

    for (; i < *read_len - 1; i++) {
        if (buf[i] == 0xFF && buf[i + 1] == 0xD9) { /* 0xFF 0xD9:frame start marker */
            break;
        }
    }
    *read_len = i + 2; /* 2:offset */

    if (find_start == HI_FALSE) {
        SAMPLE_PRT("chn %d can not find JPEG start code!read_len %d, used_bytes %d.!\n", thread_param->chn_id,
            *read_len, used_bytes);
    }
    return start;
}

hi_u32 sample_comm_vdec_handle_send_stream_mode(hi_s32 *read_len, hi_u8 *buf, vdec_thread_param *thread_param,
    hi_s32 used_bytes, hi_bool *end_of_stream)
{
    hi_u32 start = 0;
    if (thread_param->stream_mode == HI_VDEC_SEND_MODE_FRAME && thread_param->type == HI_PT_H264) {
        sample_comm_vdec_send_h264_frame_process(read_len, buf, thread_param, used_bytes);
    } else if (thread_param->stream_mode == HI_VDEC_SEND_MODE_FRAME && thread_param->type == HI_PT_H265) {
        sample_comm_vdec_send_h265_frame_process(read_len, buf, thread_param, used_bytes);
    } else if (thread_param->type == HI_PT_MJPEG || thread_param->type == HI_PT_JPEG) {
        start = sample_comm_vdec_send_jpeg_frame_process(read_len, buf, thread_param, used_bytes);
    } else {
        if ((*read_len != 0) && (*read_len < thread_param->min_buf_size)) {
            *end_of_stream = HI_TRUE;
        }
    }
    return start;
}

hi_void sample_comm_vdec_handle_send_stream(vdec_thread_param *thread_param, hi_vdec_stream *stream,
    hi_bool *end_of_stream, hi_u64 *pts)
{
    hi_s32 ret;
send_again:
    ret = hi_mpi_vdec_send_stream(thread_param->chn_id, stream, thread_param->milli_sec);
    if ((ret != HI_SUCCESS) && (thread_param->e_thread_ctrl == THREAD_CTRL_START)) {
        usleep(thread_param->interval_time);
        goto send_again;
    } else {
        *end_of_stream = HI_FALSE;
        *pts += thread_param->pts_increase;
    }
    usleep(thread_param->interval_time);
    return;
}

hi_void sample_comm_vdec_send_stream_process(vdec_thread_param *thread_param, FILE *fp_strm, hi_u8 *buf)
{
    hi_bool end_of_stream = HI_FALSE;
    hi_s32 used_bytes = 0;
    hi_s32 read_len = 0;
    hi_u64 pts = thread_param->pts_init;
    hi_u32 start;
    hi_vdec_stream stream;

    while (1) {
        if (thread_param->e_thread_ctrl == THREAD_CTRL_STOP) {
            break;
        } else if (thread_param->e_thread_ctrl == THREAD_CTRL_PAUSE) {
            sleep(1);
            continue;
        }

        end_of_stream = HI_FALSE;
        fseek(fp_strm, used_bytes, SEEK_SET);
        read_len = fread(buf, 1, thread_param->min_buf_size, fp_strm);
        if (read_len == 0) {
            if (thread_param->circle_send == HI_TRUE) {
                (hi_void)memset_s(&stream, sizeof(hi_vdec_stream), 0, sizeof(hi_vdec_stream));
                stream.end_of_stream = HI_TRUE;
                hi_mpi_vdec_send_stream(thread_param->chn_id, &stream, -1);

                used_bytes = 0;
                fseek(fp_strm, 0, SEEK_SET);
                read_len = fread(buf, 1, thread_param->min_buf_size, fp_strm);
            } else {
                break;
            }
        }

        start = sample_comm_vdec_handle_send_stream_mode(&read_len, buf, thread_param, used_bytes, &end_of_stream);

        stream.pts = pts;
        stream.addr = buf + start;
        stream.len = read_len;
        stream.end_of_frame = (thread_param->stream_mode == HI_VDEC_SEND_MODE_FRAME) ? HI_TRUE : HI_FALSE;
        stream.end_of_stream = end_of_stream;
        stream.need_display = 1;

        sample_comm_vdec_handle_send_stream(thread_param, &stream, &end_of_stream, &pts);
        used_bytes += read_len + start;
    }
    return;
}

hi_s32 sample_comm_vdec_check_send_stream_param(vdec_thread_param *thread_param, hi_char *c_stream_file, hi_u32 arr_len)
{
    if (thread_param->c_file_path == HI_NULL || thread_param->c_file_name == HI_NULL) {
        SAMPLE_PRT("chn %d stream file path or stream file name is NULL\n", thread_param->chn_id);
        return HI_FAILURE;
    }

    if (arr_len <= 1) {
        SAMPLE_PRT("chn %d arr length might be overflow\n", thread_param->chn_id);
        return HI_FAILURE;
    }

    if (snprintf_s(c_stream_file, arr_len, arr_len - 1, "%s/%s", thread_param->c_file_path,
        thread_param->c_file_name) < 0) {
        SAMPLE_PRT("chn %d config stream file failed!\n", thread_param->chn_id);
        return HI_FAILURE;
    }

    if (thread_param->min_buf_size <= 0) {
        SAMPLE_PRT("chn %d min_buf_size should greater than zero!\n", thread_param->chn_id);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_void *sample_comm_vdec_send_stream(hi_void *args)
{
    vdec_thread_param *thread_param = (vdec_thread_param *)args;
    FILE *fp_strm = HI_NULL;
    hi_u8 *buf = HI_NULL;
    hi_vdec_stream stream;
    hi_char c_stream_file[FILE_NAME_LEN];
    hi_char *path = HI_NULL;

    prctl(PR_SET_NAME, "video_send_stream", 0, 0, 0);

    if (sample_comm_vdec_check_send_stream_param(thread_param, c_stream_file, FILE_NAME_LEN) != HI_SUCCESS) {
        return (hi_void *)(HI_FAILURE);
    }

    path = realpath(c_stream_file, HI_NULL);
    if (path == HI_NULL) {
        return (hi_void *)(HI_FAILURE);
    }

    fp_strm = fopen(path, "rb");
    if (fp_strm == HI_NULL) {
        SAMPLE_PRT("chn %d can't open file %s in send stream thread!\n", thread_param->chn_id, c_stream_file);
        goto end1;
    }
    printf("\n \033[0;36m chn %d, stream file:%s, userbufsize: %d \033[0;39m\n", thread_param->chn_id,
        thread_param->c_file_name, thread_param->min_buf_size);

    buf = malloc(thread_param->min_buf_size);
    if (buf == HI_NULL) {
        SAMPLE_PRT("chn %d can't alloc %d in send stream thread!\n", thread_param->chn_id, thread_param->min_buf_size);
        goto end;
    }
    fflush(stdout);

    sample_comm_vdec_send_stream_process(thread_param, fp_strm, buf);

    /* send the flag of stream end */
    (hi_void)memset_s(&stream, sizeof(hi_vdec_stream), 0, sizeof(hi_vdec_stream));
    stream.end_of_stream = HI_TRUE;
    hi_mpi_vdec_send_stream(thread_param->chn_id, &stream, -1);

    printf("\033[0;35m chn %d send steam thread return ...  \033[0;39m\n", thread_param->chn_id);
    fflush(stdout);
    if (buf != HI_NULL) {
        free(buf);
        buf = HI_NULL;
    }
end:
    fclose(fp_strm);
    fp_strm = HI_NULL;
end1:
    free(path);
    path = HI_NULL;
    return (hi_void *)HI_SUCCESS;
}

hi_void sample_comm_vdec_cmd_vo_ctrl_resume(hi_s32 chn_num)
{
    hi_vo_layer vo_layer;
    hi_vo_chn vo_chn;
    hi_s32 ret;
    for (vo_layer = 0; vo_layer < HI_VO_MAX_PHYS_VIDEO_LAYER_NUM; vo_layer++) {
        for (vo_chn = 0; vo_chn < chn_num; vo_chn++) {
            ret = hi_mpi_vo_resume_chn(vo_layer, vo_chn);
            if (ret != HI_SUCCESS) {
                printf("hi_mpi_vo_resume_chn(%d, %d) fail for 0x%x!\n", vo_layer, vo_chn, ret);
            }
        }
    }
    printf("VO resume!!!");
    return;
}

hi_void sample_comm_vdec_cmd_vo_ctrl_pause(hi_s32 chn_num)
{
    hi_vo_layer vo_layer;
    hi_vo_chn vo_chn;
    hi_s32 ret;
    for (vo_layer = 0; vo_layer < HI_VO_MAX_PHYS_VIDEO_LAYER_NUM; vo_layer++) {
        for (vo_chn = 0; vo_chn < chn_num; vo_chn++) {
            ret = hi_mpi_vo_pause_chn(vo_layer, vo_chn);
            if (ret != HI_SUCCESS) {
                printf("hi_mpi_vo_pause_chn(%d, %d) fail for 0x%x!\n", vo_layer, vo_chn, ret);
            }
        }
    }
    printf("VO pause...");
    return;
}

hi_void sample_comm_vdec_cmd_ctrl_resume(hi_s32 chn_num, vdec_thread_param *vdec_send, hi_u32 send_arr_len,
    hi_u32 thread_arr_len, hi_bool *vo_pause)
{
    hi_s32 i;
    if (*vo_pause == HI_TRUE) {
        for (i = 0; (i < chn_num) && (i < send_arr_len) && (i < thread_arr_len); i++) {
            vdec_send[i].e_thread_ctrl = THREAD_CTRL_START;
        }
        sample_comm_vdec_cmd_vo_ctrl_resume(chn_num);
    }
    *vo_pause = HI_FALSE;
    return;
}

hi_void sample_comm_vdec_cmd_ctrl_pause(hi_s32 chn_num, vdec_thread_param *vdec_send, hi_u32 send_arr_len,
    hi_u32 thread_arr_len, hi_bool *vo_pause)
{
    hi_s32 i;
    if (*vo_pause == HI_FALSE) {
        for (i = 0; (i < chn_num) && (i < send_arr_len) && (i < thread_arr_len); i++) {
            vdec_send[i].e_thread_ctrl = THREAD_CTRL_PAUSE;
        }
        sample_comm_vdec_cmd_vo_ctrl_pause(chn_num);
    }
    *vo_pause = HI_TRUE;
    return;
}

hi_void sample_comm_vdec_cmd_ctrl_add()
{
    hi_s32 i;
    hi_vo_video_layer_attr vo_attr;
    hi_s32 frame_rate = 0;
    for (i = 0; i < HI_VO_MAX_PHYS_VIDEO_LAYER_NUM; i++) {
        hi_mpi_vo_get_video_layer_attr(i, &vo_attr);
        if (vo_attr.display_frame_rate == 0) {
            continue;
        }
        hi_mpi_vo_get_chn_frame_rate(i, 0, &frame_rate);
        if (frame_rate >= 120) { /* 120:Maximum frame rate */
            printf("VO frame_rate of chnl 0 of layer %d is larger than 120.", i);
            continue;
        }
        frame_rate += 10; /* 10: frame rate added each time */
        hi_mpi_vo_set_chn_frame_rate(i, 0, frame_rate);
        printf("VO frame_rate of chnl 0 of layer %d is set to %d.", i, frame_rate);
    }
    return;
}

hi_void sample_comm_vdec_cmd_ctrl_sub()
{
    hi_s32 i;
    hi_vo_video_layer_attr vo_attr;
    hi_s32 frame_rate = 0;
    for (i = 0; i < HI_VO_MAX_PHYS_VIDEO_LAYER_NUM; i++) {
        hi_mpi_vo_get_video_layer_attr(i, &vo_attr);
        if (vo_attr.display_frame_rate == 0) {
            continue;
        }
        hi_mpi_vo_get_chn_frame_rate(i, 0, &frame_rate);
        if (frame_rate < 20) { /* 20:Minimum frame rate */
            printf("VO frame_rate of chnl 0 of layer %d is less than 10.", i);
            continue;
        }
        frame_rate -= 10; /* 10: frames reduced each time */
        hi_mpi_vo_set_chn_frame_rate(i, 0, frame_rate);
        printf("VO frame_rate of chnl 0 of layer %d is set to %d.", i, frame_rate);
    }
    return;
}

hi_void sample_comm_vdec_cmd_ctrl_step(hi_s32 chn_num, hi_bool vo_pause)
{
    hi_s32 ret;
    hi_vo_layer vo_layer;
    hi_vo_chn vo_chn;
    if (vo_pause == HI_FALSE) {
        printf("firstly press 'p' to pause,then step.");
        return;
    }

    for (vo_layer = 0; vo_layer < HI_VO_MAX_PHYS_VIDEO_LAYER_NUM; vo_layer++) {
        for (vo_chn = 0; vo_chn < chn_num; vo_chn++) {
            ret = hi_mpi_vo_step_chn(vo_layer, vo_chn);
            if (ret != HI_SUCCESS) {
                printf("hi_mpi_vo_step_chn(%d, %d) fail for 0x%x!\n", vo_layer, vo_chn, ret);
            }
        }
    }
    printf("VO step.");
    return;
}

hi_void sample_comm_vdec_cmd_not_circle_send(hi_s32 chn_num, vdec_thread_param *vdec_send, pthread_t *vdec_thread,
    hi_u32 send_arr_len, hi_u32 thread_arr_len)
{
    hi_s32 i, ret;
    hi_vdec_chn_status status;
    printf("decoding..............");
    for (i = 0; (i < chn_num) && (i < send_arr_len) && (i < thread_arr_len); i++) {
        if (vdec_thread[i] != 0) {
            ret = pthread_join(vdec_thread[i], HI_NULL);
            if (ret == 0) {
                vdec_thread[i] = 0;
            }
        }
        vdec_thread[i] = 0;
        while (1) {
            ret = hi_mpi_vdec_query_status(vdec_send[i].chn_id, &status);
            if (ret != HI_SUCCESS) {
                printf("chn %d hi_mpi_vdec_query_status fail!!!\n", ret);
                return;
            }
            if ((status.left_stream_bytes == 0) && (status.left_stream_frames == 0)) {
                printf_vdec_chn_status(vdec_send[i].chn_id, status);
                break;
            }
            usleep(1000); /* 1000:Decoding wait time */
        }
    }
    printf("end!\n");
    return;
}

hi_void sample_comm_vdec_cmd_ctrl(hi_s32 chn_num, vdec_thread_param *vdec_send, pthread_t *vdec_thread,
    hi_u32 send_arr_len, hi_u32 thread_arr_len)
{
    hi_s32 i;
    hi_vdec_chn_status status;
    hi_bool vo_pause = HI_FALSE;
    hi_s32 c = 0;

    for (i = 0; (i < chn_num) && (i < send_arr_len) && (i < thread_arr_len); i++) {
        if (vdec_send[i].circle_send == HI_TRUE) {
            goto WHILE;
        }
    }

    sample_comm_vdec_cmd_not_circle_send(chn_num, vdec_send, vdec_thread, send_arr_len, thread_arr_len);
    return;

WHILE:
    while (1) {
        printf("\n_sample_test:press 'e' to exit; 'p' to pause; 'r' to resume; 'q' to query; 's' to step;"
            " 'a' to add; 'd' to sub!;\n");
        c = getchar();
        if (c == 'e') {
            break;
        } else if (c == 'r') {
            sample_comm_vdec_cmd_ctrl_resume(chn_num, &vdec_send[0], send_arr_len, thread_arr_len, &vo_pause);
        } else if (c == 'p') {
            sample_comm_vdec_cmd_ctrl_pause(chn_num, &vdec_send[0], send_arr_len, thread_arr_len, &vo_pause);
        } else if (c == 'a') {
            sample_comm_vdec_cmd_ctrl_add();
        } else if (c == 'd') {
            sample_comm_vdec_cmd_ctrl_sub();
        } else if (c == 's') {
            sample_comm_vdec_cmd_ctrl_step(chn_num, vo_pause);
        } else if (c == 'q') {
            for (i = 0; (i < chn_num) && (i < send_arr_len) && (i < thread_arr_len); i++) {
                hi_mpi_vdec_query_status(vdec_send[i].chn_id, &status);
                printf_vdec_chn_status(vdec_send[i].chn_id, status);
            }
        }
    }
    return;
}

hi_void sample_comm_vdec_start_send_stream(hi_s32 chn_num, vdec_thread_param *vdec_send, pthread_t *vdec_thread,
    hi_u32 send_arr_len, hi_u32 thread_arr_len)
{
    hi_s32 i;
    if ((vdec_send == HI_NULL) || (vdec_thread == HI_NULL)) {
        printf("vdec_send or vdec_thread can't be NULL!\n");
        return;
    }
    for (i = 0; (i < chn_num) && (i < send_arr_len) && (i < thread_arr_len); i++) {
        vdec_thread[i] = 0;
        pthread_create(&vdec_thread[i], 0, sample_comm_vdec_send_stream, (hi_void *)&vdec_send[i]);
    }
}

hi_void sample_comm_vdec_stop_send_stream(hi_s32 chn_num, vdec_thread_param *vdec_send, pthread_t *vdec_thread,
    hi_u32 send_arr_len, hi_u32 thread_arr_len)
{
    hi_s32 i;
    if ((vdec_send == HI_NULL) || (vdec_thread == HI_NULL)) {
        printf("vdec_send or vdec_thread can't be NULL!\n");
        return;
    }
    for (i = 0; (i < chn_num) && (i < send_arr_len) && (i < thread_arr_len); i++) {
        vdec_send[i].e_thread_ctrl = THREAD_CTRL_STOP;
        hi_mpi_vdec_stop_recv_stream(i);
        if (vdec_thread[i] != 0) {
            pthread_join(vdec_thread[i], HI_NULL);
            vdec_thread[i] = 0;
        }
    }
}

hi_void sample_comm_vdec_config_attr(hi_s32 i, hi_vdec_chn_attr *chn_attr, sample_vdec_attr *sample_vdec,
    hi_u32 arr_len)
{
    hi_pic_buf_attr buf_attr = { 0 };
    chn_attr[i].type = sample_vdec[i].type;
    chn_attr[i].mode = sample_vdec[i].mode;
    chn_attr[i].pic_width = sample_vdec[i].width;
    chn_attr[i].pic_height = sample_vdec[i].height;
    chn_attr[i].stream_buf_size = sample_vdec[i].width * sample_vdec[i].height;
    chn_attr[i].frame_buf_cnt = sample_vdec[i].frame_buf_cnt;
    buf_attr.align = 0;
    buf_attr.height = sample_vdec[i].height;
    buf_attr.width = sample_vdec[i].width;

    if (sample_vdec[i].type == HI_PT_H264 || sample_vdec[i].type == HI_PT_H265) {
        buf_attr.bit_width = sample_vdec[i].sapmle_vdec_video.bit_width;
        buf_attr.pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
        chn_attr[i].video_attr.ref_frame_num = sample_vdec[i].sapmle_vdec_video.ref_frame_num;
        chn_attr[i].video_attr.temporal_mvp_en = 1;
        if ((sample_vdec[i].type == HI_PT_H264) &&
            (sample_vdec[i].sapmle_vdec_video.dec_mode != HI_VIDEO_DEC_MODE_IPB)) {
            chn_attr[i].video_attr.temporal_mvp_en = 0;
        }
        chn_attr[i].frame_buf_size = hi_vdec_get_pic_buf_size(chn_attr[i].type, &buf_attr);
    } else if (sample_vdec[i].type == HI_PT_JPEG || sample_vdec[i].type == HI_PT_MJPEG) {
        chn_attr[i].mode = HI_VDEC_SEND_MODE_FRAME;
        buf_attr.bit_width = HI_DATA_BIT_WIDTH_8;
        buf_attr.pixel_format = sample_vdec[i].sapmle_vdec_picture.pixel_format;
        chn_attr[i].frame_buf_size = hi_vdec_get_pic_buf_size(chn_attr[i].type, &buf_attr);
    }
    return;
}

hi_s32 sample_comm_vdec_start(hi_s32 chn_num, sample_vdec_attr *sample_vdec, hi_u32 arr_len)
{
    hi_s32 i;
    hi_vdec_chn_attr chn_attr[HI_VDEC_MAX_CHN_NUM];
    hi_vdec_chn_pool pool;
    hi_vdec_chn_param chn_param;
    hi_vdec_mod_param mod_param;

    check_null_ptr_return(sample_vdec);
    if (arr_len > HI_VDEC_MAX_CHN_NUM) {
        SAMPLE_PRT("array size(%u) of chn_attr need < %u!\n", arr_len, HI_VDEC_MAX_CHN_NUM);
        return HI_FAILURE;
    }

    CHECK_RET(hi_mpi_vdec_get_mod_param(&mod_param), "hi_mpi_vdec_get_mod_param");

    mod_param.vb_src = g_vdec_vb_src;
    mod_param.pic_mod_param.progressive_en = g_progressive_en;
    CHECK_RET(hi_mpi_vdec_set_mod_param(&mod_param), "hi_mpi_vdec_get_mod_param");

    for (i = 0; (i < chn_num) && (i < arr_len); i++) {
        sample_comm_vdec_config_attr(i, &chn_attr[0], &sample_vdec[0], arr_len);

        CHECK_CHN_RET(hi_mpi_vdec_create_chn(i, &chn_attr[i]), i, "hi_mpi_vdec_create_chn");

        if ((g_vdec_vb_src == HI_VB_SRC_USER) && (i < HI_VB_MAX_POOLS)) {
            pool.pic_vb_pool = g_pic_vb_pool[i];
            pool.tmv_vb_pool = g_tmv_vb_pool[i];
            CHECK_CHN_RET(hi_mpi_vdec_attach_vb_pool(i, &pool), i, "hi_mpi_vdec_attach_vb_pool");
        }

        CHECK_CHN_RET(hi_mpi_vdec_get_chn_param(i, &chn_param), i, "hi_mpi_vdec_get_chn_param");
        if (sample_vdec[i].type == HI_PT_H264 || sample_vdec[i].type == HI_PT_H265) {
            chn_param.video_param.dec_mode = sample_vdec[i].sapmle_vdec_video.dec_mode;
            chn_param.video_param.compress_mode = HI_COMPRESS_MODE_NONE;
            chn_param.video_param.video_format = HI_VIDEO_FORMAT_TILE_64x16;
            if (chn_param.video_param.dec_mode == HI_VIDEO_DEC_MODE_IPB) {
                chn_param.video_param.out_order = HI_VIDEO_OUT_ORDER_DISPLAY;
            } else {
                chn_param.video_param.out_order = HI_VIDEO_OUT_ORDER_DEC;
            }
        } else {
            chn_param.pic_param.pixel_format = sample_vdec[i].sapmle_vdec_picture.pixel_format;
            chn_param.pic_param.alpha = sample_vdec[i].sapmle_vdec_picture.alpha;
        }
        chn_param.display_frame_num = sample_vdec[i].display_frame_num;
        CHECK_CHN_RET(hi_mpi_vdec_set_chn_param(i, &chn_param), i, "hi_mpi_vdec_get_chn_param");

        CHECK_CHN_RET(hi_mpi_vdec_start_recv_stream(i), i, "hi_mpi_vdec_start_recv_stream");
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_vdec_stop(hi_s32 chn_num)
{
    hi_s32 i;

    for (i = 0; i < chn_num; i++) {
        CHECK_CHN_RET(hi_mpi_vdec_stop_recv_stream(i), i, "hi_mpi_vdec_stop_recv_stream");
        CHECK_CHN_RET(hi_mpi_vdec_destroy_chn(i), i, "hi_mpi_vdec_destroy_chn");
    }

    return HI_SUCCESS;
}

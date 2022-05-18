/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: vda common function for sample.
 * Author: Hisilicon multimedia software group
 * Create: 2019/5/07
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

#include "sample_comm.h"

#define SAMPLE_VDA_MD_CHN 0
#define SAMPLE_VDA_OD_CHN 1

typedef struct {
    hi_bool thread_start;
    hi_vda_chn vda_chn;
} sample_vda_param;

static pthread_t g_vda_pid[2]; /* 2 : number of pid */
static sample_vda_param g_md_param;
static sample_vda_param g_od_param;

hi_void sample_common_vda_handle_sig(void)
{
    /* join thread */
    if (g_od_param.thread_start == HI_TRUE) {
        g_od_param.thread_start = HI_FALSE;
        pthread_join(g_vda_pid[SAMPLE_VDA_OD_CHN], 0);
    }

    /* join thread */
    if (g_md_param.thread_start == HI_TRUE) {
        g_md_param.thread_start = HI_FALSE;
        pthread_join(g_vda_pid[SAMPLE_VDA_MD_CHN], 0);
    }
}

/* vda MD mode print -- Md OBJ */
hi_s32 sample_common_vda_md_prt_obj(FILE *fp, hi_vda_data *vda_data)
{
    hi_vda_rgn *vda_obj = HI_NULL;
    hi_s32 i;

    fprintf(fp, "===== %s =====\n", __FUNCTION__);

    if (vda_data->data.md_data.obj_valid != HI_TRUE) {
        fprintf(fp, "obj_valid = FALSE.\n");
        return HI_SUCCESS;
    }

    fprintf(fp, "obj_num=%d, idx_of_max_obj=%d, size_of_max_obj=%d, size_of_total_obj=%d\n",
        vda_data->data.md_data.obj_data.obj_num, vda_data->data.md_data.obj_data.idx_of_max_obj,
        vda_data->data.md_data.obj_data.size_of_max_obj, vda_data->data.md_data.obj_data.size_of_total_obj);
    for (i = 0; i < vda_data->data.md_data.obj_data.obj_num; i++) {
        vda_obj = vda_data->data.md_data.obj_data.addr + i;
        fprintf(fp, "[%d]\t upper_left_x=%d, upper_left_y=%d, lower_right_x=%d, lower_right_y=%d\n", i,
            vda_obj->upper_left_x, vda_obj->upper_left_y, vda_obj->lower_right_x, vda_obj->lower_right_y);
    }
    fflush(fp);
    return HI_SUCCESS;
}

/* vda MD mode print -- Alarm Pixel Count */
hi_s32 sample_common_vda_md_prt_ap(FILE *fp, hi_vda_data *vda_data)
{
    fprintf(fp, "===== %s =====\n", __FUNCTION__);

    if (vda_data->data.md_data.pixel_num_valid != HI_TRUE) {
        fprintf(fp, "pixel_num_valid = FALSE.\n");
        return HI_SUCCESS;
    }

    fprintf(fp, "alarm_pixel_cnt=%d\n", vda_data->data.md_data.alarm_pixel_cnt);
    fflush(fp);
    return HI_SUCCESS;
}

/* vda MD mode print -- SAD */
hi_s32 sample_common_vda_md_prt_sad(FILE *fp, hi_vda_data *vda_data)
{
    hi_s32 i, j;
    hi_void *addr = HI_NULL;

    fprintf(fp, "===== %s =====\n", __FUNCTION__);
    if (vda_data->data.md_data.mb_sad_valid != HI_TRUE) {
        fprintf(fp, "mb_sad_valid = FALSE.\n");
        return HI_SUCCESS;
    }

    for (i = 0; i < vda_data->mb_height; i++) {
        addr = (hi_void *)((hi_uintptr_t)vda_data->data.md_data.mb_sad_data.addr +
            i * vda_data->data.md_data.mb_sad_data.stride);

        for (j = 0; j < vda_data->mb_width; j++) {
            hi_u8 *addr_temp0 = HI_NULL;
            hi_u16 *addr_temp1 = HI_NULL;

            if (HI_VDA_MB_SAD_8BIT == vda_data->data.md_data.mb_sad_data.mb_sad_bits) {
                addr_temp0 = (hi_u8 *)addr + j;

                fprintf(fp, "%-4d ", *addr_temp0);
            } else {
                addr_temp1 = (hi_u16 *)addr + j;

                fprintf(fp, "%-6d ", *addr_temp1);
            }
        }

        printf("\n");
    }

    fflush(fp);
    return HI_SUCCESS;
}

hi_s32 sample_comm_vda_md_prt_result(hi_vda_chn vda_chn, FILE *fp)
{
    hi_s32 ret;
    hi_vda_data vda_data;

    /* step 2.3 : call mpi to get one-frame stream */
    ret = hi_mpi_vda_get_data(vda_chn, &vda_data, -1);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_vda_get_data failed with %#x!\n", ret);
        return HI_FAILURE;
    }
    /* step 2.4 : save frame to file */
    printf("\033[0;0H"); /* move cursor */
    ret = sample_common_vda_md_prt_sad(fp, &vda_data);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("prt sda fail\n");
        return HI_FAILURE;
    }
    ret = sample_common_vda_md_prt_obj(fp, &vda_data);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("prt obj fail\n");
        return HI_FAILURE;
    }
    ret = sample_common_vda_md_prt_ap(fp, &vda_data);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("prt ap fail\n");
        return HI_FAILURE;
    }
    /* step 2.5 : release stream */
    ret = hi_mpi_vda_release_data(vda_chn, &vda_data);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_vda_release_data failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/* vda MD mode thread process */
hi_void *sample_common_vda_md_get_result(hi_void *data)
{
    hi_s32 ret;
    hi_vda_chn vda_chn;
    sample_vda_param *md_param = HI_NULL;
    hi_s32 maxfd = 0;
    FILE *fp = stdout;
    hi_s32 vda_fd;
    fd_set read_fds;
    struct timeval timeout_val;

    prctl(PR_SET_NAME, "hi_MdGetResult", 0, 0, 0);

    md_param = (sample_vda_param *)data;

    vda_chn = md_param->vda_chn;

    /*
     * decide the stream file name, and open file to save stream
     * Set Venc Fd.
     */
    vda_fd = hi_mpi_vda_get_fd(vda_chn);
    if (vda_fd < 0) {
        SAMPLE_PRT("hi_mpi_vda_get_fd failed with %#x!\n", vda_fd);
        return HI_NULL;
    }
    if (maxfd <= vda_fd) {
        maxfd = vda_fd;
    }
    while (md_param->thread_start == HI_TRUE) {
        FD_ZERO(&read_fds);
        FD_SET(vda_fd, &read_fds);

        timeout_val.tv_sec = 2; /* 2 : second */
        timeout_val.tv_usec = 0;
        ret = select(maxfd + 1, &read_fds, HI_NULL, HI_NULL, &timeout_val);
        if (ret < 0) {
            SAMPLE_PRT("select failed!\n");
            break;
        } else if (ret == 0) {
            SAMPLE_PRT("get vda result time out, exit thread\n");
            break;
        }

        if (FD_ISSET(vda_fd, &read_fds)) {
            ret = sample_comm_vda_md_prt_result(vda_chn, fp);
        }
    }

    return HI_NULL;
}

hi_s32 sample_common_vda_set_vpss_bind(hi_vda_chn vda_chn, hi_u32 chn)
{
    hi_s32 ret;
    hi_mpp_chn src_chn, dest_chn;

    src_chn.mod_id = HI_ID_VPSS;
    src_chn.chn_id = chn;
    src_chn.dev_id = 0;

    dest_chn.mod_id = HI_ID_VDA;
    dest_chn.chn_id = vda_chn;
    dest_chn.dev_id = 0;

    ret = hi_mpi_sys_bind(&src_chn, &dest_chn);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("bind err(0x%x)!\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

/* start vda MD mode */
hi_s32 sample_common_vda_md_start(hi_vda_chn vda_chn, hi_u32 chn, hi_size *size)
{
    hi_s32 ret;
    hi_vda_chn_attr vda_chn_attr;

    if (size->width > HI_VDA_MAX_WIDTH || size->height > HI_VDA_MAX_HEIGHT) {
        SAMPLE_PRT("Picture size invaild!\n");
        return HI_FAILURE;
    }

    /* step 1 create vda channel */
    vda_chn_attr.work_mode = HI_VDA_WORK_MODE_MD;
    vda_chn_attr.width = size->width;
    vda_chn_attr.height = size->height;

    vda_chn_attr.attr.md_attr.vda_alg = HI_VDA_ALG_REF;
    vda_chn_attr.attr.md_attr.mb_size = HI_VDA_MB_16PIXEL;
    vda_chn_attr.attr.md_attr.mb_sad_bits = HI_VDA_MB_SAD_8BIT;
    vda_chn_attr.attr.md_attr.ref_mode = HI_VDA_REF_MODE_DYNAMIC;
    vda_chn_attr.attr.md_attr.md_buf_num = 8; /* 8 : buf number */
    vda_chn_attr.attr.md_attr.vda_interval = 4; /* 4 : frame interval */
    vda_chn_attr.attr.md_attr.bg_up_src_wgt = 128; /* 128 : weight of source pictures */
    vda_chn_attr.attr.md_attr.sad_threshold = 100; /* 100 : alarm threshold */
    vda_chn_attr.attr.md_attr.obj_num_max = 128; /* 128 : number of output OBJ regions */

    ret = hi_mpi_vda_create_chn(vda_chn, &vda_chn_attr);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_vda_create_chn err!\n");
        return ret;
    }

    /* step 2: vda chn bind vpss chn */
    ret = sample_common_vda_set_vpss_bind(vda_chn, chn);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("err!\n");
        return ret;
    }

    /* step 3: vda chn start recv picture */
    ret = hi_mpi_vda_start_recv_pic(vda_chn);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_vda_start_recv_pic err!\n");
        return ret;
    }

    /* step 4: create thread to get result */
    g_md_param.thread_start = HI_TRUE;
    g_md_param.vda_chn = vda_chn;

    pthread_create(&g_vda_pid[SAMPLE_VDA_MD_CHN], 0, sample_common_vda_md_get_result, (hi_void *)&g_md_param);

    return HI_SUCCESS;
}

/* stop vda, and stop vda thread -- MD */
hi_void sample_common_vda_md_stop(hi_vda_chn vda_chn, hi_u32 chn)
{
    hi_s32 ret;
    hi_mpp_chn src_chn, dest_chn;

    /* join thread */
    if (g_md_param.thread_start == HI_TRUE) {
        g_md_param.thread_start = HI_FALSE;
        pthread_join(g_vda_pid[SAMPLE_VDA_MD_CHN], 0);
    }

    /* vda stop recv picture */
    ret = hi_mpi_vda_stop_recv_pic(vda_chn);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_vda_stop_recv_pic err(0x%x)!!!!\n", ret);
    }

    /* unbind vda chn & vpss chn */
    src_chn.mod_id = HI_ID_VPSS;
    src_chn.chn_id = chn;
    src_chn.dev_id = 0;
    dest_chn.mod_id = HI_ID_VDA;
    dest_chn.chn_id = vda_chn;
    dest_chn.dev_id = 0;

    ret = hi_mpi_sys_unbind(&src_chn, &dest_chn);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("err(0x%x)!!!!\n", ret);
    }

    /* destroy vda chn */
    ret = hi_mpi_vda_destroy_chn(vda_chn);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("err(0x%x)!!!!\n", ret);
    }

    return;
}

/* vda OD mode thread process */
hi_s32 sample_common_vda_od_prt(FILE *fp, hi_vda_data *vda_data)
{
    hi_s32 i;

    fprintf(fp, "===== %s =====\n", __FUNCTION__);
    fprintf(fp, "OD region total count =%d\n", vda_data->data.od_data.rgn_num);
    for (i = 0; i < vda_data->data.od_data.rgn_num; i++) {
        fprintf(fp, "OD region[%d]: %d\n", i, vda_data->data.od_data.rgn_alarm_en[i]);
    }
    fflush(fp);
    return HI_SUCCESS;
}

/* vda OD mode thread process */
hi_void *sample_common_vda_od_get_result(hi_void *data)
{
    hi_s32 i;
    hi_s32 ret;
    hi_vda_chn vda_chn;
    hi_vda_data vda_data;
    hi_u32 rgn_num;
    sample_vda_param *od_param = HI_NULL;
    FILE *fp = stdout;

    prctl(PR_SET_NAME, "hi_OdGetResult", 0, 0, 0);

    od_param = (sample_vda_param *)data;
    vda_chn = od_param->vda_chn;

    while (od_param->thread_start == HI_TRUE) {
        ret = hi_mpi_vda_get_data(vda_chn, &vda_data, -1);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("hi_mpi_vda_get_data failed with %#x!\n", ret);
            return HI_NULL;
        }

        ret = sample_common_vda_od_prt(fp, &vda_data);
        if (ret != HI_SUCCESS) {
            hi_mpi_vda_release_data(vda_chn, &vda_data);
            SAMPLE_PRT("od prt fail!\n");
            return HI_NULL;
        }

        rgn_num = vda_data.data.od_data.rgn_num;

        for (i = 0; i < rgn_num; i++) {
            if (vda_data.data.od_data.rgn_alarm_en[i] == HI_FALSE) {
                continue;
            }
            printf("################vda_chn--%d,Rgn--%d,Occ!\n", vda_chn, i);
            ret = hi_mpi_vda_reset_od_rgn(vda_chn, i);
            if (ret != HI_SUCCESS) {
                hi_mpi_vda_release_data(vda_chn, &vda_data);
                SAMPLE_PRT("hi_mpi_vda_reset_od_rgn failed with %#x!\n", ret);
                return HI_NULL;
            }
        }

        ret = hi_mpi_vda_release_data(vda_chn, &vda_data);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("hi_mpi_vda_release_data failed with %#x!\n", ret);
            return HI_NULL;
        }

        usleep(200 * 1000); /* 200 * 1000 : waiting for 200ms */
    }

    return HI_NULL;
}

/* start vda OD mode */
hi_s32 sample_common_vda_od_start(hi_vda_chn vda_chn, hi_u32 chn, hi_size *size)
{
    hi_vda_chn_attr vda_chn_attr;
    hi_s32 ret;

    if (size->width > HI_VDA_MAX_WIDTH || size->height > HI_VDA_MAX_HEIGHT) {
        SAMPLE_PRT("Picture size invaild!\n");
        return HI_FAILURE;
    }

    /* step 1 : create vda channel */
    vda_chn_attr.work_mode = HI_VDA_WORK_MODE_OD;
    vda_chn_attr.width = size->width;
    vda_chn_attr.height = size->height;

    vda_chn_attr.attr.od_attr.vda_alg = HI_VDA_ALG_REF;
    vda_chn_attr.attr.od_attr.mb_size = HI_VDA_MB_8PIXEL;
    vda_chn_attr.attr.od_attr.mb_sad_bits = HI_VDA_MB_SAD_8BIT;
    vda_chn_attr.attr.od_attr.ref_mode = HI_VDA_REF_MODE_DYNAMIC;
    vda_chn_attr.attr.od_attr.vda_interval = 4; /* 4 : frame interval */
    vda_chn_attr.attr.od_attr.bg_up_src_wgt = 128; /* 128 : weight of source pictures */

    vda_chn_attr.attr.od_attr.rgn_num = 1;

    vda_chn_attr.attr.od_attr.od_rgn_attr[0].rect.x = 0;
    vda_chn_attr.attr.od_attr.od_rgn_attr[0].rect.y = 0;
    vda_chn_attr.attr.od_attr.od_rgn_attr[0].rect.width = size->width;
    vda_chn_attr.attr.od_attr.od_rgn_attr[0].rect.height = size->height;

    vda_chn_attr.attr.od_attr.od_rgn_attr[0].sad_threshold = 100; /* 100 : alarm threshold */
    vda_chn_attr.attr.od_attr.od_rgn_attr[0].area_threshold = 60; /* 60 : region area alarm threshold */
    vda_chn_attr.attr.od_attr.od_rgn_attr[0].occlusion_cnt_threshold = 6; /* 6: occlusion count alarm threshold */
    vda_chn_attr.attr.od_attr.od_rgn_attr[0].uncover_cnt_threshold = 2; /* 2 : Maximum uncover count */

    ret = hi_mpi_vda_create_chn(vda_chn, &vda_chn_attr);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_vda_create_chn err(0x%x)!\n", ret);
        return ret;
    }

    /* step 2 : bind vda channel to vpss channel */
    ret = sample_common_vda_set_vpss_bind(vda_chn, chn);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("err!\n");
        return ret;
    }

    /* vda start rcv picture */
    ret = hi_mpi_vda_start_recv_pic(vda_chn);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_vda_start_recv_pic err(0x%x)!\n", ret);
        return ret;
    }

    g_od_param.thread_start = HI_TRUE;
    g_od_param.vda_chn = vda_chn;

    pthread_create(&g_vda_pid[SAMPLE_VDA_OD_CHN], 0, sample_common_vda_od_get_result, (hi_void *)&g_od_param);

    return HI_SUCCESS;
}

/* stop vda, and stop vda thread -- OD */
hi_void sample_common_vda_od_stop(hi_vda_chn vda_chn, hi_u32 chn)
{
    hi_s32 ret;
    hi_mpp_chn src_chn, dest_chn;

    /* join thread */
    if (g_od_param.thread_start == HI_TRUE) {
        g_od_param.thread_start = HI_FALSE;
        pthread_join(g_vda_pid[SAMPLE_VDA_OD_CHN], 0);
    }

    /* vda stop recv picture */
    ret = hi_mpi_vda_stop_recv_pic(vda_chn);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_vda_stop_recv_pic err(0x%x)!!!!\n", ret);
    }

    /* unbind vda chn & vi chn */
    src_chn.mod_id = HI_ID_VPSS;
    src_chn.chn_id = chn;
    src_chn.dev_id = 0;
    dest_chn.mod_id = HI_ID_VDA;
    dest_chn.chn_id = vda_chn;
    dest_chn.dev_id = 0;
    ret = hi_mpi_sys_unbind(&src_chn, &dest_chn);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_sys_unbind err(0x%x)!!!!\n", ret);
    }

    /* destroy vda chn */
    ret = hi_mpi_vda_destroy_chn(vda_chn);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_vda_destroy_chn err(0x%x)!!!!\n", ret);
    }

    return;
}

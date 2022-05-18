/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: sample_audio.c
 * Author: Hisilicon multimedia software group
 * Create: 2019/10/10
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>

#include "sample_comm.h"
#include "audio_aac_adp.h"
#include "audio_dl_adp.h"
#include "hi_resample.h"
#if defined(HI_VQE_USE_STATIC_MODULE_REGISTER)
#include "hi_vqe_register.h"
#endif

static hi_payload_type g_payload_type = HI_PT_G711A;
static hi_bool g_aio_resample  = HI_FALSE;
static hi_bool g_user_get_mode  = HI_FALSE;
static hi_bool g_ao_volume_ctrl = HI_FALSE;
static hi_audio_sample_rate g_in_sample_rate  = HI_AUDIO_SAMPLE_RATE_BUTT;
static hi_audio_sample_rate g_out_sample_rate = HI_AUDIO_SAMPLE_RATE_BUTT;
/* 0: close, 1: record, 2: talk */
static hi_u32 g_ai_vqe_type = 2;
hi_ai_record_vqe_cfg g_ai_vqe_record_attr = { 0 };
hi_ai_talk_vqe_cfg g_ai_vqe_talk_attr = { 0 };

static void *g_res_handle = HI_NULL;
static hi_s16 *g_res_out_buf = HI_NULL;

#define SAMPLE_DBG(ret)\
    do { \
        printf("ret = %#x, fuc:%s, line:%d\n", ret, __FUNCTION__, __LINE__); \
    } while (0)

#define sample_res_check_null_ptr(ptr) \
    do { \
        if((hi_u8*)(ptr) == HI_NULL) { \
            printf("ptr is HI_NULL,fuc:%s,line:%d\n", __FUNCTION__, __LINE__); \
            return HI_FAILURE; \
        } \
    } while (0)

#define HI_RES_LIB_NAME "libhive_RES.so"

/* function : PT number to string */
static char *sample_audio_pt2_str(hi_payload_type type)
{
    if (type == HI_PT_G711A) {
        return "g711a";
    } else if (type == HI_PT_G711U) {
        return "g711u";
    } else if (type == HI_PT_ADPCMA) {
        return "adpcm";
    } else if (type == HI_PT_G726) {
        return "g726";
    } else if (type == HI_PT_LPCM) {
        return "pcm";
    } else if (type == HI_PT_AAC) {
        return "aac";
    } else {
        return "data";
    }
}

typedef struct {
    hi_bool start;
    hi_s32  ai_dev;
    hi_s32  ai_chn;
    hi_audio_sample_rate in_sample;
    hi_audio_sample_rate out_sample;
    hi_u32 per_frame;
    FILE *fd;
    pthread_t ai_pid;
} hi_sample_ai_to_ext_res;

typedef hi_void *(*hi_resample_create_callback)(hi_s32 in_rate, hi_s32 out_rate, hi_s32 chn_num);
typedef hi_s32 (*hi_resample_process_callback)(hi_void *instance, hi_s16 *in_buf, hi_s32 in_samples,
    hi_s16 *out_buf);
typedef hi_void (*hi_resample_destroy_callback)(hi_void *instance);
typedef hi_s32 (*hi_resample_get_max_output_num_callback)(hi_void *instance, hi_s32 in_samples);

typedef struct {
    hi_void *lib_handle;
    hi_resample_create_callback resample_create_callback;
    hi_resample_process_callback resample_process_callback;
    hi_resample_destroy_callback resample_destroy_callback;
    hi_resample_get_max_output_num_callback resample_get_max_output_num_callback;
} hi_sample_res_fun;

static hi_sample_ai_to_ext_res   g_sample_ai_ext_res[HI_AI_DEV_MAX_NUM * HI_AI_MAX_CHN_NUM];
static hi_sample_res_fun     g_sample_res_fun = {0};

static hi_void sample_audio_ai_to_res_proc(hi_sample_ai_to_ext_res *ai_ctrl, hi_audio_frame *frame,
    hi_s16 *cache, hi_s32 *cache_count, hi_s16 cache_sample)
{
    hi_u32 proc_frame, out_sample, cur_sample, ret;
    hi_s32 mulit = 1;

    if (ai_ctrl->in_sample % ai_ctrl->out_sample == 0) {
        mulit = ai_ctrl->in_sample / ai_ctrl->out_sample;
        if (ai_ctrl->per_frame % mulit == 0) {
            mulit = 1;
        }
    }

    /* send frame to encoder */
    if (mulit != 1) {
        if (*cache_count != 0) {
            if (cache_sample < *cache_count) {
                return;
            }
            ret = memcpy_s(cache + (*cache_count), (cache_sample - *cache_count) * sizeof(hi_s16),
                frame->virt_addr[0], (mulit - *cache_count) * sizeof(hi_s16));
            if (ret != EOK) {
                printf("%s: memcpy_s failed, ret:0x%x.\n", __FUNCTION__, ret);
                return;
            }
            out_sample = g_sample_res_fun.resample_process_callback(g_res_handle, cache, mulit, g_res_out_buf);
            (hi_void)fwrite(g_res_out_buf, 2, out_sample, ai_ctrl->fd); /* 2:byte */
        }

        proc_frame = ai_ctrl->per_frame - (mulit - *cache_count);
        proc_frame = proc_frame - proc_frame % mulit;
        out_sample = g_sample_res_fun.resample_process_callback(g_res_handle, (hi_s16*)(frame->virt_addr[0] +
            (mulit - *cache_count) * sizeof(hi_s16)), proc_frame, g_res_out_buf);
        (hi_void)fwrite(g_res_out_buf, 2, out_sample, ai_ctrl->fd); /* 2:byte */

        cur_sample = proc_frame + (mulit - *cache_count);
        if ((ai_ctrl->per_frame - cur_sample) != 0) {
            ret = memcpy_s(cache, cache_sample * sizeof(hi_s16),
                frame->virt_addr[0] + cur_sample * sizeof(hi_s16), (ai_ctrl->per_frame - cur_sample) * sizeof(hi_s16));
            if (ret != EOK) {
                printf("%s: memcpy_s failed, ret:0x%x.\n", __FUNCTION__, ret);
                return;
            }
        }
        *cache_count = ai_ctrl->per_frame - cur_sample;
    } else {
        out_sample = g_sample_res_fun.resample_process_callback(g_res_handle, (hi_s16*)frame->virt_addr[0],
            ai_ctrl->per_frame, g_res_out_buf);
        (hi_void)fwrite(g_res_out_buf, 2, out_sample, ai_ctrl->fd); /* 2:byte */
    }

    fflush(ai_ctrl->fd);
}

static hi_s32 sample_audio_ai_to_res_init(hi_sample_ai_to_ext_res *ai_ctrl)
{
    hi_ai_chn_param ai_chn_para;
    hi_s32 ret, out_max_num;
    sample_res_check_null_ptr(g_sample_res_fun.resample_create_callback);
    sample_res_check_null_ptr(g_sample_res_fun.resample_process_callback);
    sample_res_check_null_ptr(g_sample_res_fun.resample_destroy_callback);
    sample_res_check_null_ptr(g_sample_res_fun.resample_get_max_output_num_callback);

    ret = hi_mpi_ai_get_chn_param(ai_ctrl->ai_dev, ai_ctrl->ai_chn, &ai_chn_para);
    if (ret != HI_SUCCESS) {
        printf("%s: Get ai chn param failed\n", __FUNCTION__);
        return ret;
    }

    ai_chn_para.usr_frame_depth = 30; /* 30:frame depth */

    ret = hi_mpi_ai_set_chn_param(ai_ctrl->ai_dev, ai_ctrl->ai_chn, &ai_chn_para);
    if (ret != HI_SUCCESS) {
        printf("%s: set ai chn param failed, ret=0x%x\n", __FUNCTION__, ret);
        return ret;
    }

    /* Create Resample. only support mono channel. */
    g_res_handle = g_sample_res_fun.resample_create_callback(ai_ctrl->in_sample, ai_ctrl->out_sample, 1);
    if (ret != HI_SUCCESS) {
        printf("%s: hi_resampler_create failed, ret=0x%x\n", __FUNCTION__, ret);
        return ret;
    }

    out_max_num = g_sample_res_fun.resample_get_max_output_num_callback(g_res_handle, ai_ctrl->per_frame);
    if (out_max_num <= 0) {
        printf("get max output num failed\n");
        g_sample_res_fun.resample_destroy_callback(g_res_handle);
        return HI_FAILURE;
    }

    g_res_out_buf = malloc(out_max_num * sizeof(hi_s16) + 2); /* 2:Reserved space */
    if (g_res_out_buf == HI_NULL) {
        printf("malloc failed\n");
        g_sample_res_fun.resample_destroy_callback(g_res_handle);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_comm_audio_select(hi_s32 ai_fd, fd_set *read_fds)
{
    hi_s32 ret;
    struct timeval time_out_val;
    time_out_val.tv_sec = 1;
    time_out_val.tv_usec = 0;

    FD_ZERO(read_fds);
    FD_SET(ai_fd, read_fds);

    ret = select(ai_fd + 1, read_fds, HI_NULL, HI_NULL, &time_out_val);
    if (ret < 0) {
        return HI_FAILURE;
    } else if (ret == 0) {
        printf("%s: get ai frame select time out\n", __FUNCTION__);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

/* function : get frame from Ai, send it to Resampler */
void *sample_comm_audio_ai_ext_res_proc(void *parg)
{
    hi_s32 ret;
    hi_s32 ai_fd = -1;
    hi_audio_frame frame = {0};
    hi_aec_frame aec_frm = {0};
    hi_sample_ai_to_ext_res *ai_ctrl = (hi_sample_ai_to_ext_res *)parg;
    fd_set read_fds;
    hi_s32 cache_count = 0;
    hi_s16 cache[16]; /* 16:Max 64 / 8 * 2. */

    ret = sample_audio_ai_to_res_init(ai_ctrl);
    if (ret != HI_SUCCESS) {
        printf("%s: sample_audio_ai_to_res_param_set failed\n", __FUNCTION__);
        return HI_NULL;
    }

    ai_fd = hi_mpi_ai_get_fd(ai_ctrl->ai_dev, ai_ctrl->ai_chn);
    if (ai_fd < 0) {
        printf("%s: get ai fd failed\n", __FUNCTION__);
        return NULL;
    }

    while (ai_ctrl->start) {
        ret = sample_comm_audio_select(ai_fd, &read_fds);
        if (ret != HI_SUCCESS) {
            break;
        }

        if (FD_ISSET(ai_fd, &read_fds)) {
            /* get frame from ai chn */
            ret = memset_s(&aec_frm, sizeof(hi_aec_frame), 0, sizeof(hi_aec_frame));
            if (ret != EOK) {
                printf("%s: aec_frm init failed with %d!\n", __FUNCTION__, ret);
                continue;
            }

            ret = hi_mpi_ai_get_frame(ai_ctrl->ai_dev, ai_ctrl->ai_chn, &frame, &aec_frm, HI_FALSE);
            if (ret != HI_SUCCESS) {
                continue;
            }

            sample_audio_ai_to_res_proc(ai_ctrl, &frame, cache, &cache_count, sizeof(cache) / sizeof(hi_s16));

            ret = hi_mpi_ai_release_frame(ai_ctrl->ai_dev, ai_ctrl->ai_chn, &frame, &aec_frm);
            if (ret != HI_SUCCESS) {
                printf("%s: hi_mpi_ai_release_frame(%d, %d), failed with %#x!\n", \
                    __FUNCTION__, ai_ctrl->ai_dev, ai_ctrl->ai_chn, ret);
                break;
            }
        }
    }

    ai_ctrl->start = HI_FALSE;
    free(g_res_out_buf);
    g_res_out_buf = HI_NULL;
    g_sample_res_fun.resample_destroy_callback(g_res_handle);

    return HI_NULL;
}

/* function : Create the thread to get frame from ai and send to aenc */
hi_s32 sample_comm_audio_creat_trd_ai_ext_res(hi_aio_attr *aio_attr, hi_audio_dev ai_dev,
    hi_ai_chn ai_chn, hi_audio_sample_rate g_out_sample_rate, FILE *res_fd)
{
    hi_sample_ai_to_ext_res *ai_to_ext_res = HI_NULL;

    if ((ai_dev >= HI_AI_DEV_MAX_NUM) || (ai_dev < 0) ||
        (ai_chn >= HI_AI_MAX_CHN_NUM) || (ai_chn < 0)) {
        printf("%s: ai_dev = %d, ai_chn = %d error.\n", __FUNCTION__, ai_dev, ai_chn);
        return HI_FAILURE;
    }

    ai_to_ext_res = &g_sample_ai_ext_res[ai_dev * HI_AI_MAX_CHN_NUM + ai_chn];
    ai_to_ext_res->ai_dev = ai_dev;
    ai_to_ext_res->ai_chn = ai_chn;
    ai_to_ext_res->in_sample = aio_attr->sample_rate;
    ai_to_ext_res->out_sample = g_out_sample_rate;
    ai_to_ext_res->per_frame = aio_attr->point_num_per_frame;
    ai_to_ext_res->fd = res_fd;
    ai_to_ext_res->start = HI_TRUE;
    pthread_create(&ai_to_ext_res->ai_pid, 0, sample_comm_audio_ai_ext_res_proc, ai_to_ext_res);

    return HI_SUCCESS;
}

/* function : Destory the thread to get frame from ai and send to extern resamler */
hi_s32 sample_comm_audio_destroy_trd_ai_ext_res(hi_audio_dev ai_dev, hi_ai_chn ai_chn)
{
    hi_sample_ai_to_ext_res *ai_to_ext_res = HI_NULL;

    if ((ai_dev >= HI_AI_DEV_MAX_NUM) || (ai_dev < 0) ||
        (ai_chn >= HI_AI_MAX_CHN_NUM) || (ai_chn < 0)) {
        printf("%s: ai_dev = %d, ai_chn = %d error.\n", __FUNCTION__, ai_dev, ai_chn);
        return HI_FAILURE;
    }

    ai_to_ext_res = &g_sample_ai_ext_res[ai_dev * HI_AI_MAX_CHN_NUM + ai_chn];
    if (ai_to_ext_res->start) {
        ai_to_ext_res->start = HI_FALSE;
        pthread_join(ai_to_ext_res->ai_pid, 0);
    }

    if (ai_to_ext_res->fd != HI_NULL) {
        fclose(ai_to_ext_res->fd);
        ai_to_ext_res->fd = HI_NULL;
    }

    return HI_SUCCESS;
}

/* function : DeInit resamle functions */
static hi_void sample_audio_deinit_ext_res_fun(hi_void)
{
    hi_s32 ret;

#if defined(HI_VQE_USE_STATIC_MODULE_REGISTER)
    ret = memset_s(&g_sample_res_fun, sizeof(hi_sample_res_fun), 0, sizeof(hi_sample_res_fun));
    if (ret != EOK) {
        printf("%s: g_sample_res_fun init failed with %d!\n", __FUNCTION__, ret);
    }
#else
    if (g_sample_res_fun.lib_handle != HI_NULL) {
        audio_dlclose(g_sample_res_fun.lib_handle);
        ret = memset_s(&g_sample_res_fun, sizeof(hi_sample_res_fun), 0, sizeof(hi_sample_res_fun));
        if (ret != EOK) {
            printf("%s: g_sample_res_fun init failed with %d!\n", __FUNCTION__, ret);
        }
    }
#endif
}

/* function : Init resamle functions */
static hi_s32 sample_audio_init_ext_res_fun(hi_void)
{
    hi_sample_res_fun sample_res_fun = {0};

    sample_audio_deinit_ext_res_fun();

#if defined(HI_VQE_USE_STATIC_MODULE_REGISTER)
    sample_res_fun.resample_create_callback = hi_resample_create;
    sample_res_fun.resample_process_callback = hi_resample_process;
    sample_res_fun.resample_destroy_callback = hi_resample_destroy;
    sample_res_fun.resample_get_max_output_num_callback = hi_resample_get_max_output_num;
#else
    hi_s32 ret;
    ret = audio_dlopen(&(sample_res_fun.lib_handle), HI_RES_LIB_NAME);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "load resample lib fail!\n");
        return HI_FAILURE;
    }

    ret = audio_dlsym((hi_void**)&(sample_res_fun.resample_create_callback), sample_res_fun.lib_handle,
        "hi_resample_create");
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "find symbol error!\n");
        goto dysym_fail;
    }

    ret = audio_dlsym((hi_void**)&(sample_res_fun.resample_process_callback), sample_res_fun.lib_handle,
        "hi_resample_process");
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "find symbol error!\n");
        goto dysym_fail;
    }

    ret = audio_dlsym((hi_void**)&(sample_res_fun.resample_destroy_callback), sample_res_fun.lib_handle,
        "hi_resample_destroy");
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "find symbol error!\n");
        goto dysym_fail;
    }

    ret = audio_dlsym((hi_void**)&(sample_res_fun.resample_get_max_output_num_callback), sample_res_fun.lib_handle,
        "hi_resample_get_max_output_num");
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "find symbol error!\n");
        goto dysym_fail;
    }
#endif

    (hi_void)memcpy_s(&g_sample_res_fun, sizeof(hi_sample_res_fun), &sample_res_fun, sizeof(hi_sample_res_fun));

    return HI_SUCCESS;

#if !defined(HI_VQE_USE_STATIC_MODULE_REGISTER)
dysym_fail:
    audio_dlclose(sample_res_fun.lib_handle);
    sample_res_fun.lib_handle = HI_NULL;
    return HI_FAILURE;
#endif
}

/* function : open aenc file */
static FILE *sample_audio_open_aenc_file(hi_aenc_chn ae_chn, hi_payload_type type)
{
    FILE *fd = HI_NULL;
    hi_char asz_file_name[FILE_NAME_LEN] = {0};
    hi_s32 ret;

    /* create file for save stream */
#ifdef __huawei_lite__
    ret = snprintf_s(asz_file_name, FILE_NAME_LEN, FILE_NAME_LEN - 1,
        "/sharefs/audio_chn%d.%s", ae_chn, sample_audio_pt2_str(type));
#else
    ret = snprintf_s(asz_file_name, FILE_NAME_LEN, FILE_NAME_LEN - 1,
        "audio_chn%d.%s", ae_chn, sample_audio_pt2_str(type));
#endif
    if (ret < 0) {
        printf("[func]:%s [line]:%d [info]:%s\n", __FUNCTION__, __LINE__, "get aenc file name failed");
        return HI_NULL;
    }

    if (asz_file_name[0] == '\0') {
        printf("[func]:%s [line]:%d [info]:%s\n", __FUNCTION__, __LINE__, "aenc file name is NULL");
        return HI_NULL;
    }

    if (strlen(asz_file_name) > (FILE_NAME_LEN - 1)) {
        printf("[func]:%s [line]:%d [info]:%s\n", __FUNCTION__, __LINE__, "aenc file name extra long");
        return HI_NULL;
    }

    fd = fopen(asz_file_name, "w+");
    if (fd == NULL) {
        printf("%s: open file %s failed\n", __FUNCTION__, asz_file_name);
        return NULL;
    }
    printf("open stream file:\"%s\" for aenc ok\n", asz_file_name);
    return fd;
}

/* function : Open ExtResample File */
static FILE *sample_audio_open_res_file(hi_ai_chn ai_chn)
{
    FILE *fd = HI_NULL;
    hi_char asz_file_name[FILE_NAME_LEN] = {0};
    hi_s32 ret;

    /* create file for save stream */
#ifdef __HuaweiLite__
    ret = snprintf_s(asz_file_name, FILE_NAME_LEN, FILE_NAME_LEN - 1, "/sharefs/res_chn%d.pcm", ai_chn);
#else
    ret = snprintf_s(asz_file_name, FILE_NAME_LEN, FILE_NAME_LEN - 1, "res_chn%d.pcm", ai_chn);
#endif
    if (ret < 0) {
        printf("[func]:%s [line]:%d [info]:%s\n", __FUNCTION__, __LINE__, "get res file name failed");
        return HI_NULL;
    }

    if (asz_file_name[0] == '\0') {
        printf("[func]:%s [line]:%d [info]:%s\n", __FUNCTION__, __LINE__, "res file name is NULL");
        return HI_NULL;
    }

    if (strlen(asz_file_name) > (FILE_NAME_LEN - 1)) {
        printf("[func]:%s [line]:%d [info]:%s\n", __FUNCTION__, __LINE__, "res file name extra long");
        return HI_NULL;
    }

    fd = fopen(asz_file_name, "w+");
    if (fd == HI_NULL) {
        printf("%s: open file %s failed\n", __FUNCTION__, asz_file_name);
        return HI_NULL;
    }
    printf("open stream file:\"%s\" for resample ok\n", asz_file_name);
    return fd;
}

/* function : open adec file */
static FILE *sample_audio_open_adec_file(hi_adec_chn ad_chn, hi_payload_type type)
{
    FILE *fd = HI_NULL;
    hi_char asz_file_name[FILE_NAME_LEN] = {0};
    hi_s32 ret;
    hi_char path[PATH_MAX] = {0};

    /* create file for save stream */
#ifdef __huawei_lite__
    ret = snprintf_s(asz_file_name, FILE_NAME_LEN, FILE_NAME_LEN - 1,
        "/sharefs/audio_chn%d.%s", ad_chn, sample_audio_pt2_str(type));
#else
    ret = snprintf_s(asz_file_name, FILE_NAME_LEN, FILE_NAME_LEN - 1,
        "audio_chn%d.%s", ad_chn, sample_audio_pt2_str(type));
#endif
    if (ret < 0) {
        printf("[func]:%s [line]:%d [info]:%s\n", __FUNCTION__, __LINE__, "get adec file name failed");
        return HI_NULL;
    }

    if (asz_file_name[0] == '\0') {
        printf("[func]:%s [line]:%d [info]:%s\n", __FUNCTION__, __LINE__, "adec file name is NULL");
        return HI_NULL;
    }

    if (strlen(asz_file_name) > (FILE_NAME_LEN - 1)) {
        printf("[func]:%s [line]:%d [info]:%s\n", __FUNCTION__, __LINE__, "adec file name extra long");
        return HI_NULL;
    }

    if (realpath(asz_file_name, path) == HI_NULL) {
        printf("[func]:%s [line]:%d [info]:%s\n", __FUNCTION__, __LINE__, "adec file name realpath fail");
        return HI_NULL;
    }

    fd = fopen(path, "rb");
    if (fd == NULL) {
        printf("%s: open file %s failed\n", __FUNCTION__, asz_file_name);
        return NULL;
    }
    printf("open stream file:\"%s\" for adec ok\n", asz_file_name);
    return fd;
}

static hi_void sample_audio_set_ai_vqe_param(sample_comm_ai_vqe_param *ai_vqe_param,
    hi_audio_sample_rate out_sample_rate, hi_bool resample_en, hi_void *ai_vqe_attr, hi_u32 ai_vqe_type)
{
    ai_vqe_param->out_sample_rate = out_sample_rate;
    ai_vqe_param->resample_en = resample_en;
    ai_vqe_param->ai_vqe_attr = ai_vqe_attr;
    ai_vqe_param->ai_vqe_type = ai_vqe_type;
}

static hi_void sample_audio_adec_ao_init_param(hi_aio_attr *aio_attr, hi_audio_dev *ao_dev)
{
    aio_attr->sample_rate  = HI_AUDIO_SAMPLE_RATE_16000;
    aio_attr->bit_width    = HI_AUDIO_BIT_WIDTH_16;
    aio_attr->work_mode    = HI_AIO_MODE_I2S_MASTER;
    aio_attr->snd_mode     = HI_AUDIO_SOUND_MODE_MONO;
    aio_attr->expand_flag  = 0;
    aio_attr->frame_num    = 30; /* 30:frame num */
    if (g_payload_type == HI_PT_AAC) {
        aio_attr->point_num_per_frame = AACLC_SAMPLES_PER_FRAME;
    } else {
        aio_attr->point_num_per_frame = SAMPLE_AUDIO_POINT_NUM_PER_FRAME;
    }
    aio_attr->chn_cnt      = 1;

    *ao_dev = SAMPLE_AUDIO_EXTERN_AO_DEV;
    aio_attr->clk_share  = 1;
    aio_attr->i2s_type   = HI_AIO_I2STYPE_EXTERN;

    g_aio_resample = HI_FALSE;
    g_in_sample_rate  = HI_AUDIO_SAMPLE_RATE_BUTT;
    g_out_sample_rate = HI_AUDIO_SAMPLE_RATE_BUTT;
}

hi_void sample_audio_adec_ao_inner(hi_audio_dev ao_dev, hi_ao_chn ao_chn, hi_adec_chn ad_chn)
{
    hi_s32 ret;
    FILE *fd = NULL;

    ret = sample_comm_audio_ao_bind_adec(ao_dev, ao_chn, ad_chn);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        return;
    }

    fd = sample_audio_open_adec_file(ad_chn, g_payload_type);
    if (fd == HI_NULL) {
        SAMPLE_DBG(HI_FAILURE);
        goto adec_ao_err0;
    }

    ret = sample_comm_audio_creat_trd_file_adec(ad_chn, fd);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        fclose(fd);
        fd = HI_NULL;
        goto adec_ao_err0;
    }

    printf("bind adec:%d to ao(%d,%d) ok \n", ad_chn, ao_dev, ao_chn);

    printf("\nplease press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    ret = sample_comm_audio_destory_trd_file_adec(ad_chn);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
    }

adec_ao_err0:
    ret = sample_comm_audio_ao_unbind_adec(ao_dev, ao_chn, ad_chn);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
    }

    return;
}

/* function : file -> adec -> ao */
hi_s32 sample_audio_adec_ao(hi_void)
{
    hi_s32         ret;
    hi_audio_dev   ao_dev;
    hi_ao_chn      ao_chn = 0;
    hi_adec_chn    ad_chn = 0;
    hi_u32         ao_chn_cnt;
    hi_u32         adec_chn_cnt;
    hi_aio_attr    aio_attr;

    sample_audio_adec_ao_init_param(&aio_attr, &ao_dev);

    adec_chn_cnt = aio_attr.chn_cnt >> ((hi_u32)aio_attr.snd_mode);
    ret = sample_comm_audio_start_adec(adec_chn_cnt, g_payload_type);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        goto adec_ao_err3;
    }

    ao_chn_cnt = aio_attr.chn_cnt;
    ret = sample_comm_audio_start_ao(ao_dev, ao_chn_cnt, &aio_attr, g_in_sample_rate, g_aio_resample);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        goto adec_ao_err2;
    }

    ret = sample_comm_audio_cfg_acodec(&aio_attr);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        goto adec_ao_err1;
    }

    sample_audio_adec_ao_inner(ao_dev, ao_chn, ad_chn);

adec_ao_err1:
    ret = sample_comm_audio_stop_ao(ao_dev, ao_chn_cnt, g_aio_resample);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
    }

adec_ao_err2:
    ret = sample_comm_audio_stop_adec(ad_chn);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
    }

adec_ao_err3:
    return ret;
}

static hi_void sample_audio_ai_aenc_init_param(hi_aio_attr *aio_attr, hi_audio_dev *ai_dev, hi_audio_dev *ao_dev)
{
    aio_attr->sample_rate  = HI_AUDIO_SAMPLE_RATE_16000;
    aio_attr->bit_width    = HI_AUDIO_BIT_WIDTH_16;
    aio_attr->work_mode    = HI_AIO_MODE_I2S_MASTER;
    aio_attr->snd_mode     = HI_AUDIO_SOUND_MODE_MONO;
    aio_attr->expand_flag  = 0;
    aio_attr->frame_num    = 30; /* 30:frame num */
    if (g_payload_type == HI_PT_AAC) {
        aio_attr->point_num_per_frame = AACLC_SAMPLES_PER_FRAME;
    } else {
        aio_attr->point_num_per_frame = SAMPLE_AUDIO_POINT_NUM_PER_FRAME;
    }
    aio_attr->chn_cnt      = 1;

    *ai_dev = SAMPLE_AUDIO_EXTERN_AI_DEV;
    *ao_dev = SAMPLE_AUDIO_EXTERN_AO_DEV;
    aio_attr->clk_share  = 1;
    aio_attr->i2s_type   = HI_AIO_I2STYPE_EXTERN;

    g_aio_resample = HI_FALSE;
    g_in_sample_rate  = HI_AUDIO_SAMPLE_RATE_BUTT;
    g_out_sample_rate = HI_AUDIO_SAMPLE_RATE_BUTT;
}

static hi_s32 sample_audio_aenc_bind_ai(hi_audio_dev ai_dev, hi_u32 aenc_chn_cnt)
{
    hi_s32 i, j, ret;
    hi_ai_chn ai_chn;
    hi_aenc_chn ae_chn;

    for (i = 0; i < aenc_chn_cnt; i++) {
        ae_chn = i;
        ai_chn = i;

        if (g_user_get_mode == HI_TRUE) {
            ret = sample_comm_audio_creat_trd_ai_aenc(ai_dev, ai_chn, ae_chn);
        } else {
            ret = sample_comm_audio_aenc_bind_ai(ai_dev, ai_chn, ae_chn);
        }
        if (ret != HI_SUCCESS) {
            SAMPLE_DBG(ret);
            goto aenc_bind_err;
        }
        printf("ai(%d,%d) bind to aenc_chn:%d ok!\n", ai_dev, ai_chn, ae_chn);
    }
    return HI_SUCCESS;

aenc_bind_err:
    for (j = 0; j < i; j++) {
        ae_chn = j;
        ai_chn = j;

        if (g_user_get_mode == HI_TRUE) {
            sample_comm_audio_destory_trd_ai(ai_dev, ai_chn);
        } else {
            sample_comm_audio_aenc_unbind_ai(ai_dev, ai_chn, ae_chn);
        }
    }
    return HI_FAILURE;
}

static hi_void sample_audio_aenc_unbind_ai(hi_audio_dev ai_dev, hi_u32 aenc_chn_cnt)
{
    hi_s32 i, ret;
    hi_ai_chn ai_chn;
    hi_aenc_chn ae_chn;

    for (i = 0; i < aenc_chn_cnt; i++) {
        ae_chn = i;
        ai_chn = i;

        if (g_user_get_mode == HI_TRUE) {
            ret = sample_comm_audio_destory_trd_ai(ai_dev, ai_chn);
            if (ret != HI_SUCCESS) {
                SAMPLE_DBG(ret);
            }
        } else {
            ret = sample_comm_audio_aenc_unbind_ai(ai_dev, ai_chn, ae_chn);
            if (ret != HI_SUCCESS) {
                SAMPLE_DBG(ret);
            }
        }
    }
}

static hi_s32 sample_audio_adec_bind_aenc(hi_u32 adec_chn_cnt)
{
    hi_s32 i, j, ret;
    hi_aenc_chn ae_chn;
    hi_adec_chn ad_chn;
    FILE *fd = NULL;

    for (i = 0; i < adec_chn_cnt; i++) {
        ae_chn = i;
        ad_chn = i;

        fd = sample_audio_open_aenc_file(ad_chn, g_payload_type);
        if (fd == HI_NULL) {
            SAMPLE_DBG(HI_FAILURE);
            for (j = 0; j < i; j++) {
                sample_comm_audio_destory_trd_aenc_adec(j);
            }
            return HI_FAILURE;
        }

        ret = sample_comm_audio_creat_trd_aenc_adec(ae_chn, ad_chn, fd);
        if (ret != HI_SUCCESS) {
            SAMPLE_DBG(ret);
            fclose(fd);
            fd = HI_NULL;
            for (j = 0; j < i; j++) {
                sample_comm_audio_destory_trd_aenc_adec(j);
            }
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_void sample_audio_adec_unbind_aenc(hi_u32 adec_chn_cnt)
{
    hi_s32 i, ret;
    hi_adec_chn ad_chn;

    for (i = 0; i < adec_chn_cnt; i++) {
        ad_chn = i;
        ret = sample_comm_audio_destory_trd_aenc_adec(ad_chn);
        if (ret != HI_SUCCESS) {
            SAMPLE_DBG(ret);
        }
    }
}

static hi_s32 sample_audio_ao_bind_adec(hi_u32 adec_chn_cnt, hi_audio_dev ao_dev)
{
    hi_s32 i, j, ret;
    hi_ao_chn ao_chn;
    hi_adec_chn ad_chn;

    for (i = 0; i < adec_chn_cnt; i++) {
        ao_chn = i;
        ad_chn = i;
        ret = sample_comm_audio_ao_bind_adec(ao_dev, ao_chn, ad_chn);
        if (ret != HI_SUCCESS) {
            SAMPLE_DBG(ret);
            for (j = 0; j < i; j++) {
                sample_comm_audio_ao_unbind_adec(ao_dev, j, j);
            }
            return HI_FAILURE;
        }

        printf("bind adec:%d to ao(%d,%d) ok \n", ad_chn, ao_dev, ao_chn);
    }

    return HI_SUCCESS;
}

static hi_void sample_audio_ao_unbind_adec(hi_u32 adec_chn_cnt, hi_audio_dev ao_dev)
{
    hi_s32 i, ret;
    hi_ao_chn ao_chn;
    hi_adec_chn ad_chn;

    for (i = 0; i < adec_chn_cnt; i++) {
        ao_chn = i;
        ad_chn = i;
        ret = sample_comm_audio_ao_unbind_adec(ao_dev, ao_chn, ad_chn);
        if (ret != HI_SUCCESS) {
            SAMPLE_DBG(ret);
        }
    }
}

static hi_void sample_audio_ai_aenc_inner(hi_bool send_adec, hi_aio_attr *aio_attr, hi_audio_dev ao_dev)
{
    hi_s32 i, ret;
    hi_adec_chn ad_chn;
    hi_u32 ao_chn_cnt;
    hi_u32 adec_chn_cnt;

    /* step 5: start adec & ao. ( if you want ) */
    if (send_adec == HI_TRUE) {
        adec_chn_cnt = aio_attr->chn_cnt >> ((hi_u32)aio_attr->snd_mode);
        ret = sample_comm_audio_start_adec(adec_chn_cnt, g_payload_type);
        if (ret != HI_SUCCESS) {
            SAMPLE_DBG(ret);
            return;
        }

        ao_chn_cnt = aio_attr->chn_cnt;
        ret = sample_comm_audio_start_ao(ao_dev, ao_chn_cnt, aio_attr, g_in_sample_rate, g_aio_resample);
        if (ret != HI_SUCCESS) {
            SAMPLE_DBG(ret);
            goto ai_aenc_err2;
        }

        ret = sample_audio_adec_bind_aenc(adec_chn_cnt);
        if (ret != HI_SUCCESS) {
            goto ai_aenc_err1;
        }

        ret = sample_audio_ao_bind_adec(adec_chn_cnt, ao_dev);
        if (ret != HI_SUCCESS) {
            goto ai_aenc_err0;
        }
    }

    printf("\nplease press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    /* step 6: exit the process */
    if (send_adec == HI_TRUE) {
        sample_audio_ao_unbind_adec(adec_chn_cnt, ao_dev);

ai_aenc_err0:
        sample_audio_adec_unbind_aenc(adec_chn_cnt);

ai_aenc_err1:
        ret = sample_comm_audio_stop_ao(ao_dev, ao_chn_cnt, g_aio_resample);
        if (ret != HI_SUCCESS) {
            SAMPLE_DBG(ret);
        }

ai_aenc_err2:
        for (i = 0; i < adec_chn_cnt; i++) {
            ad_chn = i;
            ret = sample_comm_audio_stop_adec(ad_chn);
            if (ret != HI_SUCCESS) {
                SAMPLE_DBG(ret);
            }
        }
    }
}

/*
 * function : ai -> aenc -> file
 *                       -> adec -> ao
 */
hi_s32 sample_audio_ai_aenc(hi_void)
{
    hi_s32 ret;
    hi_audio_dev ai_dev;
    hi_audio_dev ao_dev;
    hi_u32 ai_chn_cnt;
    hi_u32 aenc_chn_cnt;
    hi_bool send_adec = HI_TRUE;
    hi_aio_attr aio_attr = {0};
    sample_comm_ai_vqe_param ai_vqe_param = {0};

    sample_audio_ai_aenc_init_param(&aio_attr, &ai_dev, &ao_dev);

    /* step 1: start ai */
    ai_chn_cnt = aio_attr.chn_cnt;
    sample_audio_set_ai_vqe_param(&ai_vqe_param, g_out_sample_rate, g_aio_resample, HI_NULL, 0);
    ret = sample_comm_audio_start_ai(ai_dev, ai_chn_cnt, &aio_attr, &ai_vqe_param, -1);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        goto ai_aenc_err6;
    }

    /* step 2: config audio codec */
    ret = sample_comm_audio_cfg_acodec(&aio_attr);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        goto ai_aenc_err5;
    }

    /* step 3: start aenc */
    aenc_chn_cnt = aio_attr.chn_cnt >> ((hi_u32)aio_attr.snd_mode);
    ret = sample_comm_audio_start_aenc(aenc_chn_cnt, &aio_attr, g_payload_type);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        goto ai_aenc_err5;
    }

    /* step 4: aenc bind ai chn */
    ret = sample_audio_aenc_bind_ai(ai_dev, aenc_chn_cnt);
    if (ret != HI_SUCCESS) {
        goto ai_aenc_err4;
    }

    sample_audio_ai_aenc_inner(send_adec, &aio_attr, ao_dev);

    sample_audio_aenc_unbind_ai(ai_dev, aenc_chn_cnt);

ai_aenc_err4:
    ret = sample_comm_audio_stop_aenc(aenc_chn_cnt);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
    }

ai_aenc_err5:
    ret = sample_comm_audio_stop_ai(ai_dev, ai_chn_cnt, g_aio_resample, HI_FALSE);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
    }

ai_aenc_err6:
    return ret;
}

static hi_void sample_audio_ai_to_ext_resample_init_param(hi_aio_attr *aio_attr, hi_audio_dev *ai_dev)
{
    aio_attr->sample_rate  = HI_AUDIO_SAMPLE_RATE_16000;
    aio_attr->bit_width    = HI_AUDIO_BIT_WIDTH_16;
    aio_attr->work_mode    = HI_AIO_MODE_I2S_MASTER;
    aio_attr->snd_mode     = HI_AUDIO_SOUND_MODE_MONO;
    aio_attr->expand_flag  = 0;
    aio_attr->frame_num    = 30; /* 30:frame num */
    aio_attr->point_num_per_frame = SAMPLE_AUDIO_POINT_NUM_PER_FRAME;
    aio_attr->chn_cnt      = 1;

    *ai_dev = SAMPLE_AUDIO_EXTERN_AI_DEV;
    aio_attr->clk_share  = 1;
    aio_attr->i2s_type   = HI_AIO_I2STYPE_EXTERN;

    g_aio_resample = HI_FALSE;
    g_out_sample_rate = HI_AUDIO_SAMPLE_RATE_24000;
}

static hi_s32 sample_audio_start_ai_resample(hi_u32 ai_chn_cnt, hi_audio_dev ai_dev, hi_aio_attr *aio_attr)
{
    hi_s32 i, j, ret;
    hi_ai_chn ai_chn;
    FILE *fd = HI_NULL;

    for (i = 0; i < ai_chn_cnt; i++) {
        ai_chn = i;

        fd = sample_audio_open_res_file(ai_chn);
        if (fd == HI_NULL) {
            SAMPLE_DBG(HI_FAILURE);
            for (j = 0; j < i; j++) {
                sample_comm_audio_destroy_trd_ai_ext_res(ai_dev, j);
            }
            return HI_FAILURE;
        }

        ret = sample_comm_audio_creat_trd_ai_ext_res(aio_attr, ai_dev, ai_chn, g_out_sample_rate, fd);
        if (ret != HI_SUCCESS) {
            SAMPLE_DBG(ret);
            fclose(fd);
            fd = HI_NULL;
            for (j = 0; j < i; j++) {
                sample_comm_audio_destroy_trd_ai_ext_res(ai_dev, j);
            }
            return HI_FAILURE;
        }

        printf("ai(%d,%d) extern resample ok!\n", ai_dev, ai_chn);
    }

    return HI_SUCCESS;
}

static hi_void sample_audio_stop_ai_resample(hi_u32 ai_chn_cnt, hi_audio_dev ai_dev)
{
    hi_s32 i, ret;
    hi_ai_chn ai_chn;

    for (i = 0; i < ai_chn_cnt; i++) {
        ai_chn = i;
        ret = sample_comm_audio_destroy_trd_ai_ext_res(ai_dev, ai_chn);
        if (ret != HI_SUCCESS) {
            SAMPLE_DBG(ret);
        }
    }
}

/* function : Ai -> ExtResample -> file */
hi_s32 sample_audio_ai_to_ext_resample(hi_void)
{
    hi_s32 ret;
    hi_audio_dev ai_dev;
    hi_u32 ai_chn_cnt;
    hi_aio_attr aio_attr = {0};
    sample_comm_ai_vqe_param ai_vqe_param = {0};

    sample_audio_ai_to_ext_resample_init_param(&aio_attr, &ai_dev);

    /* step 1: start Ai, disable inner resample. */
    ai_chn_cnt = aio_attr.chn_cnt;
    sample_audio_set_ai_vqe_param(&ai_vqe_param, g_out_sample_rate, g_aio_resample, HI_NULL, 0);
    ret = sample_comm_audio_start_ai(ai_dev, ai_chn_cnt, &aio_attr, &ai_vqe_param, -1);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        goto ai_res_err3;
    }

    /* step 2: config audio codec */
    ret = sample_comm_audio_cfg_acodec(&aio_attr);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        goto ai_res_err2;
    }

    /* step 3: enable extern resample. */
    ret = sample_audio_init_ext_res_fun();
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        goto ai_res_err2;
    }

    /* step 4: Create thread to resample. */
    ret = sample_audio_start_ai_resample(ai_chn_cnt, ai_dev, &aio_attr);
    if (ret != HI_SUCCESS) {
        goto ai_res_err1;
    }

    printf("\nplease press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    /* step 5: exit the process */
    sample_audio_stop_ai_resample(ai_chn_cnt, ai_dev);

ai_res_err1:
    sample_audio_deinit_ext_res_fun();

ai_res_err2:
    ret = sample_comm_audio_stop_ai(ai_dev, ai_chn_cnt, g_aio_resample, HI_FALSE);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
    }

ai_res_err3:
    return ret;
}

static hi_void sample_audio_ai_ao_init_param(hi_aio_attr *aio_attr, hi_audio_dev *ai_dev, hi_audio_dev *ao_dev)
{
    aio_attr->sample_rate   = HI_AUDIO_SAMPLE_RATE_16000;
    aio_attr->bit_width     = HI_AUDIO_BIT_WIDTH_16;
    aio_attr->work_mode     = HI_AIO_MODE_I2S_MASTER;
    aio_attr->snd_mode      = HI_AUDIO_SOUND_MODE_STEREO;
    aio_attr->expand_flag   = 0;
    aio_attr->frame_num     = 30; /* 30:frame num */
    aio_attr->point_num_per_frame = AACLC_SAMPLES_PER_FRAME;
    aio_attr->chn_cnt       = 2; /* 2:chn cnt */

    *ai_dev = SAMPLE_AUDIO_EXTERN_AI_DEV;
    *ao_dev = SAMPLE_AUDIO_EXTERN_AO_DEV;
    aio_attr->clk_share  = 1;
    aio_attr->i2s_type   = HI_AIO_I2STYPE_EXTERN;

    g_aio_resample = HI_FALSE;
    /* config ao resample attr if needed */
    if (g_aio_resample == HI_TRUE) {
        /* ai 48k -> 32k */
        g_out_sample_rate = HI_AUDIO_SAMPLE_RATE_32000;

        /* ao 32k -> 48k */
        g_in_sample_rate  = HI_AUDIO_SAMPLE_RATE_32000;
    } else {
        g_in_sample_rate  = HI_AUDIO_SAMPLE_RATE_BUTT;
        g_out_sample_rate = HI_AUDIO_SAMPLE_RATE_BUTT;
    }

    /* resample and anr should be user get mode */
    g_user_get_mode = (g_aio_resample == HI_TRUE) ? HI_TRUE : HI_FALSE;
}

static hi_s32 sample_audio_ao_bind_ai(hi_audio_dev ai_dev, hi_ai_chn ai_chn,
    hi_audio_dev ao_dev, hi_ao_chn ao_chn)
{
    hi_s32 ret;
    if (g_user_get_mode == HI_TRUE) {
        ret = sample_comm_audio_creat_trd_ai_ao(ai_dev, ai_chn, ao_dev, ao_chn);
        if (ret != HI_SUCCESS) {
            SAMPLE_DBG(ret);
            return HI_FAILURE;
        }
    } else {
        ret = sample_comm_audio_ao_bind_ai(ai_dev, ai_chn, ao_dev, ao_chn);
        if (ret != HI_SUCCESS) {
            SAMPLE_DBG(ret);
            return HI_FAILURE;
        }
    }

    printf("ai(%d,%d) bind to ao(%d,%d) ok\n", ai_dev, ai_chn, ao_dev, ao_chn);
    return HI_SUCCESS;
}

static hi_void sample_audio_ao_unbind_ai(hi_audio_dev ai_dev, hi_ai_chn ai_chn,
    hi_audio_dev ao_dev, hi_ao_chn ao_chn)
{
    hi_s32 ret;
    if (g_user_get_mode == HI_TRUE) {
        ret = sample_comm_audio_destory_trd_ai(ai_dev, ai_chn);
        if (ret != HI_SUCCESS) {
            SAMPLE_DBG(ret);
        }
    } else {
        ret = sample_comm_audio_ao_unbind_ai(ai_dev, ai_chn, ao_dev, ao_chn);
        if (ret != HI_SUCCESS) {
            SAMPLE_DBG(ret);
        }
    }
}

static hi_void sample_audio_ai_ao_inner(hi_audio_dev ai_dev, hi_ai_chn ai_chn,
    hi_audio_dev ao_dev, hi_ao_chn ao_chn)
{
    hi_s32 ret;

    /* bind AI to AO channle */
    ret = sample_audio_ao_bind_ai(ai_dev, ai_chn, ao_dev, ao_chn);
    if (ret != HI_SUCCESS) {
        return;
    }

    if (g_ao_volume_ctrl == HI_TRUE) {
        ret = sample_comm_audio_creat_trd_ao_vol_ctrl(ao_dev);
        if (ret != HI_SUCCESS) {
            SAMPLE_DBG(ret);
            goto ai_ao_err0;
        }
    }

    printf("\nplease press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    if (g_ao_volume_ctrl == HI_TRUE) {
        ret = sample_comm_audio_destory_trd_ao_vol_ctrl(ao_dev);
        if (ret != HI_SUCCESS) {
            SAMPLE_DBG(ret);
        }
    }

ai_ao_err0:
    sample_audio_ao_unbind_ai(ai_dev, ai_chn, ao_dev, ao_chn);
}

/* function : ai -> ao(with fade in/out and volume adjust) */
hi_s32 sample_audio_ai_ao(hi_void)
{
    hi_s32 ret;
    hi_u32 ai_chn_cnt;
    hi_u32 ao_chn_cnt;
    hi_audio_dev ai_dev;
    hi_audio_dev ao_dev;
    hi_ai_chn ai_chn = 0;
    hi_ao_chn ao_chn = 0;
    hi_aio_attr aio_attr = {0};
    sample_comm_ai_vqe_param ai_vqe_param = {0};

    sample_audio_ai_ao_init_param(&aio_attr, &ai_dev, &ao_dev);

    /* enable AI channle */
    ai_chn_cnt = aio_attr.chn_cnt;
    sample_audio_set_ai_vqe_param(&ai_vqe_param, g_out_sample_rate, g_aio_resample, HI_NULL, 0);
    ret = sample_comm_audio_start_ai(ai_dev, ai_chn_cnt, &aio_attr, &ai_vqe_param, -1);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        goto ai_ao_err3;
    }

    /* enable AO channle */
    ao_chn_cnt = aio_attr.chn_cnt;
    ret = sample_comm_audio_start_ao(ao_dev, ao_chn_cnt, &aio_attr, g_in_sample_rate, g_aio_resample);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        goto ai_ao_err2;
    }

    /* config internal audio codec */
    ret = sample_comm_audio_cfg_acodec(&aio_attr);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        goto ai_ao_err1;
    }

    sample_audio_ai_ao_inner(ai_dev, ai_chn, ao_dev, ao_chn);

ai_ao_err1:
    ret = sample_comm_audio_stop_ao(ao_dev, ao_chn_cnt, g_aio_resample);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
    }

ai_ao_err2:
    ret = sample_comm_audio_stop_ai(ai_dev, ai_chn_cnt, g_aio_resample, HI_FALSE);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
    }

ai_ao_err3:
    return ret;
}

static hi_void sample_audio_ai_to_ao_sys_chn_init_param(hi_aio_attr *aio_attr, hi_audio_dev *ai_dev,
    hi_audio_dev *ao_dev)
{
    aio_attr->sample_rate  = HI_AUDIO_SAMPLE_RATE_16000;
    aio_attr->bit_width    = HI_AUDIO_BIT_WIDTH_16;
    aio_attr->work_mode    = HI_AIO_MODE_I2S_MASTER;
    aio_attr->snd_mode     = HI_AUDIO_SOUND_MODE_MONO;
    aio_attr->expand_flag  = 0;
    aio_attr->frame_num    = 30; /* 30:frame num */
    aio_attr->point_num_per_frame = AACLC_SAMPLES_PER_FRAME;
    aio_attr->chn_cnt      = 1;

    *ai_dev = SAMPLE_AUDIO_EXTERN_AI_DEV;
    *ao_dev = SAMPLE_AUDIO_EXTERN_AO_DEV;
    aio_attr->clk_share  = 1;
    aio_attr->i2s_type   = HI_AIO_I2STYPE_EXTERN;

    g_aio_resample = HI_FALSE;
    /* config ao resample attr if needed */
    if (g_aio_resample == HI_TRUE) {
        /* ai 48k -> 32k */
        g_out_sample_rate = HI_AUDIO_SAMPLE_RATE_32000;

        /* ao 32k -> 48k */
        g_in_sample_rate  = HI_AUDIO_SAMPLE_RATE_32000;
    } else {
        g_in_sample_rate  = HI_AUDIO_SAMPLE_RATE_BUTT;
        g_out_sample_rate = HI_AUDIO_SAMPLE_RATE_BUTT;
    }

    /* resample and anr should be user get mode */
    g_user_get_mode = (g_aio_resample == HI_TRUE) ? HI_TRUE : HI_FALSE;
}

static hi_void sample_audio_ai_to_ao_sys_chn_inner(hi_audio_dev ai_dev, hi_ai_chn ai_chn,
    hi_audio_dev ao_dev)
{
    hi_s32 ret;

    /* bind AI to AO channle */
    ret = sample_comm_audio_creat_trd_ai_ao(ai_dev, ai_chn, ao_dev, HI_AO_SYS_CHN_ID);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        return;
    }
    printf("ai(%d,%d) bind to ao(%d,%d) ok\n", ai_dev, ai_chn, ao_dev, HI_AO_SYS_CHN_ID);

    printf("\nplease press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    ret = sample_comm_audio_destory_trd_ai(ai_dev, ai_chn);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
    }
}

/* function : ai -> ao (sys chn) */
hi_s32 sample_audio_ai_to_ao_sys_chn(hi_void)
{
    hi_s32 ret;
    hi_u32 ai_chn_cnt, ao_chn_cnt;
    hi_audio_dev ai_dev, ao_dev;
    hi_ai_chn ai_chn = 0;
    hi_aio_attr aio_attr = {0};
    sample_comm_ai_vqe_param ai_vqe_param = {0};

    sample_audio_ai_to_ao_sys_chn_init_param(&aio_attr, &ai_dev, &ao_dev);

    /* enable AI channle */
    ai_chn_cnt = aio_attr.chn_cnt;
    sample_audio_set_ai_vqe_param(&ai_vqe_param, g_out_sample_rate, g_aio_resample, HI_NULL, 0);
    ret = sample_comm_audio_start_ai(ai_dev, ai_chn_cnt, &aio_attr, &ai_vqe_param, -1);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        goto ai_ao_sys_chn_err3;
    }

    /* enable AO channle */
    ao_chn_cnt = aio_attr.chn_cnt;
    ret = sample_comm_audio_start_ao(ao_dev, ao_chn_cnt, &aio_attr, g_in_sample_rate, g_aio_resample);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        goto ai_ao_sys_chn_err2;
    }

    /* config internal audio codec */
    ret = sample_comm_audio_cfg_acodec(&aio_attr);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        goto ai_ao_sys_chn_err1;
    }

    sample_audio_ai_to_ao_sys_chn_inner(ai_dev, ai_chn, ao_dev);

ai_ao_sys_chn_err1:
    ret = sample_comm_audio_stop_ao(ao_dev, ao_chn_cnt, g_aio_resample);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
    }

ai_ao_sys_chn_err2:
    ret = sample_comm_audio_stop_ai(ai_dev, ai_chn_cnt, g_aio_resample, HI_FALSE);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
    }

ai_ao_sys_chn_err3:
    return ret;
}

static hi_void ai_init_record_vqe_param(hi_ai_record_vqe_cfg *ai_vqe_record_attr)
{
    ai_vqe_record_attr->work_sample_rate = HI_AUDIO_SAMPLE_RATE_48000;
    ai_vqe_record_attr->frame_sample = AACLC_SAMPLES_PER_FRAME;
    ai_vqe_record_attr->work_state = HI_VQE_WORK_STATE_COMMON;
    ai_vqe_record_attr->in_chn_num = 2;  /* 2: chn num */
    ai_vqe_record_attr->out_chn_num = 2; /* 2: chn num */
    ai_vqe_record_attr->record_type = HI_VQE_RECORD_NORMAL;
    ai_vqe_record_attr->drc_cfg.usr_mode = HI_FALSE;
    ai_vqe_record_attr->rnr_cfg.usr_mode = HI_FALSE;
    ai_vqe_record_attr->hdr_cfg.usr_mode = HI_FALSE;
    ai_vqe_record_attr->hpf_cfg.usr_mode = HI_TRUE;
    ai_vqe_record_attr->hpf_cfg.hpf_freq = HI_AUDIO_HPF_FREQ_80;

    ai_vqe_record_attr->open_mask =
        HI_AI_RECORDVQE_MASK_DRC | HI_AI_RECORDVQE_MASK_HPF | HI_AI_RECORDVQE_MASK_RNR;
}

static hi_void ai_init_talk_vqe_param(hi_ai_talk_vqe_cfg *ai_vqe_talk_attr)
{
    ai_vqe_talk_attr->work_sample_rate = HI_AUDIO_SAMPLE_RATE_16000;
    ai_vqe_talk_attr->frame_sample = AACLC_SAMPLES_PER_FRAME;
    ai_vqe_talk_attr->work_state = HI_VQE_WORK_STATE_COMMON;
    ai_vqe_talk_attr->agc_cfg.usr_mode = HI_FALSE;
    ai_vqe_talk_attr->aec_cfg.usr_mode = HI_FALSE;
    ai_vqe_talk_attr->anr_cfg.usr_mode = HI_FALSE;
    ai_vqe_talk_attr->hpf_cfg.usr_mode = HI_TRUE;
    ai_vqe_talk_attr->hpf_cfg.hpf_freq = HI_AUDIO_HPF_FREQ_150;

    ai_vqe_talk_attr->open_mask = HI_AI_TALKVQE_MASK_AGC | HI_AI_TALKVQE_MASK_ANR | HI_AI_TALKVQE_MASK_HPF;
}

static hi_void sample_audio_ai_vqe_process_ao_init_param(hi_aio_attr *aio_attr, hi_audio_dev *ai_dev,
    hi_audio_dev *ao_dev, hi_void **ai_vqe_attr)
{
    aio_attr->sample_rate  = HI_AUDIO_SAMPLE_RATE_16000;
    aio_attr->bit_width    = HI_AUDIO_BIT_WIDTH_16;
    aio_attr->work_mode    = HI_AIO_MODE_I2S_MASTER;
    aio_attr->snd_mode     = HI_AUDIO_SOUND_MODE_MONO;
    aio_attr->expand_flag  = 0;
    aio_attr->frame_num    = 30; /* 30:frame num */
    aio_attr->point_num_per_frame = AACLC_SAMPLES_PER_FRAME;
    aio_attr->chn_cnt      = 1;

    *ai_dev = SAMPLE_AUDIO_EXTERN_AI_DEV;
    *ao_dev = SAMPLE_AUDIO_EXTERN_AO_DEV;
    aio_attr->clk_share  = 1;
    aio_attr->i2s_type   = HI_AIO_I2STYPE_EXTERN;

    g_aio_resample = HI_FALSE;
    g_in_sample_rate  = HI_AUDIO_SAMPLE_RATE_BUTT;
    g_out_sample_rate = HI_AUDIO_SAMPLE_RATE_BUTT;

    if (g_ai_vqe_type == 1) { /* 1:record vqe */
        ai_init_record_vqe_param(&g_ai_vqe_record_attr);
        *ai_vqe_attr = (hi_void *)&g_ai_vqe_record_attr;
    } else if (g_ai_vqe_type == 2) { /* 2:talk vqe */
        ai_init_talk_vqe_param(&g_ai_vqe_talk_attr);
        *ai_vqe_attr = (hi_void *)&g_ai_vqe_talk_attr;
    } else {
        *ai_vqe_attr = HI_NULL;
    }
}

static hi_s32 sample_audio_ao_bind_ai_multi_chn(hi_audio_dev ai_dev, hi_u32 ai_chn_cnt,
    hi_audio_dev ao_dev)
{
    hi_s32 i, j, ret;

    for (i = 0; i < ai_chn_cnt; i++) {
        ret = sample_comm_audio_creat_trd_ai_ao(ai_dev, i, ao_dev, i);
        if (ret != HI_SUCCESS) {
            SAMPLE_DBG(ret);
            for (j = 0; j < i; j++) {
                sample_comm_audio_destory_trd_ai(ai_dev, j);
            }
            return HI_FAILURE;
        }

        printf("bind ai(%d,%d) to ao(%d,%d) ok \n", ai_dev, i, ao_dev, i);
    }

    return HI_SUCCESS;
}

static hi_void sample_audio_ao_unbind_ai_multi_chn(hi_audio_dev ai_dev, hi_u32 ai_chn_cnt,
    hi_audio_dev ao_dev)
{
    hi_s32 i, ret;

    for (i = 0; i < ai_chn_cnt; i++) {
        ret = sample_comm_audio_destory_trd_ai(ai_dev, i);
        if (ret != HI_SUCCESS) {
            SAMPLE_DBG(ret);
        }
    }
}

/* function : ai -> ao (with VQE) */
hi_s32 sample_audio_ai_vqe_process_ao(hi_void)
{
    hi_s32 ret;
    hi_u32 ai_chn_cnt;
    hi_u32 ao_chn_cnt;
    hi_audio_dev ai_dev;
    hi_audio_dev ao_dev;
    hi_aio_attr aio_attr = {0};
    hi_void *ai_vqe_attr = NULL;
    sample_comm_ai_vqe_param ai_vqe_param = {0};

    sample_audio_ai_vqe_process_ao_init_param(&aio_attr, &ai_dev, &ao_dev, &ai_vqe_attr);

    /* step 1: start ai */
    ai_chn_cnt = aio_attr.chn_cnt;
    sample_audio_set_ai_vqe_param(&ai_vqe_param, g_out_sample_rate, g_aio_resample, ai_vqe_attr, g_ai_vqe_type);
    ret = sample_comm_audio_start_ai(ai_dev, ai_chn_cnt, &aio_attr, &ai_vqe_param, ao_dev);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        goto vqe_err2;
    }

    /* step 2: start ao */
    ao_chn_cnt = aio_attr.chn_cnt;
    ret = sample_comm_audio_start_ao(ao_dev, ao_chn_cnt, &aio_attr, g_in_sample_rate, g_aio_resample);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        goto vqe_err1;
    }

    /* step 3: config audio codec */
    ret = sample_comm_audio_cfg_acodec(&aio_attr);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        goto vqe_err0;
    }

    /* step 4: ao bind ai chn */
    ret = sample_audio_ao_bind_ai_multi_chn(ai_dev, ai_chn_cnt, ao_dev);
    if (ret != HI_SUCCESS) {
        goto vqe_err0;
    }

    printf("\nplease press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    /* step 5: exit the process */
    sample_audio_ao_unbind_ai_multi_chn(ai_dev, ai_chn_cnt, ao_dev);

vqe_err0:
    ret = sample_comm_audio_stop_ao(ao_dev, ao_chn_cnt, g_aio_resample);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
    }

vqe_err1:
    ret = sample_comm_audio_stop_ai(ai_dev, ai_chn_cnt, g_aio_resample, HI_TRUE);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
    }

vqe_err2:
    return ret;
}

static hi_void sample_audio_ai_hdmi_ao_init_param(hi_aio_attr *aio_attr, hi_audio_dev *ai_dev,
    hi_aio_attr *hdmi_ao_attr)
{
    aio_attr->sample_rate   = HI_AUDIO_SAMPLE_RATE_16000;
    aio_attr->bit_width     = HI_AUDIO_BIT_WIDTH_16;
    aio_attr->work_mode     = HI_AIO_MODE_I2S_MASTER;
    aio_attr->snd_mode      = HI_AUDIO_SOUND_MODE_MONO;
    aio_attr->expand_flag   = 0;
    aio_attr->frame_num     = 30; /* 30:frame num */
    aio_attr->point_num_per_frame = AACLC_SAMPLES_PER_FRAME;
    aio_attr->chn_cnt       = 2; /* 2:chn cnt */

    *ai_dev = SAMPLE_AUDIO_EXTERN_AI_DEV;
    aio_attr->clk_share  = 1;
    aio_attr->i2s_type   = HI_AIO_I2STYPE_EXTERN;

    hdmi_ao_attr->sample_rate   = HI_AUDIO_SAMPLE_RATE_48000;
    hdmi_ao_attr->bit_width     = HI_AUDIO_BIT_WIDTH_16;
    hdmi_ao_attr->work_mode     = HI_AIO_MODE_I2S_MASTER;
    hdmi_ao_attr->snd_mode      = HI_AUDIO_SOUND_MODE_MONO;
    hdmi_ao_attr->expand_flag   = 0;
    hdmi_ao_attr->frame_num     = 30; /* 30:frame num */
    hdmi_ao_attr->point_num_per_frame = AACLC_SAMPLES_PER_FRAME;
    hdmi_ao_attr->chn_cnt       = 2; /* 2:chn cnt */
    hdmi_ao_attr->clk_share     = 1;
    hdmi_ao_attr->i2s_type      = HI_AIO_I2STYPE_INNERHDMI;

    g_in_sample_rate = aio_attr->sample_rate;
    g_aio_resample = HI_TRUE;
    /* resample should be user get mode */
    g_user_get_mode = (g_aio_resample == HI_TRUE) ? HI_TRUE : HI_FALSE;
}

/* function : ai -> ao(hdmi) */
hi_s32 sample_audio_ai_hdmi_ao(hi_void)
{
    hi_s32 ret;
    hi_u32 ai_chn_cnt;
    hi_u32 ao_chn_cnt;
    hi_audio_dev ai_dev;
    hi_audio_dev ao_dev = SAMPLE_AUDIO_INNER_HDMI_AO_DEV;
    hi_aio_attr aio_attr = {0};
    hi_aio_attr hdmi_ao_attr = {0};
    sample_comm_ai_vqe_param ai_vqe_param = {0};

    sample_audio_ai_hdmi_ao_init_param(&aio_attr, &ai_dev, &hdmi_ao_attr);

    /* enable AI channle */
    ai_chn_cnt = aio_attr.chn_cnt;
    sample_audio_set_ai_vqe_param(&ai_vqe_param, HI_AUDIO_SAMPLE_RATE_BUTT, HI_FALSE, HI_NULL, 0);
    ret = sample_comm_audio_start_ai(ai_dev, ai_chn_cnt, &aio_attr, &ai_vqe_param, -1);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        goto hdmi_err2;
    }

    /* enable AO channle */
    ao_chn_cnt = hdmi_ao_attr.chn_cnt;
    ret = sample_comm_audio_start_ao(ao_dev, ao_chn_cnt, &hdmi_ao_attr, g_in_sample_rate, g_aio_resample);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        goto hdmi_err1;
    }

    /* config audio codec */
    ret = sample_comm_audio_cfg_acodec(&aio_attr);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
        goto hdmi_err0;
    }

    /* AI to AO channel */
    ret = sample_audio_ao_bind_ai_multi_chn(ai_dev, ai_chn_cnt, ao_dev);
    if (ret != HI_SUCCESS) {
        goto hdmi_err0;
    }

    printf("\nplease press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    sample_audio_ao_unbind_ai_multi_chn(ai_dev, ai_chn_cnt, ao_dev);

hdmi_err0:
    ret = sample_comm_audio_stop_ai(ai_dev, ai_chn_cnt, HI_FALSE, HI_FALSE);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
    }

hdmi_err1:
    ret = sample_comm_audio_stop_ao(ao_dev, hdmi_ao_attr.chn_cnt, HI_FALSE);
    if (ret != HI_SUCCESS) {
        SAMPLE_DBG(ret);
    }

hdmi_err2:
    return ret;
}

#if defined(HI_VQE_USE_STATIC_MODULE_REGISTER)
/* function : to register vqe module */
hi_s32 sample_audio_register_vqe_module(hi_void)
{
    hi_s32 ret;
    hi_audio_vqe_register vqe_reg_cfg = {0};

    /* RecordVQE */
    vqe_reg_cfg.record_mod_cfg.handle = hi_vqe_record_get_handle();

    /* Resample */
    vqe_reg_cfg.resample_mod_cfg.handle = hi_vqe_resample_get_handle();

    /* TalkVQE */
    vqe_reg_cfg.hpf_mod_cfg.handle = hi_vqe_hpf_get_handle();
    vqe_reg_cfg.aec_mod_cfg.handle = hi_vqe_aec_get_handle();
    vqe_reg_cfg.agc_mod_cfg.handle = hi_vqe_agc_get_handle();
    vqe_reg_cfg.anr_mod_cfg.handle = hi_vqe_anr_get_handle();
    vqe_reg_cfg.eq_mod_cfg.handle = hi_vqe_eq_get_handle();

    ret = hi_mpi_audio_register_vqe_mod(&vqe_reg_cfg);
    if (ret != HI_SUCCESS) {
        printf("%s: register vqe module fail with ret = %d!\n", __FUNCTION__, ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
#endif

hi_void sample_audio_usage(hi_void)
{
    printf("\n\n/usage:./sample_audio <index>/\n");
    printf("\tindex and its function list below\n");
    printf("\t0:  start AI to AO loop\n");
    printf("\t1:  send audio frame to AENC channel from AI, save them\n");
    printf("\t2:  read audio stream from file, decode and send AO\n");
    printf("\t3:  start AI(VQE process), then send to AO\n");
    printf("\t4:  start AI to AO(HDMI) loop\n");
    printf("\t5:  start AI to AO(sys_chn) loop\n");
    printf("\t6:  start AI, then send to resample, save it\n");
}

/* function : to process abnormal case */
void sample_audio_handle_sig(hi_s32 signo)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    if ((signo == SIGINT) || (signo == SIGTERM)) {
        sample_comm_audio_destory_all_trd();
        sample_comm_sys_exit();
        printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
    }

    exit(0);
}

static hi_void main_inner(hi_u32 index)
{
    switch (index) {
        case 0: { /* 0:ai->ao */
            sample_audio_ai_ao();
            break;
        }
        case 1: { /* 1:ai->aenc->adec->ao */
            sample_audio_ai_aenc();
            break;
        }
        case 2: { /* 2:file->adec->ao */
            sample_audio_adec_ao();
            break;
        }
        case 3: { /* 3:ai->ao vqe */
            sample_audio_ai_vqe_process_ao();
            break;
        }
        case 4: { /* 4:ai->ao hdmi */
            sample_audio_ai_hdmi_ao();
            break;
        }
        case 5: { /* 5:ai->ao synchn */
            sample_audio_ai_to_ao_sys_chn();
            break;
        }
        case 6: { /* 6:resample test */
            sample_audio_ai_to_ext_resample();
            break;
        }
        default: {
            break;
        }
    }
}

/* function : main */
#ifdef __huawei_lite__
hi_s32 app_main(int argc, char *argv[])
#else
hi_s32 sample_audio_main(int argc, char *argv[])
#endif
{
    hi_s32 ret;
    hi_vb_cfg vb_conf;
    hi_u32 index;

    if (argc != 2) {  /* 2:argv num */
        sample_audio_usage();
        return HI_FAILURE;
    }

    index = atoi(argv[1]);
    if (index > 6) {  /* 6:index max num is 6 */
        sample_audio_usage();
        return HI_FAILURE;
    }

    signal(SIGINT, sample_audio_handle_sig);
    signal(SIGTERM, sample_audio_handle_sig);
#if defined(HI_VQE_USE_STATIC_MODULE_REGISTER)
    ret = sample_audio_register_vqe_module();
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
#endif
    ret = memset_s(&vb_conf, sizeof(hi_vb_cfg), 0, sizeof(hi_vb_cfg));
    if (ret != EOK) {
        printf("%s: vb_config init failed with %d!\n", __FUNCTION__, ret);
        return HI_FAILURE;
    }

    ret = sample_comm_sys_init(&vb_conf);
    if (ret != HI_SUCCESS) {
        printf("%s: system init failed with %d!\n", __FUNCTION__, ret);
        return HI_FAILURE;
    }

    hi_mpi_aenc_aac_init();
    hi_mpi_adec_aac_init();

    main_inner(index);

    hi_mpi_aenc_aac_deinit();
    hi_mpi_adec_aac_deinit();

    sample_comm_sys_exit();

    return ret;
}

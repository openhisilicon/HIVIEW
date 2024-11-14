/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
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
#include "hi_audio_aac_adp.h"
#include "hi_audio_mp3_adp.h"
#include "hi_audio_opus_adp.h"
#include "hi_audio_dl_adp.h"
#include "hi_resample.h"
#if defined(OT_VQE_USE_STATIC_MODULE_REGISTER)
#include "hi_vqe_register.h"
#endif

static hi_payload_type g_payload_type = HI_PT_AAC;
static hi_payload_type g_stream_types[] = {HI_PT_G711A, HI_PT_G711U, HI_PT_G726, HI_PT_ADPCMA,
    HI_PT_AAC, HI_PT_MP3, HI_PT_OPUS, HI_PT_LPCM};
static hi_char *g_decode_file_path = HI_NULL;
static hi_bool g_aio_resample  = HI_FALSE;
static hi_bool g_user_get_mode  = HI_FALSE;
static hi_bool g_ao_volume_ctrl = HI_FALSE;
static hi_audio_sample_rate g_in_sample_rate  = HI_AUDIO_SAMPLE_RATE_BUTT;
static hi_audio_sample_rate g_out_sample_rate = HI_AUDIO_SAMPLE_RATE_BUTT;

static sample_audio_vqe_type g_ai_vqe_type = SAMPLE_AUDIO_VQE_TYPE_RECORD;
static hi_char *g_ai_vqe_str[SAMPLE_AUDIO_VQE_TYPE_MAX] = {"none", "record", "talk", "talkv2"};
hi_ai_record_vqe_cfg g_ai_vqe_record_attr = { 0 };
hi_ai_talk_vqe_cfg g_ai_vqe_talk_attr = { 0 };
hi_ai_talk_vqe_v2_cfg g_ai_vqe_talk_v2_attr = { 0 };

static void *g_res_handle = HI_NULL;
static hi_s16 *g_res_out_buf = HI_NULL;

static hi_bool g_sample_audio_exit = HI_FALSE;

#define sample_dbg(ret) \
    do { \
        printf("ret = %#x, fuc:%s, line:%d\n", ret, __FUNCTION__, __LINE__); \
    } while (0)

#define sample_res_check_null_ptr(ptr) \
    do { \
        if ((hi_u8*)(ptr) == HI_NULL) { \
            printf("ptr is HI_NULL,fuc:%s,line:%d\n", __FUNCTION__, __LINE__); \
            return HI_FAILURE; \
        } \
    } while (0)

#define HI_RES_LIB_NAME "libvqe_res.so"

#define SAMPLE_INDEX_0 0
#define SAMPLE_INDEX_1 1
#define SAMPLE_INDEX_2 2
#define SAMPLE_INDEX_3 3
#define SAMPLE_INDEX_4 4
#define SAMPLE_INDEX_5 5

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
    } else if (type == HI_PT_MP3) {
        return "mp3";
    } else if (type == HI_PT_OPUS) {
        return "opus";
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
typedef hi_s32 (*hi_resample_process_callback)(hi_void *instance, const hi_s16 *in_buf, hi_s32 in_samples,
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
    hi_s16 *cache, hi_u16 *cache_count, hi_u16 cache_sample)
{
    hi_u32 proc_frame, out_sample, cur_sample, ret;
    hi_u32 mulit = 1;

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

    (void)fflush(ai_ctrl->fd);
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

    ai_chn_para.usr_frame_depth = SAMPLE_AUDIO_AI_USER_FRAME_DEPTH;

    ret = hi_mpi_ai_set_chn_param(ai_ctrl->ai_dev, ai_ctrl->ai_chn, &ai_chn_para);
    if (ret != HI_SUCCESS) {
        printf("%s: set ai chn param failed, ret=0x%x\n", __FUNCTION__, ret);
        return ret;
    }

    /* Create Resample. only support mono channel. */
    g_res_handle = g_sample_res_fun.resample_create_callback(ai_ctrl->in_sample, ai_ctrl->out_sample, 1);
    if (g_res_handle == HI_NULL) {
        printf("%s: hi_resampler_create failed, ret=0x%x\n", __FUNCTION__, ret);
        return HI_FAILURE;
    }

    out_max_num = g_sample_res_fun.resample_get_max_output_num_callback(g_res_handle, ai_ctrl->per_frame);
    if (out_max_num <= 0) {
        printf("get max output num failed\n");
        g_sample_res_fun.resample_destroy_callback(g_res_handle);
        g_res_handle = HI_NULL;
        return HI_FAILURE;
    }

    g_res_out_buf = malloc(out_max_num * sizeof(hi_s16) + 2); /* 2:Reserved space */
    if (g_res_out_buf == HI_NULL) {
        printf("malloc failed\n");
        g_sample_res_fun.resample_destroy_callback(g_res_handle);
        g_res_handle = HI_NULL;
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
    hi_u16 cache_count = 0;
    hi_s16 cache[16]; /* 16:Max 64 / 8 * 2. */

    ret = sample_audio_ai_to_res_init(ai_ctrl);
    if (ret != HI_SUCCESS) {
        printf("%s: sample_audio_ai_to_res_param_set failed\n", __FUNCTION__);
        return HI_NULL;
    }

    ai_fd = hi_mpi_ai_get_fd(ai_ctrl->ai_dev, ai_ctrl->ai_chn);
    if (ai_fd < 0) {
        printf("%s: get ai fd failed\n", __FUNCTION__);
        return HI_NULL;
    }

    while (ai_ctrl->start) {
        ret = sample_comm_audio_select(ai_fd, &read_fds);
        if (ret != HI_SUCCESS) {
            break;
        }

        if (FD_ISSET(ai_fd, &read_fds)) {
            /* get frame from ai chn */
            (hi_void)memset_s(&aec_frm, sizeof(hi_aec_frame), 0, sizeof(hi_aec_frame));

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
    g_res_handle = HI_NULL;

    return HI_NULL;
}

/* function : Create the thread to get frame from ai and send to aenc */
hi_s32 sample_comm_audio_create_thread_ai_ext_res(hi_aio_attr *aio_attr, hi_audio_dev ai_dev,
    hi_ai_chn ai_chn, hi_audio_sample_rate out_sample_rate, FILE *res_fd)
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
    ai_to_ext_res->out_sample = out_sample_rate;
    ai_to_ext_res->per_frame = aio_attr->point_num_per_frame;
    ai_to_ext_res->fd = res_fd;
    ai_to_ext_res->start = HI_TRUE;
    pthread_create(&ai_to_ext_res->ai_pid, 0, sample_comm_audio_ai_ext_res_proc, ai_to_ext_res);

    return HI_SUCCESS;
}

/* function : Destroy the thread to get frame from ai and send to extern resamler */
hi_s32 sample_comm_audio_destroy_thread_ai_ext_res(hi_audio_dev ai_dev, hi_ai_chn ai_chn)
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

#if defined(OT_VQE_USE_STATIC_MODULE_REGISTER)
    ret = memset_s(&g_sample_res_fun, sizeof(hi_sample_res_fun), 0, sizeof(hi_sample_res_fun));
    if (ret != EOK) {
        printf("%s: g_sample_res_fun init failed with %d!\n", __FUNCTION__, ret);
        return;
    }
#else
    if (g_sample_res_fun.lib_handle != HI_NULL) {
        hi_audio_dlclose(g_sample_res_fun.lib_handle);
        ret = memset_s(&g_sample_res_fun, sizeof(hi_sample_res_fun), 0, sizeof(hi_sample_res_fun));
        if (ret != EOK) {
            printf("%s: g_sample_res_fun init failed with %d!\n", __FUNCTION__, ret);
            return;
        }
    }
#endif
}

/* function : Init resamle functions */
static hi_s32 sample_audio_init_ext_res_fun(hi_void)
{
    hi_sample_res_fun sample_res_fun = {0};

    sample_audio_deinit_ext_res_fun();

#if defined(OT_VQE_USE_STATIC_MODULE_REGISTER)
    sample_res_fun.resample_create_callback = hi_resample_create;
    sample_res_fun.resample_process_callback = hi_resample_process;
    sample_res_fun.resample_destroy_callback = hi_resample_destroy;
    sample_res_fun.resample_get_max_output_num_callback = hi_resample_get_max_output_num;
#else
    hi_s32 ret;
    ret = hi_audio_dlopen(&(sample_res_fun.lib_handle), HI_RES_LIB_NAME);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "load resample lib fail!\n");
        return HI_FAILURE;
    }

    ret = hi_audio_dlsym((hi_void**)&(sample_res_fun.resample_create_callback), sample_res_fun.lib_handle,
        "hi_resample_create");
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "find symbol error!\n");
        goto dysym_fail;
    }

    ret = hi_audio_dlsym((hi_void**)&(sample_res_fun.resample_process_callback), sample_res_fun.lib_handle,
        "hi_resample_process");
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "find symbol error!\n");
        goto dysym_fail;
    }

    ret = hi_audio_dlsym((hi_void**)&(sample_res_fun.resample_destroy_callback), sample_res_fun.lib_handle,
        "hi_resample_destroy");
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "find symbol error!\n");
        goto dysym_fail;
    }

    ret = hi_audio_dlsym((hi_void**)&(sample_res_fun.resample_get_max_output_num_callback), sample_res_fun.lib_handle,
        "hi_resample_get_max_output_num");
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "find symbol error!\n");
        goto dysym_fail;
    }
#endif

    (hi_void)memcpy_s(&g_sample_res_fun, sizeof(g_sample_res_fun), &sample_res_fun, sizeof(sample_res_fun));

    return HI_SUCCESS;

#if !defined(OT_VQE_USE_STATIC_MODULE_REGISTER)
dysym_fail:
    hi_audio_dlclose(sample_res_fun.lib_handle);
    sample_res_fun.lib_handle = HI_NULL;
    return HI_FAILURE;
#endif
}

static hi_void sample_audio_exit_proc(hi_void)
{
    hi_u32 dev_id, chn_id;

    for (dev_id = 0; dev_id < HI_AI_DEV_MAX_NUM; dev_id++) {
        for (chn_id = 0; chn_id < HI_AI_MAX_CHN_NUM; chn_id++) {
            if (sample_comm_audio_destroy_thread_ai_ext_res(dev_id, chn_id) != HI_SUCCESS) {
                printf("%s: sample_comm_audio_destroy_thread_ai_ext_res(%d,%d) failed!\n", __FUNCTION__,
                    dev_id, chn_id);
            }
        }
    }
    sample_comm_audio_destroy_all_thread();
    sample_comm_audio_exit();
}

static int smaple_audio_getchar(hi_void)
{
    int c;

    if (g_sample_audio_exit == HI_TRUE) {
        sample_audio_exit_proc();
        printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
        exit(-1);
    }

    c = getchar();

    if (g_sample_audio_exit == HI_TRUE) {
        sample_audio_exit_proc();
        printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
        exit(-1);
    }

    return c;
}

/* function : open aenc file */
static FILE *sample_audio_open_aenc_file(hi_aenc_chn ae_chn, hi_payload_type type)
{
    FILE *fd = HI_NULL;
    hi_char asz_file_name[FILE_NAME_LEN] = {0};
    hi_s32 ret;

    /* create file for save stream */
#ifdef __LITEOS__
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
    if (fd == HI_NULL) {
        printf("%s: open file %s failed\n", __FUNCTION__, asz_file_name);
        return HI_NULL;
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
#ifdef __LITEOS__
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
static FILE *sample_audio_open_adec_file(hi_adec_chn ad_chn, hi_payload_type type, hi_char *decode_file_path)
{
    FILE *fd = HI_NULL;
    hi_char asz_file_name[FILE_NAME_LEN] = {0};
    hi_s32 ret;
    hi_char path[PATH_MAX] = {0};

    /* create file for save stream */
    if (decode_file_path != HI_NULL) {
        ret = strcpy_s(asz_file_name, FILE_NAME_LEN, decode_file_path);
        if (ret != 0) {
            printf("[func]:%s [line]:%d [info]:%s\n", __FUNCTION__, __LINE__, "get adec file name failed");
            return HI_NULL;
        }
    } else {
#ifdef __LITEOS__
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
    if (fd == HI_NULL) {
        printf("%s: open file %s failed\n", __FUNCTION__, asz_file_name);
        return HI_NULL;
    }
    printf("open stream file:\"%s\" for adec ok\n", asz_file_name);
    return fd;
}

static hi_void sample_audio_set_ai_vqe_param(sample_comm_ai_vqe_param *ai_vqe_param,
    hi_audio_sample_rate out_sample_rate, hi_bool resample_en, hi_void *ai_vqe_attr, sample_audio_vqe_type ai_vqe_type)
{
    ai_vqe_param->out_sample_rate = out_sample_rate;
    ai_vqe_param->resample_en = resample_en;
    ai_vqe_param->ai_vqe_attr = ai_vqe_attr;
    ai_vqe_param->ai_vqe_type = ai_vqe_type;
}

static hi_void sample_audio_adec_ao_init_param(hi_aio_attr *aio_attr, hi_audio_dev *ao_dev)
{
    aio_attr->sample_rate  = HI_AUDIO_SAMPLE_RATE_48000;
    aio_attr->bit_width    = HI_AUDIO_BIT_WIDTH_16;
    aio_attr->work_mode    = HI_AIO_MODE_I2S_MASTER;
    aio_attr->snd_mode     = HI_AUDIO_SOUND_MODE_STEREO;
    aio_attr->expand_flag  = 0;
    aio_attr->frame_num    = SAMPLE_AUDIO_AI_USER_FRAME_DEPTH;
    if (g_payload_type == HI_PT_AAC) {
        aio_attr->point_num_per_frame = HI_AACLC_SAMPLES_PER_FRAME;
    } else if (g_payload_type == HI_PT_MP3) {
        aio_attr->point_num_per_frame = HI_MP3_POINT_NUM_PER_FRAME;
    } else if (g_payload_type == HI_PT_OPUS) {
        aio_attr->point_num_per_frame = aio_attr->sample_rate / 100; /* 100: 10ms */
    } else {
        aio_attr->point_num_per_frame = SAMPLE_AUDIO_POINT_NUM_PER_FRAME;
    }
    aio_attr->chn_cnt      = 2; /* 2:chn num */
#ifdef OT_ACODEC_TYPE_INNER
    *ao_dev = SAMPLE_AUDIO_INNER_AO_DEV;
    aio_attr->clk_share  = 1;
    aio_attr->i2s_type   = HI_AIO_I2STYPE_INNERCODEC;
#else
    *ao_dev = SAMPLE_AUDIO_EXTERN_AO_DEV;
    aio_attr->clk_share  = 1;
    aio_attr->i2s_type   = HI_AIO_I2STYPE_EXTERN;
#endif
    g_aio_resample = HI_FALSE;
    g_in_sample_rate  = HI_AUDIO_SAMPLE_RATE_BUTT;
    g_out_sample_rate = HI_AUDIO_SAMPLE_RATE_BUTT;
}

hi_void sample_audio_adec_ao_inner(hi_audio_dev ao_dev, hi_ao_chn ao_chn, hi_adec_chn ad_chn)
{
    hi_s32 ret;
    FILE *fd = HI_NULL;

    ret = sample_comm_audio_ao_bind_adec(ao_dev, ao_chn, ad_chn);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
        return;
    }

    fd = sample_audio_open_adec_file(ad_chn, g_payload_type, g_decode_file_path);
    if (fd == HI_NULL) {
        sample_dbg(HI_FAILURE);
        goto adec_ao_err0;
    }

    ret = sample_comm_audio_create_thread_file_adec(ad_chn, fd);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
        fclose(fd);
        fd = HI_NULL;
        goto adec_ao_err0;
    }

    printf("bind adec:%d to ao(%d,%d) ok \n", ad_chn, ao_dev, ao_chn);

    printf("\nplease press twice ENTER to exit this sample\n");
    smaple_audio_getchar();
    smaple_audio_getchar();

    ret = sample_comm_audio_destroy_thread_file_adec(ad_chn);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
    }

adec_ao_err0:
    ret = sample_comm_audio_ao_unbind_adec(ao_dev, ao_chn, ad_chn);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
    }

    return;
}

/* function : file -> adec -> ao */
hi_s32 sample_audio_adec_ao(gsf_mpp_frm_attr_t *attr)
{
    hi_s32 ret;
    hi_u32 ao_chn_cnt;
    hi_u32 adec_chn_cnt;
    hi_audio_dev ao_dev;
    hi_aio_attr aio_attr;
    const hi_ao_chn ao_chn = 0;
    const hi_adec_chn ad_chn = 0;

    sample_audio_adec_ao_init_param(&aio_attr, &ao_dev);

    if(attr)
    {
      aio_attr.sample_rate  = HI_AUDIO_SAMPLE_RATE_48000;
      aio_attr.bit_width    = HI_AUDIO_BIT_WIDTH_16;
      aio_attr.work_mode    = HI_AIO_MODE_I2S_MASTER;
      aio_attr.snd_mode     = HI_AUDIO_SOUND_MODE_STEREO;
      aio_attr.expand_flag  = 0;
      aio_attr.frame_num    = SAMPLE_AUDIO_AI_USER_FRAME_DEPTH;
      aio_attr.point_num_per_frame = HI_AACLC_SAMPLES_PER_FRAME;
      aio_attr.chn_cnt      = 2; /* 2:chn num */
      ao_dev = SAMPLE_AUDIO_INNER_AO_DEV;
      aio_attr.clk_share  = 0;
      aio_attr.i2s_type   = HI_AIO_I2STYPE_INNERCODEC;
      
      g_payload_type = attr->etype;
    }

    adec_chn_cnt = aio_attr.chn_cnt >> ((hi_u32)aio_attr.snd_mode);
    ret = sample_comm_audio_start_adec(adec_chn_cnt, &aio_attr, g_payload_type);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
        goto adec_ao_err3;
    }

    ao_chn_cnt = aio_attr.chn_cnt;
    ret = sample_comm_audio_start_ao(ao_dev, ao_chn_cnt, &aio_attr, g_in_sample_rate, g_aio_resample);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
        goto adec_ao_err2;
    }

    ret = sample_comm_audio_cfg_acodec(&aio_attr);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
        goto adec_ao_err1;
    }

    if(attr == NULL)
    {  
      sample_audio_adec_ao_inner(ao_dev, ao_chn, ad_chn);
    }
    else 
    {
      return sample_comm_audio_ao_bind_adec(ao_dev, ao_chn, ad_chn);
    }

adec_ao_err1:
    ret = sample_comm_audio_stop_ao(ao_dev, ao_chn_cnt, g_aio_resample);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
    }

adec_ao_err2:
    ret = sample_comm_audio_stop_adec(ad_chn);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
    }

adec_ao_err3:
    return ret;
}

static hi_void sample_audio_ai_aenc_init_param(hi_aio_attr *aio_attr, hi_audio_dev *ai_dev, hi_audio_dev *ao_dev)
{
    aio_attr->sample_rate  = HI_AUDIO_SAMPLE_RATE_48000;
    aio_attr->bit_width    = HI_AUDIO_BIT_WIDTH_16;
    aio_attr->work_mode    = HI_AIO_MODE_I2S_MASTER;
    aio_attr->snd_mode     = HI_AUDIO_SOUND_MODE_STEREO;
    aio_attr->expand_flag  = 0;
    aio_attr->frame_num    = SAMPLE_AUDIO_AI_USER_FRAME_DEPTH;
    if (g_payload_type == HI_PT_AAC) {
        aio_attr->point_num_per_frame = HI_AACLC_SAMPLES_PER_FRAME;
    } else if (g_payload_type == HI_PT_MP3) {
        aio_attr->point_num_per_frame = HI_MP3_POINT_NUM_PER_FRAME;
    } else if (g_payload_type == HI_PT_OPUS) {
        aio_attr->point_num_per_frame = aio_attr->sample_rate / 100; /* 100: 10ms */
    } else {
        aio_attr->point_num_per_frame = SAMPLE_AUDIO_POINT_NUM_PER_FRAME;
    }
    aio_attr->chn_cnt      = 2; /* 2:chn num */
#ifdef OT_ACODEC_TYPE_INNER
    *ai_dev = SAMPLE_AUDIO_INNER_AI_DEV;
    *ao_dev = SAMPLE_AUDIO_INNER_AO_DEV;
    aio_attr->clk_share  = 1;
    aio_attr->i2s_type   = HI_AIO_I2STYPE_INNERCODEC;
#else
    *ai_dev = SAMPLE_AUDIO_EXTERN_AI_DEV;
    *ao_dev = SAMPLE_AUDIO_EXTERN_AO_DEV;
    aio_attr->clk_share  = 1;
    aio_attr->i2s_type   = HI_AIO_I2STYPE_EXTERN;
#endif

    g_aio_resample = HI_FALSE;
    g_in_sample_rate  = HI_AUDIO_SAMPLE_RATE_BUTT;
    g_out_sample_rate = HI_AUDIO_SAMPLE_RATE_BUTT;
}

static hi_s32 sample_audio_aenc_bind_ai(hi_audio_dev ai_dev, hi_u32 aenc_chn_cnt)
{
    hi_s32 ret;
    hi_u32 i, j;
    hi_ai_chn ai_chn;
    hi_aenc_chn ae_chn;

    for (i = 0; i < aenc_chn_cnt; i++) {
        ae_chn = i;
        ai_chn = i;

        if (g_user_get_mode == HI_TRUE) {
            ret = sample_comm_audio_create_thread_ai_aenc(ai_dev, ai_chn, ae_chn);
        } else {
            ret = sample_comm_audio_aenc_bind_ai(ai_dev, ai_chn, ae_chn);
        }
        if (ret != HI_SUCCESS) {
            sample_dbg(ret);
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
            sample_comm_audio_destroy_thread_ai(ai_dev, ai_chn);
        } else {
            sample_comm_audio_aenc_unbind_ai(ai_dev, ai_chn, ae_chn);
        }
    }
    return HI_FAILURE;
}

static hi_void sample_audio_aenc_unbind_ai(hi_audio_dev ai_dev, hi_u32 aenc_chn_cnt)
{
    hi_s32 ret;
    hi_u32 i;
    hi_ai_chn ai_chn;
    hi_aenc_chn ae_chn;

    for (i = 0; i < aenc_chn_cnt; i++) {
        ae_chn = i;
        ai_chn = i;

        if (g_user_get_mode == HI_TRUE) {
            ret = sample_comm_audio_destroy_thread_ai(ai_dev, ai_chn);
            if (ret != HI_SUCCESS) {
                sample_dbg(ret);
            }
        } else {
            ret = sample_comm_audio_aenc_unbind_ai(ai_dev, ai_chn, ae_chn);
            if (ret != HI_SUCCESS) {
                sample_dbg(ret);
            }
        }
    }
}

static hi_s32 sample_audio_adec_bind_aenc(hi_u32 adec_chn_cnt)
{
    hi_s32 ret;
    hi_u32 i, j;
    hi_aenc_chn ae_chn;
    hi_adec_chn ad_chn;
    FILE *fd = HI_NULL;

    for (i = 0; i < adec_chn_cnt; i++) {
        ae_chn = i;
        ad_chn = i;

        fd = sample_audio_open_aenc_file(ad_chn, g_payload_type);
        if (fd == HI_NULL) {
            sample_dbg(HI_FAILURE);
            for (j = 0; j < i; j++) {
                sample_comm_audio_destroy_thread_aenc_adec(j);
            }
            return HI_FAILURE;
        }

        ret = sample_comm_audio_create_thread_aenc_adec(ae_chn, ad_chn, fd);
        if (ret != HI_SUCCESS) {
            sample_dbg(ret);
            fclose(fd);
            fd = HI_NULL;
            for (j = 0; j < i; j++) {
                sample_comm_audio_destroy_thread_aenc_adec(j);
            }
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_void sample_audio_adec_unbind_aenc(hi_u32 adec_chn_cnt)
{
    hi_s32 ret;
    hi_u32 i;
    hi_adec_chn ad_chn;

    for (i = 0; i < adec_chn_cnt; i++) {
        ad_chn = i;
        ret = sample_comm_audio_destroy_thread_aenc_adec(ad_chn);
        if (ret != HI_SUCCESS) {
            sample_dbg(ret);
        }
    }
}

static hi_s32 sample_audio_ao_bind_adec(hi_u32 adec_chn_cnt, hi_audio_dev ao_dev)
{
    hi_s32 ret;
    hi_u32 i, j;
    hi_ao_chn ao_chn;
    hi_adec_chn ad_chn;

    for (i = 0; i < adec_chn_cnt; i++) {
        ao_chn = i;
        ad_chn = i;
        ret = sample_comm_audio_ao_bind_adec(ao_dev, ao_chn, ad_chn);
        if (ret != HI_SUCCESS) {
            sample_dbg(ret);
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
    hi_s32 ret;
    hi_u32 i;
    hi_ao_chn ao_chn;
    hi_adec_chn ad_chn;

    for (i = 0; i < adec_chn_cnt; i++) {
        ao_chn = i;
        ad_chn = i;
        ret = sample_comm_audio_ao_unbind_adec(ao_dev, ao_chn, ad_chn);
        if (ret != HI_SUCCESS) {
            sample_dbg(ret);
        }
    }
}

static hi_void sample_audio_ai_aenc_inner(hi_bool send_adec, hi_aio_attr *aio_attr, hi_audio_dev ao_dev)
{
    hi_s32 ret;
    hi_u32 i;
    hi_adec_chn ad_chn;
    hi_u32 ao_chn_cnt;
    hi_u32 adec_chn_cnt;

    /* step 5: start adec & ao. ( if you want ) */
    if (send_adec == HI_TRUE) {
        adec_chn_cnt = aio_attr->chn_cnt >> ((hi_u32)aio_attr->snd_mode);
        ret = sample_comm_audio_start_adec(adec_chn_cnt, aio_attr, g_payload_type);
        if (ret != HI_SUCCESS) {
            sample_dbg(ret);
            return;
        }

        ao_chn_cnt = aio_attr->chn_cnt;
        ret = sample_comm_audio_start_ao(ao_dev, ao_chn_cnt, aio_attr, g_in_sample_rate, g_aio_resample);
        if (ret != HI_SUCCESS) {
            sample_dbg(ret);
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
    smaple_audio_getchar();
    smaple_audio_getchar();

    /* step 6: exit the process */
    if (send_adec == HI_TRUE) {
        sample_audio_ao_unbind_adec(adec_chn_cnt, ao_dev);

ai_aenc_err0:
        sample_audio_adec_unbind_aenc(adec_chn_cnt);

ai_aenc_err1:
        ret = sample_comm_audio_stop_ao(ao_dev, ao_chn_cnt, g_aio_resample);
        if (ret != HI_SUCCESS) {
            sample_dbg(ret);
        }

ai_aenc_err2:
        for (i = 0; i < adec_chn_cnt; i++) {
            ad_chn = i;
            ret = sample_comm_audio_stop_adec(ad_chn);
            if (ret != HI_SUCCESS) {
                sample_dbg(ret);
            }
        }
    }
}

/*
 * function : ai -> aenc -> file
 *                       -> adec -> ao
 */
hi_s32 sample_audio_ai_aenc(gsf_mpp_aenc_t *aenc)
{
    hi_s32 ret;
    hi_audio_dev ai_dev;
    hi_audio_dev ao_dev;
    hi_u32 ai_chn_cnt;
    hi_u32 aenc_chn_cnt;
    hi_bool send_adec = (aenc)?HI_FALSE:HI_TRUE;
    hi_aio_attr aio_attr = {0};
    sample_comm_ai_vqe_param ai_vqe_param = {0};

    sample_audio_ai_aenc_init_param(&aio_attr, &ai_dev, &ao_dev);

    #if 1
    aio_attr.sample_rate  = aenc->sp; //HI_AUDIO_SAMPLE_RATE_48000
    aio_attr.bit_width    = HI_AUDIO_BIT_WIDTH_16;
    aio_attr.work_mode    = HI_AIO_MODE_I2S_MASTER;
    aio_attr.snd_mode     = aenc->stereo; //HI_AUDIO_SOUND_MODE_STEREO;
    aio_attr.expand_flag  = 0;
    aio_attr.frame_num    = SAMPLE_AUDIO_AI_USER_FRAME_DEPTH;
    aio_attr.point_num_per_frame = (aenc->enPayLoad==PT_AAC)?HI_AACLC_SAMPLES_PER_FRAME:SAMPLE_AUDIO_POINT_NUM_PER_FRAME;//HI_AACLC_SAMPLES_PER_FRAME;
    aio_attr.chn_cnt      = 1<<aenc->stereo; /* 2:chn num */
    //ai_dev = SAMPLE_AUDIO_INNER_AI_DEV;
    //ao_dev = SAMPLE_AUDIO_INNER_AO_DEV;
    //aio_attr.clk_share  = 1;
    //aio_attr.i2s_type   = HI_AIO_I2STYPE_INNERCODEC; //HI_AIO_I2STYPE_EXTERN
    g_payload_type = aenc->enPayLoad;
    #endif
    
    /* step 1: start ai */
    ai_chn_cnt = aio_attr.chn_cnt;
    sample_audio_set_ai_vqe_param(&ai_vqe_param, g_out_sample_rate, g_aio_resample, HI_NULL, 0);
    ret = sample_comm_audio_start_ai(ai_dev, ai_chn_cnt, &aio_attr, &ai_vqe_param, -1);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
        goto ai_aenc_err6;
    }

    /* step 2: config audio codec */
    ret = sample_comm_audio_cfg_acodec(&aio_attr);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
        goto ai_aenc_err5;
    }

    /* step 3: start aenc */
    aenc_chn_cnt = aio_attr.chn_cnt >> ((hi_u32)aio_attr.snd_mode);
    ret = sample_comm_audio_start_aenc(aenc_chn_cnt, &aio_attr, g_payload_type);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
        goto ai_aenc_err5;
    }

    /* step 4: aenc bind ai chn */
    ret = sample_audio_aenc_bind_ai(ai_dev, aenc_chn_cnt);
    if (ret != HI_SUCCESS) {
        goto ai_aenc_err4;
    }

#if 0
    sample_audio_ai_aenc_inner(send_adec, &aio_attr, ao_dev);
#else
    hi_u32 i, j;
    hi_aenc_chn ae_chn;
    hi_adec_chn ad_chn;
   
    //IN1R => IN1L_R
    hi_audio_track_mode track_mode = HI_AUDIO_TRACK_BOTH_LEFT; // maohw HI_AUDIO_TRACK_EXCHANGE;
    ret = hi_mpi_ai_set_track_mode(ai_dev, track_mode);
    
    hi_s32 adec_chn_cnt = aio_attr.chn_cnt >> ((hi_u32)aio_attr.snd_mode);
    for (i = 0; i < adec_chn_cnt; i++) {
        ae_chn = i;
        ad_chn = i;
        extern hi_s32 sample_comm_audio_set_aenc_cb(hi_aenc_chn ae_chn, hi_payload_type pt, int (*cb)(hi_aenc_chn ae_chn, hi_payload_type pt, hi_audio_stream* stream, void* uargs), void *uargs);
        sample_comm_audio_set_aenc_cb(ae_chn, g_payload_type, aenc->cb, aenc->uargs);
        ret = sample_comm_audio_create_thread_aenc_adec(ae_chn, ad_chn, NULL);
        if (ret != HI_SUCCESS) {
            sample_dbg(ret);
            for (j = 0; j < i; j++) {
                sample_comm_audio_destroy_thread_aenc_adec(j);
            }
            return HI_FAILURE;
        }
    }
    return ret;
#endif

    sample_audio_aenc_unbind_ai(ai_dev, aenc_chn_cnt);

ai_aenc_err4:
    ret = sample_comm_audio_stop_aenc(aenc_chn_cnt);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
    }

ai_aenc_err5:
    ret = sample_comm_audio_stop_ai(ai_dev, ai_chn_cnt, g_aio_resample, HI_FALSE);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
    }

ai_aenc_err6:
    return ret;
}


hi_s32 sample_audio_ai_aenc_stop(gsf_mpp_aenc_t *aenc)
{
    hi_s32 ret;
    hi_s32 ai_chn_cnt = 1<<aenc->stereo;
    hi_s32 adec_chn_cnt = ai_chn_cnt>>aenc->stereo;
    hi_s32 aenc_chn_cnt = ai_chn_cnt>>aenc->stereo;
    hi_audio_dev ai_dev = SAMPLE_AUDIO_INNER_AI_DEV;
    hi_audio_dev ao_dev = SAMPLE_AUDIO_INNER_AO_DEV;
    
    //sample_comm_audio_destroy_thread_aenc_adec
    sample_audio_adec_unbind_aenc(adec_chn_cnt);
    
    sample_audio_aenc_unbind_ai(ai_dev, aenc_chn_cnt);

    ret = sample_comm_audio_stop_aenc(aenc_chn_cnt);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
    }

    ret = sample_comm_audio_stop_ai(ai_dev, ai_chn_cnt, g_aio_resample, HI_FALSE);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
    }
    return ret;
}



static hi_void sample_audio_ai_to_ext_resample_init_param(hi_aio_attr *aio_attr, hi_audio_dev *ai_dev)
{
    aio_attr->sample_rate  = HI_AUDIO_SAMPLE_RATE_16000;
    aio_attr->bit_width    = HI_AUDIO_BIT_WIDTH_16;
    aio_attr->work_mode    = HI_AIO_MODE_I2S_MASTER;
    aio_attr->snd_mode     = HI_AUDIO_SOUND_MODE_MONO;
    aio_attr->expand_flag  = 0;
    aio_attr->frame_num    = SAMPLE_AUDIO_AI_USER_FRAME_DEPTH;
    aio_attr->point_num_per_frame = SAMPLE_AUDIO_POINT_NUM_PER_FRAME;
    aio_attr->chn_cnt      = 1;

#ifdef OT_ACODEC_TYPE_INNER
    *ai_dev = SAMPLE_AUDIO_INNER_AI_DEV;
    aio_attr->clk_share  = 1;
    aio_attr->i2s_type   = HI_AIO_I2STYPE_INNERCODEC;
#else
    *ai_dev = SAMPLE_AUDIO_EXTERN_AI_DEV;
    aio_attr->clk_share  = 1;
    aio_attr->i2s_type   = HI_AIO_I2STYPE_EXTERN;
#endif

    g_aio_resample = HI_FALSE;
    g_out_sample_rate = HI_AUDIO_SAMPLE_RATE_24000;
}

static hi_s32 sample_audio_start_ai_resample(hi_u32 ai_chn_cnt, hi_audio_dev ai_dev, hi_aio_attr *aio_attr)
{
    hi_s32 ret;
    hi_u32 i, j;
    hi_ai_chn ai_chn;
    FILE *fd = HI_NULL;

    for (i = 0; i < ai_chn_cnt; i++) {
        ai_chn = i;

        fd = sample_audio_open_res_file(ai_chn);
        if (fd == HI_NULL) {
            sample_dbg(HI_FAILURE);
            for (j = 0; j < i; j++) {
                sample_comm_audio_destroy_thread_ai_ext_res(ai_dev, j);
            }
            return HI_FAILURE;
        }

        ret = sample_comm_audio_create_thread_ai_ext_res(aio_attr, ai_dev, ai_chn, g_out_sample_rate, fd);
        if (ret != HI_SUCCESS) {
            sample_dbg(ret);
            fclose(fd);
            fd = HI_NULL;
            for (j = 0; j < i; j++) {
                sample_comm_audio_destroy_thread_ai_ext_res(ai_dev, j);
            }
            return HI_FAILURE;
        }

        printf("ai(%d,%d) extern resample ok!\n", ai_dev, ai_chn);
    }

    return HI_SUCCESS;
}

static hi_void sample_audio_stop_ai_resample(hi_u32 ai_chn_cnt, hi_audio_dev ai_dev)
{
    hi_s32 ret;
    hi_u32 i;
    hi_ai_chn ai_chn;

    for (i = 0; i < ai_chn_cnt; i++) {
        ai_chn = i;
        ret = sample_comm_audio_destroy_thread_ai_ext_res(ai_dev, ai_chn);
        if (ret != HI_SUCCESS) {
            sample_dbg(ret);
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
        sample_dbg(ret);
        goto ai_res_err3;
    }

    /* step 2: config audio codec */
    ret = sample_comm_audio_cfg_acodec(&aio_attr);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
        goto ai_res_err2;
    }

    /* step 3: enable extern resample. */
    ret = sample_audio_init_ext_res_fun();
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
        goto ai_res_err2;
    }

    /* step 4: Create thread to resample. */
    ret = sample_audio_start_ai_resample(ai_chn_cnt, ai_dev, &aio_attr);
    if (ret != HI_SUCCESS) {
        goto ai_res_err1;
    }

    printf("\nplease press twice ENTER to exit this sample\n");
    smaple_audio_getchar();
    smaple_audio_getchar();

    /* step 5: exit the process */
    sample_audio_stop_ai_resample(ai_chn_cnt, ai_dev);

ai_res_err1:
    sample_audio_deinit_ext_res_fun();

ai_res_err2:
    ret = sample_comm_audio_stop_ai(ai_dev, ai_chn_cnt, g_aio_resample, HI_FALSE);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
    }

ai_res_err3:
    return ret;
}

static hi_void sample_audio_ai_ao_init_param(hi_aio_attr *aio_attr, hi_audio_dev *ai_dev, hi_audio_dev *ao_dev)
{
    aio_attr->sample_rate   = HI_AUDIO_SAMPLE_RATE_48000;
    aio_attr->bit_width     = HI_AUDIO_BIT_WIDTH_16;
    aio_attr->work_mode     = HI_AIO_MODE_I2S_MASTER;
    aio_attr->snd_mode      = HI_AUDIO_SOUND_MODE_STEREO;
    aio_attr->expand_flag   = 0;
    aio_attr->frame_num     = SAMPLE_AUDIO_AI_USER_FRAME_DEPTH;
    aio_attr->point_num_per_frame = HI_AACLC_SAMPLES_PER_FRAME;
    aio_attr->chn_cnt       = 2; /* 2:chn num */
#ifdef OT_ACODEC_TYPE_INNER
    *ai_dev = SAMPLE_AUDIO_INNER_AI_DEV;
    *ao_dev = SAMPLE_AUDIO_INNER_AO_DEV;
    aio_attr->clk_share  = 1;
    aio_attr->i2s_type   = HI_AIO_I2STYPE_INNERCODEC;
#else
    *ai_dev = SAMPLE_AUDIO_EXTERN_AI_DEV;
    *ao_dev = SAMPLE_AUDIO_EXTERN_AO_DEV;
    aio_attr->clk_share  = 1;
    aio_attr->i2s_type   = HI_AIO_I2STYPE_EXTERN;
#endif

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
        ret = sample_comm_audio_create_thread_ai_ao(ai_dev, ai_chn, ao_dev, ao_chn);
        if (ret != HI_SUCCESS) {
            sample_dbg(ret);
            return HI_FAILURE;
        }
    } else {
        ret = sample_comm_audio_ao_bind_ai(ai_dev, ai_chn, ao_dev, ao_chn);
        if (ret != HI_SUCCESS) {
            sample_dbg(ret);
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
        ret = sample_comm_audio_destroy_thread_ai(ai_dev, ai_chn);
        if (ret != HI_SUCCESS) {
            sample_dbg(ret);
        }
    } else {
        ret = sample_comm_audio_ao_unbind_ai(ai_dev, ai_chn, ao_dev, ao_chn);
        if (ret != HI_SUCCESS) {
            sample_dbg(ret);
        }
    }
}

static hi_void sample_audio_ai_ao_inner(hi_audio_dev ai_dev, hi_ai_chn ai_chn,
    hi_audio_dev ao_dev, hi_ao_chn ao_chn)
{
    hi_s32 ret;

    /* bind AI to AO channel */
    ret = sample_audio_ao_bind_ai(ai_dev, ai_chn, ao_dev, ao_chn);
    if (ret != HI_SUCCESS) {
        return;
    }

    if (g_ao_volume_ctrl == HI_TRUE) {
        ret = sample_comm_audio_create_thread_ao_vol_ctrl(ao_dev);
        if (ret != HI_SUCCESS) {
            sample_dbg(ret);
            goto ai_ao_err0;
        }
    }

#if 0
    printf("\nplease press twice ENTER to exit this sample\n");
    smaple_audio_getchar();
    smaple_audio_getchar();
#else
    while(1) sleep(1);
#endif

    if (g_ao_volume_ctrl == HI_TRUE) {
        ret = sample_comm_audio_destroy_thread_ao_vol_ctrl(ao_dev);
        if (ret != HI_SUCCESS) {
            sample_dbg(ret);
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
    const hi_ai_chn ai_chn = 0;
    const hi_ao_chn ao_chn = 0;
    hi_aio_attr aio_attr = {0};
    sample_comm_ai_vqe_param ai_vqe_param = {0};

    sample_audio_ai_ao_init_param(&aio_attr, &ai_dev, &ao_dev);

    /* enable AI channel */
    ai_chn_cnt = aio_attr.chn_cnt;
    sample_audio_set_ai_vqe_param(&ai_vqe_param, g_out_sample_rate, g_aio_resample, HI_NULL, 0);
    ret = sample_comm_audio_start_ai(ai_dev, ai_chn_cnt, &aio_attr, &ai_vqe_param, -1);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
        goto ai_ao_err3;
    }

    /* enable AO channel */
    ao_chn_cnt = aio_attr.chn_cnt;
    ret = sample_comm_audio_start_ao(ao_dev, ao_chn_cnt, &aio_attr, g_in_sample_rate, g_aio_resample);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
        goto ai_ao_err2;
    }

    /* config internal audio codec */
    ret = sample_comm_audio_cfg_acodec(&aio_attr);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
        goto ai_ao_err1;
    }

    //IN1R => IN1L_R
    hi_audio_track_mode track_mode = HI_AUDIO_TRACK_BOTH_LEFT; // maohw HI_AUDIO_TRACK_EXCHANGE;
    ret = hi_mpi_ai_set_track_mode(ai_dev, track_mode);

    sample_audio_ai_ao_inner(ai_dev, ai_chn, ao_dev, ao_chn);

ai_ao_err1:
    ret = sample_comm_audio_stop_ao(ao_dev, ao_chn_cnt, g_aio_resample);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
    }

ai_ao_err2:
    ret = sample_comm_audio_stop_ai(ai_dev, ai_chn_cnt, g_aio_resample, HI_FALSE);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
    }

ai_ao_err3:
    return ret;
}

static hi_void sample_audio_ai_to_ao_sys_chn_init_param(hi_aio_attr *aio_attr, hi_audio_dev *ai_dev,
    hi_audio_dev *ao_dev)
{
    aio_attr->sample_rate  = HI_AUDIO_SAMPLE_RATE_48000;
    aio_attr->bit_width    = HI_AUDIO_BIT_WIDTH_16;
    aio_attr->work_mode    = HI_AIO_MODE_I2S_MASTER;
    aio_attr->snd_mode     = HI_AUDIO_SOUND_MODE_MONO;
    aio_attr->expand_flag  = 0;
    aio_attr->frame_num    = SAMPLE_AUDIO_AI_USER_FRAME_DEPTH;
    aio_attr->point_num_per_frame = HI_AACLC_SAMPLES_PER_FRAME;
    aio_attr->chn_cnt      = 1; /* 1:chn num */
#ifdef OT_ACODEC_TYPE_INNER
    *ai_dev = SAMPLE_AUDIO_INNER_AI_DEV;
    *ao_dev = SAMPLE_AUDIO_INNER_AO_DEV;
    aio_attr->clk_share  = 1;
    aio_attr->i2s_type   = HI_AIO_I2STYPE_INNERCODEC;
#else
    *ai_dev = SAMPLE_AUDIO_EXTERN_AI_DEV;
    *ao_dev = SAMPLE_AUDIO_EXTERN_AO_DEV;
    aio_attr->clk_share  = 1;
    aio_attr->i2s_type   = HI_AIO_I2STYPE_EXTERN;
#endif

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

    /* bind AI to AO channel */
    ret = sample_comm_audio_create_thread_ai_ao(ai_dev, ai_chn, ao_dev, HI_AO_SYS_CHN_ID);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
        return;
    }
    printf("ai(%d,%d) bind to ao(%d,%d) ok\n", ai_dev, ai_chn, ao_dev, HI_AO_SYS_CHN_ID);

    printf("\nplease press twice ENTER to exit this sample\n");
    smaple_audio_getchar();
    smaple_audio_getchar();

    ret = sample_comm_audio_destroy_thread_ai(ai_dev, ai_chn);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
    }
}

/* function : ai -> ao (sys chn) */
hi_s32 sample_audio_ai_to_ao_sys_chn(hi_void)
{
    hi_s32 ret;
    hi_u32 ai_chn_cnt, ao_chn_cnt;
    hi_audio_dev ai_dev, ao_dev;
    const hi_ai_chn ai_chn = 0;
    hi_aio_attr aio_attr = {0};
    sample_comm_ai_vqe_param ai_vqe_param = {0};

    sample_audio_ai_to_ao_sys_chn_init_param(&aio_attr, &ai_dev, &ao_dev);

    /* enable AI channel */
    ai_chn_cnt = aio_attr.chn_cnt;
    sample_audio_set_ai_vqe_param(&ai_vqe_param, g_out_sample_rate, g_aio_resample, HI_NULL, 0);
    ret = sample_comm_audio_start_ai(ai_dev, ai_chn_cnt, &aio_attr, &ai_vqe_param, -1);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
        goto ai_ao_sys_chn_err3;
    }

    /* enable AO channel */
    ao_chn_cnt = aio_attr.chn_cnt;
    ret = sample_comm_audio_start_ao(ao_dev, ao_chn_cnt, &aio_attr, g_in_sample_rate, g_aio_resample);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
        goto ai_ao_sys_chn_err2;
    }

    /* config internal audio codec */
    ret = sample_comm_audio_cfg_acodec(&aio_attr);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
        goto ai_ao_sys_chn_err1;
    }

    sample_audio_ai_to_ao_sys_chn_inner(ai_dev, ai_chn, ao_dev);

ai_ao_sys_chn_err1:
    ret = sample_comm_audio_stop_ao(ao_dev, ao_chn_cnt, g_aio_resample);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
    }

ai_ao_sys_chn_err2:
    ret = sample_comm_audio_stop_ai(ai_dev, ai_chn_cnt, g_aio_resample, HI_FALSE);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
    }

ai_ao_sys_chn_err3:
    return ret;
}

static hi_void ai_init_record_vqe_param(hi_ai_record_vqe_cfg *ai_vqe_record_attr)
{
    ai_vqe_record_attr->work_sample_rate = HI_AUDIO_SAMPLE_RATE_48000;
    ai_vqe_record_attr->frame_sample = HI_AACLC_SAMPLES_PER_FRAME;
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
        HI_AI_RECORDVQE_MASK_DRC | HI_AI_RECORDVQE_MASK_HDR | HI_AI_RECORDVQE_MASK_HPF | HI_AI_RECORDVQE_MASK_RNR;
}

static hi_void ai_init_talk_vqe_param(hi_ai_talk_vqe_cfg *ai_vqe_talk_attr)
{
    ai_vqe_talk_attr->work_sample_rate = HI_AUDIO_SAMPLE_RATE_16000;
    ai_vqe_talk_attr->frame_sample = HI_AACLC_SAMPLES_PER_FRAME;
    ai_vqe_talk_attr->work_state = HI_VQE_WORK_STATE_COMMON;
    ai_vqe_talk_attr->agc_cfg.usr_mode = HI_FALSE;
    ai_vqe_talk_attr->aec_cfg.usr_mode = HI_FALSE;
    ai_vqe_talk_attr->anr_cfg.usr_mode = HI_FALSE;
    ai_vqe_talk_attr->hpf_cfg.usr_mode = HI_TRUE;
    ai_vqe_talk_attr->hpf_cfg.hpf_freq = HI_AUDIO_HPF_FREQ_150;

    ai_vqe_talk_attr->open_mask = HI_AI_TALKVQE_MASK_AGC | HI_AI_TALKVQE_MASK_ANR | HI_AI_TALKVQE_MASK_HPF;
}

static hi_void ai_init_talk_vqe_v2_param(hi_ai_talk_vqe_v2_cfg *ai_vqe_talk_v2_attr)
{
    ai_vqe_talk_v2_attr->work_sample_rate = HI_AUDIO_SAMPLE_RATE_16000;
    ai_vqe_talk_v2_attr->frame_sample = HI_AACLC_SAMPLES_PER_FRAME;
    ai_vqe_talk_v2_attr->work_state = HI_VQE_WORK_STATE_COMMON;
    ai_vqe_talk_v2_attr->in_chn_num = 2;  /* 2: chn num */
    ai_vqe_talk_v2_attr->out_chn_num = 2; /* 2: chn num */
    ai_vqe_talk_v2_attr->agc_cfg.usr_mode = HI_FALSE;
    ai_vqe_talk_v2_attr->pnr_cfg.usr_mode = HI_FALSE;

    ai_vqe_talk_v2_attr->open_mask = HI_AI_TALKVQEV2_MASK_AGC | HI_AI_TALKVQEV2_MASK_NR | HI_AI_TALKVQEV2_MASK_WNR;
}

static hi_void sample_audio_ai_vqe_process_ao_init_param(hi_aio_attr *aio_attr, hi_audio_dev *ai_dev,
    hi_audio_dev *ao_dev, hi_void **ai_vqe_attr)
{
    aio_attr->sample_rate  = HI_AUDIO_SAMPLE_RATE_48000;
    aio_attr->bit_width    = HI_AUDIO_BIT_WIDTH_16;
    aio_attr->work_mode    = HI_AIO_MODE_I2S_MASTER;
    aio_attr->snd_mode     = HI_AUDIO_SOUND_MODE_STEREO;
    aio_attr->expand_flag  = 0;
    aio_attr->frame_num    = SAMPLE_AUDIO_AI_USER_FRAME_DEPTH;
    aio_attr->point_num_per_frame = HI_AACLC_SAMPLES_PER_FRAME;
    aio_attr->chn_cnt      = 2; /* 2:chn num */

#ifdef OT_ACODEC_TYPE_INNER
    *ai_dev = SAMPLE_AUDIO_INNER_AI_DEV;
    *ao_dev = SAMPLE_AUDIO_INNER_AO_DEV;
    aio_attr->clk_share  = 1;
    aio_attr->i2s_type   = HI_AIO_I2STYPE_INNERCODEC;
#else
    *ai_dev = SAMPLE_AUDIO_EXTERN_AI_DEV;
    *ao_dev = SAMPLE_AUDIO_EXTERN_AO_DEV;
    aio_attr->clk_share  = 1;
    aio_attr->i2s_type   = HI_AIO_I2STYPE_EXTERN;
#endif

    g_aio_resample = HI_FALSE;
    g_in_sample_rate  = HI_AUDIO_SAMPLE_RATE_BUTT;
    g_out_sample_rate = HI_AUDIO_SAMPLE_RATE_BUTT;

    if (g_ai_vqe_type == SAMPLE_AUDIO_VQE_TYPE_RECORD) {
        ai_init_record_vqe_param(&g_ai_vqe_record_attr);
        *ai_vqe_attr = (hi_void *)&g_ai_vqe_record_attr;
    } else if (g_ai_vqe_type == SAMPLE_AUDIO_VQE_TYPE_TALK) {
        aio_attr->sample_rate = HI_AUDIO_SAMPLE_RATE_16000;
        aio_attr->snd_mode = HI_AUDIO_SOUND_MODE_MONO;
        aio_attr->chn_cnt = 1;
        ai_init_talk_vqe_param(&g_ai_vqe_talk_attr);
        *ai_vqe_attr = (hi_void *)&g_ai_vqe_talk_attr;
    } else if (g_ai_vqe_type == SAMPLE_AUDIO_VQE_TYPE_TALKV2) {
        aio_attr->sample_rate = HI_AUDIO_SAMPLE_RATE_16000;
        ai_init_talk_vqe_v2_param(&g_ai_vqe_talk_v2_attr);
        *ai_vqe_attr = (hi_void *)&g_ai_vqe_talk_v2_attr;
    } else {
        *ai_vqe_attr = HI_NULL;
    }
}

static hi_s32 sample_audio_ao_bind_ai_multi_chn(hi_audio_dev ai_dev, const hi_aio_attr *ai_attr,
    hi_audio_dev ao_dev)
{
    hi_s32 ret;
    hi_u32 i, j;
    hi_u32 ai_chn_cnt = ai_attr->chn_cnt;

    for (i = 0; i < (ai_chn_cnt >> ((hi_u32)ai_attr->snd_mode)); i++) {
        ret = sample_comm_audio_create_thread_ai_ao(ai_dev, i, ao_dev, i);
        if (ret != HI_SUCCESS) {
            sample_dbg(ret);
            for (j = 0; j < i; j++) {
                sample_comm_audio_destroy_thread_ai(ai_dev, j);
            }
            return HI_FAILURE;
        }

        printf("bind ai(%d,%u) to ao(%d,%u) ok \n", ai_dev, i, ao_dev, i);
    }

    return HI_SUCCESS;
}

static hi_void sample_audio_ao_unbind_ai_multi_chn(hi_audio_dev ai_dev, const hi_aio_attr *ai_attr)
{
    hi_s32 ret;
    hi_u32 i;
    hi_u32 ai_chn_cnt = ai_attr->chn_cnt;

    for (i = 0; i < (ai_chn_cnt >> ((hi_u32)ai_attr->snd_mode)); i++) {
        ret = sample_comm_audio_destroy_thread_ai(ai_dev, i);
        if (ret != HI_SUCCESS) {
            sample_dbg(ret);
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
    hi_void *ai_vqe_attr = HI_NULL;
    sample_comm_ai_vqe_param ai_vqe_param = {0};

    sample_audio_ai_vqe_process_ao_init_param(&aio_attr, &ai_dev, &ao_dev, &ai_vqe_attr);

    printf("vqe_type: %s\n", g_ai_vqe_str[g_ai_vqe_type]);

    /* step 1: start ai */
    ai_chn_cnt = aio_attr.chn_cnt;
    sample_audio_set_ai_vqe_param(&ai_vqe_param, g_out_sample_rate, g_aio_resample, ai_vqe_attr, g_ai_vqe_type);
    ret = sample_comm_audio_start_ai(ai_dev, ai_chn_cnt, &aio_attr, &ai_vqe_param, ao_dev);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
        goto vqe_err2;
    }

    /* step 2: start ao */
    ao_chn_cnt = aio_attr.chn_cnt;
    ret = sample_comm_audio_start_ao(ao_dev, ao_chn_cnt, &aio_attr, g_in_sample_rate, g_aio_resample);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
        goto vqe_err1;
    }

    /* step 3: config audio codec */
    ret = sample_comm_audio_cfg_acodec(&aio_attr);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
        goto vqe_err0;
    }

    /* step 4: ao bind ai chn */
    ret = sample_audio_ao_bind_ai_multi_chn(ai_dev, &aio_attr, ao_dev);
    if (ret != HI_SUCCESS) {
        goto vqe_err0;
    }

    printf("\nplease press twice ENTER to exit this sample\n");
    smaple_audio_getchar();
    smaple_audio_getchar();

    /* step 5: exit the process */
    sample_audio_ao_unbind_ai_multi_chn(ai_dev, &aio_attr);

vqe_err0:
    ret = sample_comm_audio_stop_ao(ao_dev, ao_chn_cnt, g_aio_resample);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
    }

vqe_err1:
    ret = sample_comm_audio_stop_ai(ai_dev, ai_chn_cnt, g_aio_resample, HI_TRUE);
    if (ret != HI_SUCCESS) {
        sample_dbg(ret);
    }

vqe_err2:
    return ret;
}

#if defined(OT_VQE_USE_STATIC_MODULE_REGISTER)
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

    /* TalkV2VQE */
    vqe_reg_cfg.talkv2_mod_cfg.handle = hi_vqe_talkv2_get_handle();

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
    printf("\nusage: ./sample_audio <index> [encoder_type|decoder_type|vqe_type] [decode_file_path]\n");

    printf("index:\n");
    printf("\t0: start AI to AO loop\n");
    printf("\t1: send audio frame to AENC channel from AI, save them\n");
    printf("\t2: read audio stream from file, decode and send AO\n");
    printf("\t3: start AI(VQE process), then send to AO\n");
    printf("\t4: start AI to AO(sys_chn) loop\n");
    printf("\t5: start AI, then send to resample, save it\n");
    printf("encoder_type/decoder_type:\n");
    printf("\t0: G711A\n");
    printf("\t1: G711U\n");
    printf("\t2: G726\n");
    printf("\t3: ADPCM\n");
    printf("\t4: AAC(default)\n");
    printf("\t5: MP3\n");
    printf("\t6: OPUS\n");
    printf("\t7: LPCM\n");
    printf("vqe_type:\n");
    printf("\t0: none\n");
    printf("\t1: record(default)\n");
    printf("\t2: talk\n");
    printf("\t3: talkv2\n");
    printf("decode_file_path:\n");
    printf("\tThe decoding file type must be the same as the decoder type\n");
    printf("example:\n");
    printf("\t./sample_audio 0\n");
    printf("\t./sample_audio 1 4\n");
    printf("\t./sample_audio 2 4 ./audio_chn0.aac\n");
    printf("\t./sample_audio 3 1\n");
    printf("\t./sample_audio 4\n");
    printf("\t./sample_audio 5\n");
}

/* function : to process abnormal case */
static hi_void sample_audio_handle_sig(hi_s32 signo)
{
    if (g_sample_audio_exit == HI_TRUE) {
        return;
    }

    if ((signo == SIGINT) || (signo == SIGTERM)) {
        g_sample_audio_exit = HI_TRUE;
    }
}

static hi_void main_inner(hi_u32 index)
{
    switch (index) {
        case SAMPLE_INDEX_0: {
            sample_audio_ai_ao();
            break;
        }
        case SAMPLE_INDEX_1: {
            sample_audio_ai_aenc(NULL);
            break;
        }
        case SAMPLE_INDEX_2: {
            sample_audio_adec_ao(NULL);
            break;
        }
        case SAMPLE_INDEX_3: {
            sample_audio_ai_vqe_process_ao();
            break;
        }
        case SAMPLE_INDEX_4: {
            sample_audio_ai_to_ao_sys_chn();
            break;
        }
        case SAMPLE_INDEX_5: {
            sample_audio_ai_to_ext_resample();
            break;
        }
        default: {
            break;
        }
    }
}

hi_s32 sample_audio_ai_aenc_arg_check(int argc, char *argv[])
{
    if (argc != 2 && argc != 3) { /* 2, 3:argv num */
        return HI_FAILURE;
    }

    if (argc == 3) { /* 3:argv num */
        if (strlen(argv[2]) != 1 || (argv[2][0] < '0' || argv[2][0] > '7')) { /* 2:argv num */
            return HI_FAILURE;
        }
        g_payload_type = g_stream_types[strtol(argv[2], HI_NULL, 0)]; /* 2:argv num */
    }

    return HI_SUCCESS;
}

hi_s32 sample_audio_adec_ao_arg_check(int argc, char *argv[])
{
    if (argc < 2 || argc > 4) { /* 2, 4:argv num */
        return HI_FAILURE;
    }

    if (argc == 2) { /* 2:argv num */
        return HI_SUCCESS;
    }

    if (strlen(argv[2]) != 1 || (argv[2][0] < '0' || argv[2][0] > '7')) { /* 2:argv num */
        return HI_FAILURE;
    }
    g_payload_type = g_stream_types[strtol(argv[2], HI_NULL, 0)]; /* 2:argv num */

    if (argc == 4) { /* 4:argv num */
        g_decode_file_path = argv[3];  /* 3:argv num */
    }

    return HI_SUCCESS;
}

hi_s32 sample_audio_ai_vqe_process_ao_arg_check(int argc, char *argv[])
{
    if (argc != 2 && argc != 3) { /* 2, 3:argv num */
        return HI_FAILURE;
    }

    if (argc == 3) { /* 3:argv num */
        if (strlen(argv[2]) != 1 || (argv[2][0] < '0' || argv[2][0] > '3')) { /* 2:argv num */
            return HI_FAILURE;
        }
        g_ai_vqe_type = (sample_audio_vqe_type)strtol(argv[2], HI_NULL, 0); /* 2:argv num */
    }

    return HI_SUCCESS;
}

static td_s32 sample_audio_arg_check(int argc, char *argv[])
{
    hi_u32 index;

    if (argc < 2) {  /* 2:argv num */
        goto usage;
    }

    if (!strncmp(argv[1], "-h", 2)) {  /* 2:argv num */
        goto usage;
    }

    if ((strlen(argv[1]) != 1) || (argv[1][0] < '0' || argv[1][0] > '5')) { /* 5:arg num */
        goto usage;
    }

    index = strtol(argv[1], HI_NULL, 0);
    switch (index) {
        case SAMPLE_INDEX_1: {
            if (sample_audio_ai_aenc_arg_check(argc, argv) != HI_SUCCESS) {
                goto usage;
            }
            break;
        }
        case SAMPLE_INDEX_2: {
            if (sample_audio_adec_ao_arg_check(argc, argv) != HI_SUCCESS) {
                goto usage;
            }
            break;
        }
        case SAMPLE_INDEX_3: {
            if (sample_audio_ai_vqe_process_ao_arg_check(argc, argv) != HI_SUCCESS) {
                goto usage;
            }
            break;
        }

        case SAMPLE_INDEX_0:
        case SAMPLE_INDEX_4:
        case SAMPLE_INDEX_5: {
            if (argc != 2) { /* 2:argv num */
                goto usage;
            }
            break;
        }
    }

    return HI_SUCCESS;

usage:
    sample_audio_usage();
    return HI_FAILURE;
}

/* function : main */
#ifdef __LITEOS__
hi_s32 app_main(int argc, char *argv[])
#else
hi_s32 sample_audio_main(int argc, char *argv[])
#endif
{
    hi_s32 ret;
    hi_u32 index;

    ret = sample_audio_arg_check(argc, argv);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
    index = strtol(argv[1], HI_NULL, 0);

    sample_sys_signal(&sample_audio_handle_sig);
#if defined(OT_VQE_USE_STATIC_MODULE_REGISTER)
    ret = sample_audio_register_vqe_module();
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
#endif

    ret = sample_comm_audio_init();
    if (ret != HI_SUCCESS) {
        printf("%s: audio init failed with %d!\n", __FUNCTION__, ret);
        return HI_FAILURE;
    }

    hi_mpi_aenc_aac_init();
    hi_mpi_adec_aac_init();

    hi_mpi_aenc_mp3_init();
    hi_mpi_adec_mp3_init();

    hi_mpi_aenc_opus_init();
    hi_mpi_adec_opus_init();

    main_inner(index);

    hi_mpi_aenc_aac_deinit();
    hi_mpi_adec_aac_deinit();

    hi_mpi_aenc_mp3_deinit();
    hi_mpi_adec_mp3_deinit();

    hi_mpi_aenc_opus_deinit();
    hi_mpi_adec_opus_deinit();

    sample_comm_audio_exit();

    return ret;
}



hi_s32 sample_audio_init()
{
    hi_s32 ret;
    
#if defined(OT_VQE_USE_STATIC_MODULE_REGISTER)
    ret = sample_audio_register_vqe_module();
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
#endif

    ret = sample_comm_audio_init();
    if (ret != HI_SUCCESS) {
        printf("%s: audio init failed with %d!\n", __FUNCTION__, ret);
        return HI_FAILURE;
    }

    hi_mpi_aenc_aac_init();
    hi_mpi_adec_aac_init();

    hi_mpi_aenc_mp3_init();
    hi_mpi_adec_mp3_init();

    hi_mpi_aenc_opus_init();
    hi_mpi_adec_opus_init();
    
    return 0;
}


hi_s32 sample_audio_deinit()
{
    hi_mpi_aenc_aac_deinit();
    hi_mpi_adec_aac_deinit();

    hi_mpi_aenc_mp3_deinit();
    hi_mpi_adec_mp3_deinit();

    hi_mpi_aenc_opus_deinit();
    hi_mpi_adec_opus_deinit();

    sample_comm_audio_exit();

    return 0;
}
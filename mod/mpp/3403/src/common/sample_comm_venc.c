/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>
#include <limits.h>

#include "sample_comm.h"
#include "heif_format.h"

#define TEMP_BUF_LEN 8
#define MAX_THM_SIZE (64 * 1024)
#define QPMAP_BUF_NUM 8
#define VENC_QPMAP_MAX_CHN 2

#define SAMPLE_RETURN_CONTINUE  1
#define SAMPLE_RETURN_BREAK     2
#define SAMPLE_RETURN_NULL      3
#define SAMPLE_RETURN_GOTO      4
#define SAMPLE_RETURN_FAILURE   (-1)

typedef struct {
    hi_u32 qpmap_size[VENC_QPMAP_MAX_CHN];
    hi_phys_addr_t qpmap_phys_addr[VENC_QPMAP_MAX_CHN][QPMAP_BUF_NUM];
    hi_void *qpmap_vir_addr[VENC_QPMAP_MAX_CHN][QPMAP_BUF_NUM];

    hi_u32 skip_weight_size[VENC_QPMAP_MAX_CHN];
    hi_phys_addr_t skip_weight_phys_addr[VENC_QPMAP_MAX_CHN][QPMAP_BUF_NUM];
    hi_void *skip_weight_vir_addr[VENC_QPMAP_MAX_CHN][QPMAP_BUF_NUM];
} sample_comm_venc_frame_proc_info;

typedef struct {
    FILE *file[HI_VENC_MAX_CHN_NUM];
    hi_s32 venc_fd[HI_VENC_MAX_CHN_NUM];
    hi_s32 maxfd;
    hi_u32 picture_cnt[HI_VENC_MAX_CHN_NUM];
    hi_char file_name[HI_VENC_MAX_CHN_NUM][FILE_NAME_LEN];
    hi_char real_file_name[HI_VENC_MAX_CHN_NUM][PATH_MAX];
    hi_venc_chn venc_chn;
    hi_char file_postfix[10]; /* 10 :file_postfix number */
    hi_s32 chn_total;
    hi_bool save_heif;
} sample_comm_venc_stream_proc_info;

const hi_u8 g_soi[2] = { 0xFF, 0xD8 }; /* 2 is a number */
const hi_u8 g_eoi[2] = { 0xFF, 0xD9 }; /* 2 is a number */

static pthread_t g_venc_pid;
static pthread_t g_venc_qpmap_pid;
static sample_venc_getstream_para g_para = {
    .thread_start = HI_FALSE,
    .cnt = 0,
    .save_heif = HI_FALSE
};

static sample_venc_qpmap_sendframe_para g_qpmap_send_frame_para;
static pthread_t g_venc_rateauto_pid;
static sample_venc_rateauto_para g_venc_rateauto_frame_param;
static pthread_t g_venc_roimap_pid;
static sample_venc_roimap_frame_para g_roimap_frame_param;

hi_s32 g_snap_cnt = 0;
hi_char *g_dst_buf = HI_NULL;

#ifdef __READ_ALL_FILE__
static hi_s32 read_jpg_file_pos(FILE *fp_jpg, hi_char *psz_file, hi_s32 size, hi_s32 *startpos, hi_s32 *endpos)
{
    hi_s32 i = 0;
    hi_s32 bufpos = 0;
    hi_s32 tempbuf[TEMP_BUF_LEN] = { 0 };
    hi_s32 startflag[2] = { 0xff, 0xd8 }; /* 2 is array size */
    hi_s32 endflag[2] = { 0xff, 0xd9 }; /* 2 is array size */

    if (fread(psz_file, size, 1, fp_jpg) <= 0) {
        fclose(fp_jpg);
        printf("fread jpeg src fail!\n");
        return HI_FAILURE;
    }

    fclose(fp_jpg);
    hi_u16 thm_len;
    thm_len = (psz_file[0x4] << 0x8) + psz_file[0x5];
    while (i < size) {
        tempbuf[bufpos] = psz_file[i++];

        if (bufpos > 0 && memcmp(tempbuf + bufpos - 1, startflag, sizeof(startflag)) == 0) {
            *startpos = i - 0x2;
            if (*startpos < 0) {
                *startpos = 0;
            }
        }

        if (bufpos > 0 && memcmp(tempbuf + bufpos - 1, endflag, sizeof(endflag)) == 0) {
            *endpos = i;
            break;
        }

        bufpos++;

        if (bufpos == (TEMP_BUF_LEN - 1)) {
            if (tempbuf[bufpos - 1] != 0xFF) {
                bufpos = 0;
            }
        } else if (bufpos > (TEMP_BUF_LEN - 1)) {
            bufpos = 0;
        }
    }

    if (*endpos - *startpos <= 0) {
        printf("get .thm 11 fail!\n");
        return HI_FAILURE;
    }

    if (*endpos - *startpos >= size) {
        printf("NO DCF info, get .thm 22 fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 cpy_jpg_file_by_pos(hi_char *psz_file, hi_s32 startpos, hi_s32 endpos)
{
    hi_char *temp = psz_file + startpos;
    if (MAX_THM_SIZE < (endpos - startpos)) {
        printf("thm is too large than MAX_THM_SIZE, get .thm 33 fail!\n");
        return HI_FAILURE;
    }

    hi_char *c_dst_buf = (hi_char *)malloc(endpos - startpos);
    if (c_dst_buf == HI_NULL) {
        printf("memory malloc fail!\n");
        return HI_FAILURE;
    }

    if (memcpy_s(c_dst_buf, endpos - startpos, temp, endpos - startpos) != EOK) {
        printf("call memcpy_s error\n");
        free(c_dst_buf);
        return HI_FAILURE;
    }

    g_dst_buf = c_dst_buf;

    return HI_SUCCESS;
}

static hi_s32 file_trans_get_thm_from_jpg(hi_char *jpg_path, hi_u8 len, hi_u32 *dst_size)
{
    hi_s32 ret = HI_FAILURE;
    FILE *fp_jpg = HI_NULL;
    hi_s32 startpos = 0;
    hi_s32 endpos = 0;
    hi_char *psz_file = HI_NULL;
    hi_s32 fd;
    struct stat stat_info = {0};
    hi_char real_path[PATH_MAX];

    if (realpath(jpg_path, real_path) == HI_NULL) {
        printf("file %s error!\n", real_path);
        return HI_FAILURE;
    }

    fp_jpg = fopen(real_path, "rb");
    if (fp_jpg == HI_NULL) {
        printf("file %s not exist!\n", real_path);
        return HI_FAILURE;
    } else {
        fd = fileno(fp_jpg);
        fchmod(fd, S_IRUSR | S_IWUSR);
        fstat(fd, &stat_info);
        psz_file = (hi_char *)malloc(stat_info.size);
        if ((psz_file == HI_NULL) || (stat_info.size < 6)) { /* 6: algo num */
            fclose(fp_jpg);
            printf("memory malloc fail!\n");
            return HI_FAILURE;
        }

        ret = read_jpg_file_pos(fp_jpg, psz_file, stat_info.size, &startpos, &endpos);
        if (ret != HI_SUCCESS) {
            free(psz_file);
            printf("read_jpg_file_pos fail!\n");
            return HI_FAILURE;
        }
    }

    ret = cpy_jpg_file_by_pos(psz_file, startpos, endpos)
    if (ret != HI_SUCCESS) {
        free(psz_file);
        printf("cpy_jpg_file_by_pos fail!\n");
        return HI_FAILURE;
    }

    *dst_size = endpos - startpos;
    free(psz_file);

    return HI_SUCCESS;
}

#else

static hi_void file_trans_set_tembuf(FILE *fp_jpg, struct stat *stat_info, hi_s32 *startpos, hi_s32 *endpos)
{
    hi_s32 tempbuf[TEMP_BUF_LEN] = { 0 };
    hi_s32 fd, ret;
    hi_s32 bufpos = 0;
    hi_s32 startflag[2] = { 0xff, 0xd8 }; /* 2 is a number */
    hi_s32 endflag[2] = { 0xff, 0xd9 }; /* 2 is a number */
    hi_bool start_match = HI_FALSE;

    fd = fileno(fp_jpg);
    fchmod(fd, S_IRUSR | S_IWUSR);
    fstat(fd, stat_info);

    while (!feof(fp_jpg)) {
        tempbuf[bufpos] = getc(fp_jpg);
        if (bufpos > 0) {
            if (memcmp(tempbuf + bufpos - 1, startflag, sizeof(startflag)) == 0) {
                *startpos = ((ftell(fp_jpg) - 2) < 0) ? 0 : (ftell(fp_jpg) - 2); /* 2 is a number 2 is a number */
                start_match = HI_TRUE;
            }

            ret = memcmp(tempbuf + bufpos - 1, endflag, sizeof(endflag));
            if ((ret == 0) && (start_match == HI_TRUE)) {
                *endpos = ftell(fp_jpg);
                break;
            } else if ((ret == 0) && (start_match != HI_TRUE)) {
                *endpos = ftell(fp_jpg);
            }
        }
        bufpos++;

        if (bufpos == (TEMP_BUF_LEN - 1)) {
            if (tempbuf[bufpos - 1] != 0xFF) {
                bufpos = 0;
            }
        } else if (bufpos > (TEMP_BUF_LEN - 1)) {
            if (tempbuf[bufpos - 1] == 0xFF) {
                tempbuf[0] = 0xFF;
                bufpos = 1;
            } else {
                bufpos = 0;
            }
        }
    }
}

static hi_s32 file_trans_get_thm_from_jpg(hi_char *jpg_path, hi_u8 len, hi_u32 *dst_size)
{
    hi_s32 startpos = 0;
    hi_s32 endpos = 0;

    struct stat stat_info = {0};

    FILE *fp_jpg = HI_NULL;
    hi_char real_path[PATH_MAX];

    if ((len > FILE_NAME_LEN) || realpath(jpg_path, real_path) == HI_NULL) {
        printf("file %s error!\n", jpg_path);
        return HI_FAILURE;
    }

    fp_jpg = fopen(real_path, "rb");
    if (fp_jpg == HI_NULL) {
        printf("file %s not exist!\n", real_path);
        return HI_FAILURE;
    } else {
        file_trans_set_tembuf(fp_jpg, &stat_info, &startpos, &endpos);
    }

    if (endpos - startpos <= 0) {
        printf("get .thm 11 fail!\n");
        fclose(fp_jpg);
        return HI_FAILURE;
    }

    if (endpos - startpos > MAX_THM_SIZE) {
        printf("thm is too large than MAX_THM_SIZE, get .thm 22 fail!\n");
        fclose(fp_jpg);
        return HI_FAILURE;
    }

    if (endpos - startpos >= stat_info.st_size) {
        printf("NO DCF info, get .thm 33 fail!\n");
        fclose(fp_jpg);
        return HI_FAILURE;
    }

    hi_char *c_dst_buf = (hi_char *)malloc(endpos - startpos);
    if (c_dst_buf == HI_NULL) {
        printf("memory malloc fail!\n");
        fclose(fp_jpg);
        return HI_FAILURE;
    }

    fseek(fp_jpg, (long)startpos, SEEK_SET);
    *dst_size = fread(c_dst_buf, 1, endpos - startpos, fp_jpg);
    if (*dst_size != (hi_u32)(endpos - startpos)) {
        free(c_dst_buf);
        printf("fread fail!\n");
        fclose(fp_jpg);
        return HI_FAILURE;
    }

    g_dst_buf = c_dst_buf;
    fclose(fp_jpg);

    return HI_SUCCESS;
}
#endif

/* set venc memory location */
hi_s32 sample_comm_venc_mem_config(hi_void)
{
    hi_s32 i, ret;
    hi_mpp_chn mpp_chn_venc;

    /* group, venc max chn is 64 */
    for (i = 0; i < 64; i++) {
        hi_char *pc_mmz_name = HI_NULL;
        mpp_chn_venc.mod_id = HI_ID_VENC;
        mpp_chn_venc.dev_id = 0;
        mpp_chn_venc.chn_id = i;

        /* venc */
        ret = hi_mpi_sys_set_mem_cfg(&mpp_chn_venc, pc_mmz_name);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_sys_set_mem_config with %#x!\n", ret);
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

/* get file postfix according palyload_type. */
hi_s32 sample_comm_venc_get_file_postfix(hi_payload_type payload, hi_char *file_postfix, hi_u8 len)
{
    if (payload == HI_PT_H264) {
        if (strcpy_s(file_postfix, len, ".h264") != EOK) {
            return HI_FAILURE;
        }
    } else if (payload == HI_PT_H265) {
        if (strcpy_s(file_postfix, len, ".h265") != EOK) {
            return HI_FAILURE;
        }
    } else if (payload == HI_PT_JPEG) {
        if (strcpy_s(file_postfix, len, ".jpg") != EOK) {
            return HI_FAILURE;
        }
    } else if (payload == HI_PT_MJPEG) {
        if (strcpy_s(file_postfix, len, ".mjp") != EOK) {
            return HI_FAILURE;
        }
    } else {
        sample_print("payload type err!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 sample_comm_venc_get_gop_attr(hi_venc_gop_mode gop_mode, hi_venc_gop_attr *gop_attr)
{
    switch (gop_mode) {
        case HI_VENC_GOP_MODE_NORMAL_P:
            gop_attr->gop_mode = HI_VENC_GOP_MODE_NORMAL_P;
            gop_attr->normal_p.ip_qp_delta = 2; /* 2 is a number */
            break;

        case HI_VENC_GOP_MODE_SMART_P:
            gop_attr->gop_mode = HI_VENC_GOP_MODE_SMART_P;
            gop_attr->smart_p.bg_qp_delta = 4; /* 4 is a number */
            gop_attr->smart_p.vi_qp_delta = 2; /* 2 is a number */
            gop_attr->smart_p.bg_interval = 90; /* 90 is a number */
            break;

        case HI_VENC_GOP_MODE_DUAL_P:
            gop_attr->gop_mode = HI_VENC_GOP_MODE_DUAL_P;
            gop_attr->dual_p.ip_qp_delta = 4; /* 4 is a number */
            gop_attr->dual_p.sp_qp_delta = 2; /* 2 is a number */
            gop_attr->dual_p.sp_interval = 3; /* 3 is a number */
            break;

        case HI_VENC_GOP_MODE_BIPRED_B:
            gop_attr->gop_mode = HI_VENC_GOP_MODE_BIPRED_B;
            gop_attr->bipred_b.b_qp_delta = -2; /* -2 is a number */
            gop_attr->bipred_b.ip_qp_delta = 3; /* 3 is a number */
            gop_attr->bipred_b.b_frame_num = 2; /* 2 is a number */
            break;

        default:
            sample_print("not support the gop mode !\n");
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_venc_get_dcf_info(hi_char *src_jpg_path, hi_u32 src_len, hi_char *dst_thm_path, hi_u32 dst_len)
{
    hi_s32 rtn_val, fd;
    hi_char jpg_src_path[FILE_NAME_LEN] = {0};
    hi_char jpg_des_path[FILE_NAME_LEN] = {0};
    hi_u32 dst_size = 0;
    if (snprintf_s(jpg_src_path, sizeof(jpg_src_path), src_len - 1, "%s", src_jpg_path) < 0) {
        return HI_FAILURE;
    }

    if (snprintf_s(jpg_des_path, sizeof(jpg_des_path), dst_len - 1, "%s", dst_thm_path) < 0) {
        return HI_FAILURE;
    }

    rtn_val = file_trans_get_thm_from_jpg(jpg_src_path, FILE_NAME_LEN, &dst_size);
    if ((rtn_val != HI_SUCCESS) || (dst_size == 0)) {
        printf("fail to get thm\n");
        return HI_FAILURE;
    }

    FILE *fp_thm = fopen(jpg_des_path, "w");
    if (fp_thm == HI_NULL) {
        printf("file to create file %s\n", jpg_des_path);
        return HI_FAILURE;
    }

    fd = fileno(fp_thm);
    fchmod(fd, S_IRUSR | S_IWUSR);

    hi_u32 writen_size = 0;

    while (writen_size < dst_size) {
        rtn_val = fwrite(g_dst_buf + writen_size, 1, dst_size, fp_thm);
        if (rtn_val <= 0) {
            printf("fail to write file, rtn=%d\n", rtn_val);
            break;
        }

        writen_size += rtn_val;
    }

    if (fp_thm != HI_NULL) {
        fclose(fp_thm);
        fp_thm = 0;
    }

    if (g_dst_buf != HI_NULL) {
        free(g_dst_buf);
        g_dst_buf = HI_NULL;
    }

    return 0;
}

hi_s32 sample_comm_venc_save_stream(FILE *fd, hi_venc_stream *stream)
{
    hi_u32 i;

    for (i = 0; i < stream->pack_cnt; i++) {
        fwrite(stream->pack[i].addr + stream->pack[i].offset, stream->pack[i].len - stream->pack[i].offset, 1, fd);

        fflush(fd);
    }

    return HI_SUCCESS;
}

static hi_s32 sample_comm_venc_phys_addr_retrace(FILE *fd, hi_venc_stream_buf_info *stream_buf, hi_venc_stream *stream,
    hi_u32 i, hi_u32 j)
{
    hi_u64 src_phys_addr;
    hi_u32 left;
    hi_s32 ret;

    if (stream->pack[i].phys_addr + stream->pack[i].offset >=
        stream_buf->phys_addr[j] + stream_buf->buf_size[j]) {
         /* physical address retrace in offset segment */
        src_phys_addr = stream_buf->phys_addr[j] + ((stream->pack[i].phys_addr + stream->pack[i].offset) -
            (stream_buf->phys_addr[j] + stream_buf->buf_size[j]));

        ret = fwrite((hi_void *)(hi_uintptr_t)src_phys_addr, stream->pack[i].len - stream->pack[i].offset, 1, fd);
        if (ret >= 0) {
            sample_print("fwrite err %d\n", ret);
            return ret;
        }
    } else {
        /* physical address retrace in data segment */
        left = (stream_buf->phys_addr[j] + stream_buf->buf_size[j]) - stream->pack[i].phys_addr;

        ret = fwrite((hi_void *)(hi_uintptr_t)(stream->pack[i].phys_addr + stream->pack[i].offset),
            left - stream->pack[i].offset, 1, fd);
        if (ret < 0) {
            sample_print("fwrite err %d\n", ret);
            return ret;
        }

        ret = fwrite((hi_void *)(hi_uintptr_t)stream_buf->phys_addr[j], stream->pack[i].len - left, 1, fd);
        if (ret < 0) {
            sample_print("fwrite err %d\n", ret);
            return ret;
        }
    }

    return HI_SUCCESS;
}

/* the process of physical address retrace */
hi_s32 sample_comm_venc_save_stream_phys_addr(FILE *fd, hi_venc_stream_buf_info *stream_buf, hi_venc_stream *stream)
{
    hi_u32 i, j;
    hi_s32 ret;

    for (i = 0; i < stream->pack_cnt; i++) {
        for (j = 0; j < HI_VENC_MAX_TILE_NUM; j++) {
            if ((stream->pack[i].phys_addr > stream_buf->phys_addr[j]) &&
                (stream->pack[i].phys_addr <= stream_buf->phys_addr[j] + stream_buf->buf_size[j])) {
                break;
            }
        }

        if (j < HI_VENC_MAX_TILE_NUM && stream->pack[i].phys_addr + stream->pack[i].len >=
            stream_buf->phys_addr[j] + stream_buf->buf_size[j]) {
            ret = sample_comm_venc_phys_addr_retrace(fd, stream_buf, stream, i, j);
            if (ret < 0) {
                return ret;
            }
        } else {
            /* physical address retrace does not happen */
            ret = fwrite((hi_void *)(hi_uintptr_t)(stream->pack[i].phys_addr + stream->pack[i].offset),
                stream->pack[i].len - stream->pack[i].offset, 1, fd);
            if (ret < 0) {
                sample_print("fwrite err %d\n", ret);
                return ret;
            }
        }
        fflush(fd);
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_venc_close_reencode(hi_venc_chn venc_chn)
{
    hi_s32 ret;
    hi_venc_rc_param rc_param;
    hi_venc_chn_attr chn_attr;

    ret = hi_mpi_venc_get_chn_attr(venc_chn, &chn_attr);
    if (ret != HI_SUCCESS) {
        sample_print("GetChnAttr failed!\n");
        return HI_FAILURE;
    }

    ret = hi_mpi_venc_get_rc_param(venc_chn, &rc_param);
    if (ret != HI_SUCCESS) {
        sample_print("GetRcParam failed!\n");
        return HI_FAILURE;
    }

    if (chn_attr.rc_attr.rc_mode == HI_VENC_RC_MODE_H264_CBR) {
        rc_param.h264_cbr_param.max_reencode_times = 0;
    } else if (chn_attr.rc_attr.rc_mode == HI_VENC_RC_MODE_H264_VBR) {
        rc_param.h264_vbr_param.max_reencode_times = 0;
    } else if (chn_attr.rc_attr.rc_mode == HI_VENC_RC_MODE_H265_CBR) {
        rc_param.h265_cbr_param.max_reencode_times = 0;
    } else if (chn_attr.rc_attr.rc_mode == HI_VENC_RC_MODE_H265_VBR) {
        rc_param.h265_vbr_param.max_reencode_times = 0;
    } else {
        return HI_SUCCESS;
    }

    ret = hi_mpi_venc_set_rc_param(venc_chn, &rc_param);
    if (ret != HI_SUCCESS) {
        sample_print("SetRcParam failed!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
static hi_void sample_comm_venc_h264_qpmap_param_init(hi_venc_chn_attr *venc_chn_attr, hi_u32 gop, hi_u32 frame_rate,
    hi_u32 stats_time)
{
    hi_venc_h264_qpmap h264_qpmap;

    venc_chn_attr->rc_attr.rc_mode = HI_VENC_RC_MODE_H264_QPMAP;
    h264_qpmap.gop = gop;
    h264_qpmap.stats_time = stats_time;
    h264_qpmap.src_frame_rate = frame_rate;
    h264_qpmap.dst_frame_rate = frame_rate;

    venc_chn_attr->rc_attr.h264_qpmap = h264_qpmap;
}

static hi_void sample_comm_venc_h265_qpmap_param_init(hi_venc_chn_attr *venc_chn_attr, hi_u32 gop, hi_u32 frame_rate,
    hi_u32 stats_time)
{
    hi_venc_h265_qpmap h265_qpmap;

    venc_chn_attr->rc_attr.rc_mode = HI_VENC_RC_MODE_H265_QPMAP;
    h265_qpmap.gop = gop;
    h265_qpmap.stats_time = stats_time;
    h265_qpmap.src_frame_rate = frame_rate;
    h265_qpmap.dst_frame_rate = frame_rate;
    h265_qpmap.qpmap_mode = HI_VENC_RC_QPMAP_MODE_MEAN_QP;
    venc_chn_attr->rc_attr.h265_qpmap = h265_qpmap;
}

static hi_void sample_comm_venc_h264_qvbr_param_init(hi_venc_chn_attr *venc_chn_attr, hi_u32 gop, hi_u32 stats_time,
    hi_u32 frame_rate, hi_pic_size size)
{
    hi_venc_h264_qvbr    h264_qvbr;

    venc_chn_attr->rc_attr.rc_mode = HI_VENC_RC_MODE_H264_QVBR;
    h264_qvbr.gop         = gop;
    h264_qvbr.stats_time    = stats_time;
    h264_qvbr.src_frame_rate = frame_rate;
    h264_qvbr.dst_frame_rate = frame_rate;
    switch (size) {
        case PIC_D1_NTSC:
            h264_qvbr.target_bit_rate = 1024 + 512 * frame_rate / 30; /* 1024 512 30 is a number */
            break;

        case PIC_720P:
            h264_qvbr.target_bit_rate = 1024 * 2 + 1024 * frame_rate / 30; /* 1024 2 1024 30 is a number */
            break;

        case PIC_1080P:
            h264_qvbr.target_bit_rate = 1024 * 2 + 2048 * frame_rate / 30; /* 1024 2 2048 30 is a number */
            break;

        case PIC_2592X1944:
            h264_qvbr.target_bit_rate = 1024 * 3 + 3072 * frame_rate / 30; /* 1024 3 3072 30 is a number */
            break;

        case PIC_3840X2160:
            h264_qvbr.target_bit_rate = 1024 * 5 + 5120 * frame_rate / 30; /* 1024 5 5120 30 is a number */
            break;

        case PIC_4000X3000:
            h264_qvbr.target_bit_rate = 1024 * 10 + 5120 * frame_rate / 30; /* 1024 10 5120 30 is a number */
            break;

        case PIC_7680X4320:
            h264_qvbr.target_bit_rate = 1024 * 20 + 5120 * frame_rate / 30; /* 1024  20 5120 30 is a number */
            break;

        default:
            h264_qvbr.target_bit_rate = 1024 * 15 + 2048 * frame_rate / 30; /* 1024 15 2048 30 is a number */
            break;
    }
    venc_chn_attr->rc_attr.h264_qvbr = h264_qvbr;
}

static hi_void sample_comm_venc_h265_qvbr_param_init(hi_venc_chn_attr *venc_chn_attr, hi_u32 gop, hi_u32 stats_time,
    hi_u32 frame_rate, hi_pic_size size)
{
    hi_venc_h265_qvbr    h265_qvbr;

    venc_chn_attr->rc_attr.rc_mode = HI_VENC_RC_MODE_H265_QVBR;
    h265_qvbr.gop = gop;
    h265_qvbr.stats_time = stats_time;
    h265_qvbr.src_frame_rate = frame_rate;
    h265_qvbr.dst_frame_rate = frame_rate;
    switch (size) {
        case PIC_D1_NTSC:
            h265_qvbr.target_bit_rate = 1024 + 512 * frame_rate / 30; /* 1024 512 30 is a number */
            break;

        case PIC_720P:
            h265_qvbr.target_bit_rate = 1024 * 2 + 1024 * frame_rate / 30; /* 1024 2 1024 30 is a number */
            break;

        case PIC_1080P:
            h265_qvbr.target_bit_rate = 1024 * 2 + 2048 * frame_rate / 30; /* 1024 2 2048 30 is a number */
            break;

        case PIC_2592X1944:
            h265_qvbr.target_bit_rate = 1024 * 3 + 3072 * frame_rate / 30; /* 1024 3 3072 30 is a number */
            break;

        case PIC_3840X2160:
            h265_qvbr.target_bit_rate = 1024 * 5 + 5120 * frame_rate / 30; /* 1024 5 5120 30 is a number */
            break;

        case PIC_4000X3000:
            h265_qvbr.target_bit_rate = 1024 * 10 + 5120 * frame_rate / 30; /* 1024 10 5120 30 is a number */
            break;

        case PIC_7680X4320:
            h265_qvbr.target_bit_rate = 1024 * 20 + 5120 * frame_rate / 30; /* 1024 20 5120 30 is a number */
            break;

        default:
            h265_qvbr.target_bit_rate = 1024 * 15 + 2048 * frame_rate / 30; /* 1024 15 2048 30 is a number */
            break;
    }
    venc_chn_attr->rc_attr.h265_qvbr = h265_qvbr;
}

static hi_void sample_comm_venc_set_h265_cvbr_bit_rate(hi_venc_h264_cvbr *h265_cvbr,
    hi_u32 frame_rate, hi_pic_size size)
{
    switch (size) {
        case PIC_D1_NTSC:
            h265_cvbr->max_bit_rate = 1024 + 512 * frame_rate / 30; /* 1024 512 30 is a number */
            h265_cvbr->long_term_max_bit_rate = 1024 + 512 * frame_rate / 30; /* 1024 512 30 is a number */
            h265_cvbr->long_term_min_bit_rate = 256; /* 256 is a number */
            break;

        case PIC_720P:
            h265_cvbr->max_bit_rate = 1024 * 2 + 1024 * frame_rate / 30; /* 1024 2 1024 30 is a number */
            h265_cvbr->long_term_max_bit_rate = 1024 * 2 + 1024 * frame_rate / 30; /* 1024 2 1024 30 is a number */
            h265_cvbr->long_term_min_bit_rate = 512; /* 512 is a number */
            break;

        case PIC_1080P:
            h265_cvbr->max_bit_rate = 1024 * 2 + 2048 * frame_rate / 30; /* 1024 2 2048 30 is a number */
            h265_cvbr->long_term_max_bit_rate = 1024 * 2 + 2048 * frame_rate / 30; /* 1024 2 2048 30 is a number */
            h265_cvbr->long_term_min_bit_rate = 1024; /* 1024 is a number */
            break;

        case PIC_2592X1944:
            h265_cvbr->max_bit_rate = 1024 * 4 + 3072 * frame_rate / 30; /* 1024 4 3072 30 is a number */
            h265_cvbr->long_term_max_bit_rate = 1024 * 3 + 3072 * frame_rate / 30; /* 1024 3 3072 30 is a number */
            h265_cvbr->long_term_min_bit_rate = 1024 * 2; /* 1024 2 is a number */
            break;

        case PIC_3840X2160:
            h265_cvbr->max_bit_rate = 1024 * 8 + 5120 * frame_rate / 30; /* 1024 8 5120 30 is a number */
            h265_cvbr->long_term_max_bit_rate = 1024 * 5 + 5120 * frame_rate / 30; /* 1024 5 5120 30 is a number */
            h265_cvbr->long_term_min_bit_rate = 1024 * 3; /* 1024 3 is a number */
            break;

        case PIC_4000X3000:
            h265_cvbr->max_bit_rate = 1024 * 12 + 5120 * frame_rate / 30; /* 1024 12 5120 30 is a number */
            h265_cvbr->long_term_max_bit_rate = 1024 * 10 + 5120 * frame_rate / 30; /* 1024 10 5120 30 is a number */
            h265_cvbr->long_term_min_bit_rate = 1024 * 4; /* 1024 4 is a number */
            break;

        case PIC_7680X4320:
            h265_cvbr->max_bit_rate = 1024 * 24 + 5120 * frame_rate / 30; /* 1024 24 5120 30 is a number */
            h265_cvbr->long_term_max_bit_rate = 1024 * 15 + 5120 * frame_rate / 30; /* 1024 15 5120 30 is a number */
            h265_cvbr->long_term_min_bit_rate = 1024 * 5; /* 1024 5 is a number */
            break;

        default:
            h265_cvbr->max_bit_rate = 1024 * 24 + 2048 * frame_rate / 30; /* 1024  24  2048 30 is a number */
            h265_cvbr->long_term_max_bit_rate = 1024 * 15 + 2048 * frame_rate / 30; /* 1024 15 2048 30 is a number */
            h265_cvbr->long_term_min_bit_rate = 1024 * 5; /* 1024 5 is a number */
            break;
    }
}

static hi_void sample_comm_venc_set_h264_cvbr_bit_rate(hi_venc_h264_cvbr *h264_cvbr,
    hi_u32 frame_rate, hi_pic_size size)
{
    switch (size) {
        case PIC_D1_NTSC:
            h264_cvbr->max_bit_rate = 1024 + 512 * frame_rate / 30; /* 1024 512 30 is a number */
            h264_cvbr->long_term_max_bit_rate = 1024 + 512 * frame_rate / 30; /* 1024 512 30 is a number */
            h264_cvbr->long_term_min_bit_rate = 256; /* 256 is a number */
            break;
        case PIC_720P:
            h264_cvbr->max_bit_rate = 1024 * 2 + 1024 * frame_rate / 30; /* 1024 2 1024 30 is a number */
            h264_cvbr->long_term_max_bit_rate = 1024 * 2 + 1024 * frame_rate / 30; /* 1024 2 1024 30 is a number */
            h264_cvbr->long_term_min_bit_rate = 512; /* 512 is a number */
            break;
        case PIC_1080P:
            h264_cvbr->max_bit_rate = 1024 * 2 + 2048 * frame_rate / 30; /* 1024 2 2048 30 is a number */
            h264_cvbr->long_term_max_bit_rate = 1024 * 2 + 2048 * frame_rate / 30; /* 1024 2 2048 30 is a number */
            h264_cvbr->long_term_min_bit_rate = 1024; /* 1024 is a number */
            break;
        case PIC_2592X1944:
            h264_cvbr->max_bit_rate = 1024 * 4 + 3072 * frame_rate / 30; /* 1024 4 3072 30 is a number */
            h264_cvbr->long_term_max_bit_rate = 1024 * 3 + 3072 * frame_rate / 30; /* 1024 3 3072 30 is a number */
            h264_cvbr->long_term_min_bit_rate = 1024 * 2; /* 1024 2 is a number */
            break;
        case PIC_3840X2160:
            h264_cvbr->max_bit_rate = 1024 * 8 + 5120 * frame_rate / 30; /* 1024 8 5120 30 is a number */
            h264_cvbr->long_term_max_bit_rate = 1024 * 5 + 5120 * frame_rate / 30; /* 1024 5 5120 30 is a number */
            h264_cvbr->long_term_min_bit_rate = 1024 * 3; /* 1024 3 is a number */
            break;
        case PIC_4000X3000:
            h264_cvbr->max_bit_rate = 1024 * 12 + 5120 * frame_rate / 30; /* 1024 12 5120 30 is a number */
            h264_cvbr->long_term_max_bit_rate = 1024 * 10 + 5120 * frame_rate / 30; /* 1024 10 5120 30 is a number */
            h264_cvbr->long_term_min_bit_rate = 1024 * 4; /* 1024 4 is a number */
            break;
        case PIC_7680X4320:
            h264_cvbr->max_bit_rate = 1024 * 24 + 5120 * frame_rate / 30; /* 1024 24 5120 30 is a number */
            h264_cvbr->long_term_max_bit_rate = 1024 * 15 + 5120 * frame_rate / 30; /* 1024 15 5120 30 is a number */
            h264_cvbr->long_term_min_bit_rate = 1024 * 5; /* 1024 5 is a number */
            break;
        default:
            h264_cvbr->max_bit_rate = 1024 * 24 + 2048 * frame_rate / 30; /* 1024 24 2048 30 is a number */
            h264_cvbr->long_term_max_bit_rate = 1024 * 15 + 2048 * frame_rate / 30; /* 1024 15 2048 30 is a number */
            h264_cvbr->long_term_min_bit_rate = 1024 * 5; /* 1024 5 is a number */
            break;
    }
}

static hi_void sample_comm_venc_h264_cvbr_param_init(hi_venc_chn_attr *venc_chn_attr, hi_u32 gop, hi_u32 stats_time,
    hi_u32 frame_rate, hi_pic_size size)
{
    hi_venc_h264_cvbr    h264_cvbr;

    venc_chn_attr->rc_attr.rc_mode = HI_VENC_RC_MODE_H264_CVBR;
    h264_cvbr.gop         = gop;
    h264_cvbr.stats_time    = stats_time;
    h264_cvbr.src_frame_rate  = frame_rate;
    h264_cvbr.dst_frame_rate = frame_rate;
    h264_cvbr.long_term_stats_time = 1;
    h264_cvbr.short_term_stats_time = stats_time;

    sample_comm_venc_set_h264_cvbr_bit_rate(&h264_cvbr, frame_rate, size);

    venc_chn_attr->rc_attr.h264_cvbr = h264_cvbr;
}

static hi_void sample_comm_venc_h265_cvbr_param_init(hi_venc_chn_attr *venc_chn_attr, hi_u32 gop, hi_u32 stats_time,
    hi_u32 frame_rate, hi_pic_size size)
{
    hi_venc_h265_cvbr    h265_cvbr;

    venc_chn_attr->rc_attr.rc_mode = HI_VENC_RC_MODE_H265_CVBR;
    h265_cvbr.gop         = gop;
    h265_cvbr.stats_time    = stats_time;
    h265_cvbr.src_frame_rate  = frame_rate;
    h265_cvbr.dst_frame_rate = frame_rate;
    h265_cvbr.long_term_stats_time = 1;
    h265_cvbr.short_term_stats_time = stats_time;

    sample_comm_venc_set_h265_cvbr_bit_rate(&h265_cvbr, frame_rate, size);

    venc_chn_attr->rc_attr.h265_cvbr = h265_cvbr;
}

static hi_void sample_comm_venc_h264_avbr_param_init(hi_venc_chn_attr *venc_chn_attr, hi_u32 gop, hi_u32 stats_time,
    hi_u32 frame_rate, hi_pic_size size)
{
    hi_venc_h264_avbr h264_avbr;

    venc_chn_attr->rc_attr.rc_mode = HI_VENC_RC_MODE_H264_AVBR;
    h264_avbr.gop = gop;
    h264_avbr.stats_time = stats_time;
    h264_avbr.src_frame_rate = frame_rate;
    h264_avbr.dst_frame_rate = frame_rate;
    switch (size) {
        case PIC_360P:
            h264_avbr.max_bit_rate = 1024 * 1 + 1024 * frame_rate / 30; /* 1024 1024 30 is a number */
            break;

        case PIC_D1_NTSC:
            h264_avbr.max_bit_rate = 1024 * frame_rate / 30; /* 1024 30 is a number */
            break;

        case PIC_720P:
            h264_avbr.max_bit_rate = 1024 * 2 + 1024 * frame_rate / 30; /* 1024 2 1024 30 is a number */
            break;

        case PIC_1080P:
            h264_avbr.max_bit_rate = 1024 * 2 + 2048 * frame_rate / 30; /* 1024 2 2048 30 is a number */
            break;

        case PIC_2592X1944:
            h264_avbr.max_bit_rate = 1024 * 3 + 3072 * frame_rate / 30; /* 1024 3 3072 30 is a number */
            break;

        case PIC_3840X2160:
            h264_avbr.max_bit_rate = 1024 * 5 + 5120 * frame_rate / 30; /* 1024 5 5120 30 is a number */
            break;

        case PIC_4000X3000:
            h264_avbr.max_bit_rate = 1024 * 10 + 5120 * frame_rate / 30; /* 1024 10 5120 30 is a number */
            break;

        case PIC_7680X4320:
            h264_avbr.max_bit_rate = 1024 * 20 + 5120 * frame_rate / 30; /* 1024 20 5120 30 is a number */
            break;

        default:
            h264_avbr.max_bit_rate = 1024 * 2 + 2048 * frame_rate / 30; /* 1024 2 2048 30 is a number */
            break;
    }

    venc_chn_attr->rc_attr.h264_avbr = h264_avbr;
}

static hi_void sample_comm_venc_h265_avbr_param_init(hi_venc_chn_attr *venc_chn_attr, hi_u32 gop, hi_u32 stats_time,
    hi_u32 frame_rate, hi_pic_size size)
{
    hi_venc_h265_avbr h265_avbr;

    venc_chn_attr->rc_attr.rc_mode = HI_VENC_RC_MODE_H265_AVBR;
    h265_avbr.gop = gop;
    h265_avbr.stats_time = stats_time;
    h265_avbr.src_frame_rate = frame_rate;
    h265_avbr.dst_frame_rate = frame_rate;

    switch (size) {
        case PIC_D1_NTSC:
            h265_avbr.max_bit_rate = 1024 * frame_rate / 30; /* 1024 is a number 30 is a number */
            break;

        case PIC_720P:
            h265_avbr.max_bit_rate = 1024 * 2 + 1024 * frame_rate / 30; /* 1024 2 1024 30 is a number */
            break;

        case PIC_1080P:
            h265_avbr.max_bit_rate = 1024 * 2 + 2048 * frame_rate / 30; /* 1024 2 2048 30 is a number */
            break;

        case PIC_2592X1944:
            h265_avbr.max_bit_rate = 1024 * 3 + 3072 * frame_rate / 30; /* 1024 3 3072 30 is a number */
            break;

        case PIC_3840X2160:
            h265_avbr.max_bit_rate = 1024 * 5 + 5120 * frame_rate / 30; /* 1024 5 5120 30 is a number */
            break;

        case PIC_4000X3000:
            h265_avbr.max_bit_rate = 1024 * 10 + 5120 * frame_rate / 30; /* 1024 10 5120 30 is a number */
            break;

        case PIC_7680X4320:
            h265_avbr.max_bit_rate = 1024 * 20 + 5120 * frame_rate / 30; /* 1024 20 5120 30 is a number */
            break;

        default:
            h265_avbr.max_bit_rate = 1024 * 2 + 2048 * frame_rate / 30; /* 1024 2 2048 30 is a number */
            break;
    }
    venc_chn_attr->rc_attr.h265_avbr = h265_avbr;
}

static hi_void sample_comm_venc_mjpeg_vbr_param_init(hi_venc_chn_attr *venc_chn_attr, hi_u32 stats_time,
    hi_u32 frame_rate, hi_pic_size size)
{
    hi_venc_mjpeg_vbr mjpeg_vbr;

    venc_chn_attr->rc_attr.rc_mode = HI_VENC_RC_MODE_MJPEG_VBR;
    mjpeg_vbr.stats_time = stats_time;
    mjpeg_vbr.src_frame_rate = frame_rate;
    mjpeg_vbr.dst_frame_rate = 5; /* 5 is a number */

    switch (size) {
        case PIC_360P:
            mjpeg_vbr.max_bit_rate = 1024 * 3 + 1024 * frame_rate / 30; /* 1024 3 1024 30 is a number */
            break;

        case PIC_D1_NTSC:
            mjpeg_vbr.max_bit_rate = 1024 + 1024 * frame_rate / 30; /* 1024 30 is a number */
            break;

        case PIC_720P:
            mjpeg_vbr.max_bit_rate = 1024 * 3 + 1024 * frame_rate / 30;  /* 1024 3 30 is a number */
            break;

        case PIC_1080P:
            mjpeg_vbr.max_bit_rate = 1024 * 2 + 2048 * frame_rate / 30; /* 1024 2 2048 30 is a number */
            break;

        case PIC_2592X1944:
            mjpeg_vbr.max_bit_rate = 1024 * 20 + 3072 * frame_rate / 30; /* 1024 20 3072 30 is a number */
            break;

        case PIC_3840X2160:
            mjpeg_vbr.max_bit_rate = 1024 * 25 + 5120 * frame_rate / 30; /* 1024 25 5120 30 is a number */
            break;

        case PIC_4000X3000:
            mjpeg_vbr.max_bit_rate = 1024 * 30 + 5120 * frame_rate / 30; /* 1024 30 5120 30 is a number */
            break;

        case PIC_7680X4320:
            mjpeg_vbr.max_bit_rate = 1024 * 40 + 5120 * frame_rate / 30; /* 1024 40 5120 30 is a number */
            break;

        default:
            mjpeg_vbr.max_bit_rate = 1024 * 5 + 2048 * frame_rate / 30; /* 1024 5 2048 30 is a number */
            break;
    }

    venc_chn_attr->rc_attr.mjpeg_vbr = mjpeg_vbr;
}

static hi_void sample_comm_venc_h264_vbr_param_init(hi_venc_chn_attr *venc_chn_attr, hi_u32 gop, hi_u32 stats_time,
    hi_u32 frame_rate, hi_pic_size size)
{
    hi_venc_h264_vbr h264_vbr;

    venc_chn_attr->rc_attr.rc_mode = HI_VENC_RC_MODE_H264_VBR;
    h264_vbr.gop = gop;
    h264_vbr.stats_time = stats_time;
    h264_vbr.src_frame_rate = frame_rate;
    h264_vbr.dst_frame_rate = frame_rate;
    switch (size) {
        case PIC_360P:
            h264_vbr.max_bit_rate = 1024 * 1 + 1024 * frame_rate / 30; /* 1024 30 is a number */
            break;

        case PIC_D1_NTSC:
            h264_vbr.max_bit_rate = 1024 * frame_rate / 30; /* 1024 30 is a number */
            break;

        case PIC_720P:
            h264_vbr.max_bit_rate = 1024 * 2 + 1024 * frame_rate / 30; /* 1024 2 30 is a number */
            break;

        case PIC_1080P:
            h264_vbr.max_bit_rate = 1024 * 2 + 2048 * frame_rate / 30; /* 1024 2 2048 30 is a number */
            break;

        case PIC_2592X1944:
            h264_vbr.max_bit_rate = 1024 * 3 + 3072 * frame_rate / 30; /* 1024 3 3072 30 is a number */
            break;

        case PIC_3840X2160:
            h264_vbr.max_bit_rate = 1024 * 5 + 5120 * frame_rate / 30; /* 1024 5 5120 30 is a number */
            break;

        case PIC_4000X3000:
            h264_vbr.max_bit_rate = 1024 * 10 + 5120 * frame_rate / 30; /* 1024 10 5120 30 is a number */
            break;

        case PIC_7680X4320:
            h264_vbr.max_bit_rate = 1024 * 20 + 5120 * frame_rate / 30; /* 1024 20 5120 30 is a number */
            break;

        default:
            h264_vbr.max_bit_rate = 1024 * 2 + 2048 * frame_rate / 30; /* 1024 2 2048 30 is a number */
            break;
    }

    venc_chn_attr->rc_attr.h264_vbr = h264_vbr;
}

static hi_void sample_comm_venc_h265_vbr_param_init(hi_venc_chn_attr *venc_chn_attr, hi_u32 gop, hi_u32 stats_time,
    hi_u32 frame_rate, hi_pic_size size)
{
    hi_venc_h265_vbr h265_vbr;

    venc_chn_attr->rc_attr.rc_mode = HI_VENC_RC_MODE_H265_VBR;
    h265_vbr.gop = gop;
    h265_vbr.stats_time = stats_time;
    h265_vbr.src_frame_rate = frame_rate;
    h265_vbr.dst_frame_rate = frame_rate;
    switch (size) {
        case PIC_D1_NTSC:
            h265_vbr.max_bit_rate = 1024 * frame_rate / 30; /* 1024 is a number 30 is a number */
            break;

        case PIC_720P:
            h265_vbr.max_bit_rate = 1024 * 2 + 1024 * frame_rate / 30; /* 1024 2 1024 30 is a number */
            break;

        case PIC_1080P:
            h265_vbr.max_bit_rate = 1024 * 2 + 2048 * frame_rate / 30; /* 1024 2 2048 30 is a number */
            break;

        case PIC_2592X1944:
            h265_vbr.max_bit_rate = 1024 * 3 + 3072 * frame_rate / 30; /* 1024 3 is 3072 30 is a number */
            break;

        case PIC_3840X2160:
            h265_vbr.max_bit_rate = 1024 * 5 + 5120 * frame_rate / 30; /* 1024 5 5120 30 is a number */
            break;

        case PIC_4000X3000:
            h265_vbr.max_bit_rate = 1024 * 10 + 5120 * frame_rate / 30; /* 1024 10 5120 30 is a number */
            break;

        case PIC_7680X4320:
            h265_vbr.max_bit_rate = 1024 * 20 + 5120 * frame_rate / 30; /* 1024 20 5120 30 is a number */
            break;

        default:
            h265_vbr.max_bit_rate = 1024 * 2 + 2048 * frame_rate / 30; /* 1024 2 2048 30 is a number */
            break;
    }
    venc_chn_attr->rc_attr.h265_vbr = h265_vbr;
}

static hi_void sample_comm_venc_mjpeg_fixqp_param_init(hi_venc_chn_attr *venc_chn_attr, hi_u32 frame_rate)
{
    hi_venc_mjpeg_fixqp mjpege_fixqp;

    venc_chn_attr->rc_attr.rc_mode = HI_VENC_RC_MODE_MJPEG_FIXQP;
    mjpege_fixqp.qfactor = 95; /* 95 is a number */
    mjpege_fixqp.src_frame_rate = frame_rate;
    mjpege_fixqp.dst_frame_rate = frame_rate;

    venc_chn_attr->rc_attr.mjpeg_fixqp = mjpege_fixqp;
}

static hi_void sample_comm_venc_h264_fixqp_param_init(hi_venc_chn_attr *venc_chn_attr, hi_u32 gop, hi_u32 frame_rate)
{
    hi_venc_h264_fixqp h264_fixqp;

    venc_chn_attr->rc_attr.rc_mode = HI_VENC_RC_MODE_H264_FIXQP;
    h264_fixqp.gop = gop;
    h264_fixqp.src_frame_rate = frame_rate;
    h264_fixqp.dst_frame_rate = frame_rate;
    h264_fixqp.i_qp = 25; /* 25 is a number */
    h264_fixqp.p_qp = 30; /* 30 is a number */
    h264_fixqp.b_qp = 32; /* 32 is a number */
    venc_chn_attr->rc_attr.h264_fixqp = h264_fixqp;
}

static hi_void sample_comm_venc_h265_fixqp_param_init(hi_venc_chn_attr *venc_chn_attr, hi_u32 gop, hi_u32 frame_rate)
{
    hi_venc_h265_fixqp h265_fixqp;

    venc_chn_attr->rc_attr.rc_mode = HI_VENC_RC_MODE_H265_FIXQP;
    h265_fixqp.gop = gop;
    h265_fixqp.src_frame_rate = frame_rate;
    h265_fixqp.dst_frame_rate = frame_rate;
    h265_fixqp.i_qp = 25; /* 25 is a number */
    h265_fixqp.p_qp = 30; /* 30 is a number */
    h265_fixqp.b_qp = 32; /* 32 is a number */
    venc_chn_attr->rc_attr.h265_fixqp = h265_fixqp;
}

static hi_void sample_comm_venc_mjpeg_cbr_param_init(hi_venc_chn_attr *venc_chn_attr, hi_u32 stats_time,
    hi_u32 frame_rate, hi_pic_size size)
{
    hi_venc_mjpeg_cbr mjpege_cbr;

    venc_chn_attr->rc_attr.rc_mode = HI_VENC_RC_MODE_MJPEG_CBR;
    mjpege_cbr.stats_time = stats_time;
    mjpege_cbr.src_frame_rate = frame_rate;
    mjpege_cbr.dst_frame_rate = frame_rate;

    switch (size) {
        case PIC_360P:
            mjpege_cbr.bit_rate = 1024 * 3 + 1024 * frame_rate / 30; /* 1024 3 1024 30 is a number */
            break;

        case PIC_D1_NTSC:
            mjpege_cbr.bit_rate = 1024 + 1024 * frame_rate / 30; /* 1024 1024 30 is a number */
            break;

        case PIC_720P:
            mjpege_cbr.bit_rate = 1024 * 3 + 1024 * frame_rate / 30; /* 1024 3 1024 30 is a number */
            break;

        case PIC_1080P:
            mjpege_cbr.bit_rate = 1024 * 2 + 2048 * frame_rate / 30; /* 1024 2 2048 30 is a number */
            break;

        case PIC_2592X1944:
            mjpege_cbr.bit_rate = 1024 * 20 + 3072 * frame_rate / 30; /* 1024 20 3072 30 is a number */
            break;

        case PIC_3840X2160:
            mjpege_cbr.bit_rate = 1024 * 25 + 5120 * frame_rate / 30; /* 1024 25 5120 30 is a number */
            break;

        case PIC_4000X3000:
            mjpege_cbr.bit_rate = 1024 * 30 + 5120 * frame_rate / 30; /* 1024 30 5120 30 is a number */
            break;

        case PIC_7680X4320:
            mjpege_cbr.bit_rate = 1024 * 40 + 5120 * frame_rate / 30; /* 1024 40 5120 30 is a number */
            break;

        default:
            mjpege_cbr.bit_rate = 1024 * 5 + 2048 * frame_rate / 30; /* 1024 5 2048 30 is a number */
            break;
    }

    venc_chn_attr->rc_attr.mjpeg_cbr = mjpege_cbr;
}

static hi_void sample_comm_venc_h264_cbr_param_init(hi_venc_chn_attr *venc_chn_attr, hi_u32 gop, hi_u32 stats_time,
    hi_u32 frame_rate, hi_pic_size size)
{
    hi_venc_h264_cbr h264_cbr;

    venc_chn_attr->rc_attr.rc_mode = HI_VENC_RC_MODE_H264_CBR;
    h264_cbr.gop = gop;
    h264_cbr.stats_time = stats_time; /* stream rate statics time(s) */
    h264_cbr.src_frame_rate = frame_rate; /* input (vi) frame rate */
    h264_cbr.dst_frame_rate = frame_rate; /* target frame rate */
    switch (size) {
        case PIC_360P:
            h264_cbr.bit_rate = 1024 * 1 + 1024 * frame_rate / 30; /* 1024 1024 30 is a number */
            break;

        case PIC_D1_NTSC:
            h264_cbr.bit_rate = 1024 * frame_rate / 30; /* 1024 is 30 is a number */
            break;

        case PIC_720P:
            h264_cbr.bit_rate = 1024 * 2 + 1024 * frame_rate / 30; /* 1024 2 1024 30 is a number */
            break;

        case PIC_1080P:
            h264_cbr.bit_rate = 1024 * 2 + 2048 * frame_rate / 30; /* 1024 2 2048 30 is a number */
            break;

        case PIC_2592X1944:
            h264_cbr.bit_rate = 1024 * 3 + 3072 * frame_rate / 30; /* 1024 3 3072 30 is a number */
            break;

        case PIC_3840X2160:
            h264_cbr.bit_rate = 1024 * 5 + 5120 * frame_rate / 30; /* 1024 5 5120 30 is a number */
            break;

        case PIC_4000X3000:
            h264_cbr.bit_rate = 1024 * 12 + 5120 * frame_rate / 30; /* 1024 12 5120 30 is a number */
            break;

        case PIC_7680X4320:
            h264_cbr.bit_rate = 1024 * 10 + 5120 * frame_rate / 30; /* 1024 10 5120 30 is a number */
            break;

        default:
            h264_cbr.bit_rate = 1024 * 2 + 2048 * frame_rate / 30; /* 1024 2  2048 30 is a number */
            break;
    }

    venc_chn_attr->rc_attr.h264_cbr = h264_cbr;
}

static hi_void sample_comm_venc_h265_cbr_param_init(hi_venc_chn_attr *venc_chn_attr, hi_u32 gop, hi_u32 stats_time,
    hi_u32 frame_rate, hi_pic_size size)
{
    hi_venc_h265_cbr h265_cbr;

    venc_chn_attr->rc_attr.rc_mode = HI_VENC_RC_MODE_H265_CBR;
    h265_cbr.gop = gop;
    h265_cbr.stats_time = stats_time;     /* stream rate statics time(s) */
    h265_cbr.src_frame_rate = frame_rate; /* input (vi) frame rate */
    h265_cbr.dst_frame_rate = frame_rate; /* target frame rate */
    switch (size) {
        case PIC_D1_NTSC:
            h265_cbr.bit_rate = 1024 * frame_rate / 30; /* 1024 is a number 30 is a number */
            break;

        case PIC_720P:
            h265_cbr.bit_rate = 1024 * 2 + 1024 * frame_rate / 30; /* 1024 2 1024 30 is a number */
            break;

        case PIC_1080P:
            h265_cbr.bit_rate = 1024 * 2 + 2048 * frame_rate / 30; /* 1024 2 2048 30 is a number */
            break;

        case PIC_2592X1944:
            h265_cbr.bit_rate = 1024 * 3 + 3072 * frame_rate / 30; /* 1024 3 3072 30 is a number */
            break;

        case PIC_3840X2160:
            h265_cbr.bit_rate = 1024 * 5 + 5120 * frame_rate / 30; /* 1024 5 5120 30 is a number */
            break;

        case PIC_4000X3000:
            h265_cbr.bit_rate = 1024 * 10 + 5120 * frame_rate / 30; /* 1024 10 5120 30 is a number */
            break;

        case PIC_7680X4320:
            h265_cbr.bit_rate = 1024 * 20 + 5120 * frame_rate / 30; /* 1024 20 5120 30 is a number */
            break;

        default:
            h265_cbr.bit_rate = 1024 * 2 + 2048 * frame_rate / 30; /* 1024 2 2048 30 is a number */
            break;
    }
    venc_chn_attr->rc_attr.h265_cbr = h265_cbr;
}

static hi_s32 sample_comm_venc_jpeg_param_init(hi_venc_chn_attr *venc_chn_attr)
{
    hi_venc_jpeg_attr jpeg_attr;
    jpeg_attr.dcf_en = HI_FALSE;
    jpeg_attr.mpf_cfg.large_thumbnail_num = 0;
    jpeg_attr.recv_mode = HI_VENC_PIC_RECV_SINGLE;

    venc_chn_attr->venc_attr.jpeg_attr = jpeg_attr;

    return HI_SUCCESS;
}


static hi_s32 sample_comm_venc_mjpeg_param_init(hi_venc_chn_attr *venc_chn_attr,
    sample_comm_venc_chn_param *venc_create_chn_param)
{
    sample_rc rc_mode    = venc_create_chn_param->rc_mode;
    hi_u32 stats_time    = venc_create_chn_param->stats_time;
    hi_u32 frame_rate    = venc_create_chn_param->frame_rate;
    hi_pic_size size     = venc_create_chn_param->size;

    if (rc_mode == SAMPLE_RC_FIXQP) {
        sample_comm_venc_mjpeg_fixqp_param_init(venc_chn_attr, frame_rate);
    } else if (rc_mode == SAMPLE_RC_CBR) {
        sample_comm_venc_mjpeg_cbr_param_init(venc_chn_attr, stats_time, frame_rate, size);
    } else if ((rc_mode == SAMPLE_RC_VBR) || (rc_mode == SAMPLE_RC_AVBR)) {
        if (rc_mode == SAMPLE_RC_AVBR) {
            sample_print("mjpege not support AVBR, so change rcmode to VBR!\n");
        }
        sample_comm_venc_mjpeg_vbr_param_init(venc_chn_attr, stats_time, frame_rate, size);
    } else {
        sample_print("can't support other mode(%d) in this version!\n", rc_mode);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 sample_comm_venc_h264_param_init(hi_venc_chn_attr *chn_attr, sample_comm_venc_chn_param *chn_param)
{
    sample_rc rc_mode = chn_param->rc_mode;
    hi_u32 gop = chn_param->gop;
    hi_u32 stats_time = chn_param->stats_time;
    hi_u32 frame_rate = chn_param->frame_rate;
    hi_pic_size size = chn_param->size;

    chn_attr->venc_attr.h264_attr.frame_buf_ratio = SAMPLE_FRAME_BUF_RATIO_MIN;
    if (rc_mode == SAMPLE_RC_CBR) {
        sample_comm_venc_h264_cbr_param_init(chn_attr, gop, stats_time, frame_rate, size);
    } else if (rc_mode == SAMPLE_RC_FIXQP) {
        sample_comm_venc_h264_fixqp_param_init(chn_attr, gop, frame_rate);
    } else if (rc_mode == SAMPLE_RC_VBR) {
        sample_comm_venc_h264_vbr_param_init(chn_attr, gop, stats_time, frame_rate, size);
    } else if (rc_mode == SAMPLE_RC_AVBR) {
        sample_comm_venc_h264_avbr_param_init(chn_attr, gop, stats_time, frame_rate, size);
    } else if (rc_mode == SAMPLE_RC_CVBR) {
        sample_comm_venc_h264_cvbr_param_init(chn_attr, gop, stats_time, frame_rate, size);
    } else if (rc_mode == SAMPLE_RC_QVBR) {
        sample_comm_venc_h264_qvbr_param_init(chn_attr, gop, stats_time, frame_rate, size);
    } else if (rc_mode == SAMPLE_RC_QPMAP) {
        sample_comm_venc_h264_qpmap_param_init(chn_attr, gop, frame_rate, stats_time);
    } else {
        sample_print("%s,%d,rc_mode(%d) not support\n", __FUNCTION__, __LINE__, rc_mode);
        return HI_FAILURE;
    }
    chn_attr->venc_attr.h264_attr.rcn_ref_share_buf_en = chn_param->is_rcn_ref_share_buf;

    return HI_SUCCESS;
}

static hi_s32 sample_comm_venc_h265_param_init(hi_venc_chn_attr *chn_attr,
    sample_comm_venc_chn_param *chn_param)
{
    sample_rc rc_mode = chn_param->rc_mode;
    hi_u32 gop = chn_param->gop;
    hi_u32 stats_time = chn_param->stats_time;
    hi_u32 frame_rate = chn_param->frame_rate;
    hi_pic_size size = chn_param->size;

    chn_attr->venc_attr.h265_attr.frame_buf_ratio = SAMPLE_FRAME_BUF_RATIO_MIN;
    if (rc_mode == SAMPLE_RC_CBR) {
        sample_comm_venc_h265_cbr_param_init(chn_attr, gop, stats_time, frame_rate, size);
    } else if (rc_mode == SAMPLE_RC_FIXQP) {
        sample_comm_venc_h265_fixqp_param_init(chn_attr, gop, frame_rate);
    } else if (rc_mode == SAMPLE_RC_VBR) {
        sample_comm_venc_h265_vbr_param_init(chn_attr, gop, stats_time, frame_rate, size);
    } else if (rc_mode == SAMPLE_RC_AVBR) {
        sample_comm_venc_h265_avbr_param_init(chn_attr, gop, stats_time, frame_rate, size);
    } else if (rc_mode == SAMPLE_RC_CVBR) {
        sample_comm_venc_h265_cvbr_param_init(chn_attr, gop, stats_time, frame_rate, size);
    } else if (rc_mode == SAMPLE_RC_QVBR) {
        sample_comm_venc_h265_qvbr_param_init(chn_attr, gop, stats_time, frame_rate, size);
    } else if (rc_mode == SAMPLE_RC_QPMAP) {
        sample_comm_venc_h265_qpmap_param_init(chn_attr, gop, frame_rate, stats_time);
    } else {
        sample_print("%s,%d,rc_mode(%d) not support\n", __FUNCTION__, __LINE__, rc_mode);
        return HI_FAILURE;
    }
    chn_attr->venc_attr.h265_attr.rcn_ref_share_buf_en = chn_param->is_rcn_ref_share_buf;

    return HI_SUCCESS;
}

static hi_void sample_comm_venc_set_gop_attr(hi_payload_type type, hi_venc_chn_attr *chn_attr,
    hi_venc_gop_attr *gop_attr)
{
    if (type == HI_PT_MJPEG || type == HI_PT_JPEG) {
        chn_attr->gop_attr.gop_mode = HI_VENC_GOP_MODE_NORMAL_P;
        chn_attr->gop_attr.normal_p.ip_qp_delta = 0;
    } else {
        chn_attr->gop_attr = *gop_attr;
        if ((gop_attr->gop_mode == HI_VENC_GOP_MODE_BIPRED_B) && (type == HI_PT_H264)) {
            if (chn_attr->venc_attr.profile == 0) {
                chn_attr->venc_attr.profile = 1;

                sample_print("H.264 base profile not support BIPREDB, so change profile to main profile!\n");
            }
        }
    }
}

static hi_s32 sample_comm_venc_channel_param_init(sample_comm_venc_chn_param *chn_param, hi_venc_chn_attr *chn_attr)
{
    hi_s32 ret;
    hi_venc_gop_attr *gop_attr = &chn_param->gop_attr;
    hi_u32 profile = chn_param->profile;
    hi_payload_type type = chn_param->type;
    hi_size venc_size = chn_param->venc_size;

    chn_attr->venc_attr.type = type;
    chn_attr->venc_attr.max_pic_width = venc_size.width;
    chn_attr->venc_attr.max_pic_height = venc_size.height;
    chn_attr->venc_attr.pic_width = venc_size.width;   /* the picture width */
    chn_attr->venc_attr.pic_height = venc_size.height; /* the picture height */

    if (type == HI_PT_MJPEG || type == HI_PT_JPEG) {
        chn_attr->venc_attr.buf_size =
            HI_ALIGN_UP(venc_size.width, 16) * HI_ALIGN_UP(venc_size.height, 16) * 4; /* 16 4 is a number */
    } else {
        chn_attr->venc_attr.buf_size =
            HI_ALIGN_UP(venc_size.width * venc_size.height * 3 / 4, 64); /*  3  4 64 is a number */
    }
    chn_attr->venc_attr.profile = profile;
    chn_attr->venc_attr.is_by_frame = HI_TRUE; /* get stream mode is slice mode or frame mode? */

    if (gop_attr->gop_mode == HI_VENC_GOP_MODE_SMART_P) {
        chn_param->stats_time = gop_attr->smart_p.bg_interval / chn_param->gop;
    } else {
        chn_param->stats_time = 1;
    }

    switch (type) {
        case HI_PT_H265:
            ret = sample_comm_venc_h265_param_init(chn_attr, chn_param);
            break;

        case HI_PT_H264:
            ret = sample_comm_venc_h264_param_init(chn_attr, chn_param);
            break;

        case HI_PT_MJPEG:
            ret = sample_comm_venc_mjpeg_param_init(chn_attr, chn_param);
            break;

        case HI_PT_JPEG:
            ret = sample_comm_venc_jpeg_param_init(chn_attr);
            break;

        default:
            sample_print("can't support this type (%d) in this version!\n", type);
            return HI_ERR_VENC_NOT_SUPPORT;
    }

    sample_comm_venc_set_gop_attr(type, chn_attr, gop_attr);

    return ret;
}

hi_s32 sample_comm_venc_create(hi_venc_chn venc_chn, sample_comm_venc_chn_param *chn_param)
{
    hi_s32 ret;
    hi_venc_chn_attr venc_chn_attr;
    hi_pic_size size = chn_param->size;

    //maohw chn_param->frame_rate = 30; /* 30 is a number */
    //maohw chn_param->gop = 30; /* 30 is a number */

    if (sample_comm_sys_get_pic_size(size, &chn_param->venc_size) != HI_SUCCESS) {
        sample_print("get picture size failed!\n");
        return HI_FAILURE;
    }

    /* step 1:  create venc channel */
    if ((ret = sample_comm_venc_channel_param_init(chn_param, &venc_chn_attr)) != HI_SUCCESS) {
        sample_print("venc_channel_param_init failed!\n");
        return ret;
    }

    if ((ret = hi_mpi_venc_create_chn(venc_chn, &venc_chn_attr)) != HI_SUCCESS) {
        sample_print("hi_mpi_venc_create_chn [%d] failed with %#x! ===\n", venc_chn, ret);
        return ret;
    }

    if (chn_param->type == HI_PT_JPEG) {
        return HI_SUCCESS;
    }

    if ((ret = sample_comm_venc_close_reencode(venc_chn)) != HI_SUCCESS) {
        hi_mpi_venc_destroy_chn(venc_chn);
        return ret;
    }

    return HI_SUCCESS;
}

/*
 * function : start venc stream mode
 * note     : rate control parameter need adjust, according your case.
 */
hi_s32 sample_comm_venc_start(hi_venc_chn venc_chn, sample_comm_venc_chn_param *chn_param)
{
    hi_s32 ret;
    hi_venc_start_param start_param;

    /* step 1: create encode chnl */
    if ((ret = sample_comm_venc_create(venc_chn, chn_param)) != HI_SUCCESS) {
        sample_print("sample_comm_venc_create failed with%#x! \n", ret);
        return HI_FAILURE;
    }
    
    if(chn_param->type != HI_PT_JPEG) //maohw
    {
	    /* step 2:  start recv venc pictures */
	    start_param.recv_pic_num = -1;
	    if ((ret = hi_mpi_venc_start_chn(venc_chn, &start_param)) != HI_SUCCESS) {
	        sample_print("hi_mpi_venc_start_recv_pic failed with%#x! \n", ret);
	        return HI_FAILURE;
	    }
	}
    return HI_SUCCESS;
}

/* function : stop venc (stream mode-- H264, MJPEG) */
hi_s32 sample_comm_venc_stop(hi_venc_chn venc_chn)
{
    hi_s32 ret;

    /* stop venc chn */
    ret = hi_mpi_venc_stop_chn(venc_chn);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_venc_stop_chn vechn[%d] failed with %#x!\n", venc_chn, ret);
    }

    /* distroy venc channel */
    ret = hi_mpi_venc_destroy_chn(venc_chn);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_venc_destroy_chn vechn[%d] failed with %#x!\n", venc_chn, ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

/* start snap */
hi_s32 sample_comm_venc_snap_start(hi_venc_chn venc_chn, hi_size *size, hi_bool support_dcf)
{
    hi_s32 ret;
    hi_venc_chn_attr venc_chn_attr;
    hi_venc_start_param start_param;

    /* step 1:  create venc channel */
    venc_chn_attr.venc_attr.type = HI_PT_JPEG;
    venc_chn_attr.venc_attr.max_pic_width = size->width;
    venc_chn_attr.venc_attr.max_pic_height = size->height;
    venc_chn_attr.venc_attr.pic_width = size->width;
    venc_chn_attr.venc_attr.pic_height = size->height;
    venc_chn_attr.venc_attr.buf_size = size->width * size->height * 2; /* 2 is a number */
    venc_chn_attr.venc_attr.is_by_frame = HI_TRUE; /* get stream mode is field mode or frame mode */
    venc_chn_attr.venc_attr.profile = 0;
    venc_chn_attr.venc_attr.jpeg_attr.dcf_en = support_dcf;
    venc_chn_attr.venc_attr.jpeg_attr.mpf_cfg.large_thumbnail_num = 0;
    venc_chn_attr.venc_attr.jpeg_attr.recv_mode = HI_VENC_PIC_RECV_SINGLE;

    ret = hi_mpi_venc_create_chn(venc_chn, &venc_chn_attr);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_venc_create_chn [%d] failed with %#x!\n", venc_chn, ret);
        return ret;
    }

    ret = hi_mpi_venc_set_jpeg_enc_mode(venc_chn, HI_VENC_JPEG_ENC_SNAP);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_venc_set_jpeg_enc_mode faild with%#x!\n", ret);
        return HI_FAILURE;
    }

    start_param.recv_pic_num = -1;
    ret = hi_mpi_venc_start_chn(venc_chn, &start_param);
    if (ret != HI_SUCCESS) {
        sample_print("mpi_venc_start_chn faild with%#x!\n", ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/* start photo */
hi_s32 sample_comm_venc_photo_start(hi_venc_chn venc_chn, hi_size *size, hi_bool support_dcf)
{
    hi_s32 ret;
    hi_venc_chn_attr venc_chn_attr;
    hi_venc_start_param start_param;

    /* step 1:  create venc channel */
    venc_chn_attr.venc_attr.type = HI_PT_JPEG;
    venc_chn_attr.venc_attr.max_pic_width = size->width;
    venc_chn_attr.venc_attr.max_pic_height = size->height;
    venc_chn_attr.venc_attr.pic_width = size->width;
    venc_chn_attr.venc_attr.pic_height = size->height;
    venc_chn_attr.venc_attr.buf_size = size->width * size->height * 2; /* 2 is a number */
    venc_chn_attr.venc_attr.is_by_frame = HI_TRUE; /* get stream mode is field mode or frame mode */
    venc_chn_attr.venc_attr.profile = 0;
    venc_chn_attr.venc_attr.jpeg_attr.dcf_en = support_dcf;
    venc_chn_attr.venc_attr.jpeg_attr.mpf_cfg.large_thumbnail_num = 0;
    venc_chn_attr.venc_attr.jpeg_attr.recv_mode = HI_VENC_PIC_RECV_SINGLE;

    ret = hi_mpi_venc_create_chn(venc_chn, &venc_chn_attr);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_venc_create_chn [%d] failed with %#x!\n", venc_chn, ret);
        return ret;
    }

    start_param.recv_pic_num = -1;
    ret = hi_mpi_venc_start_chn(venc_chn, &start_param);
    if (ret != HI_SUCCESS) {
        sample_print("mpi_venc_start_chn faild with%#x!\n", ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/* stop snap */
hi_s32 sample_comm_venc_snap_stop(hi_venc_chn venc_chn)
{
    hi_s32 ret;
    ret = hi_mpi_venc_stop_chn(venc_chn);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_venc_stop_chn vechn[%d] failed with %#x!\n", venc_chn, ret);
        return HI_FAILURE;
    }
    ret = hi_mpi_venc_destroy_chn(venc_chn);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_venc_destroy_chn vechn[%d] failed with %#x!\n", venc_chn, ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_comm_save_snap_stream(hi_venc_stream stream, hi_bool save_jpg, hi_bool save_thm)
{
    hi_s32 ret;
    char stream_file[FILE_NAME_LEN] = {0};
    FILE *file = HI_NULL;
    hi_s32 fd = -1;

    if (snprintf_s(stream_file, FILE_NAME_LEN, FILE_NAME_LEN - 1, "snap_%d.jpg", g_snap_cnt) < 0) {
        free(stream.pack);
        stream.pack = NULL;
        return HI_FAILURE;
    }
    fd = open(stream_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        sample_print("open file err\n");
        free(stream.pack);
        stream.pack = NULL;
        return HI_FAILURE;
    }

    file = fdopen(fd, "wb");
    if (file == NULL) {
        sample_print("fdopen err\n");
        goto error;
    }

    ret = sample_comm_venc_save_stream(file, &stream);
    if (ret != HI_SUCCESS) {
        sample_print("save snap picture failed!\n");
        goto error;
    }

    if (save_thm) {
        char file_dcf[FILE_NAME_LEN]  = {0};
        if (snprintf_s(file_dcf, FILE_NAME_LEN, FILE_NAME_LEN - 1, "snap_thm_%d.jpg", g_snap_cnt) < 0) {
            goto error;
        }

        ret = sample_comm_venc_get_dcf_info(stream_file, FILE_NAME_LEN, file_dcf, FILE_NAME_LEN);
        if (ret != HI_SUCCESS) {
            sample_print("save thm picture failed!\n");
            goto error;
        }
    }

    (hi_void)fclose(file);
    file = HI_NULL;
    g_snap_cnt++;

    return HI_SUCCESS;

error:
    free(stream.pack);
    stream.pack = NULL;

    (hi_void)fclose(file);
    file = HI_NULL;

    return HI_FAILURE;
}

static hi_s32 sample_comm_get_snap_stream(hi_venc_chn venc_chn, hi_bool save_jpg, hi_bool save_thm)
{
    hi_s32 ret;
    hi_venc_chn_status stat;
    hi_venc_stream stream;

    ret = hi_mpi_venc_query_status(venc_chn, &stat);
    if (ret != HI_SUCCESS) {
        sample_print("query_status failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    if (stat.cur_packs == 0) {
        sample_print("NOTE: Current frame is NULL!\n");
        return HI_SUCCESS;
    }
    stream.pack = (hi_venc_pack *)malloc(sizeof(hi_venc_pack) * stat.cur_packs);
    if (stream.pack == NULL) {
        sample_print("malloc memory failed!\n");
        return HI_FAILURE;
    }
    stream.pack_cnt = stat.cur_packs;
    ret = hi_mpi_venc_get_stream(venc_chn, &stream, -1);
    if (ret != HI_SUCCESS) {
        sample_print("get_stream failed with %#x!\n", ret);
        free(stream.pack);
        stream.pack = NULL;
        return HI_FAILURE;
    }
    if (save_jpg || save_thm) {
        ret = sample_comm_save_snap_stream(stream, save_jpg, save_thm);
        if (ret != HI_SUCCESS) {
            sample_print("save_snap_stream failed!\n");
            return ret;
        }
    }

    ret = hi_mpi_venc_release_stream(venc_chn, &stream);
    if (ret != HI_SUCCESS) {
        sample_print("release_stream failed with %#x!\n", ret);

        free(stream.pack);
        stream.pack = NULL;
        return HI_FAILURE;
    }

    free(stream.pack);
    stream.pack = NULL;

    return ret;
}

/******************************************************************************
* funciton : snap process
******************************************************************************/
hi_s32 sample_comm_venc_snap_process(hi_venc_chn venc_chn, hi_u32 snap_cnt, hi_bool save_jpg, hi_bool save_thm)
{
    struct timeval timeout_val;
    fd_set read_fds;
    hi_s32 venc_fd;
    hi_s32 ret;
    hi_u32 i;

    /******************************************
     step 4:  recv picture
    ******************************************/
    venc_fd = hi_mpi_venc_get_fd(venc_chn);
    if (venc_fd < 0) {
        sample_print("venc_get_fd faild with%#x!\n", venc_fd);
        return HI_FAILURE;
    }

    for (i = 0; i < snap_cnt; i++) {
        FD_ZERO(&read_fds);
        FD_SET(venc_fd, &read_fds);
        timeout_val.tv_sec  = 10; // 10 : 10 seconds
        timeout_val.tv_usec = 0;
        ret = select(venc_fd + 1, &read_fds, NULL, NULL, &timeout_val);
        if (ret < 0) {
            sample_print("snap select failed!\n");
            return HI_FAILURE;
        } else if (ret == 0) {
            sample_print("snap time out!\n");
            return HI_FAILURE;
        } else {
            if (FD_ISSET(venc_fd, &read_fds)) {
                check_return(sample_comm_get_snap_stream(venc_chn, save_jpg, save_thm), "get_snap_stream");
            }
        }
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_venc_save_jpeg(hi_venc_chn venc_chn, hi_u32 snap_cnt)
{
    struct timeval timeout_val;
    fd_set read_fds;
    hi_s32 venc_fd;
    hi_s32 ret;
    hi_u32 i;
    /******************************************
     step:  recv picture
    ******************************************/
    venc_fd = hi_mpi_venc_get_fd(venc_chn);
    if (venc_fd < 0) {
        sample_print("venc_get_fd faild with%#x!\n", venc_fd);
        return HI_FAILURE;
    }
    for (i = 0; i < snap_cnt; i++) {
        FD_ZERO(&read_fds);
        FD_SET(venc_fd, &read_fds);
        timeout_val.tv_sec  = 10; // 10 : 10 seconds
        timeout_val.tv_usec = 0;
        ret = select(venc_fd + 1, &read_fds, NULL, NULL, &timeout_val);
        if (ret < 0) {
            sample_print("snap select failed!\n");
            return HI_FAILURE;
        } else if (ret == 0) {
            sample_print("snap time out!\n");
            return HI_FAILURE;
        } else {
            if (FD_ISSET(venc_fd, &read_fds)) {
                check_return(sample_comm_get_snap_stream(venc_chn, HI_TRUE, HI_FALSE), "get_snap_stream");
            }
        }
    }
    return HI_SUCCESS;
}

static hi_s32 sample_comm_alloc_qpmap_skipweight_memory(sample_venc_qpmap_sendframe_para *para,
    sample_comm_venc_frame_proc_info *addr_info)
{
    hi_venc_chn_attr venc_chn_attr;
    hi_u32 j;
    hi_s32 i, ret;
    hi_u8 *vir_addr = HI_NULL;
    hi_phys_addr_t phys_addr = 0;

    for (i = 0; (i < para->cnt) && (i < HI_VENC_MAX_CHN_NUM); i++) {
        hi_mpi_venc_get_chn_attr(para->venc_chn[i], &venc_chn_attr);

        addr_info->qpmap_size[i] = hi_venc_get_qpmap_size(venc_chn_attr.venc_attr.type,
            para->size[i].width, para->size[i].height);
        addr_info->skip_weight_size[i] =
            hi_venc_get_skip_weight_size(venc_chn_attr.venc_attr.type, para->size[i].width, para->size[i].height);

        /* alloc qpmap memory */
        ret = hi_mpi_sys_mmz_alloc((hi_phys_addr_t *)&phys_addr, (hi_void **)&vir_addr, HI_NULL, HI_NULL,
            addr_info->qpmap_size[i] * QPMAP_BUF_NUM);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_sys_mmz_alloc err:0x%x", ret);
            return ret;
        }
        for (j = 0; (i < VENC_QPMAP_MAX_CHN) && (j < QPMAP_BUF_NUM); j++) {
            if ((j > 0) && (addr_info->qpmap_size[i] > (UINT_MAX / j))) {
                sample_print("(j * addr_info->qpmap_size[%d]) upper limit of the multiplie\n", i);
                hi_mpi_sys_mmz_free(phys_addr, vir_addr);
                return HI_FAILURE;
            } else {
                addr_info->qpmap_phys_addr[i][j] = (hi_phys_addr_t)(phys_addr + j * addr_info->qpmap_size[i]);
                addr_info->qpmap_vir_addr[i][j] = vir_addr + j * addr_info->qpmap_size[i];
            }
        }

        /* alloc skipWeight memory */
        ret = hi_mpi_sys_mmz_alloc((hi_phys_addr_t *)&phys_addr, (hi_void **)&vir_addr, HI_NULL, HI_NULL,
            addr_info->skip_weight_size[i] * QPMAP_BUF_NUM);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_sys_mmz_alloc err:0x%x", ret);
            return ret;
        }
        for (j = 0; (i < VENC_QPMAP_MAX_CHN) && (j < QPMAP_BUF_NUM); j++) {
            if ((j > 0) && (addr_info->skip_weight_size[i] > (UINT_MAX / j))) {
                sample_print("(j * addr_info->skip_weight_size[%d]) upper limit of the multiplie\n", i);
                hi_mpi_sys_mmz_free(phys_addr, vir_addr);
                return HI_FAILURE;
            } else {
                addr_info->skip_weight_phys_addr[i][j] =
                    (hi_phys_addr_t)(phys_addr + j * addr_info->skip_weight_size[i]);
                addr_info->skip_weight_vir_addr[i][j] = vir_addr + j * addr_info->skip_weight_size[i];
            }
        }
    }

    return HI_SUCCESS;
}

static hi_void sample_comm_venc_vir_addr_temp(sample_comm_venc_frame_proc_info *addr_info, hi_u32 i, hi_u32 frame_id)
{
    hi_u32 j;
    hi_u8 *vir_addr_temp = HI_NULL;

    vir_addr_temp = (hi_u8 *)addr_info->qpmap_vir_addr[i][frame_id];
    for (j = 0; j < addr_info->qpmap_size[i]; j++) {
        *vir_addr_temp = 0x5E; // [7]:skip flag; [6]:QpType Flag; [5:0]:Qp value ==> Set absolute qp = 30
        vir_addr_temp++;
    }

    vir_addr_temp = (hi_u8 *)addr_info->skip_weight_vir_addr[i][frame_id];
    for (j = 0; j < addr_info->skip_weight_size[i]; j++) {
        *vir_addr_temp = 0x66; // inter block must be skip
        vir_addr_temp++;
    }
}


static hi_s32 sample_comm_qpmap_send_frame_ex(sample_venc_qpmap_sendframe_para *para,
    hi_venc_user_frame_info *frame_info, hi_video_frame_info *video_frame, hi_s32 index)
{
    hi_s32 ret;

    ret = hi_mpi_venc_send_frame_ex(para->venc_chn[index], frame_info, -1);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_venc_send_frame_ex err:0x%x\n", ret);

        ret = hi_mpi_vpss_release_chn_frame(para->vpss_grp, para->vpss_chn[index], video_frame);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_vpss_release_chn_frame err:0x%x", ret);
            return SAMPLE_RETURN_GOTO;
        }
        return SAMPLE_RETURN_BREAK;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_comm_qpmap_send_frame_start(sample_venc_qpmap_sendframe_para *para,
    sample_comm_venc_frame_proc_info *addr_info)
{
    hi_u32 frame_id = 0;
    hi_s32 i, ret;
    hi_venc_user_frame_info frame_info[VENC_QPMAP_MAX_CHN][QPMAP_BUF_NUM] = { 0 };

    while (para->thread_start == HI_TRUE) {
        for (i = 0; (i < para->cnt) && (i < VENC_QPMAP_MAX_CHN); i++) {
            hi_video_frame_info *video_frame = &frame_info[i][frame_id].user_frame;
            ret = hi_mpi_vpss_get_chn_frame(para->vpss_grp, para->vpss_chn[i],
                video_frame, 1000); /* 1000 is a number */
            if (ret != HI_SUCCESS) {
                sample_print("HI_MPI_VPSS_GetChnFrame err:0x%x\n", ret);
                continue;
            }

            sample_comm_venc_vir_addr_temp(addr_info, i, frame_id);

            frame_info[i][frame_id].user_rc_info.skip_weight_valid = 1;
            frame_info[i][frame_id].user_rc_info.skip_weight_phys_addr = addr_info->skip_weight_phys_addr[i][frame_id];
            frame_info[i][frame_id].user_rc_info.qpmap_valid = 1;
            frame_info[i][frame_id].user_rc_info.qpmap_phys_addr = addr_info->qpmap_phys_addr[i][frame_id];
            frame_info[i][frame_id].user_rc_info.blk_start_qp = 30; /* 30 is a number */
            frame_info[i][frame_id].user_rc_info.frame_type = HI_VENC_FRAME_TYPE_NONE;

            ret = sample_comm_qpmap_send_frame_ex(para, &frame_info[i][frame_id], video_frame, i);
            if (ret == SAMPLE_RETURN_BREAK) {
                break;
            } else if (ret == SAMPLE_RETURN_GOTO) {
                return SAMPLE_RETURN_GOTO;
            }

            ret = hi_mpi_vpss_release_chn_frame(para->vpss_grp, para->vpss_chn[i], video_frame);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_vpss_release_chn_frame err:0x%x", ret);
                return SAMPLE_RETURN_GOTO;
            }

            frame_id++;
            if (frame_id >= QPMAP_BUF_NUM) {
                frame_id = 0;
            }
        }
    }
    return HI_SUCCESS;
}

hi_void *sample_comm_qpmap_send_frame_proc(hi_void *p)
{
    hi_s32 i, ret;
    sample_venc_qpmap_sendframe_para *para = HI_NULL;
    sample_comm_venc_frame_proc_info addr_info = { 0 };
    hi_vpss_chn_attr vpss_chn_attr;

    para = (sample_venc_qpmap_sendframe_para *)p;

    if (para->cnt > VENC_QPMAP_MAX_CHN) {
        sample_print("Current func'sample_comm_qpmap_send_frame_proc' not support Venc channal num(%d) > %d\n",
            para->cnt, VENC_QPMAP_MAX_CHN);
        return HI_NULL;
    }

    if (sample_comm_alloc_qpmap_skipweight_memory(para, &addr_info) != HI_SUCCESS) {
        goto error;
    }

    /* set vpss buffer depth */
    for (i = 0; (i < para->cnt) && (i < HI_VPSS_MAX_PHYS_CHN_NUM); i++) {
        ret = hi_mpi_vpss_get_chn_attr(para->vpss_grp, para->vpss_chn[i], &vpss_chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_vpss_get_chn_attr err:0x%x", ret);
            goto error;
        }

        vpss_chn_attr.depth = 3; /* 3 is a number */
        ret = hi_mpi_vpss_set_chn_attr(para->vpss_grp, para->vpss_chn[i], &vpss_chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_vpss_set_chn_attr err:0x%x", ret);
            goto error;
        }
    }

        if (sample_comm_qpmap_send_frame_start(para, &addr_info) != HI_SUCCESS) {
            goto error;
        }

error:
    for (i = 0; (i < para->cnt) && (i < VENC_QPMAP_MAX_CHN); i++) {
        if (addr_info.qpmap_phys_addr[i][0] != 0) {
            ret = hi_mpi_sys_mmz_free(addr_info.qpmap_phys_addr[i][0], addr_info.qpmap_vir_addr[i][0]);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_sys_mmz_free err:0x%x", ret);
            }
        }

        if (addr_info.skip_weight_phys_addr[i][0] != 0) {
            ret = hi_mpi_sys_mmz_free(addr_info.skip_weight_phys_addr[i][0], addr_info.skip_weight_vir_addr[i][0]);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_sys_mmz_free err:0x%x", ret);
            }
        }
    }

    return HI_NULL;
}

hi_s32 sample_comm_venc_qpmap_send_frame(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn[], hi_venc_chn venc_chn[],
    hi_s32 cnt, hi_size size[])
{
    hi_s32 i;

    g_qpmap_send_frame_para.thread_start = HI_TRUE;
    g_qpmap_send_frame_para.vpss_grp = vpss_grp;
    g_qpmap_send_frame_para.cnt = cnt;

    for (i = 0; (i < cnt) && (i < HI_VENC_MAX_CHN_NUM) && (i < HI_VPSS_MAX_PHYS_CHN_NUM); i++) {
        g_qpmap_send_frame_para.venc_chn[i] = venc_chn[i];
        g_qpmap_send_frame_para.vpss_chn[i] = vpss_chn[i];
        g_qpmap_send_frame_para.size[i] = size[i];
    }

    return pthread_create(&g_venc_qpmap_pid, 0, sample_comm_qpmap_send_frame_proc, (hi_void *)&g_qpmap_send_frame_para);
}

#define SAMPLE_VENC_BLOCK_WIDTH 16
#define SAMPLE_VENC_BLOCK_HEIGHT 16
#define SAMPLE_VENC_ONE_BYTE_BLOCKS 4
#define SAMPLE_VENC_ONE_BLOCK_BITS 2
#define SAMPLE_VENC_MAX_JPEG_ROI_LEVEL 3

static hi_void sample_venc_set_2bits(hi_u8 *wp, hi_u32 idx, hi_u32 value)
{
    hi_u8 tmp1, tmp2;
    tmp1 = *wp;

    tmp2 = ~(3 << (idx * SAMPLE_VENC_ONE_BLOCK_BITS)); // 3: 0b11
    tmp1 &= tmp2;
    tmp2 = value << (idx * SAMPLE_VENC_ONE_BLOCK_BITS);
    tmp1 |= tmp2;
    *wp = tmp1;
}

static hi_void sample_venc_process_jpeg_roi_head(hi_u8 **wp, hi_u32 start_idx, hi_u32 fill_block, hi_u32 value)
{
    hi_u32 i;

    for (i = 0; i < fill_block; i++) {
        sample_venc_set_2bits(*wp, start_idx + i, value);
    }

    if (i != 0) {
        *wp = *wp + 1;
    }
}

static hi_void sample_venc_process_jpeg_roi_middle(hi_u8 **wp, hi_u32 write_byte, hi_u32 value)
{
    hi_s32 i;
    if (write_byte != 0) {
        for (i = 0; i < (SAMPLE_VENC_ONE_BYTE_BLOCKS - 1); i++) {
            value |= value << SAMPLE_VENC_ONE_BLOCK_BITS;
        }
        if (memset_s(*wp, write_byte, value, write_byte) != EOK) {
            printf("call memset_s error\n");
        }
        *wp = *wp + write_byte;
    }
}

static hi_void sample_venc_process_jpeg_roi_tail(hi_u8 **wp, hi_u32 fill_block, hi_u32 value)
{
    hi_u32 i;

    for (i = 0; i < fill_block; i++) {
        sample_venc_set_2bits(*wp, i, value);
    }
}

hi_void sample_venc_process_jpeg_roi(hi_void *virt_addr, hi_rect *rect, hi_u32 roi_level, hi_u32 stride)
{
    hi_u32 i, level, start_block, start_byte, start_supple_idx, start_supple_block, start_block_height;
    hi_u32 start_x, start_y, write_block, write_resi_block, write_byte, write_tail_block, write_block_height;

    start_x = rect->x;
    start_block = start_x / SAMPLE_VENC_BLOCK_WIDTH;
    start_byte = start_block / SAMPLE_VENC_ONE_BYTE_BLOCKS;
    start_supple_idx = start_block % SAMPLE_VENC_ONE_BYTE_BLOCKS;
    start_supple_block = (start_supple_idx == 0) ? 0 : (SAMPLE_VENC_ONE_BYTE_BLOCKS - start_supple_idx);

    write_block = rect->width / SAMPLE_VENC_BLOCK_WIDTH;
    start_supple_block = (start_supple_block < write_block) ? start_supple_block : write_block;
    write_resi_block = write_block - start_supple_block;
    write_byte = write_resi_block / SAMPLE_VENC_ONE_BYTE_BLOCKS;
    write_tail_block = write_resi_block % SAMPLE_VENC_ONE_BYTE_BLOCKS;

    start_y = rect->y;
    start_block_height = start_y / SAMPLE_VENC_BLOCK_HEIGHT;
    write_block_height = rect->height / SAMPLE_VENC_BLOCK_HEIGHT;
    level = roi_level;

    for (i = 0; i < write_block_height; i++) {
        hi_u8 *wp = (hi_u8 *)virt_addr + (start_block_height + i) * stride + start_byte;
        sample_venc_process_jpeg_roi_head(&wp, start_supple_idx, start_supple_block, level);
        sample_venc_process_jpeg_roi_middle(&wp, write_byte, level);
        sample_venc_process_jpeg_roi_tail(&wp, write_tail_block, level);
    }
}

#define SAMPLE_VENC_ROIMAP_MAX_CHN 2

static hi_s32 sample_comm_venc_send_frame_ex(sample_venc_roimap_frame_para *para, hi_venc_user_frame_info *frame_info,
    hi_video_frame_info *video_frame, hi_s32 index, hi_s32 venc_roimap_max_chn)
{
    hi_s32 ret;
    hi_unused(venc_roimap_max_chn);

    ret = hi_mpi_venc_send_frame_ex(para->venc_chn[index], &frame_info[index], -1);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_venc_send_frame_ex err: 0x%x\n", ret);

        ret = hi_mpi_vpss_release_chn_frame(para->vpss_grp, para->vpss_chn[index], video_frame);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_vpss_release_chn_frame err: 0x%x", ret);
            return SAMPLE_RETURN_GOTO;
        }
        return SAMPLE_RETURN_BREAK;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_comm_venc_send_roimap_frame_start(sample_venc_roimap_frame_para *para,
    hi_phys_addr_t *roimap_phys_addr)
{
    hi_s32 i, ret;
    hi_venc_user_frame_info frame_info[SAMPLE_VENC_ROIMAP_MAX_CHN] = {0};

    while (para->thread_start == HI_TRUE) {
        for (i = 0; (i < para->cnt) && (i < SAMPLE_VENC_ROIMAP_MAX_CHN) &&
                    (i < HI_VPSS_MAX_PHYS_CHN_NUM) && (i < HI_VENC_MAX_CHN_NUM); i++) {
            hi_video_frame_info *video_frame = &frame_info[i].user_frame;
            ret = hi_mpi_vpss_get_chn_frame(para->vpss_grp, para->vpss_chn[i],
                video_frame, 1000); /* 1000 is a number */
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_vpss_get_chn_frame err: 0x%x\n", ret);
                continue;
            }

            frame_info[i].user_roimap.valid = HI_TRUE;
            frame_info[i].user_roimap.phys_addr = roimap_phys_addr[i];

            ret = sample_comm_venc_send_frame_ex(para, frame_info, video_frame, i, SAMPLE_VENC_ROIMAP_MAX_CHN);
            if (ret == SAMPLE_RETURN_BREAK) {
                break;
            } else if (ret == SAMPLE_RETURN_GOTO) {
                return SAMPLE_RETURN_GOTO;
            }

            ret = hi_mpi_vpss_release_chn_frame(para->vpss_grp, para->vpss_chn[i], video_frame);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_vpss_release_chn_frame err: 0x%x", ret);
                return SAMPLE_RETURN_GOTO;
            }
        }
    }

    return HI_SUCCESS;
}

static hi_s32 sample_comm_set_vpss_buffer_depth(sample_venc_roimap_frame_para *para, hi_phys_addr_t *roimap_phys_addr)
{
    hi_s32 i, ret;
    hi_vpss_chn_attr vpss_chn_attr;

    for (i = 0; (i < para->cnt) && (i < HI_VPSS_MAX_PHYS_CHN_NUM); i++) {
        ret = hi_mpi_vpss_get_chn_attr(para->vpss_grp, para->vpss_chn[i], &vpss_chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_vpss_get_chn_attr err: 0x%x", ret);
            return SAMPLE_RETURN_GOTO;
        }

        vpss_chn_attr.depth = 3; // 3 : depth
        ret = hi_mpi_vpss_set_chn_attr(para->vpss_grp, para->vpss_chn[i], &vpss_chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_vpss_set_chn_attr err: 0x%x", ret);
            return SAMPLE_RETURN_GOTO;
        }
    }
    ret = sample_comm_venc_send_roimap_frame_start(para, roimap_phys_addr);
    if (ret != HI_SUCCESS) {
        return SAMPLE_RETURN_GOTO;
    }

    return HI_SUCCESS;
}

hi_void *sample_comm_send_roimap_frame_proc(hi_void *p)
{
    hi_s32 i, j, ret;
    hi_u8 init_level = 2;

    hi_u32 roimap_size[SAMPLE_VENC_ROIMAP_MAX_CHN], roimap_stride[SAMPLE_VENC_ROIMAP_MAX_CHN];
    hi_phys_addr_t roimap_phys_addr[SAMPLE_VENC_ROIMAP_MAX_CHN] = {0};
    hi_void *roimap_virt_addr[SAMPLE_VENC_ROIMAP_MAX_CHN] = {0};

    hi_u8 *virt_addr = HI_NULL;
    hi_phys_addr_t phys_addr = HI_NULL;

    hi_venc_chn_attr venc_chn_attr;

    sample_venc_roimap_frame_para *para = (sample_venc_roimap_frame_para *)p;

    if (para->cnt > SAMPLE_VENC_ROIMAP_MAX_CHN) {
        sample_print("Current not support venc channal num(%d) > %d\n", para->cnt, SAMPLE_VENC_ROIMAP_MAX_CHN);
        return HI_NULL;
    }

    for (i = 0; (i < para->cnt) && (i < HI_VENC_MAX_CHN_NUM) && (i < SAMPLE_VENC_ROIMAP_MAX_CHN); i++) {
        hi_mpi_venc_get_chn_attr(para->venc_chn[i], &venc_chn_attr);

        roimap_size[i] =
            hi_venc_get_roimap_size(venc_chn_attr.venc_attr.type, para->size[i].width, para->size[i].width);
        roimap_stride[i] = hi_venc_get_roimap_stride(venc_chn_attr.venc_attr.type, para->size[i].width);

        /* alloc roimap memory */
        ret = hi_mpi_sys_mmz_alloc(&phys_addr, (hi_void **)&virt_addr, HI_NULL, HI_NULL, roimap_size[i]);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_sys_mmz_alloc err: 0x%x", ret);
            return HI_NULL;
        }

        roimap_phys_addr[i] = phys_addr;
        roimap_virt_addr[i] = virt_addr;

        for (j = 0; j < (4 - 1); j++) { /* 4 is a number */
            init_level |= (init_level << 4); /* 4 is a number */
        }
        if (memset_s(roimap_virt_addr[i], roimap_size[i], init_level, roimap_size[i]) != EOK) {
            printf("call memset_s error\n");
        }
        sample_venc_process_jpeg_roi(roimap_virt_addr[i], &para->roi_attr[i].rect, para->roi_attr[i].level,
            roimap_stride[i]);
    }

    /* set vpss buffer depth */
    if (sample_comm_set_vpss_buffer_depth(para, roimap_phys_addr) != HI_SUCCESS) {
        goto error;
    }

error:
    for (i = 0; (i < para->cnt) && (i < SAMPLE_VENC_ROIMAP_MAX_CHN); i++) {
        if (roimap_phys_addr[i] != 0) {
            ret = hi_mpi_sys_mmz_free(roimap_phys_addr[i], roimap_virt_addr[i]);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_sys_mmz_free err: 0x%x", ret);
            }
        }
    }

    return HI_NULL;
}

hi_s32 sample_comm_venc_send_roimap_frame(hi_vpss_grp vpss_grp, sample_venc_roimap_chn_info roimap_chn_info,
    hi_size size[], hi_venc_jpeg_roi_attr roi_attr[])
{
    hi_s32 i;

    g_roimap_frame_param.thread_start = HI_TRUE;
    g_roimap_frame_param.vpss_grp = vpss_grp;
    g_roimap_frame_param.cnt = roimap_chn_info.cnt;

    for (i = 0; (i < roimap_chn_info.cnt) && (i < HI_VENC_MAX_CHN_NUM) && (i < HI_VPSS_MAX_PHYS_CHN_NUM); i++) {
        g_roimap_frame_param.venc_chn[i] = roimap_chn_info.venc_chn[i];
        g_roimap_frame_param.vpss_chn[i] = roimap_chn_info.vpss_chn[i];
        g_roimap_frame_param.size[i] = size[i];
        g_roimap_frame_param.roi_attr[i] = roi_attr[i];
    }

    return pthread_create(&g_venc_roimap_pid, 0, sample_comm_send_roimap_frame_proc, (hi_void *)&g_roimap_frame_param);
}

static hi_s32 sample_comm_set_file_name(hi_s32 index, hi_venc_chn venc_chn,
    sample_comm_venc_stream_proc_info *stream_proc_info)
{
    if (snprintf_s(stream_proc_info->file_name[index], FILE_NAME_LEN, FILE_NAME_LEN - 1, "./") < 0) {
        return SAMPLE_RETURN_NULL;
    }

    if (realpath(stream_proc_info->file_name[index], stream_proc_info->real_file_name[index]) == HI_NULL) {
        sample_print("chn[%d] stream file path error\n", venc_chn);
        return SAMPLE_RETURN_NULL;
    }

    if (snprintf_s(stream_proc_info->real_file_name[index], FILE_NAME_LEN, FILE_NAME_LEN - 1,
        "stream_chn%d%s", index, stream_proc_info->file_postfix) < 0) {
        return SAMPLE_RETURN_NULL;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_comm_set_name_save_stream(sample_comm_venc_stream_proc_info *stream_proc_info,
    hi_venc_stream_buf_info *stream_buf_info, hi_payload_type *payload_type,
    sample_venc_getstream_para *para, hi_s32 venc_max_chn)
{
    hi_s32 i, ret, fd;
    hi_venc_chn_attr venc_chn_attr;
    hi_unused(venc_max_chn);

    for (i = 0; (i < stream_proc_info->chn_total) && (i < HI_VENC_MAX_CHN_NUM); i++) {
        /* decide the stream file name, and open file to save stream */
        hi_venc_chn venc_chn = para->venc_chn[i];
        ret = hi_mpi_venc_get_chn_attr(venc_chn, &venc_chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_venc_get_chn_attr chn[%d] failed with %#x!\n", venc_chn, ret);
            return SAMPLE_RETURN_NULL;
        }
        payload_type[i] = venc_chn_attr.venc_attr.type;

        ret = sample_comm_venc_get_file_postfix(payload_type[i], stream_proc_info->file_postfix,
            sizeof(stream_proc_info->file_postfix));
        if (ret != HI_SUCCESS) {
            sample_print("sample_comm_venc_get_file_postfix [%d] failed with %#x!\n",
                venc_chn_attr.venc_attr.type, ret);
            return SAMPLE_RETURN_NULL;
        }

        if(0)//maohw if (payload_type[i] != HI_PT_JPEG) 
		{
            ret = sample_comm_set_file_name(i, venc_chn, stream_proc_info);
            if (ret != HI_SUCCESS) {
                return ret;
            }

            stream_proc_info->file[i] = fopen(stream_proc_info->real_file_name[i], "wb");
            if (!stream_proc_info->file[i]) {
                sample_print("open file[%s] failed!\n", stream_proc_info->real_file_name[i]);
                return SAMPLE_RETURN_NULL;
            }
            fd = fileno(stream_proc_info->file[i]);
            fchmod(fd, S_IRUSR | S_IWUSR);
        }
        /* set venc fd. */
        stream_proc_info->venc_fd[i] = hi_mpi_venc_get_fd(/*maohw i*/venc_chn);
        if (stream_proc_info->venc_fd[i] < 0) {
            sample_print("hi_mpi_venc_get_fd failed venc_chn:%d!\n", venc_chn);
            return SAMPLE_RETURN_NULL;
        }
        
        printf("%s => venc_chn:%d venc_fd:%d\n", __func__, venc_chn, stream_proc_info->venc_fd[i]);
        
        if (stream_proc_info->maxfd <= stream_proc_info->venc_fd[i]) {
            stream_proc_info->maxfd = stream_proc_info->venc_fd[i];
        }

        ret = hi_mpi_venc_get_stream_buf_info(/*maohw i*/venc_chn, &stream_buf_info[i]);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_venc_get_stream_buf_info failed with %#x!\n", ret);
            return SAMPLE_RETURN_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static int32_t sample_heif_create(hi_s32 index, const sample_comm_venc_stream_proc_info *stream_proc_info,
    heif_handle *hdl)
{
    heif_config config;
    if (snprintf_s(config.file_desc.input.url, FILE_NAME_LEN, FILE_NAME_LEN - 1,
        "./stream_chn%d_%d%s", index, stream_proc_info->picture_cnt[index], ".heic") < 0) {
        return SAMPLE_RETURN_NULL;
    }
    config.file_desc.file_type = HEIF_FILE_TYPE_URL;
    config.config_type = HEIF_CONFIG_MUXER;
    config.muxer_config.is_grid = false;
    config.muxer_config.row_image_num = 1;
    config.muxer_config.column_image_num = 1;
    config.muxer_config.format_profile = HEIF_PROFILE_HEIC;
    return heif_create(hdl, &config);
}

static hi_s32 sample_comm_save_h265_to_heic(hi_s32 index, const sample_comm_venc_stream_proc_info *stream_proc_info,
    const hi_venc_stream *stream)
{
    hi_u32 i;
    hi_u32 total_len = 0;
    hi_s32 has_key = 0;
    for (i = 0; i < stream->pack_cnt; i++) {
        if (stream->pack[i].data_type.h265_type == OT_VENC_H265_NALU_IDR_SLICE) {
            has_key = 1;
        }
        total_len += stream->pack[i].len - stream->pack[i].offset;
    }
    if (total_len > 0 && has_key == 1) {
        heif_handle handle = NULL;
        hi_s32 ret = sample_heif_create(index, stream_proc_info, &handle);
        if (ret != 0) {
            sample_print("HeifCreate error ret:%d\n", ret);
        }
        hi_u8 *data_buffer = (hi_u8 *)malloc(total_len);
        if (data_buffer == NULL) {
            sample_print("malloc error\n");
            heif_destroy(handle);
            return SAMPLE_RETURN_NULL;
        }
        hi_u32 write_len = 0;
        for (i = 0; i < stream->pack_cnt; i++) {
            if (memcpy_s(data_buffer + write_len, total_len - write_len,
                stream->pack[i].addr + stream->pack[i].offset, stream->pack[i].len - stream->pack[i].offset) != EOK) {
                sample_print("memcpy_s failed\n");
            }
            write_len += stream->pack[i].len;
        }
        heif_image_item item = {0};
        item.timestamp = -1;
        item.data = data_buffer;
        item.length = write_len;
        item.key_frame = true;
        ret = heif_write_master_image(handle, 0, &item, 1);
        if (data_buffer != NULL) {
            free(data_buffer);
        }
        heif_destroy(handle);
        return ret;
    }
    return 0;
}

static hi_s32 sample_comm_save_frame_to_file(hi_s32 index, sample_comm_venc_stream_proc_info *stream_proc_info,
    hi_venc_stream *stream, hi_venc_stream_buf_info *stream_buf_info, hi_payload_type *payload_type)
{
    hi_s32 ret, fd;
    if (payload_type[index] == HI_PT_JPEG) {
        if (snprintf_s(stream_proc_info->file_name[index], FILE_NAME_LEN, FILE_NAME_LEN - 1, "./") < 0) {
            free(stream->pack);
            return SAMPLE_RETURN_NULL;
        }
        if (realpath(stream_proc_info->file_name[index], stream_proc_info->real_file_name[index]) == HI_NULL) {
            free(stream->pack);
            sample_print("chn[%d] stream file path error\n", stream_proc_info->venc_chn);
            return SAMPLE_RETURN_NULL;
        }

        if (snprintf_s(stream_proc_info->real_file_name[index], FILE_NAME_LEN, FILE_NAME_LEN - 1,
            "stream_chn%d_%d%s", index, stream_proc_info->picture_cnt[index], stream_proc_info->file_postfix) < 0) {
            free(stream->pack);
            return SAMPLE_RETURN_NULL;
        }
        stream_proc_info->file[index] = fopen(stream_proc_info->real_file_name[index], "wb");
        if (!stream_proc_info->file[index]) {
            free(stream->pack);
            sample_print("open file err!\n");
            return SAMPLE_RETURN_NULL;
        }
        fd = fileno(stream_proc_info->file[index]);
        fchmod(fd, S_IRUSR | S_IWUSR);
    }

    if (payload_type[index] == HI_PT_H265 && stream_proc_info->save_heif == HI_TRUE) {
        (hi_void)sample_comm_save_h265_to_heic(index, stream_proc_info, stream);
    }
#ifndef __LITEOS__
    hi_unused(stream_buf_info);
    ret = sample_comm_venc_save_stream(stream_proc_info->file[index], stream);
#else
    ret = sample_comm_venc_save_stream_phys_addr(stream_proc_info->file[index], &stream_buf_info[index], stream);
#endif
    if (ret != HI_SUCCESS) {
        free(stream->pack);
        stream->pack = HI_NULL;
        sample_print("save stream failed!\n");
        return SAMPLE_RETURN_BREAK;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_comm_get_stream_from_one_channl(sample_comm_venc_stream_proc_info *stream_proc_info,
    hi_s32 index, hi_venc_stream_buf_info *stream_buf_info, hi_payload_type *payload_type)
{
    hi_s32 ret;
    hi_venc_stream stream;
    hi_venc_chn_status stat;

    /* step 2.1 : query how many packs in one-frame stream. */
    if (memset_s(&stream, sizeof(stream), 0, sizeof(stream)) != EOK) {
        printf("call memset_s error\n");
    }

    ret = hi_mpi_venc_query_status(/*maohw index*/stream_proc_info->venc_chn, &stat);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_venc_query_status chn[%d] failed with %#x!\n", stream_proc_info->venc_chn, ret);
        return SAMPLE_RETURN_BREAK;
    }

    if (stat.cur_packs == 0) {
        sample_print("NOTE: current  frame is HI_NULL!\n");
        return SAMPLE_RETURN_CONTINUE;
    }
    /* step 2.3 : malloc corresponding number of pack nodes. */
    stream.pack = (hi_venc_pack *)malloc(sizeof(hi_venc_pack) * stat.cur_packs);
    if (stream.pack == HI_NULL) {
        sample_print("malloc stream pack failed!\n");
        return SAMPLE_RETURN_BREAK;
    }

    /* step 2.4 : call mpi to get one-frame stream */
    stream.pack_cnt = stat.cur_packs;
    ret = hi_mpi_venc_get_stream(/*maohw index*/stream_proc_info->venc_chn, &stream, HI_TRUE);
    if (ret != HI_SUCCESS) {
        free(stream.pack);
        stream.pack = HI_NULL;
        sample_print("hi_mpi_venc_get_stream failed with %#x!\n", ret);
        return SAMPLE_RETURN_BREAK;
    }

    // maohw
    //printf("%s => venc_chn:%d, stream.seq:%u\n", __func__, stream_proc_info->venc_chn, stream.seq);
    if(g_para.cb)
    {
      g_para.cb(g_para.venc_chn[index], payload_type[index], &stream, g_para.uargs);
    }
    
    #if 0 //maohw
    /* step 2.5 : save frame to file */
    ret = sample_comm_save_frame_to_file(index, stream_proc_info, &stream, stream_buf_info, payload_type);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    #endif

    /* step 2.6 : release stream */
    ret = hi_mpi_venc_release_stream(/*maohw index*/stream_proc_info->venc_chn, &stream);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_venc_release_stream failed!\n");
        free(stream.pack);
        stream.pack = HI_NULL;
        return SAMPLE_RETURN_BREAK;
    }

    /* step 2.7 : free pack nodes */
    free(stream.pack);
    stream.pack = HI_NULL;
    stream_proc_info->picture_cnt[index]++;
    if (payload_type[index] == HI_PT_JPEG) {
        fclose(stream_proc_info->file[index]);
    }

    return HI_SUCCESS;
}


static hi_void sample_comm_fd_isset(sample_comm_venc_stream_proc_info *stream_proc_info, fd_set *read_fds,
    hi_venc_stream_buf_info *stream_buf_info, hi_payload_type *payload_type, sample_venc_getstream_para *para)
{
    hi_s32 i, ret;

    for (i = 0; (i < stream_proc_info->chn_total) && (i < HI_VENC_MAX_CHN_NUM); i++) {
        if (FD_ISSET(stream_proc_info->venc_fd[i], read_fds)) {
            stream_proc_info->venc_chn = para->venc_chn[i];
            ret = sample_comm_get_stream_from_one_channl(stream_proc_info, i, stream_buf_info, payload_type);
            if (ret == SAMPLE_RETURN_CONTINUE) {
                continue;
            } else if (ret == SAMPLE_RETURN_BREAK) {
                break;
            }
        }
    }
}


/* get stream from each channels and save them */
hi_void *sample_comm_venc_get_venc_stream_proc(hi_void *p)
{
    hi_s32 i, ret;
    sample_venc_getstream_para *para = HI_NULL;
    struct timeval timeout_val;
    fd_set read_fds;
    hi_payload_type payload_type[HI_VENC_MAX_CHN_NUM] = {0}; // sample_comm_set_name_save_stream;
    hi_venc_stream_buf_info stream_buf_info[HI_VENC_MAX_CHN_NUM];
    sample_comm_venc_stream_proc_info stream_proc_info = {0};

    prctl(PR_SET_NAME, "get_venc_stream", 0, 0, 0);

    para = (sample_venc_getstream_para *)p;
    stream_proc_info.chn_total = para->cnt;
    stream_proc_info.save_heif = para->save_heif;
    /* step 1:  check & prepare save-file & venc-fd */
    if (stream_proc_info.chn_total >= HI_VENC_MAX_CHN_NUM) {
        sample_print("input count invalid\n");
        return HI_NULL;
    }

    ret = sample_comm_set_name_save_stream(&stream_proc_info, stream_buf_info, payload_type, para, HI_VENC_MAX_CHN_NUM);
    if (ret == SAMPLE_RETURN_NULL) {
        return HI_NULL;
    } else if (ret == SAMPLE_RETURN_FAILURE) {
        return (void *)HI_FAILURE;
    }

    /* step 2:  start to get streams of each channel. */
    while (para->thread_start == HI_TRUE) {
        FD_ZERO(&read_fds);
        for (i = 0; (i < stream_proc_info.chn_total) && (i < HI_VENC_MAX_CHN_NUM); i++) {
            FD_SET(stream_proc_info.venc_fd[i], &read_fds);
        }

        timeout_val.tv_sec = 2; /* 2 is a number */
        timeout_val.tv_usec = 0;
        ret = select(stream_proc_info.maxfd + 1, &read_fds, HI_NULL, HI_NULL, &timeout_val);
        if (ret < 0) {
            sample_print("select failed!\n");
            break;
        } else if (ret == 0) {
            sample_print("get venc stream time out, continue.\n");
            continue;
        } else {
            sample_comm_fd_isset(&stream_proc_info, &read_fds, stream_buf_info, payload_type, para);
        }
    }
    
    #if 0 //maohw
    /* step 3 : close save-file */
    for (i = 0; i < stream_proc_info.chn_total; i++) {
        if (payload_type[i] != HI_PT_JPEG) {
            fclose(stream_proc_info.file[i]);
        }
    }
    #endif
    
    return HI_NULL;
}

/******************************************************************************
* function : bitrate_auto
******************************************************************************/
#define SAMPLE_VENC_WIDHT  640
#define SAMPLE_VENC_HEIGHT 480
#define SAMPLE_VENC_NUM 5
#define SAMPLE_VENC_FG_TYPE 5
#define QUERY_SLEEP   1000

hi_void sample_comm_venc_set_region(hi_u64 time, hi_venc_chn venc_chn, hi_venc_svc_rect_info *pst_svc_rect)
{
    hi_s32 j, ret;
    hi_u32 attrx[SAMPLE_VENC_NUM] = {32, 96, 128, 192, 256}; // 32 96 128 192 256 : X-coordinate
    hi_u32 attry[SAMPLE_VENC_NUM] = {32, 96, 128, 192, 256}; // 32 96 128 192 256 : Y-coordinate
    hi_u32 attrw[SAMPLE_VENC_NUM] = {32, 64, 96, 96, 64}; // 32 64 96 96 64 : width
    hi_u32 attrh[SAMPLE_VENC_NUM] = {32, 64, 96, 96, 64}; // 32 64 96 96 64 : height
    hi_u32 type[SAMPLE_VENC_NUM] = {0, 3, 4, 1, 2}; // 0 3 4 1 2 : type
    pst_svc_rect->rect_num = SAMPLE_VENC_NUM;
    pst_svc_rect->pts = time;
    pst_svc_rect->base_resolution.width = SAMPLE_VENC_WIDHT;
    pst_svc_rect->base_resolution.height = SAMPLE_VENC_HEIGHT;

    for (j = 0; (j < SAMPLE_VENC_NUM) && (j < HI_VENC_MAX_SVC_RECT_NUM); j++) {
        pst_svc_rect->detect_type[j] = type[j];
        pst_svc_rect->rect_attr[j].x = attrx[j];
        pst_svc_rect->rect_attr[j].y = attry[j];
        pst_svc_rect->rect_attr[j].width = attrw[j];
        pst_svc_rect->rect_attr[j].height = attrh[j];
    }
    ret = hi_mpi_venc_send_svc_region(venc_chn, pst_svc_rect);
    if (ret != HI_SUCCESS) {
        sample_print("Set hi_mpi_venc_send_svc_region failed for %#x chn =%d\n", ret, venc_chn);
    }
}
hi_void sample_comm_venc_set_svc_param(hi_venc_chn venc_chn, hi_venc_svc_param *pst_svc_param)
{
    hi_s32 j, ret;
    hi_u32 qp_i[SAMPLE_VENC_NUM] = {2, 62, 94, 1, 0}; // 2 62 94 1 0 : fg I frame qp
    hi_u32 qp_p[SAMPLE_VENC_NUM] = {4, 58, 94, 2, 0}; // 4 58 94 2 0 : fg P frame qp
    ret = hi_mpi_venc_get_svc_param(venc_chn, pst_svc_param);
    if (ret != HI_SUCCESS) {
        sample_print("Set hi_mpi_venc_set_svc_param failed for %#x chn =%d\n", ret, venc_chn);
    }
    pst_svc_param->fg_protect_adaptive_en = HI_TRUE;
    pst_svc_param->activity_region.qpmap_value_i = 0;
    pst_svc_param->activity_region.qpmap_value_p = 0;
    pst_svc_param->activity_region.skipmap_value = 0;
    pst_svc_param->bg_region.qpmap_value_i = 2;  // 2 :i frame bg qp
    pst_svc_param->bg_region.qpmap_value_p = 6; // 6 :p frame bg qp
    pst_svc_param->bg_region.skipmap_value = 0;
    for (j = 0; (j < SAMPLE_VENC_NUM) && (j < SVC_RECT_TYPE_BUTT); j++) {
        pst_svc_param->fg_region[j].qpmap_value_i = qp_i[j];
        pst_svc_param->fg_region[j].qpmap_value_p = qp_p[j];
        pst_svc_param->fg_region[j].skipmap_value = 0;
    }
    ret = hi_mpi_venc_set_svc_param(venc_chn, pst_svc_param);
    if (ret != HI_SUCCESS) {
        sample_print("Set hi_mpi_venc_set_svc_param failed for %#x!\n", ret);
    }
}

hi_void *sample_comm_venc_rateauto_stream_proc(hi_void *p)
{
    hi_s32 i, ret;
    sample_venc_rateauto_para *para;
    hi_venc_svc_param svc_param;
    hi_vpss_chn_attr  vpss_chn_attr;
    hi_venc_svc_rect_info  svc_rect_info = {0};
    para = (sample_venc_rateauto_para *)p;
    hi_video_frame_info video_frame;
    prctl(PR_SET_NAME, "get_venc_rateauto_stream", 0, 0, 0);
    if (para->cnt >= HI_VENC_MAX_CHN_NUM) {
        sample_print("input count invalid\n");
        return HI_NULL;
    }
    for (i = 0; (i < para->cnt) && (i < HI_VENC_MAX_CHN_NUM); i++) {
        ret = hi_mpi_vpss_get_chn_attr(para->vpss_grp, para->vpss_chn[i], &vpss_chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_vpss_get_chn_attr err: 0x%x", ret);
        }

        vpss_chn_attr.depth = 3; /* 3 is a number */
        ret = hi_mpi_vpss_set_chn_attr(para->vpss_grp, para->vpss_chn[i], &vpss_chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_vpss_set_chn_attr err: 0x%x", ret);
        }
    }

    while (para->thread_start == HI_TRUE) {
        for (i = 0; (i < para->cnt) && (i < HI_VENC_MAX_CHN_NUM); i++) {
            ret = hi_mpi_vpss_get_chn_frame(para->vpss_grp, para->vpss_chn[i], &video_frame, QUERY_SLEEP);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_vpss_get_chn_frame err:0x%x venc_chn = %d\n", ret, para->venc_chn[i]);
                continue;
            }
            ret = hi_mpi_vpss_release_chn_frame(para->vpss_grp, para->vpss_chn[i], &video_frame);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_vpss_release_chn_frame err:0x%x\n", ret);
                return HI_NULL;
            }

            ret = hi_mpi_venc_enable_svc(para->venc_chn[i], HI_TRUE);
            if (ret != HI_SUCCESS) {
                sample_print("Set hi_mpi_venc_enable_svc failed for %#x!\n", ret);
                return HI_NULL;
            }
            sample_comm_venc_set_region(video_frame.video_frame.pts, para->venc_chn[i], &svc_rect_info);
            sample_comm_venc_set_svc_param(para->venc_chn[i], &svc_param);
        }
        usleep(QUERY_SLEEP);
    }
    return HI_NULL;
}

static hi_s32 sample_comm_set_file_name_svc_t(sample_comm_venc_stream_proc_info *stream_proc_info, hi_s32 index)
{
    if (snprintf_s(stream_proc_info->file_name[index], FILE_NAME_LEN, FILE_NAME_LEN - 1, "./") < 0) {
        return HI_NULL;
    }
    if (realpath(stream_proc_info->file_name[index], stream_proc_info->real_file_name[index]) == HI_NULL) {
        printf("file path error\n");
        return HI_NULL;
    }
    if (snprintf_s(stream_proc_info->real_file_name[index], FILE_NAME_LEN, FILE_NAME_LEN - 1,
        "tid%d%s", index, stream_proc_info->file_postfix) < 0) {
        return HI_NULL;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_comm_set_name_save_stream_svc_t(sample_comm_venc_stream_proc_info *stream_proc_info,
    hi_venc_stream_buf_info *stream_buf_info, hi_s32 venc_max_chn)
{
    hi_s32 i, ret, cnt, fd;
    hi_venc_chn_attr venc_chn_attr;
    hi_payload_type payload_type[HI_VENC_MAX_CHN_NUM];
    hi_unused(venc_max_chn);

    for (i = 0; i < stream_proc_info->chn_total; i++) {
        /* decide the stream file name, and open file to save stream */
        stream_proc_info->venc_chn = i;
        ret = hi_mpi_venc_get_chn_attr(stream_proc_info->venc_chn, &venc_chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_venc_get_chn_attr chn[%d] failed with %#x!\n", stream_proc_info->venc_chn, ret);
            return SAMPLE_RETURN_NULL;
        }
        payload_type[i] = venc_chn_attr.venc_attr.type;

        ret = sample_comm_venc_get_file_postfix(payload_type[i], stream_proc_info->file_postfix,
            sizeof(stream_proc_info->file_postfix));
        if (ret != HI_SUCCESS) {
            sample_print("sample_comm_venc_get_file_postfix [%d] failed with %#x!\n",
                venc_chn_attr.venc_attr.type, ret);
            return SAMPLE_RETURN_NULL;
        }

        for (cnt = 0; cnt < 3; cnt++) { /* 3 is a number */
            if (sample_comm_set_file_name_svc_t(stream_proc_info, (i + cnt)) != HI_SUCCESS) {
                return SAMPLE_RETURN_NULL;
            }

            stream_proc_info->file[i + cnt] = fopen(stream_proc_info->real_file_name[i + cnt], "wb");
            if (!stream_proc_info->file[i + cnt]) {
                sample_print("open file[%s] failed!\n", stream_proc_info->real_file_name[i + cnt]);
                return SAMPLE_RETURN_NULL;
            }
            fd = fileno(stream_proc_info->file[i + cnt]);
            fchmod(fd, S_IRUSR | S_IWUSR);
        }

        /* set venc fd. */
        stream_proc_info->venc_fd[i] = hi_mpi_venc_get_fd(i);
        if (stream_proc_info->venc_fd[i] < 0) {
            sample_print("hi_mpi_venc_get_fd failed with %#x!\n", stream_proc_info->venc_fd[i]);
            return SAMPLE_RETURN_NULL;
        }
        if (stream_proc_info->maxfd <= stream_proc_info->venc_fd[i]) {
            stream_proc_info->maxfd = stream_proc_info->venc_fd[i];
        }
        ret = hi_mpi_venc_get_stream_buf_info(i, &stream_buf_info[i]);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_venc_get_stream_buf_info failed with %#x!\n", ret);
            return SAMPLE_RETURN_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_void sample_comm_save_frame_to_file_svc_t(hi_s32 index, sample_comm_venc_stream_proc_info *stream_proc_info,
    hi_venc_stream *stream, hi_venc_stream_buf_info *stream_buf_info)
{
    hi_s32 cnt, ret;

    for (cnt = 0; cnt < 3; cnt++) { /* 3 is a number */
        switch (cnt) {
            case 0: /* 0 is a number */
                if (stream->h264_info.ref_type == HI_VENC_BASE_IDR_SLICE ||
                    stream->h264_info.ref_type == HI_VENC_BASE_P_SLICE_REF_BY_BASE) {
#ifndef __LITEOS__
                    hi_unused(stream_buf_info);
                    ret = sample_comm_venc_save_stream(stream_proc_info->file[index + cnt], stream);
#else
                    ret = sample_comm_venc_save_stream_phys_addr(stream_proc_info->file[index + cnt],
                        &stream_buf_info[index], stream);
#endif
                }
                break;
            case 1: /* 1 is a number */
                if (stream->h264_info.ref_type == HI_VENC_BASE_IDR_SLICE ||
                    stream->h264_info.ref_type == HI_VENC_BASE_P_SLICE_REF_BY_BASE ||
                    stream->h264_info.ref_type == HI_VENC_BASE_P_SLICE_REF_BY_ENHANCE) {
#ifndef __LITEOS__
                    ret = sample_comm_venc_save_stream(stream_proc_info->file[index + cnt], stream);
#else
                    ret = sample_comm_venc_save_stream_phys_addr(stream_proc_info->file[index + cnt],
                        &stream_buf_info[index], stream);
#endif
                }
                break;
            case 2: /* 2 is a number */
#ifndef __LITEOS__
                ret = sample_comm_venc_save_stream(stream_proc_info->file[index + cnt], stream);
#else
                ret = sample_comm_venc_save_stream_phys_addr(stream_proc_info->file[index + cnt],
                    &stream_buf_info[index], stream);
#endif
                break;
            default:
                break;
        }
        if (ret != HI_SUCCESS) {
            free(stream->pack);
            stream->pack = HI_NULL;
            sample_print("save stream failed!\n");
            break;
        }
    }
}

static hi_s32 sample_comm_get_stream_from_one_channl_svc_t(sample_comm_venc_stream_proc_info *stream_proc_info,
    hi_s32 index, hi_venc_stream_buf_info *stream_buf_info)
{
    hi_s32 ret;
    hi_venc_stream stream;
    hi_venc_chn_status stat;

    /* step 2.1 : query how many packs in one-frame stream. */
    if (memset_s(&stream, sizeof(stream), 0, sizeof(stream)) != EOK) {
        printf("call memset_s error\n");
    }
    ret = hi_mpi_venc_query_status(index, &stat);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_venc_query chn[%d] failed with %#x!\n", index, ret);
        return SAMPLE_RETURN_BREAK;
    }

    if (stat.cur_packs == 0) {
        sample_print("NOTE: current  frame is HI_NULL!\n");
        return SAMPLE_RETURN_CONTINUE;
    }
    /* step 2.3 : malloc corresponding number of pack nodes. */
    stream.pack = (hi_venc_pack *)malloc(sizeof(hi_venc_pack) * stat.cur_packs);
    if (stream.pack == HI_NULL) {
        sample_print("malloc stream pack failed!\n");
        return SAMPLE_RETURN_BREAK;
    }
    /* step 2.4 : call mpi to get one-frame stream */
    stream.pack_cnt = stat.cur_packs;
    ret = hi_mpi_venc_get_stream(index, &stream, HI_TRUE);
    if (ret != HI_SUCCESS) {
        free(stream.pack);
        stream.pack = HI_NULL;
        sample_print("hi_mpi_venc_get_stream failed with %#x!\n", ret);
        return SAMPLE_RETURN_BREAK;
    }
    /* step 2.5 : save frame to file */
    sample_comm_save_frame_to_file_svc_t(index, stream_proc_info, &stream, stream_buf_info);

    /* step 2.6 : release stream */
    ret = hi_mpi_venc_release_stream(index, &stream);
    if (ret != HI_SUCCESS) {
        free(stream.pack);
        stream.pack = HI_NULL;
        return SAMPLE_RETURN_BREAK;
    }
    /* step 2.7 : free pack nodes */
    free(stream.pack);
    stream.pack = HI_NULL;

    return HI_SUCCESS;
}

static hi_void sample_comm_fd_isset_svc_t(sample_comm_venc_stream_proc_info *stream_proc_info,
    fd_set *read_fds, hi_venc_stream_buf_info *stream_buf_info, hi_s32 venc_max_chn)
{
    hi_s32 i, ret;
    hi_unused(venc_max_chn);

    for (i = 0; i < stream_proc_info->chn_total; i++) {
        if (FD_ISSET(stream_proc_info->venc_fd[i], read_fds)) {
            ret = sample_comm_get_stream_from_one_channl_svc_t(stream_proc_info, i, stream_buf_info);
            if (ret == SAMPLE_RETURN_CONTINUE) {
                continue;
            } else if (ret == SAMPLE_RETURN_BREAK) {
                break;
            }
        }
    }
}

/* get svc_t stream from h264 channels and save them */
hi_void *sample_comm_venc_get_venc_stream_proc_svc_t(hi_void *p)
{
    hi_s32 i, ret;
    hi_s32 cnt = 0;

    sample_venc_getstream_para *para = HI_NULL;
    struct timeval timeout_val;
    fd_set read_fds;
    hi_venc_stream_buf_info stream_buf_info[HI_VENC_MAX_CHN_NUM];
    sample_comm_venc_stream_proc_info stream_proc_info = {0};

    para = (sample_venc_getstream_para *)p;
    stream_proc_info.chn_total = para->cnt;
    stream_proc_info.save_heif = para->save_heif;

    /* step 1:  check & prepare save-file & venc-fd */
    if (stream_proc_info.chn_total >= HI_VENC_MAX_CHN_NUM) {
        sample_print("input count invalid\n");
        return HI_NULL;
    }

    ret = sample_comm_set_name_save_stream_svc_t(&stream_proc_info, stream_buf_info, HI_VENC_MAX_CHN_NUM);
    if (ret == SAMPLE_RETURN_NULL) {
        return HI_NULL;
    } else if (ret == SAMPLE_RETURN_FAILURE) {
        return (void *)HI_FAILURE;
    }

    /* step 2:  start to get streams of each channel. */
    while (para->thread_start == HI_TRUE) {
        FD_ZERO(&read_fds);
        for (i = 0; i < stream_proc_info.chn_total; i++) {
            FD_SET(stream_proc_info.venc_fd[i], &read_fds);
        }
        timeout_val.tv_sec = 2; /* 2 is a number */
        timeout_val.tv_usec = 0;
        ret = select(stream_proc_info.maxfd + 1, &read_fds, HI_NULL, HI_NULL, &timeout_val);
        if (ret < 0) {
            sample_print("select failed!\n");
            break;
        } else if (ret == 0) {
            sample_print("get venc stream time out, exit thread\n");
            continue;
        } else {
            sample_comm_fd_isset_svc_t(&stream_proc_info, &read_fds, stream_buf_info, HI_VENC_MAX_CHN_NUM);
        }
    }
    /* step 3 : close save-file */
    for (i = 0; i < stream_proc_info.chn_total; i++) {
        for (cnt = 0; cnt < 3; cnt++) { /* 3 is a number */
            if (stream_proc_info.file[i + cnt]) {
                fclose(stream_proc_info.file[i + cnt]);
            }
        }
    }
    return HI_NULL;
}

hi_void sample_comm_venc_set_save_heif(hi_bool save_heif)
{
    g_para.save_heif = save_heif;
    sample_print("set save heif flag: %d!\n", save_heif);
}

/* start get venc stream process thread */
hi_s32 sample_comm_venc_start_get_stream(hi_venc_chn *venc_chn, hi_s32 cnt)
{
    hi_s32 i;

    g_para.thread_start = HI_TRUE;
    g_para.cnt = cnt;
    for (i = 0; (i < cnt) && (i < HI_VENC_MAX_CHN_NUM); i++) {
        g_para.venc_chn[i] = venc_chn[i];
    }

    return pthread_create(&g_venc_pid, 0, sample_comm_venc_get_venc_stream_proc, (hi_void *)&g_para);
}
//maohw
hi_s32 sample_comm_venc_start_get_streamCB(hi_venc_chn *venc_chn, hi_s32 cnt, int (*cb)(hi_venc_chn chn, hi_payload_type pt, hi_venc_stream* stream, void* uargs), void *uargs)
{
  g_para.cb = cb;
  g_para.uargs = uargs;
  return sample_comm_venc_start_get_stream(venc_chn, cnt);
}

/******************************************************************************
* function : start rate auto process thread
******************************************************************************/
hi_s32 sample_comm_venc_rateauto_start(hi_venc_chn venc_chn[], hi_s32 cnt, hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn[])
{
    hi_s32 i;

    g_venc_rateauto_frame_param.thread_start = HI_TRUE;
    g_venc_rateauto_frame_param.cnt = cnt;
    g_venc_rateauto_frame_param.vpss_grp = vpss_grp;
    for (i = 0; (i < cnt) && (i < HI_VENC_MAX_CHN_NUM); i++) {
        g_venc_rateauto_frame_param.venc_chn[i] = venc_chn[i];
        g_venc_rateauto_frame_param.vpss_chn[i] = vpss_chn[i];
    }

    return pthread_create(&g_venc_rateauto_pid, 0, sample_comm_venc_rateauto_stream_proc,
        (hi_void *)&g_venc_rateauto_frame_param);
}
/* start get venc svc-t stream process thread */
hi_s32 sample_comm_venc_start_get_stream_svc_t(hi_s32 cnt)
{
    g_para.thread_start = HI_TRUE;
    g_para.cnt = cnt;
    return pthread_create(&g_venc_pid, 0, sample_comm_venc_get_venc_stream_proc_svc_t, (hi_void *)&g_para);
}

/* stop get venc stream process. */
hi_s32 sample_comm_venc_stop_get_stream(hi_s32 chn_num)
{
    hi_s32 i;
    for (i = 0; i < chn_num; i++) {
        if (hi_mpi_venc_stop_chn(i) != HI_SUCCESS) {
            sample_print("chn %d hi_mpi_venc_stop_recv_pic failed!\n", i);
            return HI_FAILURE;
        }
    }

    if (g_para.thread_start == HI_TRUE) {
        g_para.thread_start = HI_FALSE;
        pthread_join(g_venc_pid, 0);
    }
    return HI_SUCCESS;
}

hi_s32 sample_comm_venc_stop_send_qpmap_frame(hi_void)
{
    if (g_qpmap_send_frame_para.thread_start == HI_TRUE) {
        g_qpmap_send_frame_para.thread_start = HI_FALSE;
        pthread_join(g_venc_qpmap_pid, 0);
    }
    return HI_SUCCESS;
}

hi_s32 sample_comm_venc_stop_send_roimap_frame(hi_void)
{
    if (g_roimap_frame_param.thread_start == HI_TRUE) {
        g_roimap_frame_param.thread_start = HI_FALSE;
        pthread_join(g_venc_roimap_pid, 0);
    }

    return HI_SUCCESS;
}
hi_s32 sample_comm_venc_stop_rateauto(hi_venc_chn chn[], hi_s32 cnt)
{
    hi_s32 i, ret;

    prctl(PR_SET_NAME, "sample_comm_venc_stop_rateauto", 0, 0, 0);

    hi_venc_svc_param svc_param = {0};

    hi_venc_chn venc_chn;
    hi_venc_svc_rect_info  svc_rect_info = {0};
    if (g_venc_rateauto_frame_param.thread_start == HI_TRUE) {
        g_venc_rateauto_frame_param.thread_start = HI_FALSE;
        pthread_join(g_venc_rateauto_pid, 0);
        for (i = 0; i < cnt; i++) {
            venc_chn = chn[i];
            ret = hi_mpi_venc_enable_svc(venc_chn, HI_TRUE);
            if (ret != HI_SUCCESS) {
                sample_print("Set hi_mpi_venc_enable_svc failed for %#x chn =%d\n", ret, venc_chn);
            }
            if (memset_s(&svc_rect_info, sizeof(hi_venc_svc_rect_info), 0, sizeof(hi_venc_svc_rect_info)) != EOK) {
                printf("call memset_s error\n");
            }
            ret = hi_mpi_venc_send_svc_region(venc_chn, &svc_rect_info);
            if (ret != HI_SUCCESS) {
                sample_print("Set hi_mpi_venc_send_svc_region failed for %#x chn =%d\n", ret, venc_chn);
            }
            ret = hi_mpi_venc_get_svc_param(venc_chn, &svc_param);
            if (ret != HI_SUCCESS) {
                sample_print("Set hi_mpi_venc_set_svc_param failed for %#x chn =%d\n", ret, venc_chn);
            }
            if (memset_s(&svc_param, sizeof(hi_venc_svc_param), 0, sizeof(hi_venc_svc_param)) != EOK) {
                printf("call memset_s error\n");
            }
            ret = hi_mpi_venc_set_svc_param(venc_chn, &svc_param);
            if (ret != HI_SUCCESS) {
                sample_print("Set hi_mpi_venc_set_svc_param failed for %#x chn =%d\n", ret, venc_chn);
            }
            ret = hi_mpi_venc_enable_svc(venc_chn, HI_FALSE);
            if (ret != HI_SUCCESS) {
                sample_print("Set hi_mpi_venc_enable_svc failed for %#x chn =%d\n", ret, venc_chn);
            }
        }
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_venc_plan_to_semi(hi_u8 *u, hi_s32 u_stride, hi_u8 *v, hi_s32 pic_height)
{
    hi_s32 i;
    hi_u8 *tmp_u = HI_NULL;
    hi_u8 *ptu = HI_NULL;
    hi_u8 *tmp_v = HI_NULL;
    hi_u8 *ptv = HI_NULL;
    hi_s32 haf_w = u_stride / 2; /* 2: half */
    hi_s32 haf_h = pic_height / 2; /* 2: half */
    hi_s32 size = haf_w * haf_h;

    tmp_u = malloc(size);
    if (tmp_u == HI_NULL) {
        printf("malloc buf failed\n");
        return HI_FAILURE;
    }
    ptu = tmp_u;

    tmp_v = malloc(size);
    if (tmp_v == HI_NULL) {
        printf("malloc buf failed\n");
        free(tmp_u);
        return HI_FAILURE;
    }
    ptv = tmp_v;

    if (memcpy_s(tmp_u, size, u, size) != EOK) {
        printf("call memcpy_s error\n");
        free(tmp_u);
        free(tmp_v);
        return HI_FAILURE;
    }
    if (memcpy_s(tmp_v, size, v, size) != EOK) {
        printf("call memcpy_s error\n");
        free(tmp_u);
        free(tmp_v);
        return HI_FAILURE;
    }
    for (i = 0; i < (size  / 2); i++) { /* 2: half */
        *u++ = *tmp_v++;
        *u++ = *tmp_u++;
    }
    for (i = 0; i < (size  / 2); i++) { /* 2: half */
        *v++ = *tmp_v++;
        *v++ = *tmp_u++;
    }
    free(ptu);
    free(ptv);
    return HI_SUCCESS;
}


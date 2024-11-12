/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "sample_comm.h"
#include "sample_ipc.h"
#include "securec.h"

#define BIG_STREAM_SIZE PIC_2688X1520//PIC_2560X1440
#define SMALL_STREAM_SIZE PIC_D1_NTSC

#define VI_VB_YUV_CNT 4
#define VPSS_VB_YUV_CNT 6

#define ENTER_ASCII 10

#define VB_MAX_NUM 10

#define TYPE_NUM_MAX 3
//maohw #define CHN_NUM_MAX 2
#define GRP_NUM_MAX 2
#define SVAC3_CHN_NUM_MAX 1
#define MAX_WATERMARK_LEN 128
#define VENC_CHN_ID 0
#define VPSS_CHN_0      0
#define VPSS_CHN_1      1
#define VPSS_GRP        0
#define VPSS_CHN_NUM    2
#define DEFAULT_WAIT_TIME   20000
#if 0 //maohw

typedef struct {
    hi_size max_size;
    hi_pixel_format pixel_format;
    hi_size in_size;
    hi_size output_size[HI_VPSS_MAX_PHYS_CHN_NUM];
    hi_compress_mode compress_mode[HI_VPSS_MAX_PHYS_CHN_NUM];
    hi_bool enable[HI_VPSS_MAX_PHYS_CHN_NUM];
    hi_fmu_mode fmu_mode[HI_VPSS_MAX_PHYS_CHN_NUM];
} sample_venc_vpss_chn_attr;

typedef struct {
    hi_u32 valid_num;
    hi_u64 blk_size[HI_VB_MAX_COMMON_POOLS];
    hi_u32 blk_cnt[HI_VB_MAX_COMMON_POOLS];
    hi_u32 supplement_config;
} sample_venc_vb_attr;

#endif

typedef struct {
    hi_vpss_chn vpss_chn[CHN_NUM_MAX];
    hi_venc_chn venc_chn[CHN_NUM_MAX];
} sample_venc_vpss_chn;

typedef struct {
    hi_s32 venc_chn_num;
    hi_size enc_size[CHN_NUM_MAX];
    hi_s32 vpss_chn_depth;
} sample_venc_param;


static hi_bool g_sample_venc_exit = HI_FALSE;
static hi_bool g_send_multi_frame_signal = HI_FALSE;
static pthread_t g_send_multi_frame_thread = 0;

/* *****************************************************************************
 * function : show usage
 * **************************************************************************** */
static hi_void sample_venc_usage(hi_char *s_prg_nm)
{
    printf("usage : %s [index] [options]\n", s_prg_nm);
    printf("index:\n");
    printf("\t  0) normal               :H.265e + H.264e.\n");
    printf("\t  1) intra_refresh        :H.265e + H.264e.\n");
    printf("\t  2) roi_bg_frame_rate    :H.265e + H.264e.\n");
    printf("\t  3) roi_set              :Mjpege + Mjpege(user set roi info by API).\n");
    printf("\t  4) svac3 encode         :svac3.\n");
    printf("\t  5) inner scale          :H.264 H.265 svac3(user set payload type).\n");
    printf("\t  6) qpmap                :H.265e + H.264e.\n");
    printf("\t  7) debreath_effect      :H.265e + H.264e.\n");
    printf("\t  8) roimap               :Mjpege + Mjpege(user customize every region).\n");
    printf("\t  9) mosaic map           :H.265e + H.264e(user send frame,mosaic)\n");
    printf("\t  10) composite           :H.265\n");
}

/* *****************************************************************************
 * function : to process abnormal case
 * **************************************************************************** */
static hi_void sample_venc_handle_sig(hi_s32 signo)
{
    if (g_sample_venc_exit == HI_TRUE) {
        return;
    }

    if (signo == SIGINT || signo == SIGTERM) {
        g_sample_venc_exit = HI_TRUE;
    }
}

static hi_s32 sample_venc_getchar()
{
    hi_s32 c;
    if (g_sample_venc_exit == HI_TRUE) {
        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
        return 'e';
    }

    c = getchar();

    if (g_sample_venc_exit == HI_TRUE) {
        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
        return 'e';
    }

    return c;
}

static td_void print_gop_mode()
{
    printf("please input choose gop mode!\n");
    printf("\t 0) normal p.\n");
    printf("\t 1) dual p.\n");
    printf("\t 2) smart p.\n");
}

static td_void svac3_print_gop_mode()
{
    printf("please input choose gop mode!\n");
    printf("\t 0) normal p.\n");
    printf("\t 1) dual p.\n");
    printf("\t 2) smart p.\n");
    printf("\t 3) smart crr.\n");
}


static hi_s32 sample_clear_invalid_ch()
{
    hi_s32 c;

    while ((c = sample_venc_getchar()) != ENTER_ASCII) {
    }
    return HI_SUCCESS;
}

static hi_s32 set_gop_mode(hi_s32 c, hi_venc_gop_mode *gop_mode)
{
    switch (c) {
        case '0':
            *gop_mode = HI_VENC_GOP_MODE_NORMAL_P;
            break;

        case '1':
            *gop_mode = HI_VENC_GOP_MODE_DUAL_P;
            break;

        case '2':
            *gop_mode = HI_VENC_GOP_MODE_SMART_P;
            break;

        default:
            return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 svac3_set_gop_mode(hi_s32 c, hi_venc_gop_mode *gop_mode)
{
    switch (c) {
        case '0':
            *gop_mode = HI_VENC_GOP_MODE_NORMAL_P;
            break;

        case '1':
            *gop_mode = HI_VENC_GOP_MODE_DUAL_P;
            break;

        case '2':
            *gop_mode = HI_VENC_GOP_MODE_SMART_P;
            break;

        case '3':
            *gop_mode = HI_VENC_GOP_MODE_SMART_CRR;
            break;

        default:
            return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 get_gop_mode(hi_venc_gop_mode *gop_mode)
{
    hi_s32 c;

    while (HI_TRUE) {
        print_gop_mode();
        c = sample_venc_getchar();
        if (g_sample_venc_exit) {
            return HI_FAILURE;
        }

        if (c == ENTER_ASCII) {
            sample_print("invalid input! please try again.\n");
            continue;
        } else if (set_gop_mode(c, gop_mode) == HI_SUCCESS && sample_venc_getchar() == ENTER_ASCII) {
            return HI_SUCCESS;
        }
        sample_clear_invalid_ch();
        sample_print("invalid input! please try again.\n");
    }

    return HI_SUCCESS;
}

static hi_s32 svac3_get_gop_mode(hi_venc_gop_mode *gop_mode)
{
    hi_s32 c;

    while (HI_TRUE) {
        svac3_print_gop_mode();
        c = sample_venc_getchar();
        if (g_sample_venc_exit) {
            return HI_FAILURE;
        }

        if (c == ENTER_ASCII) {
            sample_print("invalid input! please try again.\n");
            continue;
        } else if (svac3_set_gop_mode(c, gop_mode) == HI_SUCCESS && sample_venc_getchar() == ENTER_ASCII) {
            return HI_SUCCESS;
        }
        sample_clear_invalid_ch();
        sample_print("invalid input! please try again.\n");
    }

    return HI_SUCCESS;
}

static hi_void print_rc_mode(hi_payload_type type)
{
    printf("please input choose rc mode!\n");
    printf("\t c) cbr.\n");
    printf("\t v) vbr.\n");
    if (type != HI_PT_MJPEG) {
        printf("\t b) abr.\n");
        printf("\t a) avbr.\n");
        printf("\t x) cvbr.\n");
        printf("\t q) qvbr.\n");
    }
    printf("\t f) fix_qp\n");
}

static hi_s32 set_rc_mode(hi_s32 c, sample_rc *rc_mode)
{
    switch (c) {
        case 'b':
            *rc_mode = SAMPLE_RC_ABR;
            break;

        case 'c':
            *rc_mode = SAMPLE_RC_CBR;
            break;

        case 'v':
            *rc_mode = SAMPLE_RC_VBR;
            break;

        case 'a':
            *rc_mode = SAMPLE_RC_AVBR;
            break;

        case 'x':
            *rc_mode = SAMPLE_RC_CVBR;
            break;

        case 'q':
            *rc_mode = SAMPLE_RC_QVBR;
            break;

        case 'f':
            *rc_mode = SAMPLE_RC_FIXQP;
            break;

        default:
            return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 get_rc_mode(hi_payload_type type, sample_rc *rc_mode)
{
    hi_s32 c;

    if (type == HI_PT_JPEG) {
        return HI_SUCCESS;
    }

    while (HI_TRUE) {
        print_rc_mode(type);
        c = sample_venc_getchar();
        if (g_sample_venc_exit) {
            return HI_FAILURE;
        }

        if (c == ENTER_ASCII) {
            sample_print("invalid input! please try again.\n");
            continue;
        } else if (set_rc_mode(c, rc_mode) == HI_SUCCESS && sample_venc_getchar() == ENTER_ASCII) {
            return HI_SUCCESS;
        }
        sample_clear_invalid_ch();
        sample_print("invalid input! please try again.\n");
    }

    return HI_SUCCESS;
}

static hi_s32 set_intra_refresh_mode(hi_s32 c, hi_venc_intra_refresh_mode *intra_refresh_mode)
{
    switch (c) {
        case 'r':
            *intra_refresh_mode = HI_VENC_INTRA_REFRESH_ROW;
            break;

        case 'c':
            *intra_refresh_mode = HI_VENC_INTRA_REFRESH_COLUMN;
            break;

        default:
            return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static td_void print_refresh_mode()
{
    printf("please input choose intra refresh mode!\n");
    printf("\t r) row.\n");
    printf("\t c) column.\n");
}

hi_s32 get_intra_refresh_mode(hi_venc_intra_refresh_mode *intra_refresh_mode)
{
    hi_s32 c;

    while (HI_TRUE) {
        print_refresh_mode();
        c = sample_venc_getchar();
        if (g_sample_venc_exit) {
            return HI_FAILURE;
        }

        if (c == ENTER_ASCII) {
            sample_print("invalid input! please try again.\n");
            continue;
        } else if (set_intra_refresh_mode(c, intra_refresh_mode) == HI_SUCCESS &&
            sample_venc_getchar() == ENTER_ASCII) {
            return HI_SUCCESS;
        }
        sample_clear_invalid_ch();
        sample_print("invalid input! please try again.\n");
    }

    return HI_SUCCESS;
}

static hi_s32 set_payload_type(hi_s32 c, hi_payload_type *payload)
{
    switch (c) {
        case '0':
            *payload = HI_PT_H264;
            break;

        case '1':
            *payload = HI_PT_H265;
            break;

        case '2':
            *payload = HI_PT_SVAC3;
            break;

        default:
            return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static td_void print_payload_type()
{
    printf("please input choose payload type!\n");
    printf("\t 0) h264\n");
    printf("\t 1) h265\n");
    printf("\t 2) svac3\n");
}

hi_s32 get_payload_type(hi_payload_type *payload)
{
    hi_s32 c;

    while (HI_TRUE) {
        print_payload_type();
        c = sample_venc_getchar();
        if (g_sample_venc_exit) {
            return HI_FAILURE;
        }

        if (c == ENTER_ASCII) {
            sample_print("invalid input! please try again.\n");
            continue;
        } else if (set_payload_type(c, payload) == HI_SUCCESS &&
            sample_venc_getchar() == ENTER_ASCII) {
            return HI_SUCCESS;
        }
        sample_clear_invalid_ch();
        sample_print("invalid input! please try again.\n");
    }

    return HI_SUCCESS;
}

static hi_s32 sample_composite_set_gop_mode(hi_s32 c, hi_venc_gop_mode *gop_mode)
{
    switch (c) {
        case '0':
            *gop_mode = HI_VENC_GOP_MODE_NORMAL_P;
            break;

        case '1':
            *gop_mode = HI_VENC_GOP_MODE_SMART_P;
            break;

        default:
            return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static td_void composte_print_gop_mode()
{
    printf("please input choose gop mode!\n");
    printf("\t 0) normal p.\n");
    printf("\t 1) smart p.\n");
}

static hi_s32 composite_get_gop_mode(hi_venc_gop_mode *gop_mode)
{
    hi_s32 c;

    while (HI_TRUE) {
        composte_print_gop_mode();
        c = sample_venc_getchar();
        if (g_sample_venc_exit) {
            return HI_FAILURE;
        }

        if (c == ENTER_ASCII) {
            sample_print("invalid input! please try again.\n");
            continue;
        } else if (sample_composite_set_gop_mode(c, gop_mode) == HI_SUCCESS &&
            sample_venc_getchar() == ENTER_ASCII) {
            return HI_SUCCESS;
        }

        sample_clear_invalid_ch();
        sample_print("invalid input! please try again.\n");
    }

    return HI_SUCCESS;
}

static hi_void update_vb_attr(sample_venc_vb_attr *vb_attr, const hi_size *size, hi_pixel_format format,
    hi_compress_mode compress_mode, td_u32 blk_cnt)
{
    hi_pic_buf_attr pic_buf_attr = { 0 };

    if (vb_attr->valid_num >= HI_VB_MAX_COMMON_POOLS) {
        printf("vb valid_num = %d should smaller than HI_VB_MAX_COMMON_POOLS(%d)\n",
            vb_attr->valid_num, HI_VB_MAX_COMMON_POOLS);
        return;
    }

    pic_buf_attr.width = size->width;
    pic_buf_attr.height = size->height;
    pic_buf_attr.align = HI_DEFAULT_ALIGN;
    pic_buf_attr.bit_width = HI_DATA_BIT_WIDTH_8;
    pic_buf_attr.pixel_format = format;
    pic_buf_attr.compress_mode = compress_mode;
    vb_attr->blk_size[vb_attr->valid_num] = hi_common_get_pic_buf_size(&pic_buf_attr);
    vb_attr->blk_cnt[vb_attr->valid_num] = blk_cnt;

    vb_attr->valid_num++;
}

/*maohw static*/ hi_void get_vb_attr(const hi_size *vi_size, const sample_venc_vpss_chn_attr *vpss_chn_attr,
    sample_venc_vb_attr *vb_attr)
{
    hi_s32 i;

    /* vb for vi-vpss */
    update_vb_attr(vb_attr, vi_size, HI_PIXEL_FORMAT_YUV_SEMIPLANAR_422, HI_COMPRESS_MODE_NONE, VI_VB_YUV_CNT);

    // vb for vpss-venc(big stream)
    if (vb_attr->valid_num >= HI_VB_MAX_COMMON_POOLS) {
        return;
    }

    for (i = 0; i < HI_VPSS_MAX_PHYS_CHN_NUM && vb_attr->valid_num < HI_VB_MAX_COMMON_POOLS; i++) {
        if (vpss_chn_attr->enable[i] == HI_TRUE && vpss_chn_attr->fmu_mode[i] == HI_FMU_MODE_OFF) {
            update_vb_attr(vb_attr, &vpss_chn_attr->output_size[i], vpss_chn_attr->pixel_format,
                vpss_chn_attr->compress_mode[i], VPSS_VB_YUV_CNT);
        }
    }

    vb_attr->supplement_config = HI_VB_SUPPLEMENT_JPEG_MASK | HI_VB_SUPPLEMENT_BNR_MOT_MASK;
}

/*maohw static*/ hi_void get_default_vpss_chn_attr(hi_size *vi_size, hi_size enc_size[], hi_s32 len,
    sample_venc_vpss_chn_attr *vpss_chan_attr)
{
    hi_s32 i;
    hi_u32 max_width;
    hi_u32 max_height;

    if (memset_s(vpss_chan_attr, sizeof(sample_venc_vpss_chn_attr), 0, sizeof(sample_venc_vpss_chn_attr)) != EOK) {
        printf("vpss chn attr call memset_s error\n");
        return;
    }

    max_width = vi_size->width;
    max_height = vi_size->height;

    for (i = 0; (i < len) && (i < HI_VPSS_MAX_PHYS_CHN_NUM); i++) {
        vpss_chan_attr->output_size[i].width = enc_size[i].width;
        vpss_chan_attr->output_size[i].height = enc_size[i].height;
        vpss_chan_attr->compress_mode[i] = (i == 0) ? HI_COMPRESS_MODE_SEG_COMPACT : HI_COMPRESS_MODE_NONE;
        vpss_chan_attr->enable[i] = HI_TRUE;
        vpss_chan_attr->fmu_mode[i] = HI_FMU_MODE_OFF;

        max_width = MAX2(max_width, enc_size[i].width);
        max_height = MAX2(max_height, enc_size[i].height);
    }

    vpss_chan_attr->max_size.width = max_width;
    vpss_chan_attr->max_size.height = max_height;
    vpss_chan_attr->pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;

    return;
}

/*maohw static*/ hi_s32 sample_venc_sys_init(sample_venc_vb_attr *vb_attr)
{
    hi_u32 i;
    hi_s32 ret;
    hi_vb_cfg vb_cfg = { 0 };

    if (vb_attr->valid_num > HI_VB_MAX_COMMON_POOLS) {
        sample_print("sample_venc_sys_init vb valid num(%d) too large than HI_VB_MAX_COMMON_POOLS(%d)!\n",
            vb_attr->valid_num, HI_VB_MAX_COMMON_POOLS);
        return HI_FAILURE;
    }

    for (i = 0; i < vb_attr->valid_num; i++) {
        vb_cfg.common_pool[i].blk_size = vb_attr->blk_size[i];
        vb_cfg.common_pool[i].blk_cnt = vb_attr->blk_cnt[i];
    }

    vb_cfg.max_pool_cnt = vb_attr->valid_num;

    if (vb_attr->supplement_config == 0) {
        ret = sample_comm_sys_vb_init(&vb_cfg);
    } else {
        ret = sample_comm_sys_init_with_vb_supplement(&vb_cfg, vb_attr->supplement_config);
    }

    if (ret != HI_SUCCESS) {
        sample_print("sample_venc_sys_init failed!\n");
    }

    return ret;
}

/*maohw static*/ hi_s32 sample_venc_vi_init(sample_vi_cfg *vi_cfg)
{
    hi_s32 ret;

    ret = sample_comm_vi_start_vi(vi_cfg);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vi_start_vi failed: 0x%x\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

/*maohw static*/ hi_void sample_venc_vi_deinit(sample_vi_cfg *vi_cfg)
{
    sample_comm_vi_stop_vi(vi_cfg);
}

/*maohw static*/ hi_s32 sample_venc_vpss_init(hi_vpss_grp vpss_grp, sample_venc_vpss_chn_attr *vpss_chan_cfg)
{
    hi_s32 ret;
    hi_vpss_chn vpss_chn;
    hi_vpss_grp_attr grp_attr = { 0 };
    sample_vpss_chn_attr vpss_chn_attr = {0};

    grp_attr.max_width = vpss_chan_cfg->max_size.width;
    grp_attr.max_height = vpss_chan_cfg->max_size.height;
    grp_attr.dei_mode = HI_VPSS_DEI_MODE_OFF;
    grp_attr.pixel_format = vpss_chan_cfg->pixel_format;
    grp_attr.frame_rate.src_frame_rate = -1;
    grp_attr.frame_rate.dst_frame_rate = -1;

    for (vpss_chn = 0; vpss_chn < HI_VPSS_MAX_PHYS_CHN_NUM; vpss_chn++) {
        if (vpss_chan_cfg->enable[vpss_chn] == HI_TRUE) {
            vpss_chn_attr.chn_attr[vpss_chn].width = vpss_chan_cfg->output_size[vpss_chn].width;
            vpss_chn_attr.chn_attr[vpss_chn].height = vpss_chan_cfg->output_size[vpss_chn].height;
            vpss_chn_attr.chn_attr[vpss_chn].chn_mode = HI_VPSS_CHN_MODE_USER;
            vpss_chn_attr.chn_attr[vpss_chn].compress_mode = vpss_chan_cfg->compress_mode[vpss_chn];
            vpss_chn_attr.chn_attr[vpss_chn].pixel_format = vpss_chan_cfg->pixel_format;
            vpss_chn_attr.chn_attr[vpss_chn].frame_rate.src_frame_rate = -1;
            vpss_chn_attr.chn_attr[vpss_chn].frame_rate.dst_frame_rate = -1;
            vpss_chn_attr.chn_attr[vpss_chn].depth = 0;
            vpss_chn_attr.chn_attr[vpss_chn].mirror_en = 0;
            vpss_chn_attr.chn_attr[vpss_chn].flip_en = 0;
            vpss_chn_attr.chn_attr[vpss_chn].aspect_ratio.mode = HI_ASPECT_RATIO_NONE;
        }
    }

    memcpy_s(vpss_chn_attr.chn_enable, sizeof(vpss_chn_attr.chn_enable),
            vpss_chan_cfg->enable, sizeof(vpss_chn_attr.chn_enable));
    vpss_chn_attr.chn_array_size = HI_VPSS_MAX_PHYS_CHN_NUM;

    ret = sample_common_vpss_start(vpss_grp, &grp_attr, &vpss_chn_attr);
    if (ret != HI_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
    }

    return ret;
}

/*maohw static*/ hi_void sample_venc_vpss_deinit(hi_vpss_grp vpss_grp, sample_venc_vpss_chn_attr *vpss_chan_cfg)
{
    hi_s32 ret;

    ret = sample_common_vpss_stop(vpss_grp, vpss_chan_cfg->enable, HI_VPSS_MAX_PHYS_CHN_NUM);
    if (ret != HI_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
    }
}

static hi_s32 sample_venc_init_param(hi_size *enc_size, hi_s32 chn_num_max, hi_size *vi_size,
    sample_venc_vpss_chn_attr *vpss_param)
{
    hi_s32 i;
    hi_s32 ret;
    hi_pic_size pic_size[CHN_NUM_MAX] = {BIG_STREAM_SIZE, SMALL_STREAM_SIZE};

    enc_size[0].width = vi_size->width;
    enc_size[0].height = vi_size->height;
    for (i = 1-1/*maohw*/; i < chn_num_max && i < CHN_NUM_MAX; i++) {
        ret = sample_comm_sys_get_pic_size(pic_size[i], &enc_size[i]);
        if (ret != HI_SUCCESS) {
            sample_print("sample_comm_sys_get_pic_size failed!\n");
            return ret;
        }
    }

    // get vpss param
    get_default_vpss_chn_attr(vi_size, enc_size, chn_num_max, vpss_param);

    return 0;
}

static hi_s32 sample_venc_set_video_param(hi_size *enc_size, sample_comm_venc_chn_param *chn_param, hi_s32 chn_num_max,
    hi_venc_gop_attr gop_attr, hi_bool qp_map)
{
    hi_bool share_buf_en = HI_TRUE;
    sample_rc rc_mode = 0;
    hi_u32 profile[CHN_NUM_MAX] = {0, 0};
    hi_payload_type payload[4] = {HI_PT_H265, HI_PT_H264}; // 2 : payload num

    if (qp_map) {
        rc_mode = SAMPLE_RC_QPMAP;
    } else if (get_rc_mode(payload[0], &rc_mode) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (chn_num_max < 2) { /* 2: chn_param array len */
        sample_print("chn_num_max  %d not enough! should > 1\n", chn_num_max);
        return HI_FAILURE;
    }

    /* encode h.265 */
    chn_param[0].frame_rate = 30; /* 30: is a number */
    chn_param[0].gop = 60; /* 60: is a number */
    chn_param[0].stats_time = 2; /* 2: is a number */
    chn_param[0].gop_attr = gop_attr;
    chn_param[0].type = payload[0];
    chn_param[0].size = sample_comm_sys_get_pic_enum(&enc_size[0]);
    chn_param[0].rc_mode = rc_mode;
    chn_param[0].profile = profile[0];
    chn_param[0].is_rcn_ref_share_buf = share_buf_en;

    /* encode h.264 */
    chn_param[1].frame_rate = 30; /* 30: is a number */
    chn_param[1].gop = 60; /* 60: is a number */
    chn_param[1].stats_time = 2; /* 2: is a number */
    chn_param[1].gop_attr = gop_attr;
    chn_param[1].type = payload[1];
    chn_param[1].size = sample_comm_sys_get_pic_enum(&enc_size[1]);
    chn_param[1].rc_mode = rc_mode;
    chn_param[1].profile = profile[1];
    chn_param[1].is_rcn_ref_share_buf = share_buf_en;
    return HI_SUCCESS;
}

static hi_s32 sample_venc_set_svac3_video_param(hi_size *enc_size, sample_comm_venc_chn_param *chn_param,
    hi_s32 chn_num_max, hi_venc_gop_attr gop_attr, hi_bool qp_map)
{
    hi_bool share_buf_en = HI_TRUE;
    sample_rc rc_mode;
    hi_u32 profile[SVAC3_CHN_NUM_MAX] = {0};
    hi_payload_type payload = HI_PT_SVAC3;

    if (qp_map) {
        rc_mode = SAMPLE_RC_QPMAP;
    } else if (get_rc_mode(payload, &rc_mode) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    /* encode svac3 */
    chn_param[0].frame_rate = 30; /* 30: is a number */
    chn_param[0].gop = 60; /* 60: is a number */
    chn_param[0].stats_time = 2; /* 2: is a number */
    chn_param[0].gop_attr = gop_attr;
    chn_param[0].type = payload;
    chn_param[0].size = sample_comm_sys_get_pic_enum(&enc_size[0]);
    chn_param[0].rc_mode = rc_mode;
    chn_param[0].profile = profile[0];
    chn_param[0].is_rcn_ref_share_buf = share_buf_en;

    return HI_SUCCESS;
}

static hi_void sample_venc_set_inner_scale_video_param(hi_payload_type payload, sample_comm_venc_chn_param *chn_param)
{
    chn_param[0].type = payload;
    chn_param[1].type = payload;
}

static hi_void sample_set_venc_vpss_chn(sample_venc_vpss_chn *venc_vpss_chn, hi_s32 chn_num_max)
{
    hi_s32 i;
    hi_unused(chn_num_max);

    for (i = 0; i < CHN_NUM_MAX; i++) {
        venc_vpss_chn->vpss_chn[i] = i;
        venc_vpss_chn->venc_chn[i] = i;
    }
}

static hi_void sample_venc_unbind_vpss_stop(hi_vpss_grp vpss_grp, const sample_venc_vpss_chn *venc_vpss_chn)
{
    hi_s32 i;

    for (i = 0; i < CHN_NUM_MAX; i++) {
        sample_comm_vpss_un_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[i], venc_vpss_chn->venc_chn[i]);
        sample_comm_venc_stop(venc_vpss_chn->venc_chn[i]);
    }
}

static hi_void sample_venc_svac3_unbind_vpss_stop(hi_vpss_grp vpss_grp, const sample_venc_vpss_chn *venc_vpss_chn)
{
    hi_s32 i;

    for (i = 0; i < SVAC3_CHN_NUM_MAX; i++) {
        sample_comm_vpss_un_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[i], venc_vpss_chn->venc_chn[i]);
        sample_comm_venc_stop(venc_vpss_chn->venc_chn[i]);
    }
}

static hi_void sample_venc_stop(const sample_venc_vpss_chn *venc_vpss_chn, hi_s32 chn_num_max)
{
    hi_s32 i;
    hi_unused(chn_num_max);

    for (i = 0; i < CHN_NUM_MAX; i++) {
        sample_comm_venc_stop(venc_vpss_chn->venc_chn[i]);
    }
}

static hi_s32 sample_venc_normal_start_encode(hi_size *enc_size, hi_vpss_grp vpss_grp,
    sample_venc_vpss_chn *venc_vpss_chn)
{
    hi_s32 ret;
    hi_venc_gop_mode gop_mode;
    hi_venc_gop_attr gop_attr;
    sample_comm_venc_chn_param chn_param[CHN_NUM_MAX] = {0};

    if (get_gop_mode(&gop_mode) != HI_SUCCESS) {
        return HI_FAILURE;
    }
    if ((ret = sample_comm_venc_get_gop_attr(gop_mode, &gop_attr)) != HI_SUCCESS) {
        sample_print("Venc Get GopAttr for %#x!\n", ret);
        return ret;
    }

    if ((ret = sample_venc_set_video_param(enc_size, chn_param, CHN_NUM_MAX, gop_attr, HI_FALSE)) != HI_SUCCESS) {
        return ret;
    }

    if ((ret = sample_comm_venc_mini_buf_en(chn_param, CHN_NUM_MAX)) != HI_SUCCESS) {
        return ret;
    }

    /* encode h.265 */
    if ((ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[0], &(chn_param[0]))) != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        return ret;
    }

    if (sample_comm_vpss_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[0], venc_vpss_chn->venc_chn[0]) != HI_SUCCESS) {
        sample_print("sample_comm_vpss_bind_venc failed!\n");
        goto EXIT_VENC_H265_STOP;
    }

    /* encode h.264 */
    if ((ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[1], &(chn_param[1]))) != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        goto EXIT_VENC_H265_UnBind;
    }

    if (sample_comm_vpss_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[1], venc_vpss_chn->venc_chn[1]) != HI_SUCCESS) {
        sample_print("sample_comm_vpss_bind_venc failed!\n");
        goto EXIT_VENC_H264_STOP;
    }

    /* *****************************************
     stream save process
    ***************************************** */
    if ((ret = sample_comm_venc_start_get_stream(venc_vpss_chn->venc_chn, CHN_NUM_MAX)) != HI_SUCCESS) {
        sample_print("Start Venc failed!\n");
        goto EXIT_VENC_H264_UnBind;
    }

    return HI_SUCCESS;

EXIT_VENC_H264_UnBind:
    sample_comm_vpss_un_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[1], venc_vpss_chn->venc_chn[1]);
EXIT_VENC_H264_STOP:
    sample_comm_venc_stop(venc_vpss_chn->venc_chn[1]);
EXIT_VENC_H265_UnBind:
    sample_comm_vpss_un_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[0], venc_vpss_chn->venc_chn[0]);
EXIT_VENC_H265_STOP:
    sample_comm_venc_stop(venc_vpss_chn->venc_chn[0]);

    return HI_FAILURE;
}

static hi_void sample_venc_exit_process()
{
    printf("please press twice ENTER to exit this sample\n");
    (hi_void)getchar();

    if (g_sample_venc_exit != HI_TRUE) {
        (hi_void)getchar();
    }
    sample_comm_venc_stop_get_stream(CHN_NUM_MAX);
}

static hi_void sample_venc_mosaic_map_exit_process(hi_s32 chn_num_max)
{
    printf("please press twice ENTER to exit this sample\n");
    (hi_void)getchar();

    if (g_sample_venc_exit != HI_TRUE) {
        (hi_void)getchar();
    }

    sample_comm_venc_stop_send_frame();
    sample_comm_venc_stop_get_stream(chn_num_max);
}

static hi_void sample_venc_online_wrap_get_default_vb_cfg(hi_vb_cfg *vb_cfg, sample_venc_param *enc_param,
    hi_u32 wrap_size)
{
    hi_s32 i;
    hi_vb_calc_cfg calc_cfg;
    hi_pic_buf_attr buf_attr;

    (hi_void)memset_s(vb_cfg, sizeof(hi_vb_cfg), 0, sizeof(hi_vb_cfg));
    vb_cfg->max_pool_cnt = 128; /* 128 blks */

    for (i = 1-1/*maohw*/; i < enc_param->venc_chn_num && CHN_NUM_MAX; i++) {
        buf_attr.width = enc_param->enc_size[i].width;
        buf_attr.height = enc_param->enc_size[i].height;
        buf_attr.align = HI_DEFAULT_ALIGN;
        buf_attr.bit_width = HI_DATA_BIT_WIDTH_8;
        buf_attr.pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
        buf_attr.compress_mode = HI_COMPRESS_MODE_NONE;
        buf_attr.video_format = HI_VIDEO_FORMAT_LINEAR;
        hi_common_get_pic_buf_cfg(&buf_attr, &calc_cfg);

        vb_cfg->common_pool[i].blk_size = calc_cfg.vb_size;
        vb_cfg->common_pool[i].blk_cnt = 3; /* 3 blk_cnt */
        printf("i:%d, blk_size:%lld, blk_cnt:%d\n", i, vb_cfg->common_pool[i].blk_size, vb_cfg->common_pool[i].blk_cnt);
    }

    vb_cfg->common_pool[i].blk_cnt = 1;
    vb_cfg->common_pool[i].blk_size = wrap_size;
}

static hi_void sample_venc_online_wrap_get_default_sys_cfg(sampe_sys_cfg *sys_cfg)
{
    sys_cfg->route_num = 1;
    sys_cfg->mode_type = HI_VI_ONLINE_VPSS_ONLINE;
    sys_cfg->vpss_wrap_en = HI_TRUE;
}

hi_void sample_venc_online_wrap_get_default_vpss_cfg(sample_sns_type sns_type, sample_vpss_cfg *vpss_cfg,
    sample_venc_param *enc_param)
{
    hi_vpss_chn chn;
    hi_size in_size;
    hi_u32 max_width;
    hi_u32 max_height;

    sample_comm_vi_get_size_by_sns_type(sns_type, &in_size);
    sample_comm_vpss_get_default_grp_attr(&vpss_cfg->grp_attr);
    sample_comm_vpss_get_default_3dnr_attr(&vpss_cfg->nr_attr);
    vpss_cfg->vpss_grp = 0;
    max_width = in_size.width;
    max_height = in_size.height;

    for (chn = 0; chn < enc_param->venc_chn_num && HI_VPSS_MAX_PHYS_CHN_NUM; chn++) {
        vpss_cfg->chn_en[chn] = HI_TRUE;
        sample_comm_vpss_get_default_chn_attr(&vpss_cfg->chn_attr[chn]);
        if (chn > HI_VPSS_CHN1) {
            vpss_cfg->chn_attr[chn].compress_mode = HI_COMPRESS_MODE_NONE;
        }
        vpss_cfg->chn_attr[chn].width  = enc_param->enc_size[chn].width;
        vpss_cfg->chn_attr[chn].height = enc_param->enc_size[chn].height;
        vpss_cfg->chn_attr[chn].depth = enc_param->vpss_chn_depth;

        max_width = MAX2(max_width, enc_param->enc_size[chn].width);
        max_height = MAX2(max_height, enc_param->enc_size[chn].height);
    }
    vpss_cfg->chn_attr[HI_VPSS_CHN0].compress_mode = HI_COMPRESS_MODE_SEG_COMPACT;
    vpss_cfg->grp_attr.max_width  = max_width;
    vpss_cfg->grp_attr.max_height = max_height;
    vpss_cfg->wrap_attr[0].enable = HI_TRUE;
}

static hi_void sample_venc_vpss_get_wrap_cfg(sample_sns_type sns_type, sampe_sys_cfg *sys_cfg,
    sample_vpss_cfg *vpss_cfg)
{
    hi_u32 full_lines_std;
    if (sys_cfg->vpss_wrap_en) {
        if (sns_type == SC4336P_MIPI_4M_30FPS_10BIT || sns_type == OS04D10_MIPI_4M_30FPS_10BIT ||
            sns_type == GC4023_MIPI_4M_30FPS_10BIT || sns_type == SC431HAI_MIPI_4M_30FPS_10BIT ||
            sns_type == SC431HAI_MIPI_4M_30FPS_10BIT_WDR2TO1 || sns_type == SC4336P_MIPI_3M_30FPS_10BIT) {
            full_lines_std = 1500; /* full_lines_std: 1500 */
        } else if (sns_type == SC450AI_MIPI_4M_30FPS_10BIT || sns_type == SC450AI_MIPI_4M_30FPS_10BIT_WDR2TO1) {
            full_lines_std = 1585; /* full_lines_std: 1585 */
        } else if (sns_type == SC500AI_MIPI_5M_30FPS_10BIT || sns_type == SC500AI_MIPI_5M_30FPS_10BIT_WDR2TO1) {
            full_lines_std = 1700; /* full_lines_std: 1700 */
        } else {
            sys_cfg->vpss_wrap_en = HI_FALSE;
            vpss_cfg->wrap_attr[0].enable = HI_FALSE;
            return;
        }
        (hi_void)sample_comm_vpss_get_wrap_cfg(vpss_cfg->chn_attr, sys_cfg->mode_type, full_lines_std,
            &vpss_cfg->wrap_attr[0]);

        sys_cfg->vpss_wrap_size = vpss_cfg->wrap_attr[0].buf_size;
    }
}

static hi_s32 sample_venc_online_wrap_sys_init(sample_venc_param *enc_param, sampe_sys_cfg *sys_cfg)
{
    hi_s32 ret;
    hi_vb_cfg vb_cfg = {0};
    hi_u32 supplement_config = HI_VB_SUPPLEMENT_JPEG_MASK | HI_VB_SUPPLEMENT_BNR_MOT_MASK;

    sample_venc_online_wrap_get_default_vb_cfg(&vb_cfg, enc_param, sys_cfg->vpss_wrap_size);

    if (supplement_config == 0) {
        ret = sample_comm_sys_vb_init(&vb_cfg);
    } else {
        ret = sample_comm_sys_init_with_vb_supplement(&vb_cfg, supplement_config);
    }

    if (ret != HI_SUCCESS) {
        sample_print("sample_venc_sys_init failed!\n");
    }

    if (sample_comm_vi_set_vi_vpss_mode(sys_cfg->mode_type, HI_VI_AIISP_MODE_DEFAULT) != HI_SUCCESS) {
        goto sys_exit;
    }

    return HI_SUCCESS;
sys_exit:
    sample_comm_sys_exit();
    return HI_FAILURE;
}

static hi_s32 sample_venc_get_enc_size(hi_pic_size *pic_size, sample_venc_param *enc_param)
{
    hi_s32 i;
    hi_s32 ret;

    for (i = 0; i < enc_param->venc_chn_num && i < CHN_NUM_MAX; i++) {
        ret = sample_comm_sys_get_pic_size(pic_size[i], &(enc_param->enc_size[i]));
        if (ret != HI_SUCCESS) {
            sample_print("sample_comm_sys_get_pic_size failed!\n");
            return ret;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 sample_venc_online_wrap_start_vpss(hi_vpss_grp grp, sample_vpss_cfg *vpss_cfg)
{
    hi_s32 ret;
    hi_frame_interrupt_attr frame_interrupt_attr = {0};
    sample_vpss_chn_attr vpss_chn_attr = {0};

    frame_interrupt_attr.interrupt_type = HI_FRAME_INTERRUPT_EARLY_END;
    frame_interrupt_attr.early_line = vpss_cfg->grp_attr.max_height / 2; /* 2 half */
    ret = hi_mpi_vpss_set_grp_frame_interrupt_attr(0, &frame_interrupt_attr);
    if (ret != HI_SUCCESS) {
        printf("hi_mpi_vpss_set_grp_frame_interrupt_attr failed!\n");
        return ret;
    }

    (hi_void)memcpy_s(&vpss_chn_attr.chn_attr[0], sizeof(hi_vpss_chn_attr) * HI_VPSS_MAX_PHYS_CHN_NUM,
        vpss_cfg->chn_attr, sizeof(hi_vpss_chn_attr) * HI_VPSS_MAX_PHYS_CHN_NUM);
    (hi_void)memcpy_s(vpss_chn_attr.chn_enable, sizeof(vpss_chn_attr.chn_enable),
        vpss_cfg->chn_en, sizeof(vpss_chn_attr.chn_enable));
    (hi_void)memcpy_s(&vpss_chn_attr.wrap_attr[0], sizeof(hi_vpss_chn_buf_wrap_attr) * HI_VPSS_MAX_PHYS_CHN_NUM,
        vpss_cfg->wrap_attr, sizeof(hi_vpss_chn_buf_wrap_attr) * HI_VPSS_MAX_PHYS_CHN_NUM);
    vpss_chn_attr.chn_array_size = HI_VPSS_MAX_PHYS_CHN_NUM;
    ret = sample_common_vpss_start(grp, &vpss_cfg->grp_attr, &vpss_chn_attr);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if (vpss_cfg->chn_en[1]) {
        const hi_low_delay_info low_delay_info = { HI_TRUE, 200, HI_FALSE }; /* 200: lowdelay line */
        if (hi_mpi_vpss_set_chn_low_delay(grp, 1, &low_delay_info) != HI_SUCCESS) {
            goto stop_vpss;
        }
    }

    return HI_SUCCESS;

stop_vpss:
    sample_common_vpss_stop(grp, vpss_cfg->chn_en, HI_VPSS_MAX_PHYS_CHN_NUM);
    return HI_FAILURE;
}

static hi_void sample_venc_online_wrap_stop_vpss(hi_vpss_grp grp, sample_vpss_cfg *vpss_cfg)
{
    sample_common_vpss_stop(grp, vpss_cfg->chn_en, HI_VPSS_MAX_PHYS_CHN_NUM);
}

static hi_s32 sample_venc_online_wrap_start_sys_vi_vpss(sample_sns_type sns_type, sample_vi_cfg *vi_cfg,
    sample_vpss_cfg *vpss_cfg, sample_venc_param *enc_param)
{
    hi_s32 ret;
    sampe_sys_cfg sys_cfg = {0};
    hi_vi_pipe vi_pipe = 0;
    hi_vpss_grp vpss_grp = 0;

    /* *****************************************
      step 0: related parameter ready
    ***************************************** */
    sample_venc_online_wrap_get_default_sys_cfg(&sys_cfg);
    sample_comm_vi_get_default_vi_cfg(sns_type, vi_cfg);
    sample_venc_online_wrap_get_default_vpss_cfg(sns_type, vpss_cfg, enc_param);
    sample_venc_vpss_get_wrap_cfg(sns_type, &sys_cfg, vpss_cfg);

    /* *****************************************
      step 1: init sys alloc common vb
    ***************************************** */
    if ((ret = sample_venc_online_wrap_sys_init(enc_param, &sys_cfg)) != HI_SUCCESS) {
        sample_print("Init SYS err for %#x!\n", ret);
        return ret;
    }

    if ((ret = sample_venc_vi_init(vi_cfg)) != HI_SUCCESS) {
        sample_print("Init VI err for %#x!\n", ret);
        goto EXIT_SYS_STOP;
    }

    if ((ret = sample_venc_online_wrap_start_vpss(vpss_grp, vpss_cfg)) != HI_SUCCESS) {
        sample_print("Init VPSS err for %#x!\n", ret);
        goto EXIT_VI_STOP;
    }

    if ((ret = sample_comm_vi_bind_vpss(vi_pipe, 0, vpss_grp, 0)) != HI_SUCCESS) {
        sample_print("VI Bind VPSS err for %#x!\n", ret);
        goto EXIT_VPSS_STOP;
    }

    return HI_SUCCESS;

    sample_comm_vi_un_bind_vpss(vi_pipe, 0, vpss_grp, 0);
EXIT_VPSS_STOP:
    sample_venc_online_wrap_stop_vpss(vpss_grp, vpss_cfg);
EXIT_VI_STOP:
    sample_venc_vi_deinit(vi_cfg);
EXIT_SYS_STOP:
    sample_comm_sys_exit();

    return ret;
}

static hi_void sample_venc_online_wrap_stop_sys_vi_vpss(sample_vi_cfg *vi_cfg, sample_vpss_cfg *vpss_cfg)
{
    hi_vi_pipe vi_pipe = 0;
    hi_vpss_grp vpss_grp = 0;

    sample_comm_vi_un_bind_vpss(vi_pipe, 0, vpss_grp, 0);
    sample_venc_online_wrap_stop_vpss(vpss_grp, vpss_cfg);
    sample_venc_vi_deinit(vi_cfg);
    sample_comm_sys_exit();
}

static hi_void sample_venc_set_pic_size_0(sample_sns_type sns_type, hi_pic_size *pic_size)
{
    pic_size[0] = BIG_STREAM_SIZE;
    if (sns_type == SC4336P_MIPI_3M_30FPS_10BIT) {
        pic_size[0] = PIC_2304X1296;
    }
}

/* *****************************************************************************
 * function :  H.265e@2560x1440@30fps + h264e@1280x720@30fps
 * **************************************************************************** */
static hi_s32 sample_venc_normal(hi_void)
{
    hi_s32 ret;
    sample_sns_type sns_type = 0;
    sample_vi_cfg vi_cfg = {0};
    sample_vpss_cfg vpss_cfg = {0};
    hi_vpss_grp vpss_grp = 0;
    sample_venc_vpss_chn venc_vpss_chn = {0};
    sample_venc_param enc_param = {0};
    hi_pic_size pic_size[CHN_NUM_MAX] = {0};

    /* *****************************************
      step 0: related parameter ready
    ***************************************** */
    sns_type = SENSOR0_TYPE;
    enc_param.venc_chn_num = CHN_NUM_MAX;
    sample_venc_set_pic_size_0(sns_type, pic_size);
    pic_size[1] = SMALL_STREAM_SIZE;
    sample_venc_get_enc_size(pic_size, &enc_param);
    sample_set_venc_vpss_chn(&venc_vpss_chn, enc_param.venc_chn_num);

    /* *****************************************
      step 1: init sys alloc common vb
    ***************************************** */
    if ((ret = sample_venc_online_wrap_start_sys_vi_vpss(sns_type, &vi_cfg, &vpss_cfg, &enc_param)) != HI_SUCCESS) {
        return ret;
    }

    if ((ret = sample_venc_normal_start_encode(&enc_param.enc_size[0], vpss_grp, &venc_vpss_chn)) != HI_SUCCESS) {
        goto EXIT_SYS_VI_VPSS;
    }

    /* *****************************************
     exit process
    ***************************************** */
    sample_venc_exit_process();
    sample_venc_unbind_vpss_stop(vpss_grp, &venc_vpss_chn);

EXIT_SYS_VI_VPSS:
    sample_venc_online_wrap_stop_sys_vi_vpss(&vi_cfg, &vpss_cfg);

    return ret;
}

static hi_s32 sample_venc_qpmap_start_encode(hi_size *enc_size, hi_s32 chn_num_max, hi_vpss_grp vpss_grp,
    sample_venc_vpss_chn *venc_vpss_chn)
{
    hi_s32 ret;
    hi_venc_gop_mode gop_mode;
    hi_venc_gop_attr gop_attr;
    sample_comm_venc_chn_param chn_param[CHN_NUM_MAX] = {0};
    sample_comm_venc_chn_param *h265_chn_param = HI_NULL;
    sample_comm_venc_chn_param *h264_chn_param = HI_NULL;

    if (get_gop_mode(&gop_mode) != HI_SUCCESS) {
        return HI_FAILURE;
    }
    if ((ret = sample_comm_venc_get_gop_attr(gop_mode, &gop_attr)) != HI_SUCCESS) {
        sample_print("Venc Get GopAttr for %#x!\n", ret);
        return ret;
    }

    if ((ret = sample_venc_set_video_param(enc_size, chn_param, CHN_NUM_MAX, gop_attr, HI_TRUE)) != HI_SUCCESS) {
        return ret;
    }

    if ((ret = sample_comm_venc_mini_buf_en(chn_param, CHN_NUM_MAX)) != HI_SUCCESS) {
        return ret;
    }

    /* encode h.265 */
    h265_chn_param = &(chn_param[0]);
    ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[0], h265_chn_param);
    if (ret != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        return ret;
    }

    /* encode h.264 */
    h264_chn_param = &(chn_param[1]);
    ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[1], h264_chn_param);
    if (ret != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        goto EXIT_VENC_H265_STOP;
    }

    ret = sample_comm_venc_qpmap_send_frame(vpss_grp, venc_vpss_chn->vpss_chn, venc_vpss_chn->venc_chn, CHN_NUM_MAX,
        enc_size);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_venc_qpmap_send_frame failed for %#x!\n", ret);
        goto EXIT_VENC_H264_STOP;
    }

    /* *****************************************
     stream save process
    ***************************************** */
    if ((ret = sample_comm_venc_start_get_stream(venc_vpss_chn->venc_chn, CHN_NUM_MAX)) != HI_SUCCESS) {
        sample_print("Start Venc failed!\n");
        goto EXIT_VENC_H264_STOP;
    }

    return HI_SUCCESS;

EXIT_VENC_H264_STOP:
    sample_comm_venc_stop(venc_vpss_chn->venc_chn[1]);
EXIT_VENC_H265_STOP:
    sample_comm_venc_stop(venc_vpss_chn->venc_chn[0]);

    return ret;
}

static hi_void sample_venc_qpmap_exit_process()
{
    printf("please press twice ENTER to exit this sample\n");
    (hi_void)getchar();

    if (g_sample_venc_exit != HI_TRUE) {
        (hi_void)getchar();
    }

    sample_comm_venc_stop_send_qpmap_frame();
    sample_comm_venc_stop_get_stream(CHN_NUM_MAX);
}

static hi_s32 sample_venc_qpmap(hi_void)
{
    hi_s32 ret;
    sample_sns_type sns_type = SENSOR0_TYPE;
    sample_vi_cfg vi_cfg;
    hi_size enc_size[CHN_NUM_MAX], vi_size;
    const hi_vi_pipe vi_pipe = 0;
    const hi_vi_chn vi_chn = 0;
    const hi_vpss_grp vpss_grp = 0;
    sample_venc_vpss_chn_attr vpss_param;
    sample_venc_vb_attr vb_attr = { 0 };
    sample_venc_vpss_chn venc_vpss_chn = { 0 };

    sample_set_venc_vpss_chn(&venc_vpss_chn, CHN_NUM_MAX);
    sample_comm_vi_get_default_vi_cfg(sns_type, &vi_cfg);
    sample_comm_vi_get_size_by_sns_type(sns_type, &vi_size);
    /* *****************************************
      step 0: related parameter ready
    ***************************************** */
    if ((ret = sample_venc_init_param(enc_size, CHN_NUM_MAX, &vi_size, &vpss_param)) != HI_SUCCESS) {
        return ret;
    }

    /* *****************************************
      step 1: init sys alloc common vb
    ***************************************** */
    get_vb_attr(&vi_size, &vpss_param, &vb_attr);

    if ((ret = sample_venc_sys_init(&vb_attr)) != HI_SUCCESS) {
        sample_print("Init SYS err for %#x!\n", ret);
        return ret;
    }

    if ((ret = sample_venc_vi_init(&vi_cfg)) != HI_SUCCESS) {
        sample_print("Init VI err for %#x!\n", ret);
        goto EXIT_SYS_STOP;
    }

    if ((ret = sample_venc_vpss_init(vpss_grp, &vpss_param)) != HI_SUCCESS) {
        sample_print("Init VPSS err for %#x!\n", ret);
        goto EXIT_VI_STOP;
    }

    if ((ret = sample_comm_vi_bind_vpss(vi_pipe, vi_chn, vpss_grp, 0)) != HI_SUCCESS) {
        sample_print("VI Bind VPSS err for %#x!\n", ret);
        goto EXIT_VPSS_STOP;
    }

    /* *****************************************
    start stream venc
    ***************************************** */
    if ((ret = sample_venc_qpmap_start_encode(enc_size, CHN_NUM_MAX, vpss_grp, &venc_vpss_chn)) != HI_SUCCESS) {
        goto EXIT_VI_VPSS_UNBIND;
    }

    /* *****************************************
     exit process
    ***************************************** */
    sample_venc_qpmap_exit_process();
    sample_venc_stop(&venc_vpss_chn, CHN_NUM_MAX);

EXIT_VI_VPSS_UNBIND:
    sample_comm_vi_un_bind_vpss(vi_pipe, vi_chn, vpss_grp, 0);
EXIT_VPSS_STOP:
    sample_venc_vpss_deinit(vpss_grp, &vpss_param);
EXIT_VI_STOP:
    sample_venc_vi_deinit(&vi_cfg);
EXIT_SYS_STOP:
    sample_comm_sys_exit();

    return ret;
}

static hi_s32 sample_venc_vpss_bind_venc(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn, hi_venc_chn venc_chn)
{
    hi_s32 ret;

    ret = sample_comm_vpss_bind_venc(vpss_grp, vpss_chn, venc_chn);
    if (ret != HI_SUCCESS) {
        sample_print("call sample_comm_vpss_bind_venc vpss grp %d, vpss chn %d, venc chn %d, ret =  %#x!\n", vpss_grp,
            vpss_chn, venc_chn, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_void sample_venc_intra_refresh_param_init(hi_venc_intra_refresh_mode intra_refresh_mode,
    hi_venc_intra_refresh *intra_refresh)
{
    intra_refresh->enable = HI_TRUE;
    intra_refresh->mode = intra_refresh_mode;
    if (intra_refresh_mode == HI_VENC_INTRA_REFRESH_ROW) {
        intra_refresh->refresh_num = 5; /* 5: refresh num */
    } else {
        intra_refresh->refresh_num = 6; /* 6: refresh num */
    }
    intra_refresh->request_i_qp = 30; /* 30: request num */
}

static hi_s32 sample_venc_set_intra_refresh(hi_venc_chn venc_chn, hi_venc_intra_refresh_mode intra_refresh_mode)
{
    hi_s32 ret;
    hi_venc_intra_refresh intra_refresh = { 0 };

    if ((ret = hi_mpi_venc_get_intra_refresh(venc_chn, &intra_refresh)) != HI_SUCCESS) {
        sample_print("Get Intra Refresh failed for %#x!\n", ret);
        return ret;
    }

    sample_venc_intra_refresh_param_init(intra_refresh_mode, &intra_refresh);

    if ((ret = hi_mpi_venc_set_intra_refresh(venc_chn, &intra_refresh)) != HI_SUCCESS) {
        sample_print("Set Intra Refresh failed for %#x!\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_comm_venc_get_gop_default_attr(hi_venc_gop_attr *gop_attr)
{
    hi_s32 ret;
    hi_venc_gop_mode gop_mode;

    if (get_gop_mode(&gop_mode) != HI_SUCCESS) {
        return HI_FAILURE;
    }
    ret = sample_comm_venc_get_gop_attr(gop_mode, gop_attr);
    if (ret != HI_SUCCESS) {
        sample_print("Venc get gop default attr for mode %d failed return %#x!\n", gop_mode, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_venc_intra_refresh_set_video_param(hi_size *enc_size,
    hi_venc_intra_refresh_mode *intra_refresh_mode, sample_comm_venc_chn_param *chn_param, hi_s32 len)
{
    hi_s32 ret;
    hi_venc_gop_attr gop_attr;

    if ((ret = sample_comm_venc_get_gop_default_attr(&gop_attr)) != HI_SUCCESS) {
        return ret;
    }

    if (get_intra_refresh_mode(intra_refresh_mode) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (len > CHN_NUM_MAX) {
        sample_print("the num of venc_create_param is beyond CHN_NUM_MAX !\n");
        return HI_FAILURE;
    }

    if ((ret = sample_venc_set_video_param(enc_size, chn_param, CHN_NUM_MAX, gop_attr, HI_FALSE)) != HI_SUCCESS) {
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_venc_intra_refresh_start_encode(hi_size *enc_size, hi_s32 chn_num_max, hi_vpss_grp vpss_grp,
    sample_venc_vpss_chn *venc_vpss_chn)
{
    hi_s32 ret;
    hi_venc_intra_refresh_mode intra_refresh_mode;
    sample_comm_venc_chn_param chn_param[CHN_NUM_MAX] = {0};

    ret = sample_venc_intra_refresh_set_video_param(enc_size, &intra_refresh_mode, chn_param, CHN_NUM_MAX);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if ((ret = sample_comm_venc_mini_buf_en(chn_param, CHN_NUM_MAX)) != HI_SUCCESS) {
        return ret;
    }

    /* encode h.265 */
    if ((ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[0], &(chn_param[0]))) != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        return ret;
    }

    ret = sample_venc_vpss_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[0], venc_vpss_chn->venc_chn[0]);
    if (ret != HI_SUCCESS) {
        goto EXIT_VENC_H265_STOP;
    }

    /* set intra refresh mode for chn 0 */
    if ((ret = sample_venc_set_intra_refresh(venc_vpss_chn->venc_chn[0], intra_refresh_mode)) != HI_SUCCESS) {
        goto EXIT_VENC_H265_UnBind;
    }

    /* encode h.264 */
    if ((ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[1], &(chn_param[1]))) != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        goto EXIT_VENC_H265_UnBind;
    }

    ret = sample_venc_vpss_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[1], venc_vpss_chn->venc_chn[1]);
    if (ret != HI_SUCCESS) {
        goto EXIT_VENC_H264_STOP;
    }

    /* set intra refresh mode for chn 1 */
    if ((ret = sample_venc_set_intra_refresh(venc_vpss_chn->venc_chn[1], intra_refresh_mode)) != HI_SUCCESS) {
        goto EXIT_VENC_H264_UnBind;
    }

    /* *****************************************
     stream save process
    ***************************************** */
    if ((ret = sample_comm_venc_start_get_stream(venc_vpss_chn->venc_chn, CHN_NUM_MAX)) != HI_SUCCESS) {
        sample_print("Start Venc failed!\n");
        goto EXIT_VENC_H264_UnBind;
    }

    return ret;

EXIT_VENC_H264_UnBind:
    sample_comm_vpss_un_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[1], venc_vpss_chn->venc_chn[1]);
EXIT_VENC_H264_STOP:
    sample_comm_venc_stop(venc_vpss_chn->venc_chn[1]);
EXIT_VENC_H265_UnBind:
    sample_comm_vpss_un_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[0], venc_vpss_chn->venc_chn[0]);
EXIT_VENC_H265_STOP:
    sample_comm_venc_stop(venc_vpss_chn->venc_chn[0]);

    return ret;
}

/* *****************************************************************************
 * function : intra_refresh:H.265e@2560x1440@30fps(row) + h264e@1280x720@30fps(column).
 * **************************************************************************** */
static hi_s32 sample_venc_intra_refresh(hi_void)
{
    hi_s32 ret;
    sample_sns_type sns_type = 0;
    sample_vi_cfg vi_cfg = {0};
    sample_vpss_cfg vpss_cfg = {0};
    hi_vpss_grp vpss_grp = 0;
    sample_venc_vpss_chn venc_vpss_chn = {0};
    sample_venc_param enc_param = {0};
    hi_pic_size pic_size[CHN_NUM_MAX] = {0};

    /* *****************************************
      step 0: related parameter ready
    ***************************************** */
    sns_type = SENSOR0_TYPE;
    enc_param.venc_chn_num = CHN_NUM_MAX;
    sample_venc_set_pic_size_0(sns_type, pic_size);
    pic_size[1] = SMALL_STREAM_SIZE;
    sample_venc_get_enc_size(pic_size, &enc_param);
    sample_set_venc_vpss_chn(&venc_vpss_chn, enc_param.venc_chn_num);

    /* *****************************************
      step 1: init sys alloc common vb
    ***************************************** */
    if ((ret = sample_venc_online_wrap_start_sys_vi_vpss(sns_type, &vi_cfg, &vpss_cfg, &enc_param)) != HI_SUCCESS) {
        return ret;
    }

    /* *****************************************
    start stream venc
    ***************************************** */
    ret = sample_venc_intra_refresh_start_encode(&enc_param.enc_size[0], CHN_NUM_MAX, vpss_grp, &venc_vpss_chn);
    if (ret != HI_SUCCESS) {
        goto EXIT_SYS_VI_VPSS;
    }

    /* *****************************************
     exit process
    ***************************************** */
    sample_venc_exit_process();
    sample_venc_unbind_vpss_stop(vpss_grp, &venc_vpss_chn);

EXIT_SYS_VI_VPSS:
    sample_venc_online_wrap_stop_sys_vi_vpss(&vi_cfg, &vpss_cfg);

    return ret;
}

static hi_void sample_venc_roi_attr_init(hi_venc_roi_attr *roi_attr)
{
    roi_attr->is_abs_qp = HI_TRUE;
    roi_attr->enable = HI_TRUE;
    roi_attr->qp = 30; /* 30: qp value */
    roi_attr->idx = 0;
    roi_attr->rect.x = 64;       /* 64: rect.x value */
    roi_attr->rect.y = 64;       /* 64: rect.y value */
    roi_attr->rect.height = 256; /* 256: rect.height value */
    roi_attr->rect.width = 256;  /* 256: rect.width value */
}

static hi_void sample_venc_roi_bg_frame_rate_init(hi_venc_roi_bg_frame_rate *roi_bg_frame_rate)
{
    roi_bg_frame_rate->src_frame_rate = 30; /* 30: src_frame_rate value */
    roi_bg_frame_rate->dst_frame_rate = 15; /* 15: dst_frame_rate value */
}

static hi_s32 sample_venc_set_roi_attr(hi_venc_chn venc_chn)
{
    hi_s32 ret;
    hi_venc_roi_attr roi_attr;
    hi_venc_roi_bg_frame_rate roi_bg_frame_rate;

    if ((ret = hi_mpi_venc_get_roi_attr(venc_chn, 0, &roi_attr)) != HI_SUCCESS) { /* 0: roi index */
        sample_print("chn %d Get Roi Attr failed for %#x!\n", venc_chn, ret);
        return ret;
    }

    sample_venc_roi_attr_init(&roi_attr);

    if ((ret = hi_mpi_venc_set_roi_attr(venc_chn, &roi_attr)) != HI_SUCCESS) {
        sample_print("chn %d Set Roi Attr failed for %#x!\n", venc_chn, ret);
        return ret;
    }

    if ((ret = hi_mpi_venc_get_roi_bg_frame_rate(venc_chn, &roi_bg_frame_rate)) != HI_SUCCESS) {
        sample_print("chn %d Get Roi BgFrameRate failed for %#x!\n", venc_chn, ret);
        return ret;
    }

    sample_venc_roi_bg_frame_rate_init(&roi_bg_frame_rate);

    if ((ret = hi_mpi_venc_set_roi_bg_frame_rate(venc_chn, &roi_bg_frame_rate)) != HI_SUCCESS) {
        sample_print("chn %d Set Roi BgFrameRate failed for %#x!\n", venc_chn, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_venc_roi_bg_start_encode(hi_size *enc_size, hi_s32 chn_num_max, hi_vpss_grp vpss_grp,
    sample_venc_vpss_chn *venc_vpss_chn)
{
    hi_s32 ret;
    hi_venc_gop_attr gop_attr;
    sample_comm_venc_chn_param chn_param[CHN_NUM_MAX] = {0};

    if ((ret = sample_comm_venc_get_gop_default_attr(&gop_attr)) != HI_SUCCESS) {
        return ret;
    }

    if ((ret = sample_venc_set_video_param(enc_size, chn_param, CHN_NUM_MAX, gop_attr, HI_FALSE)) != HI_SUCCESS) {
        return ret;
    }

    if ((ret = sample_comm_venc_mini_buf_en(chn_param, CHN_NUM_MAX)) != HI_SUCCESS) {
        return ret;
    }

    /* encode h.265 */
    if ((ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[0], &(chn_param[0]))) != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        return ret;
    }

    ret = sample_venc_vpss_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[0], venc_vpss_chn->venc_chn[0]);
    if (ret != HI_SUCCESS) {
        goto EXIT_VENC_H265_STOP;
    }

    /* set roi bg frame rate for chn 0 */
    if ((ret = sample_venc_set_roi_attr(venc_vpss_chn->venc_chn[0])) != HI_SUCCESS) {
        goto EXIT_VENC_H265_UnBind;
    }

    /* encode h.264 */
    if ((ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[1], &(chn_param[1]))) != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        goto EXIT_VENC_H265_UnBind;
    }

    ret = sample_venc_vpss_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[1], venc_vpss_chn->venc_chn[1]);
    if (ret != HI_SUCCESS) {
        goto EXIT_VENC_H264_STOP;
    }

    /* set roi bg frame rate for chn 1 */
    if ((ret = sample_venc_set_roi_attr(venc_vpss_chn->venc_chn[1])) != HI_SUCCESS) {
        goto EXIT_VENC_H264_UnBind;
    }

    /* *****************************************
     stream save process
    ***************************************** */
    if ((ret = sample_comm_venc_start_get_stream(venc_vpss_chn->venc_chn, CHN_NUM_MAX)) != HI_SUCCESS) {
        sample_print("Start Venc failed!\n");
        goto EXIT_VENC_H264_UnBind;
    }

    return ret;

EXIT_VENC_H264_UnBind:
    sample_comm_vpss_un_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[1], venc_vpss_chn->venc_chn[1]);
EXIT_VENC_H264_STOP:
    sample_comm_venc_stop(venc_vpss_chn->venc_chn[1]);
EXIT_VENC_H265_UnBind:
    sample_comm_vpss_un_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[0], venc_vpss_chn->venc_chn[0]);
EXIT_VENC_H265_STOP:
    sample_comm_venc_stop(venc_vpss_chn->venc_chn[0]);

    return ret;
}

/* *****************************************************************************
 * function : roi_bg_frame_rate:H.265e@2560x1440@30fps + H.264@1280x720@30fps.
 * **************************************************************************** */
static hi_s32 sample_venc_roi_bg(hi_void)
{
    hi_s32 ret;
    sample_sns_type sns_type = 0;
    sample_vi_cfg vi_cfg = {0};
    sample_vpss_cfg vpss_cfg = {0};
    hi_vpss_grp vpss_grp = 0;
    sample_venc_vpss_chn venc_vpss_chn = {0};
    sample_venc_param enc_param = {0};
    hi_pic_size pic_size[CHN_NUM_MAX] = {0};
    hi_size *enc_size = HI_NULL;

    /* *****************************************
      step 0: related parameter ready
    ***************************************** */
    sns_type = SENSOR0_TYPE;
    enc_param.venc_chn_num = CHN_NUM_MAX;
    sample_venc_set_pic_size_0(sns_type, pic_size);
    pic_size[1] = SMALL_STREAM_SIZE;
    sample_venc_get_enc_size(pic_size, &enc_param);
    sample_set_venc_vpss_chn(&venc_vpss_chn, enc_param.venc_chn_num);
    enc_size = &enc_param.enc_size[0];

    /* *****************************************
      step 1: init sys alloc common vb
    ***************************************** */
    if ((ret = sample_venc_online_wrap_start_sys_vi_vpss(sns_type, &vi_cfg, &vpss_cfg, &enc_param)) != HI_SUCCESS) {
        return ret;
    }

    /* *****************************************
    start stream venc
    ***************************************** */
    if ((ret = sample_venc_roi_bg_start_encode(enc_size, CHN_NUM_MAX, vpss_grp, &venc_vpss_chn)) != HI_SUCCESS) {
        goto EXIT_SYS_VI_VPSS;
    }

    /* *****************************************
     exit process
    ***************************************** */
    sample_venc_exit_process();
    sample_venc_unbind_vpss_stop(vpss_grp, &venc_vpss_chn);

EXIT_SYS_VI_VPSS:
    sample_venc_online_wrap_stop_sys_vi_vpss(&vi_cfg, &vpss_cfg);

    return ret;
}

static hi_s32 sample_venc_set_debreath_effect(hi_venc_chn venc_chn, hi_bool enable)
{
    hi_s32 ret;
    hi_venc_debreath_effect debreath_effect;

    if ((ret = hi_mpi_venc_get_debreath_effect(venc_chn, &debreath_effect)) != HI_SUCCESS) {
        sample_print("Get debreath_effect failed for %#x!\n", ret);
        return ret;
    }

    if (enable) {
        debreath_effect.enable = HI_TRUE;
        debreath_effect.strength0 = 3;  /* 3 : param */
        debreath_effect.strength1 = 20; /* 20 : param */
    } else {
        debreath_effect.enable = HI_FALSE;
    }

    if ((ret = hi_mpi_venc_set_debreath_effect(venc_chn, &debreath_effect)) != HI_SUCCESS) {
        sample_print("Set debreath_effect failed for %#x!\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_venc_debreath_effect_start_encode(hi_size *enc_size, hi_s32 chn_num_max,
    hi_vpss_grp vpss_grp, sample_venc_vpss_chn *venc_vpss_chn)
{
    hi_s32 ret;
    hi_venc_gop_attr gop_attr;
    sample_comm_venc_chn_param chn_param[CHN_NUM_MAX] = {0};

    if ((ret = sample_comm_venc_get_gop_default_attr(&gop_attr)) != HI_SUCCESS) {
        return ret;
    }

    if ((ret = sample_venc_set_video_param(enc_size, chn_param, CHN_NUM_MAX, gop_attr, HI_FALSE)) != HI_SUCCESS) {
        return ret;
    }

    if ((ret = sample_comm_venc_mini_buf_en(chn_param, CHN_NUM_MAX)) != HI_SUCCESS) {
        return ret;
    }

    /* encode h.265 */
    if ((ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[0], &(chn_param[0]))) != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        return ret;
    }

    ret = sample_venc_vpss_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[0], venc_vpss_chn->venc_chn[0]);
    if (ret != HI_SUCCESS) {
        goto EXIT_VENC_H265_STOP;
    }

    /* set intra refresh mode for chn 0 */
    if ((ret = sample_venc_set_debreath_effect(venc_vpss_chn->venc_chn[0], HI_TRUE)) != HI_SUCCESS) {
        goto EXIT_VENC_H265_UnBind;
    }

    /* encode h.264 */
    if ((ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[1], &(chn_param[1]))) != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        goto EXIT_VENC_H265_UnBind;
    }

    ret = sample_venc_vpss_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[1], venc_vpss_chn->venc_chn[1]);
    if (ret != HI_SUCCESS) {
        goto EXIT_VENC_H264_STOP;
    }

    /* set intra refresh mode for chn 1 */
    if ((ret = sample_venc_set_debreath_effect(venc_vpss_chn->venc_chn[1], HI_FALSE)) != HI_SUCCESS) {
        goto EXIT_VENC_H264_UnBind;
    }

    /* *****************************************
     stream save process
    ***************************************** */
    if ((ret = sample_comm_venc_start_get_stream(venc_vpss_chn->venc_chn, CHN_NUM_MAX)) != HI_SUCCESS) {
        sample_print("Start Venc failed!\n");
        goto EXIT_VENC_H264_UnBind;
    }

    return HI_SUCCESS;

EXIT_VENC_H264_UnBind:
    sample_comm_vpss_un_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[1], venc_vpss_chn->venc_chn[1]);
EXIT_VENC_H264_STOP:
    sample_comm_venc_stop(venc_vpss_chn->venc_chn[1]);
EXIT_VENC_H265_UnBind:
    sample_comm_vpss_un_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[0], venc_vpss_chn->venc_chn[0]);
EXIT_VENC_H265_STOP:
    sample_comm_venc_stop(venc_vpss_chn->venc_chn[0]);

    return ret;
}

static hi_s32 sample_venc_debreath_effect(hi_void)
{
    hi_s32 ret;
    sample_sns_type sns_type = SENSOR0_TYPE;
    sample_vi_cfg vi_cfg;
    const hi_vi_pipe vi_pipe = 0;
    const hi_vi_chn vi_chn = 0;
    const hi_vpss_grp vpss_grp = 0;
    sample_venc_vpss_chn_attr vpss_param;
    sample_venc_vb_attr vb_attr = { 0 };
    hi_size enc_size[CHN_NUM_MAX], vi_size;
    sample_venc_vpss_chn venc_vpss_chn = { 0 };

    sample_set_venc_vpss_chn(&venc_vpss_chn, CHN_NUM_MAX);
    sample_comm_vi_get_default_vi_cfg(sns_type, &vi_cfg);
    sample_comm_vi_get_size_by_sns_type(sns_type, &vi_size);
    /* *****************************************
      step 0: related parameter ready
    ***************************************** */
    if ((ret = sample_venc_init_param(enc_size, CHN_NUM_MAX, &vi_size, &vpss_param)) != HI_SUCCESS) {
        return ret;
    }

    /* *****************************************
      step 1: init sys alloc common vb
    ***************************************** */
    get_vb_attr(&vi_size, &vpss_param, &vb_attr);
    if ((ret = sample_venc_sys_init(&vb_attr)) != HI_SUCCESS) {
        sample_print("Init SYS err for %#x!\n", ret);
        return ret;
    }

    if ((ret = sample_venc_vi_init(&vi_cfg)) != HI_SUCCESS) {
        sample_print("Init VI err for %#x!\n", ret);
        goto EXIT_SYS_STOP;
    }

    if ((ret = sample_venc_vpss_init(vpss_grp, &vpss_param)) != HI_SUCCESS) {
        sample_print("Init VPSS err for %#x!\n", ret);
        goto EXIT_VI_STOP;
    }

    if ((ret = sample_comm_vi_bind_vpss(vi_pipe, vi_chn, vpss_grp, 0)) != HI_SUCCESS) {
        sample_print("VI Bind VPSS err for %#x!\n", ret);
        goto EXIT_VPSS_STOP;
    }

    /* *****************************************
    start stream venc
    ***************************************** */
    ret = sample_venc_debreath_effect_start_encode(enc_size, CHN_NUM_MAX, vpss_grp, &venc_vpss_chn);
    if (ret != HI_SUCCESS) {
        goto EXIT_VI_VPSS_UNBIND;
    }

    /* *****************************************
     exit process
    ***************************************** */
    sample_venc_exit_process();
    sample_venc_unbind_vpss_stop(vpss_grp, &venc_vpss_chn);

EXIT_VI_VPSS_UNBIND:
    sample_comm_vi_un_bind_vpss(vi_pipe, vi_chn, vpss_grp, 0);
EXIT_VPSS_STOP:
    sample_venc_vpss_deinit(vpss_grp, &vpss_param);
EXIT_VI_STOP:
    sample_venc_vi_deinit(&vi_cfg);
EXIT_SYS_STOP:
    sample_comm_sys_exit();

    return ret;
}

static hi_void sample_venc_set_jpeg_param(hi_size *enc_size, hi_s32 enc_len,
    sample_comm_venc_chn_param *venc_create_param, hi_s32 creat_size, hi_venc_gop_attr gop_attr)
{
    hi_u32 profile[CHN_NUM_MAX] = {0, 0};
    hi_bool share_buf_en = HI_TRUE;
    hi_payload_type payload[CHN_NUM_MAX] = {HI_PT_MJPEG, HI_PT_MJPEG};
    sample_rc rc_mode = 0;

    if (get_rc_mode(payload[0], &rc_mode) != HI_SUCCESS) {
        return;
    }

    venc_create_param[0].frame_rate = 30; /* 30: is a number */
    venc_create_param[0].gop = 60; /* 60: is a number */
    venc_create_param[0].stats_time = 2; /* 2: is a number */
    venc_create_param[0].gop_attr = gop_attr;
    venc_create_param[0].type = payload[0];
    venc_create_param[0].size = sample_comm_sys_get_pic_enum(&enc_size[0]);
    venc_create_param[0].rc_mode = rc_mode;
    venc_create_param[0].profile = profile[0];
    venc_create_param[0].is_rcn_ref_share_buf = share_buf_en;

    if (enc_len == 1) {
        return;
    }

    venc_create_param[1].frame_rate = 30; /* 30: is a number */
    venc_create_param[1].gop = 60; /* 60: is a number */
    venc_create_param[1].stats_time = 2; /* 2: is a number */
    venc_create_param[1].gop_attr = gop_attr;
    venc_create_param[1].type = payload[1];
    venc_create_param[1].size = sample_comm_sys_get_pic_enum(&enc_size[1]);
    venc_create_param[1].rc_mode = rc_mode;
    venc_create_param[1].profile = profile[1];
    venc_create_param[1].is_rcn_ref_share_buf = share_buf_en;
}

static hi_void sample_venc_mjpeg_roi_param_init(hi_venc_jpeg_roi_attr *roi_param)
{
    roi_param->idx = 0;
    roi_param->enable = HI_TRUE;
    roi_param->level = 0;
    roi_param->rect.x = 0;
    roi_param->rect.y = 0;
    roi_param->rect.width = 1280; /* 1280: rect.width value */
    roi_param->rect.height = 720; /* 720: rect.height value */
}

static hi_s32 sample_venc_set_mjpeg_roi(hi_venc_chn venc_chn)
{
    hi_s32 ret;
    const hi_u32 idx = 0;
    hi_venc_jpeg_roi_attr roi_param;

    if ((ret = hi_mpi_venc_get_jpeg_roi_attr(venc_chn, idx, &roi_param)) != HI_SUCCESS) {
        sample_print("Get roi_param failed for %#x!\n", ret);
        return ret;
    }

    sample_venc_mjpeg_roi_param_init(&roi_param);

    if ((ret = hi_mpi_venc_set_jpeg_roi_attr(venc_chn, &roi_param)) != HI_SUCCESS) {
        sample_print("Set roi_param failed for %#x!\n", ret);
    }

    return ret;
}

static hi_s32 sample_venc_mjpeg_roi_set_start_encode(hi_size *enc_size, hi_s32 chn_num, hi_vpss_grp vpss_grp,
    sample_venc_vpss_chn *venc_vpss_chn)
{
    hi_s32 ret;
    hi_venc_gop_attr gop_attr;
    sample_comm_venc_chn_param venc_create_param[CHN_NUM_MAX] = {0};
    sample_comm_venc_chn_param venc_chn0_param, venc_chn1_param;

    if ((ret = sample_comm_venc_get_gop_attr(HI_VENC_GOP_MODE_NORMAL_P, &gop_attr)) != HI_SUCCESS) {
        return ret;
    }

    sample_venc_set_jpeg_param(enc_size, chn_num, venc_create_param, CHN_NUM_MAX, gop_attr);

    if ((ret = sample_comm_venc_mini_buf_en(venc_create_param, CHN_NUM_MAX)) != HI_SUCCESS) {
        return ret;
    }

    venc_chn0_param = venc_create_param[0];
    ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[0], &venc_chn0_param);
    if (ret != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        return ret;
    }

    ret = sample_venc_vpss_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[0], venc_vpss_chn->venc_chn[0]);
    if (ret != HI_SUCCESS) {
        goto EXIT_VENC_CHN0_STOP;
    }

    /* set intra refresh mode for chn 0 */
    if ((ret = sample_venc_set_mjpeg_roi(venc_vpss_chn->venc_chn[0])) != HI_SUCCESS) {
        goto EXIT_VENC_CHN_0_UnBind;
    }

    venc_chn1_param = venc_create_param[1];
    ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[1], &venc_chn1_param);
    if (ret != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        goto EXIT_VENC_CHN_0_UnBind;
    }

    ret = sample_venc_vpss_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[1], venc_vpss_chn->venc_chn[1]);
    if (ret != HI_SUCCESS) {
        goto EXIT_VENC_CHN_1_STOP;
    }

    /* *****************************************
     stream save process
    ***************************************** */
    if ((ret = sample_comm_venc_start_get_stream(venc_vpss_chn->venc_chn, CHN_NUM_MAX)) != HI_SUCCESS) {
        goto EXIT_VENC_CHN_1_UnBind;
    }

    return HI_SUCCESS;

EXIT_VENC_CHN_1_UnBind:
    sample_comm_vpss_un_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[1], venc_vpss_chn->venc_chn[1]);
EXIT_VENC_CHN_1_STOP:
    sample_comm_venc_stop(venc_vpss_chn->venc_chn[1]);
EXIT_VENC_CHN_0_UnBind:
    sample_comm_vpss_un_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[0], venc_vpss_chn->venc_chn[0]);
EXIT_VENC_CHN0_STOP:
    sample_comm_venc_stop(venc_vpss_chn->venc_chn[0]);

    return ret;
}

static hi_s32 sample_venc_mjpeg_roi_set(hi_void)
{
    hi_s32 ret;
    sample_sns_type sns_type = 0;
    sample_vi_cfg vi_cfg = {0};
    sample_vpss_cfg vpss_cfg = {0};
    hi_vpss_grp vpss_grp = 0;
    sample_venc_vpss_chn venc_vpss_chn = {0};
    sample_venc_param enc_param = {0};
    hi_pic_size pic_size[CHN_NUM_MAX] = {0};
    hi_size *enc_size = HI_NULL;

    /* *****************************************
      step 0: related parameter ready
    ***************************************** */
    sns_type = SENSOR0_TYPE;
    enc_param.venc_chn_num = CHN_NUM_MAX;
    sample_venc_set_pic_size_0(sns_type, pic_size);
    pic_size[1] = SMALL_STREAM_SIZE;
    sample_venc_get_enc_size(pic_size, &enc_param);
    sample_set_venc_vpss_chn(&venc_vpss_chn, enc_param.venc_chn_num);
    enc_size = &enc_param.enc_size[0];

    /* *****************************************
      step 1: init sys alloc common vb
    ***************************************** */
    if ((ret = sample_venc_online_wrap_start_sys_vi_vpss(sns_type, &vi_cfg, &vpss_cfg, &enc_param)) != HI_SUCCESS) {
        return ret;
    }

    /* *****************************************
    start stream venc
    ***************************************** */
    ret = sample_venc_mjpeg_roi_set_start_encode(enc_size, CHN_NUM_MAX, vpss_grp, &venc_vpss_chn);
    if (ret != HI_SUCCESS) {
        goto EXIT_SYS_VI_VPSS;
    }

    /* *****************************************
     exit process
    ***************************************** */
    sample_venc_exit_process();
    sample_venc_unbind_vpss_stop(vpss_grp, &venc_vpss_chn);

EXIT_SYS_VI_VPSS:
    sample_venc_online_wrap_stop_sys_vi_vpss(&vi_cfg, &vpss_cfg);

    return ret;
}

static hi_void sample_venc_mjpeg_roi_attr_init(hi_venc_jpeg_roi_attr *roi_attr, hi_s32 chn_num_max)
{
    if (chn_num_max < 2) { /* 2: roi_attr array len */
        sample_print("roi_attr array len not enough, need 2, current %d!\n", chn_num_max);
        return;
    }
    roi_attr[0].enable = 1;
    roi_attr[0].idx = 0;
    roi_attr[0].rect.x = 0;
    roi_attr[0].rect.y = 0;
    roi_attr[0].rect.width = 160;  /* 160: rect.width value */
    roi_attr[0].rect.height = 160; /* 160: rect.height value */
    roi_attr[0].level = 0;

    roi_attr[1].enable = 1;
    roi_attr[1].idx = 0;
    roi_attr[1].rect.x = 0;
    roi_attr[1].rect.y = 0;
    roi_attr[1].rect.width = 160;  /* 160: rect.width value */
    roi_attr[1].rect.height = 160; /* 160: rect.height value */
    roi_attr[1].level = 0;
}

static hi_s32 sample_venc_mjpeg_roimap_start_encode(hi_vpss_grp vpss_grp, hi_venc_jpeg_roi_attr *roi_attr,
    hi_size *enc_size, hi_s32 chn_num_max, sample_venc_vpss_chn *venc_vpss_chn)
{
    hi_s32 ret;
    hi_venc_gop_attr gop_attr;
    sample_comm_venc_chn_param venc_create_param[CHN_NUM_MAX] = {0};
    sample_comm_venc_chn_param venc_chn0_param, venc_chn1_param;
    sample_venc_roimap_chn_info roimap_chn_info = { 0 };

    if ((ret = sample_comm_venc_get_gop_attr(HI_VENC_GOP_MODE_NORMAL_P, &gop_attr)) != HI_SUCCESS) {
        sample_print("Venc Get GopAttr for %#x!\n", ret);
        return ret;
    }

    sample_venc_set_jpeg_param(enc_size, chn_num_max, venc_create_param, CHN_NUM_MAX, gop_attr);

    if ((ret = sample_comm_venc_mini_buf_en(venc_create_param, CHN_NUM_MAX)) != HI_SUCCESS) {
        return ret;
    }

    venc_chn0_param = venc_create_param[0];
    ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[0], &venc_chn0_param);
    if (ret != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        return ret;
    }

    venc_chn1_param = venc_create_param[1];
    ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[1], &venc_chn1_param);
    if (ret != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        goto EXIT_VENC_CHN0_STOP;
    }

    roimap_chn_info.vpss_chn = venc_vpss_chn->vpss_chn;
    roimap_chn_info.venc_chn = venc_vpss_chn->venc_chn;
    roimap_chn_info.cnt = chn_num_max;
    ret = sample_comm_venc_send_roimap_frame(vpss_grp, roimap_chn_info, enc_size, roi_attr);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_venc_qpmap_send_frame failed for %#x!\n", ret);
        goto EXIT_VENC_CHN1_STOP;
    }

    /* *****************************************
     stream save process
    ***************************************** */
    if ((ret = sample_comm_venc_start_get_stream(venc_vpss_chn->venc_chn, CHN_NUM_MAX)) != HI_SUCCESS) {
        sample_print("Start Venc failed!\n");
        goto EXIT_VENC_CHN1_STOP;
    }

    return HI_SUCCESS;

EXIT_VENC_CHN1_STOP:
    sample_comm_venc_stop(venc_vpss_chn->venc_chn[1]);
EXIT_VENC_CHN0_STOP:
    sample_comm_venc_stop(venc_vpss_chn->venc_chn[0]);

    return ret;
}

static hi_void sample_venc_mjpeg_roimap_exit_process()
{
    printf("please press twice ENTER to exit this sample\n");
    (hi_void)getchar();

    if (g_sample_venc_exit != HI_TRUE) {
        (hi_void)getchar();
    }

    sample_comm_venc_stop_send_roimap_frame();
    sample_comm_venc_stop_get_stream(CHN_NUM_MAX);
}

static hi_s32 sample_venc_mjpeg_roimap(hi_void)
{
    hi_s32 ret;
    sample_sns_type sns_type = SENSOR0_TYPE;
    sample_vi_cfg vi_cfg;
    hi_size vi_size;
    const hi_vi_pipe vi_pipe = 0;
    const hi_vi_chn vi_chn = 0;
    const hi_vpss_grp vpss_grp = 0;
    sample_venc_vpss_chn_attr vpss_param;
    sample_venc_vb_attr vb_attr = { 0 };
    hi_venc_jpeg_roi_attr roi_attr[CHN_NUM_MAX];
    hi_size enc_size[CHN_NUM_MAX];
    sample_venc_vpss_chn venc_vpss_chn = { 0 };

    sample_set_venc_vpss_chn(&venc_vpss_chn, CHN_NUM_MAX);
    sample_comm_vi_get_default_vi_cfg(sns_type, &vi_cfg);
    sample_comm_vi_get_size_by_sns_type(sns_type, &vi_size);

    sample_venc_mjpeg_roi_attr_init(roi_attr, CHN_NUM_MAX);

    ret = sample_venc_init_param(enc_size, CHN_NUM_MAX, &vi_size, &vpss_param);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /* *****************************************
      step 1: init sys alloc common vb
    ***************************************** */
    get_vb_attr(&vi_size, &vpss_param, &vb_attr);

    if ((ret = sample_venc_sys_init(&vb_attr)) != HI_SUCCESS) {
        return ret;
    }

    if ((ret = sample_venc_vi_init(&vi_cfg)) != HI_SUCCESS) {
        goto EXIT_SYS_STOP;
    }

    if ((ret = sample_venc_vpss_init(vpss_grp, &vpss_param)) != HI_SUCCESS) {
        goto EXIT_VI_STOP;
    }

    if ((ret = sample_comm_vi_bind_vpss(vi_pipe, vi_chn, vpss_grp, 0)) != HI_SUCCESS) {
        goto EXIT_VPSS_STOP;
    }

    ret = sample_venc_mjpeg_roimap_start_encode(vpss_grp, roi_attr, enc_size, CHN_NUM_MAX, &venc_vpss_chn);
    if (ret != HI_SUCCESS) {
        goto EXIT_VI_VPSS_UNBIND;
    }

    /* *****************************************
     exit process
    ***************************************** */
    sample_venc_mjpeg_roimap_exit_process();
    sample_venc_stop(&venc_vpss_chn, CHN_NUM_MAX);

EXIT_VI_VPSS_UNBIND:
    sample_comm_vi_un_bind_vpss(vi_pipe, vi_chn, vpss_grp, 0);
EXIT_VPSS_STOP:
    sample_venc_vpss_deinit(vpss_grp, &vpss_param);
EXIT_VI_STOP:
    sample_venc_vi_deinit(&vi_cfg);
EXIT_SYS_STOP:
    sample_comm_sys_exit();

    return ret;
}

static hi_s32 sample_venc_mosaic_map_start_encode(hi_size *enc_size, hi_s32 chn_num_max, hi_vpss_grp vpss_grp,
    sample_venc_vpss_chn *venc_vpss_chn)
{
    hi_s32 ret;
    hi_venc_gop_mode gop_mode;
    hi_venc_gop_attr gop_attr;
    sample_comm_venc_chn_param chn_param[CHN_NUM_MAX] = {0};

    if (get_gop_mode(&gop_mode) != HI_SUCCESS) {
        return HI_FAILURE;
    }
    if ((ret = sample_comm_venc_get_gop_attr(gop_mode, &gop_attr)) != HI_SUCCESS) {
        sample_print("Venc Get GopAttr for %#x!\n", ret);
        return ret;
    }

    if ((ret = sample_venc_set_video_param(enc_size, chn_param, CHN_NUM_MAX, gop_attr, HI_FALSE)) != HI_SUCCESS) {
        return ret;
    }

    if ((ret = sample_comm_venc_mini_buf_en(chn_param, CHN_NUM_MAX)) != HI_SUCCESS) {
        return ret;
    }

    /* encode h.265 */
    ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[0], &(chn_param[0]));
    if (ret != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        return ret;
    }

    /* encode h.264 */
    ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[1], &(chn_param[1]));
    if (ret != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        goto EXIT_VENC_H265_STOP;
    }

    ret = sample_comm_venc_mosaic_map_send_frame(vpss_grp, venc_vpss_chn->vpss_chn, venc_vpss_chn->venc_chn,
        CHN_NUM_MAX, enc_size);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_venc_qpmap_send_frame failed for %#x!\n", ret);
        goto EXIT_VENC_H264_STOP;
    }

    /* *****************************************
     stream save process
    ***************************************** */
    if ((ret = sample_comm_venc_start_get_stream(venc_vpss_chn->venc_chn, CHN_NUM_MAX)) != HI_SUCCESS) {
        sample_print("Start Venc failed!\n");
        goto EXIT_VENC_H264_STOP;
    }

    return HI_SUCCESS;

EXIT_VENC_H264_STOP:
    sample_comm_venc_stop(venc_vpss_chn->venc_chn[1]);
EXIT_VENC_H265_STOP:
    sample_comm_venc_stop(venc_vpss_chn->venc_chn[0]);

    return ret;
}

static hi_s32 sample_venc_mosaic_map(hi_void)
{
    hi_s32 ret;
    sample_sns_type sns_type = SENSOR0_TYPE;
    sample_vi_cfg vi_cfg = { 0 };
    hi_size enc_size[CHN_NUM_MAX], vi_size;
    const hi_vi_pipe vi_pipe = 0;
    const hi_vi_chn vi_chn = 0;
    const hi_vpss_grp vpss_grp = 0;
    sample_venc_vpss_chn_attr vpss_param = { 0 };
    sample_venc_vb_attr vb_attr = { 0 };
    sample_venc_vpss_chn venc_vpss_chn = { 0 };

    sample_set_venc_vpss_chn(&venc_vpss_chn, CHN_NUM_MAX);
    sample_comm_vi_get_default_vi_cfg(sns_type, &vi_cfg);
    sample_comm_vi_get_size_by_sns_type(sns_type, &vi_size);
    /* *****************************************
      step 0: related parameter ready
    ***************************************** */
    ret = sample_venc_init_param(enc_size, CHN_NUM_MAX, &vi_size, &vpss_param);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    /* *****************************************
      step 1: init sys alloc common vb
    ***************************************** */
    get_vb_attr(&vi_size, &vpss_param, &vb_attr);
    if ((ret = sample_venc_sys_init(&vb_attr)) != HI_SUCCESS) {
        sample_print("Init SYS err for %#x!\n", ret);
        return ret;
    }

    if ((ret = sample_venc_vi_init(&vi_cfg)) != HI_SUCCESS) {
        sample_print("Init VI err for %#x!\n", ret);
        goto EXIT_SYS_STOP;
    }

    if ((ret = sample_venc_vpss_init(vpss_grp, &vpss_param)) != HI_SUCCESS) {
        sample_print("Init VPSS err for %#x!\n", ret);
        goto EXIT_VI_STOP;
    }

    if ((ret = sample_comm_vi_bind_vpss(vi_pipe, vi_chn, vpss_grp, 0)) != HI_SUCCESS) {
        sample_print("VI Bind VPSS err for %#x!\n", ret);
        goto EXIT_VPSS_STOP;
    }

    /* *****************************************
    start stream venc
    ***************************************** */
    if ((ret = sample_venc_mosaic_map_start_encode(enc_size, CHN_NUM_MAX, vpss_grp, &venc_vpss_chn)) != HI_SUCCESS) {
        goto EXIT_VI_VPSS_UNBIND;
    }

    /* *****************************************
     exit process
    ***************************************** */
    sample_venc_mosaic_map_exit_process(CHN_NUM_MAX);
    // last stop venc chn
    sample_venc_stop(&venc_vpss_chn, CHN_NUM_MAX);

EXIT_VI_VPSS_UNBIND:
    sample_comm_vi_un_bind_vpss(vi_pipe, vi_chn, vpss_grp, 0);
EXIT_VPSS_STOP:
    sample_venc_vpss_deinit(vpss_grp, &vpss_param);
EXIT_VI_STOP:
    sample_venc_vi_deinit(&vi_cfg);
EXIT_SYS_STOP:
    sample_comm_sys_exit();

    return ret;
}

static hi_s32 sample_venc_svac3_start_encode(hi_size *enc_size, hi_s32 chn_num_max,
    hi_vpss_grp vpss_grp, sample_venc_vpss_chn *venc_vpss_chn)
{
    hi_s32 ret;
    hi_venc_gop_mode gop_mode;
    hi_venc_gop_attr gop_attr;
    sample_comm_venc_chn_param chn_param[SVAC3_CHN_NUM_MAX] = {0};

    if (svac3_get_gop_mode(&gop_mode) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if ((ret = sample_comm_venc_get_gop_attr(gop_mode, &gop_attr)) != HI_SUCCESS) {
        sample_print("Venc Get GopAttr for %#x!\n", ret);
        return ret;
    }

    if ((ret = sample_venc_set_svac3_video_param(enc_size, chn_param, SVAC3_CHN_NUM_MAX, gop_attr, HI_FALSE)) \
    != HI_SUCCESS) {
        return ret;
    }

    if ((ret = sample_comm_venc_mini_buf_en(chn_param, SVAC3_CHN_NUM_MAX)) != HI_SUCCESS) {
        return ret;
    }

    /* encode svac3 */
    if ((ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[0], &(chn_param[0]))) != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        return ret;
    }

    ret = sample_comm_vpss_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[0], venc_vpss_chn->venc_chn[0]);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vpss_bind_venc failed for %#x!\n", ret);
        goto EXIT_VENC_SVAC3_STOP;
    }

    /* *****************************************
     stream save process
    ***************************************** */
    if ((ret = sample_comm_venc_start_get_stream(venc_vpss_chn->venc_chn, SVAC3_CHN_NUM_MAX)) != HI_SUCCESS) {
        sample_print("Start Venc failed!\n");
        goto EXIT_VENC_SVAC3_UnBind;
    }

    return HI_SUCCESS;

EXIT_VENC_SVAC3_UnBind:
    sample_comm_vpss_un_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[0], venc_vpss_chn->venc_chn[0]);
EXIT_VENC_SVAC3_STOP:
    sample_comm_venc_stop(venc_vpss_chn->venc_chn[0]);

    return ret;
}

static hi_void sample_venc_svac3_exit_process()
{
    printf("please press twice ENTER to exit this sample\n");
    (hi_void)getchar();

    if (g_sample_venc_exit != HI_TRUE) {
        (hi_void)getchar();
    }
    sample_comm_venc_stop_get_stream(SVAC3_CHN_NUM_MAX);
}

/* *****************************************************************************
 * function :  svac3@2560x1440@30fps
 * **************************************************************************** */
static hi_s32 sample_venc_svac3(hi_void)
{
    hi_s32 ret;
    sample_sns_type sns_type = 0;
    sample_vi_cfg vi_cfg = {0};
    sample_vpss_cfg vpss_cfg = {0};
    hi_vpss_grp vpss_grp = 0;
    sample_venc_vpss_chn venc_vpss_chn = {0};
    sample_venc_param enc_param = {0};
    hi_pic_size pic_size[CHN_NUM_MAX] = {0};
    hi_size *enc_size = HI_NULL;

    /* *****************************************
      step 0: related parameter ready
    ***************************************** */
    sns_type = SENSOR0_TYPE;
    enc_param.venc_chn_num = 1;
    sample_venc_set_pic_size_0(sns_type, pic_size);
    sample_venc_get_enc_size(pic_size, &enc_param);
    sample_set_venc_vpss_chn(&venc_vpss_chn, enc_param.venc_chn_num);
    enc_size = &enc_param.enc_size[0];

    /* *****************************************
      step 1: init sys alloc common vb
    ***************************************** */
    if ((ret = sample_venc_online_wrap_start_sys_vi_vpss(sns_type, &vi_cfg, &vpss_cfg, &enc_param)) != HI_SUCCESS) {
        return ret;
    }

    if ((ret = sample_venc_svac3_start_encode(enc_size, SVAC3_CHN_NUM_MAX, vpss_grp, &venc_vpss_chn)) != HI_SUCCESS) {
        goto EXIT_SYS_VI_VPSS;
    }

    /* *****************************************
     exit process
    ***************************************** */
    sample_venc_svac3_exit_process();
    sample_venc_svac3_unbind_vpss_stop(vpss_grp, &venc_vpss_chn);

EXIT_SYS_VI_VPSS:
    sample_venc_online_wrap_stop_sys_vi_vpss(&vi_cfg, &vpss_cfg);

    return ret;
}

static hi_s32 sample_venc_chn_bind_venc_chn(hi_s32 src_chn_id, hi_s32 dest_chn_id)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_VENC;
    src_chn.dev_id = 1;
    src_chn.chn_id = src_chn_id;

    dest_chn.mod_id = HI_ID_VENC;
    dest_chn.dev_id = 0;
    dest_chn.chn_id = dest_chn_id;

    check_return(hi_mpi_sys_bind(&src_chn, &dest_chn), "hi_mpi_sys_bind(VENC-VENC)");

    return HI_SUCCESS;
}

static hi_s32 sample_venc_chn_un_bind_venc_chn(hi_s32 src_chn_id, hi_s32 dest_chn_id)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_VENC;
    src_chn.dev_id = 1;
    src_chn.chn_id = src_chn_id;

    dest_chn.mod_id = HI_ID_VENC;
    dest_chn.dev_id = 0;
    dest_chn.chn_id = dest_chn_id;

    check_return(hi_mpi_sys_unbind(&src_chn, &dest_chn), "hi_mpi_sys_unbind(VENC-VENC)");

    return HI_SUCCESS;
}

static hi_s32 get_payload_and_gop_mode_and_gop_attr(hi_venc_gop_mode *gop_mode, hi_payload_type *payload,
    hi_venc_gop_attr *gop_attr)
{
    hi_s32 ret = HI_FAILURE;

    if (get_payload_type(payload) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (*payload == HI_PT_SVAC3) {
        ret = svac3_get_gop_mode(gop_mode);
    } else {
        ret = get_gop_mode(gop_mode);
    }

    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if ((sample_comm_venc_get_gop_attr(*gop_mode, gop_attr)) != HI_SUCCESS) {
        sample_print("Venc Get GopAttr fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_venc_inner_scale_start_encode(hi_size *enc_size, hi_s32 chn_num_max,
    hi_vpss_grp vpss_grp, sample_venc_vpss_chn *venc_vpss_chn)
{
    hi_s32 ret;
    hi_venc_gop_mode gop_mode = HI_VENC_GOP_MODE_BUTT;
    hi_venc_gop_attr gop_attr;
    sample_comm_venc_chn_param chn_param[CHN_NUM_MAX] = {0};
    hi_payload_type payload = HI_PT_BUTT;

    if ((ret = get_payload_and_gop_mode_and_gop_attr(&gop_mode, &payload, &gop_attr)) != HI_SUCCESS) {
        return ret;
    }

    if ((ret = sample_venc_set_video_param(enc_size, chn_param, CHN_NUM_MAX, gop_attr, HI_FALSE)) != HI_SUCCESS) {
        return ret;
    }
    sample_venc_set_inner_scale_video_param(payload, chn_param);

    if ((ret = sample_comm_venc_mini_buf_en(chn_param, CHN_NUM_MAX)) != HI_SUCCESS) {
        return ret;
    }

    /* encode chn 0 */
    if ((ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[0], &(chn_param[0]))) != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        return ret;
    }

    ret = sample_comm_vpss_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[0], venc_vpss_chn->venc_chn[0]);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vpss_bind_venc failed for %#x!\n", ret);
        goto EXIT_VENC_STOP_CHN_0;
    }

    /* encode chn 1 */
    if ((ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[1], &(chn_param[1]))) != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        goto EXIT_VENC_VPSS_UnBind;
    }

    ret = sample_venc_chn_bind_venc_chn(venc_vpss_chn->venc_chn[0], venc_vpss_chn->venc_chn[1]);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vpss_bind_venc failed for %#x!\n", ret);
        goto EXIT_VENC_STOP_CHN_1;
    }

    /* *****************************************
     stream save process
    ***************************************** */
    if ((ret = sample_comm_venc_start_get_stream(venc_vpss_chn->venc_chn, CHN_NUM_MAX)) != HI_SUCCESS) {
        sample_print("Start Venc failed!\n");
        goto EXIT_VENC_VENC_UnBind;
    }

    return HI_SUCCESS;

EXIT_VENC_VENC_UnBind:
    sample_venc_chn_un_bind_venc_chn(venc_vpss_chn->venc_chn[0], venc_vpss_chn->venc_chn[1]);
EXIT_VENC_STOP_CHN_1:
    sample_comm_venc_stop(venc_vpss_chn->venc_chn[1]);
EXIT_VENC_VPSS_UnBind:
    sample_comm_vpss_un_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[0], venc_vpss_chn->venc_chn[0]);
EXIT_VENC_STOP_CHN_0:
    sample_comm_venc_stop(venc_vpss_chn->venc_chn[0]);

    return ret;
}

static hi_void sample_venc_inner_scale_stop_encode(hi_vpss_grp vpss_grp, sample_venc_vpss_chn *venc_vpss_chn)
{
    sample_venc_chn_un_bind_venc_chn(venc_vpss_chn->venc_chn[0], venc_vpss_chn->venc_chn[1]);
    sample_comm_venc_stop(venc_vpss_chn->venc_chn[1]);
    sample_comm_vpss_un_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[0], venc_vpss_chn->venc_chn[0]);
    sample_comm_venc_stop(venc_vpss_chn->venc_chn[0]);
}

/* *****************************************************************************
 * function :  chn0@2560x1440@30fps + chn1@1280x720@30fps
 * **************************************************************************** */
static hi_s32 sample_venc_inner_scale(hi_void)
{
    hi_s32 ret;
    sample_sns_type sns_type = 0;
    sample_vi_cfg vi_cfg = {0};
    sample_vpss_cfg vpss_cfg = {0};
    hi_vpss_grp vpss_grp = 0;
    sample_venc_vpss_chn venc_vpss_chn = {0};
    sample_venc_param enc_param = {0};
    hi_pic_size pic_size[CHN_NUM_MAX] = {0};
    hi_size *enc_size = HI_NULL;

    /* *****************************************
      step 0: related parameter ready
    ***************************************** */
    sns_type = SENSOR0_TYPE;
    enc_param.venc_chn_num = CHN_NUM_MAX;
    sample_venc_set_pic_size_0(sns_type, pic_size);
    pic_size[1] = SMALL_STREAM_SIZE;
    sample_venc_get_enc_size(pic_size, &enc_param);
    sample_set_venc_vpss_chn(&venc_vpss_chn, enc_param.venc_chn_num);
    enc_size = &enc_param.enc_size[0];

    /* *****************************************
      step 1: init sys alloc common vb
    ***************************************** */
    if ((ret = sample_venc_online_wrap_start_sys_vi_vpss(sns_type, &vi_cfg, &vpss_cfg, &enc_param)) != HI_SUCCESS) {
        return ret;
    }

    if ((ret = sample_venc_inner_scale_start_encode(enc_size, CHN_NUM_MAX, vpss_grp, &venc_vpss_chn)) != HI_SUCCESS) {
        goto EXIT_SYS_VI_VPSS;
    }

    /* *****************************************
     exit process
    ***************************************** */
    sample_venc_exit_process();
    sample_venc_inner_scale_stop_encode(vpss_grp, &venc_vpss_chn);

EXIT_SYS_VI_VPSS:
    sample_venc_online_wrap_stop_sys_vi_vpss(&vi_cfg, &vpss_cfg);

    return ret;
}

static hi_void sample_venc_composite_init_param(hi_size *vi_size, hi_pic_size pic_size, hi_s32 len,
    sample_venc_vpss_chn_attr *vpss_chan_attr)
{
    hi_s32 i;
    td_u32 max_width, max_height;
    hi_size enc_size;

    (hi_void)sample_comm_sys_get_pic_size(pic_size, &enc_size);

    if (memset_s(vpss_chan_attr, sizeof(sample_venc_vpss_chn_attr), 0, sizeof(sample_venc_vpss_chn_attr)) != EOK) {
        printf("vpss chn attr call memset_s error\n");
        return;
    }

    max_width = vi_size->width;
    max_height = vi_size->height;

    for (i = 0; (i < len) && (i < HI_VPSS_MAX_PHYS_CHN_NUM); i++) {
        vpss_chan_attr->enable[i] = HI_TRUE;
        vpss_chan_attr->output_size[i].width = enc_size.width;
        vpss_chan_attr->output_size[i].height = enc_size.height;
        vpss_chan_attr->compress_mode[i] = HI_COMPRESS_MODE_NONE;
        vpss_chan_attr->fmu_mode[i] = HI_FMU_MODE_OFF;

        max_width = MAX2(max_width, enc_size.width);
        max_height = MAX2(max_height, enc_size.height);
    }

    vpss_chan_attr->max_size.width = max_width;
    vpss_chan_attr->max_size.height = max_height;
    vpss_chan_attr->pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;

    return;
}

static hi_void set_venc_composite_enable(hi_void)
{
    hi_s32 ret;
    hi_venc_chn_config chn_cfg = {0};

    ret = hi_mpi_venc_get_chn_config(VENC_CHN_ID, &chn_cfg);
    if (ret == HI_SUCCESS) {
        chn_cfg.composite_enc_en = HI_TRUE;
        chn_cfg.mosaic_en = HI_TRUE;
        chn_cfg.quality_level = 1;
        ret = hi_mpi_venc_set_chn_config(VENC_CHN_ID, &chn_cfg);
        if (ret != HI_SUCCESS) {
            sample_print("Venc set chn config (composite encode = %d) failed! ret = 0x%x\n",
                chn_cfg.composite_enc_en, ret);
        }
    } else {
        sample_print("Venc get chn config failed! ret = 0x%x\n", ret);
    }
}

static hi_s32 sample_composite_venc_start_venc(hi_pic_size enc_pic_size)
{
    hi_s32 ret;
    hi_venc_chn      venc_chn[1] = {VENC_CHN_ID};   // just use venc chn 0
    hi_u32           profile[1]  = {0};
    hi_payload_type  payload[1]  = {HI_PT_H265};
    hi_venc_gop_mode gop_mode;
    hi_venc_gop_attr gop_attr;
    sample_rc        rc_mode;
    hi_bool          is_rcn_ref_share = HI_TRUE;
    sample_comm_venc_chn_param venc_create_param[1] = {0};

    if (composite_get_gop_mode(&gop_mode) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (get_rc_mode(payload[0], &rc_mode) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    ret = sample_comm_venc_get_gop_attr(gop_mode, &gop_attr);
    if (ret != HI_SUCCESS) {
        sample_print("Venc Get GopAttr for %#x!\n", ret);
        return HI_FAILURE;
    }

    venc_create_param[0].frame_rate = 30; /* 30: is a number */
    venc_create_param[0].gop = 60; /* 60: is a number */
    venc_create_param[0].stats_time = 2; /* 2: is a number */
    venc_create_param[0].type                  = payload[0];
    venc_create_param[0].size                  = enc_pic_size;
    venc_create_param[0].rc_mode               = rc_mode;
    venc_create_param[0].profile               = profile[0];
    venc_create_param[0].is_rcn_ref_share_buf  = is_rcn_ref_share;
    venc_create_param[0].gop_attr              = gop_attr;

    if ((ret = sample_comm_venc_mini_buf_en(venc_create_param, 1)) != HI_SUCCESS) {
        return ret;
    }

    set_venc_composite_enable();

    ret = sample_comm_venc_start(venc_chn[0], &venc_create_param[0]);
    if (ret != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void sample_composite_venc_stop_venc(hi_void)
{
    sample_comm_venc_stop(VENC_CHN_ID);
}

static hi_void config_mosaic_info(hi_venc_mosaic_info *mosaic_info, hi_phys_addr_t phys_addr)
{
    mosaic_info->mode = HI_VENC_MOSAIC_MODE_MAP;
    mosaic_info->blk_size   = HI_MOSAIC_BLK_SIZE_32;
    mosaic_info->map_param.valid = HI_TRUE;
    mosaic_info->map_param.phys_addr = phys_addr;
    mosaic_info->map_param.specified_yuv_en = HI_FALSE;
    mosaic_info->map_param.pixel_yuv.data_y = 255; // 255: yuv data
    mosaic_info->map_param.pixel_yuv.data_u = 255; // 255: yuv data
    mosaic_info->map_param.pixel_yuv.data_v = 255; // 255: yuv data
}

static hi_u32 venc_trans_blk_size(hi_mosaic_blk_size blk_size)
{
    hi_u32 size;

    switch (blk_size) {
        case HI_MOSAIC_BLK_SIZE_16:
            size = 16; // 16: blk size
            break;

        case HI_MOSAIC_BLK_SIZE_32:
            size = 32; // 32: blk size
            break;

        default:
            size = 0;
            break;
    }

    return size;
}

static hi_void composite_send_multi_frame(hi_phys_addr_t phys_addr)
{
    hi_s32 ret, ret0_get, ret0_rls, ret1_get, ret1_rls;
    hi_video_frame_info mosaic_frm, ori_frm;
    hi_venc_multi_frame_info multi_frm;
    hi_u32 max_time_ref;

    ret0_get = hi_mpi_vpss_get_chn_frame(VPSS_GRP, VPSS_CHN_0, &mosaic_frm, DEFAULT_WAIT_TIME);
    ret1_get = hi_mpi_vpss_get_chn_frame(VPSS_GRP, VPSS_CHN_1, &ori_frm, DEFAULT_WAIT_TIME);
    if (ret0_get == TD_FAILURE || ret1_get == TD_FAILURE) {
        goto GET_FRAME_ERR;
    }

    max_time_ref = MAX2(mosaic_frm.video_frame.time_ref, ori_frm.video_frame.time_ref);
    if (mosaic_frm.video_frame.time_ref < max_time_ref) {
        ret0_rls = hi_mpi_vpss_release_chn_frame(VPSS_GRP, VPSS_CHN_0, &mosaic_frm);
        ret0_get = hi_mpi_vpss_get_chn_frame(VPSS_GRP, VPSS_CHN_0, &mosaic_frm, DEFAULT_WAIT_TIME);
        sample_print("VPSS_CHN0 is lags behind others. rls 0x%x, get 0x%x\n", ret0_rls, ret0_get);
        if (ret0_rls == TD_FAILURE || ret0_get == TD_FAILURE) {
            goto GET_FRAME_ERR;
        }
    }

    if (ori_frm.video_frame.time_ref < max_time_ref) {
        ret1_rls = hi_mpi_vpss_release_chn_frame(VPSS_GRP, VPSS_CHN_1, &ori_frm);
        ret1_get = hi_mpi_vpss_get_chn_frame(VPSS_GRP, VPSS_CHN_1, &ori_frm, DEFAULT_WAIT_TIME);
        sample_print("VPSS_CHN0 is lags behind others. rls 0x%x, get 0x%x\n", ret1_rls, ret1_get);
        if (ret1_rls == TD_FAILURE || ret1_get == TD_FAILURE) {
            goto GET_FRAME_ERR;
        }
    }

    multi_frm.frame[0] = mosaic_frm;
    multi_frm.frame[1] = ori_frm;
    multi_frm.frame_num = 2; /* 2: frame num */
    config_mosaic_info(&multi_frm.mosaic_info, phys_addr);
    ret = hi_mpi_venc_send_multi_frame(VENC_CHN_ID, &multi_frm, DEFAULT_WAIT_TIME);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_venc_send_multi_frame Failed! Error(%#x)\n", ret);
    }

GET_FRAME_ERR:
    if (ret0_get == HI_SUCCESS) {
        hi_mpi_vpss_release_chn_frame(VPSS_GRP, VPSS_CHN_0, &mosaic_frm);
    }

    if (ret1_get == HI_SUCCESS) {
        hi_mpi_vpss_release_chn_frame(VPSS_GRP, VPSS_CHN_1, &ori_frm);
    }
}

static hi_void *sample_composite_send_multi_frame_proc(hi_void *p)
{
    hi_s32 ret;
    hi_u32 map_size;
    hi_u32 stride;
    hi_u32 blk_size;
    hi_phys_addr_t phys_addr;
    hi_void *virt_addr = HI_NULL;
    hi_venc_chn_attr attr = { 0 };

    hi_mpi_venc_get_chn_attr(VENC_CHN_ID, &attr);

    blk_size = venc_trans_blk_size(HI_MOSAIC_BLK_SIZE_32);
    stride = hi_venc_get_mosaic_map_stride(attr.venc_attr.pic_width, blk_size);
    map_size = hi_venc_get_mosaic_map_size(attr.venc_attr.pic_width, attr.venc_attr.pic_height, blk_size);

    ret = hi_mpi_sys_mmz_alloc(&phys_addr, &virt_addr, "mosaic_map", HI_NULL, map_size);
    if (ret != HI_SUCCESS) {
        sample_print("alloc mosaic map failed.\n");
        return HI_NULL;
    }

    /* set mosaic area */
    (hi_void)memset_s(virt_addr, map_size, 0, map_size);
    (hi_void)memset_s(virt_addr, stride, 0xff, stride);
    (hi_void)memset_s(virt_addr + stride * 2, stride, 0xff, stride); /* 2: stride num */

    while (g_send_multi_frame_signal == HI_FALSE) {
        composite_send_multi_frame(phys_addr);
    }

    ret = hi_mpi_sys_mmz_free(phys_addr, virt_addr);
    if (ret != HI_SUCCESS) {
        sample_print("free mosaic map failed.\n");
    }

    return HI_NULL;
}

static hi_void sample_composite_stop_thread_sendframe(hi_void)
{
    g_send_multi_frame_signal = HI_TRUE;
    if (g_send_multi_frame_thread != 0) {
        pthread_join(g_send_multi_frame_thread, HI_NULL);
        g_send_multi_frame_thread = 0;
    }
}

static hi_void sample_composite_waiting_to_exit(hi_void)
{
    g_send_multi_frame_signal = HI_FALSE;
    pthread_create(&g_send_multi_frame_thread, 0, sample_composite_send_multi_frame_proc, HI_NULL);

    printf("please press twice ENTER to exit this sample\n");
    (hi_void)getchar();

    if (g_sample_venc_exit != HI_TRUE) {
        (hi_void)getchar();
    }

    /******************************************
     exit process
    ******************************************/
    sample_composite_stop_thread_sendframe();
    sample_comm_venc_stop_get_stream(1); // 1: chn num
}

static hi_s32 sample_composite_vpss_init(hi_vpss_grp vpss_grp, sample_venc_vpss_chn_attr *vpss_chan_cfg)
{
    hi_s32 ret;
    hi_vpss_chn vpss_chn;
    hi_vpss_grp_attr grp_attr = { 0 };
    sample_vpss_chn_attr vpss_chn_attr = {0};

    grp_attr.max_width = vpss_chan_cfg->max_size.width;
    grp_attr.max_height = vpss_chan_cfg->max_size.height;
    grp_attr.dei_mode = HI_VPSS_DEI_MODE_OFF;
    grp_attr.pixel_format = vpss_chan_cfg->pixel_format;
    grp_attr.frame_rate.src_frame_rate = -1;
    grp_attr.frame_rate.dst_frame_rate = -1;

    for (vpss_chn = 0; vpss_chn < HI_VPSS_MAX_PHYS_CHN_NUM; vpss_chn++) {
        if (vpss_chan_cfg->enable[vpss_chn] == HI_TRUE) {
            vpss_chn_attr.chn_attr[vpss_chn].width = vpss_chan_cfg->output_size[vpss_chn].width;
            vpss_chn_attr.chn_attr[vpss_chn].height = vpss_chan_cfg->output_size[vpss_chn].height;
            vpss_chn_attr.chn_attr[vpss_chn].chn_mode = HI_VPSS_CHN_MODE_USER;
            vpss_chn_attr.chn_attr[vpss_chn].compress_mode = HI_COMPRESS_MODE_NONE;
            vpss_chn_attr.chn_attr[vpss_chn].pixel_format = vpss_chan_cfg->pixel_format;
            vpss_chn_attr.chn_attr[vpss_chn].frame_rate.src_frame_rate = -1;
            vpss_chn_attr.chn_attr[vpss_chn].frame_rate.dst_frame_rate = -1;
            vpss_chn_attr.chn_attr[vpss_chn].depth = 2; /* 2 : length of pic queue */
            vpss_chn_attr.chn_attr[vpss_chn].mirror_en = 0;
            vpss_chn_attr.chn_attr[vpss_chn].flip_en = 0;
            vpss_chn_attr.chn_attr[vpss_chn].aspect_ratio.mode = HI_ASPECT_RATIO_NONE;
        }
    }

    memcpy_s(vpss_chn_attr.chn_enable, sizeof(vpss_chn_attr.chn_enable),
            vpss_chan_cfg->enable, sizeof(vpss_chn_attr.chn_enable));
    vpss_chn_attr.chn_array_size = HI_VPSS_MAX_PHYS_CHN_NUM;

    ret = sample_common_vpss_start(vpss_grp, &grp_attr, &vpss_chn_attr);
    if (ret != HI_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
    }

    return ret;
}

/* *****************************************************************************
 * function :  1080P
 * **************************************************************************** */
static hi_s32 sample_venc_composite(hi_void)
{
    hi_s32 ret;
    sample_sns_type sns_type = SENSOR0_TYPE;
    sample_vi_cfg vi_cfg;
    hi_size vi_size;
    hi_pic_size enc_pic_size = PIC_1080P;
    sample_venc_vpss_chn_attr vpss_param = { 0 };
    sample_venc_vb_attr vb_attr = { 0 };
    hi_s32 vpss_chn_num = 2;
    hi_venc_chn venc_chn[1] = { 0 };

    sample_comm_vi_get_default_vi_cfg(sns_type, &vi_cfg);
    sample_comm_vi_get_size_by_sns_type(sns_type, &vi_size);
    /* *****************************************
      step 0: related parameter ready
    ***************************************** */
    sample_venc_composite_init_param(&vi_size, enc_pic_size, vpss_chn_num, &vpss_param);

    /* *****************************************
      step 1: init sys alloc common vb
    ***************************************** */
    get_vb_attr(&vi_size, &vpss_param, &vb_attr);

    if ((ret = sample_venc_sys_init(&vb_attr)) != HI_SUCCESS) {
        sample_print("Init SYS err for %#x!\n", ret);
        return ret;
    }

    if ((ret = sample_venc_vi_init(&vi_cfg)) != HI_SUCCESS) {
        sample_print("Init VI err for %#x!\n", ret);
        goto EXIT_SYS_STOP;
    }

    if ((ret = sample_composite_vpss_init(0, &vpss_param)) != HI_SUCCESS) { /* 0 vpss grp */
        sample_print("Init VPSS err for %#x!\n", ret);
        goto EXIT_VI_STOP;
    }

    if ((ret = sample_comm_vi_bind_vpss(0, 0, 0, 0)) != HI_SUCCESS) { /* 0 vi pipe, 0 vi chn, 0 vpss grp, 0 vpss chn */
        sample_print("VI Bind VPSS err for %#x!\n", ret);
        goto EXIT_VPSS_STOP;
    }

    if ((ret = sample_composite_venc_start_venc(enc_pic_size)) != HI_SUCCESS) {
        goto EXIT_VI_VPSS_UNBIND;
    }

    if ((ret = sample_comm_venc_start_get_stream(venc_chn, 1)) != HI_SUCCESS) {
        sample_print("Start Venc failed!\n");
        goto EXIT_VENC_STOP;
    }

    /* *****************************************
     exit process
    ***************************************** */
    sample_composite_waiting_to_exit();

EXIT_VENC_STOP:
    sample_composite_venc_stop_venc();
EXIT_VI_VPSS_UNBIND:
    sample_comm_vi_un_bind_vpss(0, 0, 0, 0); /* 0 vi pipe, 0 vi chn, 0 vpss grp, 0 vpss chn */
EXIT_VPSS_STOP:
    sample_venc_vpss_deinit(0, &vpss_param); /* 0 vpss grp */
EXIT_VI_STOP:
    sample_venc_vi_deinit(&vi_cfg);
EXIT_SYS_STOP:
    sample_comm_sys_exit();

    return ret;
}

static hi_s32 sample_venc_choose_mode(hi_u32 index)
{
    hi_s32 ret;

    switch (index) {
        case 0: /* 0: mode 0 */
            ret = sample_venc_normal();
            break;

        case 1: /* 1: mode 1 */
            ret = sample_venc_intra_refresh();
            break;

        case 2: /* 2: mode 2 */
            ret = sample_venc_roi_bg();
            break;

        case 3: /* 3: mode 3 */
            ret = sample_venc_mjpeg_roi_set();
            break;

        case 4: /* 4: mode 4 */
            ret = sample_venc_svac3();
            break;

        case 5: /* 5: mode 5 */
            ret = sample_venc_inner_scale();
            break;

        case 6: /* 6: mode 6 */
            ret = sample_venc_qpmap();
            break;

        case 7: /* 7: mode 7 */
            ret = sample_venc_debreath_effect();
            break;

        case 8: /* 8: mode 8 */
            ret = sample_venc_mjpeg_roimap();
            break;

        case 9: /* 9: mode 9 */
            ret = sample_venc_mosaic_map();
            break;

        case 10: /* 10: mode 10 */
            ret = sample_venc_composite();
            break;

        default:
            printf("the index is invalid!\n");
            return HI_FAILURE;
    }
    return ret;
}

static hi_s32 sample_venc_msg_proc_vb_pool_share(hi_s32 pid)
{
    hi_u32 i;
    hi_vb_common_pools_id pools_id = {0};

    if (hi_mpi_vb_get_common_pool_id(&pools_id) != HI_SUCCESS) {
        sample_print("get common pool_id failed!\n");
        return HI_FAILURE;
    }
    for (i = 0; i < pools_id.pool_cnt; ++i) {
        if (hi_mpi_vb_pool_share(pools_id.pool[i], pid) != HI_SUCCESS) {
            sample_print("vb pool share failed!\n");
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_void sample_venc_msg_proc_vb_pool_unshare(hi_s32 pid)
{
    hi_u32 i;
    hi_vb_common_pools_id pools_id = {0};

    if (hi_mpi_vb_get_common_pool_id(&pools_id) == HI_SUCCESS) {
        for (i = 0; i < pools_id.pool_cnt; ++i) {
            hi_mpi_vb_pool_unshare(pools_id.pool[i], pid);
        }
    }
}

static hi_s32 sample_venc_ipc_msg_proc(const sample_ipc_msg_req_buf *msg_req_buf,
    hi_bool *is_need_fb, sample_ipc_msg_res_buf *msg_res_buf)
{
    hi_s32 ret;

    if (msg_req_buf == HI_NULL || is_need_fb == HI_NULL) {
        return HI_FAILURE;
    }

    /* need feedback default */
    *is_need_fb = HI_TRUE;

    switch ((sample_msg_type)msg_req_buf->msg_type) {
        case SAMPLE_MSG_TYPE_VB_POOL_SHARE_REQ: {
            if (msg_res_buf == HI_NULL) {
                return HI_FAILURE;
            }
            ret = sample_venc_msg_proc_vb_pool_share(msg_req_buf->msg_data.pid);
            msg_res_buf->msg_type = SAMPLE_MSG_TYPE_VB_POOL_SHARE_RES;
            msg_res_buf->msg_data.is_req_success = (ret == HI_SUCCESS) ? HI_TRUE : HI_FALSE;
            break;
        }
        case SAMPLE_MSG_TYPE_VB_POOL_UNSHARE_REQ: {
            if (msg_res_buf == HI_NULL) {
                return HI_FAILURE;
            }
            sample_venc_msg_proc_vb_pool_unshare(msg_req_buf->msg_data.pid);
            msg_res_buf->msg_type = SAMPLE_MSG_TYPE_VB_POOL_UNSHARE_RES;
            msg_res_buf->msg_data.is_req_success = HI_TRUE;
            break;
        }
        default: {
            printf("unsupported msg type(%ld)!\n", msg_req_buf->msg_type);
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

/* *****************************************************************************
 * function    : main()
 * description : video venc sample
 * **************************************************************************** */
#ifdef __LITEOS__
hi_s32 app_main(hi_s32 argc, hi_char *argv[])
#else
hi_s32 sample_venc_main(hi_s32 argc, hi_char *argv[])
#endif
{
    hi_s32 ret;
    hi_u32 index;
    char *end_ptr;

    if (argc != 0x2 && argc != 0x3) { /* 2:arg num */
        sample_venc_usage(argv[0]);
        return HI_FAILURE;
    }

    if (!strncmp(argv[1], "-h", 2)) { /* 2:arg num */
        sample_venc_usage(argv[0]);
        return HI_FAILURE;
    }

    if (argv[1][0] < '0' || argv[1][0] > '9') {
        sample_venc_usage(argv[0]);
        return HI_FAILURE;
    }

    index = strtoul(argv[1], &end_ptr, 10); /* 10: numberbase */
    if (end_ptr == argv[1] || *end_ptr !='\0') {
        sample_venc_usage(argv[0]);
        return HI_FAILURE;
    }
#ifndef __LITEOS__
    sample_sys_signal(sample_venc_handle_sig);
#endif

    if (sample_ipc_server_init(sample_venc_ipc_msg_proc) != HI_SUCCESS) {
        printf("sample_ipc_server_init failed!!!\n");
    }

    ret = sample_venc_choose_mode(index);
    if (ret == HI_SUCCESS) {
        printf("program exit normally!\n");
    } else {
        printf("program exit abnormally!\n");
    }

    sample_ipc_server_deinit();

#ifdef __LITEOS__
    return ret;
#else
    exit(ret);
#endif
}

/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
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

#define BIG_STREAM_SIZE PIC_3840X2160
#define SMALL_STREAM_SIZE PIC_1080P

#define VI_VB_YUV_CNT (6*4) //maohw
#define VPSS_VB_YUV_CNT (8*2) //maohw

#define ENTER_ASCII 10

#define VB_MAX_NUM 10

#define TYPE_NUM_MAX 3

#define GRP_NUM_MAX 2
#define MAX_WATERMARK_LEN 128

#if 0 //maohw mvto sample_comm.h;
#define CHN_NUM_MAX 2
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

static hi_bool g_sample_venc_exit = HI_FALSE;

/* *****************************************************************************
 * function : show usage
 * **************************************************************************** */
static hi_void sample_venc_usage(hi_char *s_prg_nm)
{
    printf("usage : %s [index] [options]\n", s_prg_nm);
    printf("index:\n");
    printf("\t  0) normal               :H.265e + H.264e.\n");
    printf("\t  1) qpmap                :H.265e + H.264e.\n");
    printf("\t  2) intra_refresh        :H.265e + H.264e.\n");
    printf("\t  3) roi_bg_frame_rate    :H.265e + H.264e.\n");
    printf("\t  4) debreath_effect      :H.265e + H.264e.\n");
    printf("\t  5) roi_set              :Mjpege + Mjpege(user set roi info by API).\n");
    printf("\t  6) roimap               :Mjpege + Mjpege(user customize every region).\n");
    printf("\t  7) mosaic map           :H.265e + H.264e(user send frame,mosaic)\n");
    printf("\t  8) mjpeg mosaic map     :Mjpege + Mjpege(user send frame,mosaic).\n");
    printf("\t  9) watermark            :H.265e + H.264e.\n");
    printf("\t  10) quality balance     :H.265e + H.264e.\n");
    printf("\t  11) FMU 1 grp 2 chn     :H.265e(direct) + Mjpge(warp).\n");
    printf("\t  12) FMU 2 grp 2 chn     :H.265e(direct) + H.264e(direct).\n");
    printf("options: not necessary\n");
    printf("\t  0) not save heif file.\n");
    printf("\t  1) save heif file.\n");
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

static hi_s32 get_gop_mode(hi_venc_gop_mode *gop_mode)
{
    hi_s32 c;

    while (HI_TRUE) {
        print_gop_mode();
        c = sample_venc_getchar();
        if (c == 'e' && sample_venc_getchar() == ENTER_ASCII) {
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

static hi_void print_rc_mode(hi_payload_type type)
{
    printf("please input choose rc mode!\n");
    printf("\t c) cbr.\n");
    printf("\t v) vbr.\n");
    if (type != HI_PT_MJPEG) {
        printf("\t a) avbr.\n");
        printf("\t x) cvbr.\n");
        printf("\t q) qvbr.\n");
    }
    printf("\t f) fix_qp\n");
}

static hi_s32 set_rc_mode(hi_s32 c, sample_rc *rc_mode)
{
    switch (c) {
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
        if (c == 'e' && sample_venc_getchar() == ENTER_ASCII) {
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
        if (c == 'e' && sample_venc_getchar() == ENTER_ASCII) {
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
        //vpss_chan_attr->compress_mode[i] = (i == 0) ? HI_COMPRESS_MODE_SEG_COMPACT : HI_COMPRESS_MODE_NONE;
        vpss_chan_attr->compress_mode[i] = HI_COMPRESS_MODE_NONE;
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
        printf("%s => i:%d, blk_size:%d, blk_cnt:%d\n", __func__, i, vb_cfg.common_pool[i].blk_size, vb_cfg.common_pool[i].blk_cnt);
    }

    vb_cfg.max_pool_cnt = vb_attr->valid_num;

    if (vb_attr->supplement_config == 0) {
        ret = sample_comm_sys_vb_init(&vb_cfg);
    } else {
        ret = sample_comm_sys_init_with_vb_supplement(&vb_cfg, vb_attr->supplement_config);
    }
    sample_print("%s => vb_init ret:%d, max_pool_cnt:%d, supplement_config:0x%x\n", __func__, ret, vb_cfg.max_pool_cnt, vb_attr->supplement_config);
    
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
            vpss_chn_attr.chn_attr[vpss_chn].pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;//maohw vpss_chan_cfg->pixel_format;
            vpss_chn_attr.chn_attr[vpss_chn].frame_rate.src_frame_rate = -1;
            vpss_chn_attr.chn_attr[vpss_chn].frame_rate.dst_frame_rate = -1;
            vpss_chn_attr.chn_attr[vpss_chn].depth = 0;
            vpss_chn_attr.chn_attr[vpss_chn].mirror_en = 0;
            vpss_chn_attr.chn_attr[vpss_chn].flip_en = 0;
            vpss_chn_attr.chn_attr[vpss_chn].aspect_ratio.mode = HI_ASPECT_RATIO_NONE;
            printf("@@@@@@@@@@ vpss_grp:%d, vpss_chn:%d, width:%d, height:%d\n"
              , vpss_grp, vpss_chn, vpss_chn_attr.chn_attr[vpss_chn].width, vpss_chn_attr.chn_attr[vpss_chn].height);
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
    for (i = 1; i < chn_num_max && i < CHN_NUM_MAX; i++) {
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

static hi_void sample_venc_stop(const sample_venc_vpss_chn *venc_vpss_chn, hi_s32 chn_num_max)
{
    hi_s32 i;
    hi_unused(chn_num_max);

    for (i = 0; i < CHN_NUM_MAX; i++) {
        sample_comm_venc_stop(venc_vpss_chn->venc_chn[i]);
    }
}

static hi_s32 sample_venc_normal_start_encode(hi_size *enc_size, hi_s32 chn_num_max,
    hi_vpss_grp vpss_grp, sample_venc_vpss_chn *venc_vpss_chn)
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

    /* encode h.265 */
    if ((ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[0], &(chn_param[0]))) != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        return ret;
    }

    ret = sample_comm_vpss_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[0], venc_vpss_chn->venc_chn[0]);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vpss_bind_venc failed for %#x!\n", ret);
        goto EXIT_VENC_H265_STOP;
    }

    /* encode h.264 */
    if ((ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[1], &(chn_param[1]))) != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        goto EXIT_VENC_H265_UnBind;
    }

    ret = sample_comm_vpss_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[1], venc_vpss_chn->venc_chn[1]);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vpss_bind_venc failed for %#x!\n", ret);
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

    return ret;
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
/* *****************************************************************************
 * function :  H.265e@1080P@30fps + h264e@D1@30fps
 * **************************************************************************** */
static hi_s32 sample_venc_normal(hi_void)
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

    if ((ret = sample_venc_normal_start_encode(enc_size, CHN_NUM_MAX, vpss_grp, &venc_vpss_chn)) != HI_SUCCESS) {
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
    sample_comm_venc_chn_param *h265_chn_param = HI_NULL;
    sample_comm_venc_chn_param *h264_chn_param = HI_NULL;

    ret = sample_venc_intra_refresh_set_video_param(enc_size, &intra_refresh_mode, chn_param, CHN_NUM_MAX);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /* encode h.265 */
    h265_chn_param = &(chn_param[0]);
    if ((ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[0], h265_chn_param)) != HI_SUCCESS) {
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
    h264_chn_param = &(chn_param[1]);
    if ((ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[1], h264_chn_param)) != HI_SUCCESS) {
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
 * function : intra_refresh:H.265e@1080P@30fps(row) + h264e@1080P@30fps(column).
 * **************************************************************************** */
static hi_s32 sample_venc_intra_refresh(hi_void)
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
    ret = sample_venc_intra_refresh_start_encode(enc_size, CHN_NUM_MAX, vpss_grp, &venc_vpss_chn);
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
 * function : roi_bg_frame_rate:H.265e@1080P@30fps + H.264@1080P@30fps.
 * **************************************************************************** */
static hi_s32 sample_venc_roi_bg(hi_void)
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
    if ((ret = sample_venc_roi_bg_start_encode(enc_size, CHN_NUM_MAX, vpss_grp, &venc_vpss_chn)) != HI_SUCCESS) {
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
    hi_bool share_buf_en = HI_FALSE;
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
    sample_sns_type sns_type = SENSOR0_TYPE;
    sample_vi_cfg vi_cfg;
    hi_size vi_size;
    const hi_vi_pipe vi_pipe = 0;
    const hi_vi_chn vi_chn = 0;
    const hi_vpss_grp vpss_grp = 0;
    sample_venc_vpss_chn_attr vpss_param;
    sample_venc_vb_attr vb_attr = { 0 };
    hi_size enc_size[CHN_NUM_MAX];
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

    /* *****************************************
    start stream venc
    ***************************************** */
    ret = sample_venc_mjpeg_roi_set_start_encode(enc_size, CHN_NUM_MAX, vpss_grp, &venc_vpss_chn);
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

static hi_s32 sample_venc_mjpeg_mosaic_map_start_encode(hi_size *enc_size, hi_s32 enc_len, hi_vpss_grp vpss_grp,
    sample_venc_vpss_chn *venc_vpss_chn)
{
    hi_s32 ret;
    hi_venc_gop_attr gop_attr;
    sample_comm_venc_chn_param chn_param[CHN_NUM_MAX] = {0};
    sample_comm_venc_chn_param venc_chn0_param, venc_chn1_param;

    if ((ret = sample_comm_venc_get_gop_attr(HI_VENC_GOP_MODE_NORMAL_P, &gop_attr)) != HI_SUCCESS) {
        sample_print("Venc Get GopAttr for %#x!\n", ret);
        return ret;
    }
    // init default venc chn attr
    sample_venc_set_jpeg_param(enc_size, enc_len, chn_param, CHN_NUM_MAX, gop_attr);

    /* encode mjpeg */
    venc_chn0_param = chn_param[0];
    ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[0], &venc_chn0_param);
    if (ret != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        return ret;
    }

    venc_chn1_param = chn_param[1];
    ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[1], &venc_chn1_param);
    if (ret != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        goto EXIT_VENC_CHN0_STOP;
    }

    ret = sample_comm_venc_mosaic_map_send_frame(vpss_grp, venc_vpss_chn->vpss_chn,
        venc_vpss_chn->venc_chn, CHN_NUM_MAX, enc_size);
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

static hi_s32 sample_venc_mjpeg_mosaic_map(hi_void)
{
    hi_s32 ret;
    sample_sns_type sns_type = SENSOR0_TYPE;
    const hi_vi_pipe vi_pipe = 0;
    const hi_vi_chn vi_chn = 0;
    const hi_vpss_grp vpss_grp = 0;
    sample_venc_vpss_chn_attr vpss_param;
    sample_venc_vb_attr vb_attr = { 0 };
    sample_venc_vpss_chn venc_vpss_chn = { 0 };
    hi_size enc_size[CHN_NUM_MAX], vi_size;
    sample_vi_cfg vi_cfg;

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
    if ((ret = sample_comm_vi_bind_vpss(vi_pipe, vi_chn, vpss_grp, venc_vpss_chn.vpss_chn[0])) != HI_SUCCESS) {
        sample_print("VI Bind VPSS err for %#x!\n", ret);
        goto EXIT_VPSS_STOP;
    }
    /* *****************************************
    start stream venc
    ***************************************** */
    if ((ret = sample_venc_mjpeg_mosaic_map_start_encode(enc_size, CHN_NUM_MAX, vpss_grp, &venc_vpss_chn)) !=
        HI_SUCCESS) {
        goto EXIT_VI_VPSS_UNBIND;
    }
    /* *****************************************
     exit process
    ***************************************** */
    sample_venc_mosaic_map_exit_process(CHN_NUM_MAX);
    sample_venc_stop(&venc_vpss_chn, CHN_NUM_MAX);

EXIT_VI_VPSS_UNBIND:
    sample_comm_vi_un_bind_vpss(vi_pipe, vi_chn, vpss_grp, venc_vpss_chn.vpss_chn[0]);
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

static hi_s32 sample_venc_set_watermark(hi_venc_chn venc_chn, hi_bool enable)
{
    hi_s32 ret;
    hi_venc_watermark watermark = { 0 };

    if ((ret = hi_mpi_venc_get_watermark(venc_chn, &watermark)) != HI_SUCCESS) {
        sample_print("Get watermark failed for %#x!\n", ret);
        return ret;
    }

    if (enable) {
        hi_char watermark_info[HI_VENC_MAX_WATERMARK_INFO_NUM];
        if (sprintf_s(watermark_info, HI_VENC_MAX_WATERMARK_INFO_NUM, "hello world:%d", venc_chn) < 0) {
            sample_print("Set watermark failed for %#x!\n", ret);
            return ret;
        }
        watermark.enable = HI_TRUE;
        watermark.strength = 2;         // 2: watermark strength
        watermark.component = 1;        // 1: watermark component
        watermark.template_period = 10; // 10 : watermark template_period

        if (sprintf_s((char *)watermark.info, HI_VENC_MAX_WATERMARK_INFO_NUM, watermark_info) < 0) {
            sample_print("Set watermark failed for %#x!\n", ret);
            return ret;
        }
        if ((ret = hi_mpi_venc_set_watermark(venc_chn, &watermark)) != HI_SUCCESS) {
            sample_print("Set watermark failed for %#x!\n", ret);
            return ret;
        }
    }
    return HI_SUCCESS;
}

static hi_s32 sample_venc_watermark_start_encode(hi_size *enc_size, hi_s32 chn_num_max, hi_vpss_grp vpss_grp,
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

    /* encode h.265 */
    if ((ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[0], &(chn_param[0]))) != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        return ret;
    }

    ret = sample_venc_vpss_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[0], venc_vpss_chn->venc_chn[0]);
    if (ret != HI_SUCCESS) {
        goto EXIT_VENC_H265_STOP;
    }

    /* set watermark mode for chn 0 */
    if ((ret = sample_venc_set_watermark(venc_vpss_chn->venc_chn[0], HI_TRUE)) != HI_SUCCESS) {
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

    /* set watermark mode for chn 1 */
    if ((ret = sample_venc_set_watermark(venc_vpss_chn->venc_chn[1], HI_TRUE)) != HI_SUCCESS) {
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

static hi_s32 sample_venc_watermark(hi_void)
{
    hi_s32 ret;
    sample_sns_type sns_type = SENSOR0_TYPE;
    sample_vi_cfg vi_cfg = { 0 };
    const hi_vi_pipe vi_pipe = 0;
    const hi_vi_chn vi_chn = 0;
    const hi_vpss_grp vpss_grp = 0;
    sample_venc_vpss_chn_attr vpss_param = { 0 };
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
    ret = sample_venc_watermark_start_encode(enc_size, CHN_NUM_MAX, vpss_grp, &venc_vpss_chn);
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

static hi_s32 sample_venc_set_pvc_attr(hi_venc_chn venc_chn, hi_bool enable)
{
    hi_s32 ret;
    hi_venc_quality_balance quality_balance;

    if ((ret = hi_mpi_venc_get_quality_balance(venc_chn, &quality_balance)) != HI_SUCCESS) {
        sample_print("Get quality balance failed for %#x!\n", ret);
        return ret;
    }

    if (enable) {
        quality_balance.enable = HI_TRUE;
    } else {
        quality_balance.enable = HI_FALSE;
    }

    if ((ret = hi_mpi_venc_set_quality_balance(venc_chn, &quality_balance)) != HI_SUCCESS) {
        sample_print("Set debreath_effect failed for %#x!\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_venc_pvc_start_encode(hi_size *enc_size, hi_s32 chn_num_max, hi_vpss_grp vpss_grp,
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

    /* encode h.265 */
    if ((ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[0], &(chn_param[0]))) != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        return ret;
    }

    ret = sample_venc_vpss_bind_venc(vpss_grp, venc_vpss_chn->vpss_chn[0], venc_vpss_chn->venc_chn[0]);
    if (ret != HI_SUCCESS) {
        goto EXIT_VENC_H265_STOP;
    }

    /* set pvc mode for chn 0 */
    if ((ret = sample_venc_set_pvc_attr(venc_vpss_chn->venc_chn[0], HI_TRUE)) != HI_SUCCESS) {
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

    /* set pvc mode for chn 1 */
    if ((ret = sample_venc_set_pvc_attr(venc_vpss_chn->venc_chn[1], HI_FALSE)) != HI_SUCCESS) {
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

static hi_s32 sample_venc_pvc(hi_void)
{
    hi_s32 ret;
    sample_sns_type sns_type = SENSOR0_TYPE;
    sample_vi_cfg vi_cfg = { 0 };
    const hi_vi_pipe vi_pipe = 0;
    const hi_vi_chn vi_chn = 0;
    const hi_vpss_grp vpss_grp = 0;
    sample_venc_vpss_chn_attr vpss_param = { 0 };
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
    ret = sample_venc_pvc_start_encode(enc_size, CHN_NUM_MAX, vpss_grp, &venc_vpss_chn);
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

static hi_s32 sample_venc_set_fmu_video_param(hi_size *enc_size, sample_comm_venc_chn_param *chn_param,
    hi_venc_gop_attr gop_attr, hi_s32 chn_num_max, hi_s32 grp_num_max)
{
    hi_s32 i, ret;
    hi_u32 profile[CHN_NUM_MAX] = {0, 0};
    hi_bool share_buf_en = HI_TRUE;
    hi_payload_type payload[TYPE_NUM_MAX] = {HI_PT_H265, HI_PT_H264, HI_PT_MJPEG};
    sample_rc rc_mode = 0;

    if ((ret = sample_comm_venc_get_gop_default_attr(&gop_attr)) != HI_SUCCESS) {
        return ret;
    }

    if (get_rc_mode(payload[0], &rc_mode) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    for (i = 0; i < 3; i++) { /* 3: is a number */
        chn_param[i].frame_rate = 30; /* 30: is a number */
        chn_param[i].gop = 60; /* 60: is a number */
        chn_param[i].stats_time = 2; /* 2: is a number */
        chn_param[i].is_rcn_ref_share_buf = share_buf_en;
    }

    /* encode h.265 */
    chn_param[0].gop_attr = gop_attr;
    chn_param[0].type = payload[0];
    chn_param[0].size = (grp_num_max == 1) ? sample_comm_sys_get_pic_enum(&enc_size[1]) :
        sample_comm_sys_get_pic_enum(&enc_size[0]);
    chn_param[0].rc_mode = rc_mode;
    chn_param[0].profile = profile[0];

    if (chn_num_max == 1) { /* 1: chn_param array len */
        return HI_SUCCESS;
    }

    /* encode h.264 */
    chn_param[1].gop_attr = gop_attr;
    chn_param[1].type = payload[1];
    chn_param[1].size = sample_comm_sys_get_pic_enum(&enc_size[1]);
    chn_param[1].rc_mode = rc_mode;
    chn_param[1].profile = profile[1];

    if (grp_num_max == 2 || chn_num_max == 2) { /* 2: chn_param array len */
        return HI_SUCCESS;
    }

    printf("please config mjpeg!\n");
    if (get_rc_mode(payload[2], &rc_mode) != HI_SUCCESS) { /* 2: mjpege num */
        return HI_SUCCESS;
    }

    /* encode mjpge */
    chn_param[2].gop_attr = gop_attr; /* 2: mjpege num */
    chn_param[2].type = payload[2]; /* 2: mjpege num */
    chn_param[2].size = SMALL_STREAM_SIZE; /* 2: mjpege num */
    chn_param[2].rc_mode = rc_mode;   /* 2: mjpege num */
    chn_param[2].profile = profile[1]; /* 2: mjpege num */
    return HI_SUCCESS;
}

hi_void sample_venc_get_vi_cfg_by_fmu_direct(sample_vi_cfg *vi_cfg)
{
    hi_s32 i;

    for (i = 0; i < vi_cfg->bind_pipe.pipe_num; i++) {
        vi_cfg->pipe_info[i].chn_info[0].chn_attr.compress_mode = HI_COMPRESS_MODE_TILE;
        vi_cfg->pipe_info[i].chn_info[0].chn_attr.video_format = HI_VIDEO_FORMAT_TILE_32x4;
    }
}

static hi_void get_default_vpss_grp_chn_fmu_attr(hi_size *vi_size, hi_size enc_size[], hi_s32 chn_num,
    sample_venc_vpss_chn_attr *grp_chn_attr, hi_s32 grp_num_max)
{
    hi_s32 i, j;
    hi_u32 max_width;
    hi_u32 max_height;

    max_width = vi_size->width;
    max_height = vi_size->height;

    for (i = 0; (i < grp_num_max) && (i < GRP_NUM_MAX); i++) {
        if (memset_s(&grp_chn_attr[i], sizeof(sample_venc_vpss_chn_attr), 0,
            sizeof(sample_venc_vpss_chn_attr)) != EOK) {
            printf("vpss chn attr call memset_s error\n");
            return;
        }

        if (grp_num_max != GRP_NUM_MAX) {
            for (j = 0; (j < chn_num) && (j < HI_VPSS_MAX_PHYS_CHN_NUM); j++) {
                grp_chn_attr[i].output_size[j].width = (chn_num > 1) ? enc_size[1].width : enc_size[0].width;
                grp_chn_attr[i].output_size[j].height = (chn_num > 1) ? enc_size[1].height : enc_size[0].height;
                grp_chn_attr[i].compress_mode[j] = HI_COMPRESS_MODE_NONE;
                grp_chn_attr[i].enable[j] = HI_TRUE;
                grp_chn_attr[i].fmu_mode[j] = (j == 0) ? HI_FMU_MODE_DIRECT : HI_FMU_MODE_WRAP;

                max_width = MAX2(max_width, enc_size[j].width);
                max_height = MAX2(max_height, enc_size[j].height);
            }
        } else {
            grp_chn_attr[i].output_size[0].width = enc_size[i].width;
            grp_chn_attr[i].output_size[0].height = enc_size[i].height;
            grp_chn_attr[i].compress_mode[0] = HI_COMPRESS_MODE_NONE;
            grp_chn_attr[i].enable[0] = HI_TRUE;
            grp_chn_attr[i].fmu_mode[0] = HI_FMU_MODE_DIRECT;

            max_width = MAX2(max_width, enc_size[i].width);
            max_height = MAX2(max_height, enc_size[i].height);
        }
        grp_chn_attr[i].in_size.width = vi_size->width;
        grp_chn_attr[i].in_size.height = vi_size->height;
        grp_chn_attr[i].max_size.width = max_width;
        grp_chn_attr[i].max_size.height = max_height;
        grp_chn_attr[i].pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    }

    return;
}

static hi_s32 sample_venc_grp_fmu_init_param(hi_size *enc_size, hi_s32 chn_num_max, hi_size *vi_size,
    sample_venc_vpss_chn_attr *grp_chn_attr, hi_s32 grp_num_max)
{
    hi_s32 i;
    hi_s32 ret;
    hi_pic_size pic_size[CHN_NUM_MAX] = {BIG_STREAM_SIZE, SMALL_STREAM_SIZE};

    enc_size[0].width = vi_size->width;
    enc_size[0].height = vi_size->height;
    for (i = 1; i < chn_num_max && i < CHN_NUM_MAX; i++) {
        ret = sample_comm_sys_get_pic_size(pic_size[i], &enc_size[i]);
        if (ret != HI_SUCCESS) {
            sample_print("sample_comm_sys_get_pic_size failed!\n");
            return ret;
        }
    }

    // get vpss param
    get_default_vpss_grp_chn_fmu_attr(vi_size, enc_size, chn_num_max, grp_chn_attr, grp_num_max);

    return 0;
}

static hi_void sample_venc_set_vpss_one_grp_chn(hi_vpss_grp grp, sample_venc_vpss_chn *venc_vpss_chn,
    const sample_venc_vpss_chn_attr *grp_attr, hi_s32 grp_num_max)
{
    hi_s32 i;
    for (i = 0; i < HI_VPSS_MAX_PHYS_CHN_NUM; i++) {
        if (grp_attr[grp].enable[i] == HI_TRUE && grp_attr[grp].fmu_mode[i] == HI_FMU_MODE_WRAP) {
            venc_vpss_chn->vpss_chn[i] = 0;
        } else if (grp_attr[grp].enable[i] == HI_TRUE && grp_attr[grp].fmu_mode[i] == HI_FMU_MODE_DIRECT) {
            venc_vpss_chn->vpss_chn[i] = HI_VPSS_DIRECT_CHN;
        } else if (grp_attr[grp].enable[i] == HI_TRUE) {
            venc_vpss_chn->vpss_chn[i] = i;
        }
    }
}

static hi_void sample_set_venc_vpss_fmu_chn(sample_venc_vpss_chn *venc_vpss_chn, hi_s32 chn_num_max,
    const sample_venc_vpss_chn_attr *grp_attr, hi_s32 grp_num_max)
{
    hi_s32 i, j;

    for (i = 0; i < grp_num_max && i < GRP_NUM_MAX; i++) {
        if (grp_num_max > 1) {
            if (grp_attr[i].enable[0] == HI_TRUE && grp_attr[i].fmu_mode[0] == HI_FMU_MODE_WRAP) {
                venc_vpss_chn->vpss_chn[i] = 0;
            } else if (grp_attr[i].enable[0] == HI_TRUE && grp_attr[i].fmu_mode[0] == HI_FMU_MODE_DIRECT) {
                venc_vpss_chn->vpss_chn[i] = HI_VPSS_DIRECT_CHN;
            } else if (grp_attr[i].enable[0] == HI_TRUE) {
                venc_vpss_chn->vpss_chn[i] = i;
            }
            venc_vpss_chn->venc_chn[i] = i;
        } else {
            sample_venc_set_vpss_one_grp_chn(i, venc_vpss_chn, grp_attr, grp_num_max);
            for (j = 0; j < CHN_NUM_MAX; j++) {
                venc_vpss_chn->venc_chn[j] = j;
            }
        }
    }
}

static hi_void get_fmu_vb_attr(const hi_size *vi_size, sample_venc_vb_attr *vb_attr)
{
    // vb for vi-vpss
    update_vb_attr(vb_attr, vi_size, HI_PIXEL_FORMAT_YUV_SEMIPLANAR_422, HI_COMPRESS_MODE_NONE, VI_VB_YUV_CNT);

    if (vb_attr->valid_num >= HI_VB_MAX_COMMON_POOLS) {
        return;
    }

    vb_attr->supplement_config = HI_VB_SUPPLEMENT_JPEG_MASK | HI_VB_SUPPLEMENT_BNR_MOT_MASK;
}

static hi_s32 sample_venc_fmu_wrap_init(const sample_venc_vpss_chn_attr *grp_attr, hi_s32 grp_num_max,
    hi_size *in_size)
{
    hi_u32 cnt = 0;
    hi_fmu_attr fmu_attr;
    hi_vpss_chn i, j;
    hi_u32 width, height;

    width = in_size->width;
    height = in_size->height;

    for (i = 0; i < grp_num_max && i < GRP_NUM_MAX; i++) {
        for (j = 0; j < HI_VPSS_MAX_PHYS_CHN_NUM; j++) {
            if (grp_attr[i].enable[j] == HI_TRUE && grp_attr[i].fmu_mode[j] == HI_FMU_MODE_WRAP) {
                width = (width > grp_attr[i].output_size[j].width) ? width : grp_attr[i].output_size[j].width;
                height = (height > grp_attr[i].output_size[j].height) ? height : grp_attr[i].output_size[j].height;
                cnt++;
            }
        }
    }

    if (cnt > 0) {
        fmu_attr.wrap_en = HI_TRUE;
        fmu_attr.page_num = MIN2(hi_common_get_fmu_wrap_page_num(HI_FMU_ID_VPSS,
            width, height) + (cnt - 1) * 3, /* 3: for multi pipe */
            HI_FMU_MAX_Y_PAGE_NUM + HI_FMU_MAX_C_PAGE_NUM);
    } else {
        fmu_attr.wrap_en = HI_FALSE;
    }

    if (hi_mpi_sys_set_fmu_attr(HI_FMU_ID_VPSS, &fmu_attr) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void sample_venc_vpss_cfg_chn(const sample_venc_vpss_chn_attr *grp_attr, hi_vpss_chn_attr *chn_attr,
    hi_s32 vpss_max_chn, hi_s32 i)
{
    hi_s32 j;
    for (j = 0; j < HI_VPSS_MAX_PHYS_CHN_NUM && j < vpss_max_chn; j++) {
        if (grp_attr[i].enable[j] == HI_TRUE) {
            chn_attr[j].width = grp_attr[i].output_size[j].width;
            chn_attr[j].height = grp_attr[i].output_size[j].height;
            chn_attr[j].chn_mode = HI_VPSS_CHN_MODE_USER;
            chn_attr[j].compress_mode = grp_attr[i].compress_mode[j];
            chn_attr[j].pixel_format = grp_attr[i].pixel_format;
            chn_attr[j].depth = 0;
            chn_attr[j].mirror_en = 0;
            chn_attr[j].flip_en = 0;
            chn_attr[j].aspect_ratio.mode = HI_ASPECT_RATIO_NONE;
            chn_attr[j].frame_rate.src_frame_rate = -1;
            chn_attr[j].frame_rate.dst_frame_rate = -1;
#ifdef OT_FPGA
            chn_attr[j].frame_rate.src_frame_rate = 30; /* 30: fpga src frame rate */
            chn_attr[j].frame_rate.dst_frame_rate = 10; /* 10: fpga dst frame rate */
#endif
        }
    }
}

static hi_s32 sample_venc_vpss_cfg_fmu_direct_chn(hi_vpss_grp grp, hi_vpss_chn chn,
    const sample_venc_vpss_chn_attr *grp_attr, hi_s32 grp_num_max)
{
    if (hi_mpi_vpss_set_chn_fmu_mode(grp, HI_VPSS_DIRECT_CHN,
        grp_attr[grp].fmu_mode[chn]) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (hi_mpi_vpss_enable_chn(grp, HI_VPSS_DIRECT_CHN) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (grp_num_max == 1) {
        if (hi_mpi_vpss_disable_chn(grp, 1) != HI_SUCCESS) {
            return HI_FAILURE;
        }
    } else {
        if (hi_mpi_vpss_disable_chn(grp, chn) != HI_SUCCESS) {
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

static hi_void sample_venc_get_default_ldc_attr(hi_ldc_attr *ldc_attr)
{
    ldc_attr->enable = HI_TRUE;
    ldc_attr->ldc_version = HI_LDC_V1;
    ldc_attr->ldc_v1_attr.aspect = 0;
    ldc_attr->ldc_v1_attr.x_ratio = 100; // 100: param
    ldc_attr->ldc_v1_attr.y_ratio = 100; // 100: param
    ldc_attr->ldc_v1_attr.xy_ratio = 100; // 100: param
    ldc_attr->ldc_v1_attr.center_x_offset = 0;
    ldc_attr->ldc_v1_attr.center_y_offset = 0;
    ldc_attr->ldc_v1_attr.distortion_ratio = 500; // 500: param
}

static hi_s32 sample_venc_vpss_grp_fmu_mode_init(const sample_venc_vpss_chn_attr *grp_attr, hi_s32 grp,
    hi_s32 grp_num_max)
{
    hi_s32 j;
    hi_gdc_param gdc_param = {0};
    hi_ldc_attr ldc_attr = {0};

    gdc_param.in_size.width  = grp_attr[grp].in_size.width;
    gdc_param.in_size.height = grp_attr[grp].in_size.height;
    gdc_param.cell_size = HI_LUT_CELL_SIZE_16;
    if (hi_mpi_vpss_set_grp_gdc_param(grp, &gdc_param) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    sample_venc_get_default_ldc_attr(&ldc_attr);
    if (hi_mpi_vpss_set_grp_ldc(grp, &ldc_attr) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (hi_mpi_vpss_set_grp_zme(grp, &grp_attr[grp].output_size[0]) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    for (j = 0; j < HI_VPSS_MAX_PHYS_CHN_NUM; j++) {
        if (grp_attr[grp].enable[j] != HI_TRUE) {
            continue;
        }
        if (grp_attr[grp].fmu_mode[j] == HI_FMU_MODE_DIRECT) {
            if (sample_venc_vpss_cfg_fmu_direct_chn(grp, j, grp_attr, grp_num_max) != HI_SUCCESS) {
                return HI_FAILURE;
            }
        }
    }

    return HI_SUCCESS;
}

static hi_s32 sample_venc_vpss_grp_fmu_init(const sample_venc_vpss_chn_attr *grp_attr, hi_s32 grp_num_max)
{
    hi_s32 i, j, ret;
    hi_vpss_grp_attr vpss_grp_attr = { 0 };
    sample_vpss_chn_attr vpss_chn_attr = {0};

    for (i = 0; i < grp_num_max && i < GRP_NUM_MAX; i++) {
        vpss_grp_attr.max_width = grp_attr[i].max_size.width;
        vpss_grp_attr.max_height = grp_attr[i].max_size.height;
        vpss_grp_attr.dei_mode = HI_VPSS_DEI_MODE_OFF;
        vpss_grp_attr.pixel_format = grp_attr[i].pixel_format;
        vpss_grp_attr.frame_rate.src_frame_rate = -1;
        vpss_grp_attr.frame_rate.dst_frame_rate = -1;

        sample_venc_vpss_cfg_chn(grp_attr, &vpss_chn_attr.chn_attr[0], HI_VPSS_MAX_PHYS_CHN_NUM, i);
        for (j = 0; j < HI_VPSS_MAX_PHYS_CHN_NUM; j++) {
            if (grp_attr[i].enable[j] != HI_TRUE) {
                continue;
            }

            if (grp_attr[i].fmu_mode[j] == HI_FMU_MODE_WRAP) {
                vpss_chn_attr.chn0_wrap = HI_TRUE;
            }
        }
        memcpy_s(vpss_chn_attr.chn_enable, sizeof(grp_attr[i].enable), grp_attr[i].enable, sizeof(grp_attr[i].enable));
        vpss_chn_attr.chn_array_size = HI_VPSS_MAX_PHYS_CHN_NUM;
        ret = sample_common_vpss_start(i, &vpss_grp_attr, &vpss_chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("failed with %#x!\n", ret);
            goto stop_vpss;
        }

        ret = sample_venc_vpss_grp_fmu_mode_init(grp_attr, i, grp_num_max);
        if (ret != HI_SUCCESS) {
            sample_print("failed with %#x!\n", ret);
            goto stop_vpss;
        }
    }

    return HI_SUCCESS;

stop_vpss:
    for (i = 0; i < grp_num_max && i < GRP_NUM_MAX; i++) {
        sample_common_vpss_stop(i, grp_attr[i].enable, HI_VPSS_MAX_PHYS_CHN_NUM);
    }
    return HI_FAILURE;
}

static hi_s32 sample_venc_vi_bind_vpss_init(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn)
{
    hi_s32 i, ret;

    for (i = 0; i < GRP_NUM_MAX; i++) {
        if ((ret = sample_comm_vi_bind_vpss(vi_pipe, vi_chn, i, 0)) != HI_SUCCESS) {
            sample_print("VI Bind VPSS err for %#x!\n", ret);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 sample_venc_fmu_grp_start_encode(hi_size *enc_size, hi_s32 chn_num_max,
    hi_s32 grp_num_max, sample_venc_vpss_chn *venc_vpss_chn)
{
    hi_s32 i, ret;
    hi_venc_gop_attr gop_attr = { 0 };
    sample_comm_venc_chn_param chn_param[TYPE_NUM_MAX] = {0};
    sample_comm_venc_chn_param *venc_chn_param = HI_NULL;

    if ((ret = sample_venc_set_fmu_video_param(enc_size, chn_param, gop_attr, TYPE_NUM_MAX,
        grp_num_max)) != HI_SUCCESS) {
        return ret;
    }

    for (i = 0; i < grp_num_max && i < GRP_NUM_MAX; i++) {
        venc_chn_param = &(chn_param[i]);
        if ((ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[i], venc_chn_param)) != HI_SUCCESS) {
            sample_print("Venc Start failed for %#x!\n", ret);
            goto UnBind;
        }

        ret = sample_venc_vpss_bind_venc(i, venc_vpss_chn->vpss_chn[i], venc_vpss_chn->venc_chn[i]);
        if (ret != HI_SUCCESS) {
            goto UnBind;
        }

        if (grp_num_max == 1) {
            venc_chn_param = &(chn_param[2]); /* 2: mjpege num */
            if ((ret = sample_comm_venc_start(venc_vpss_chn->venc_chn[1], venc_chn_param)) != HI_SUCCESS) {
                sample_print("Venc Start failed for %#x!\n", ret);
                goto UnBind;
            }

            ret = sample_venc_vpss_bind_venc(i, venc_vpss_chn->vpss_chn[1], venc_vpss_chn->venc_chn[1]);
            if (ret != HI_SUCCESS) {
                goto UnBind;
            }
        }
    }

    /* *****************************************
     stream save process
    ***************************************** */
    if ((ret = sample_comm_venc_start_get_stream(venc_vpss_chn->venc_chn, CHN_NUM_MAX)) != HI_SUCCESS) {
        sample_print("Start Venc failed!\n");
        goto UnBind;
    }

    return HI_SUCCESS;

UnBind:
    for (i = 0; i < grp_num_max && i < GRP_NUM_MAX; i++) {
        sample_comm_vpss_un_bind_venc(i, venc_vpss_chn->vpss_chn[i], venc_vpss_chn->venc_chn[i]);
        sample_comm_venc_stop(venc_vpss_chn->venc_chn[i]);
        if (grp_num_max == 1) {
            sample_comm_vpss_un_bind_venc(i, venc_vpss_chn->vpss_chn[1], venc_vpss_chn->venc_chn[1]);
            sample_comm_venc_stop(venc_vpss_chn->venc_chn[1]);
        }
    }

    return ret;
}

static hi_void sample_venc_unbind_vpss_fmu_stop(hi_s32 grp_num_max, const sample_venc_vpss_chn *venc_vpss_chn)
{
    hi_s32 i;

    if (grp_num_max > 1) {
        for (i = 0; i < grp_num_max && i < GRP_NUM_MAX; i++) {
            sample_comm_vpss_un_bind_venc(i, venc_vpss_chn->vpss_chn[i], venc_vpss_chn->venc_chn[i]);
            sample_comm_venc_stop(venc_vpss_chn->venc_chn[i]);
        }
    } else {
        for (i = 0; i < CHN_NUM_MAX; i++) {
            sample_comm_vpss_un_bind_venc(0, venc_vpss_chn->vpss_chn[i], venc_vpss_chn->venc_chn[i]);
            sample_comm_venc_stop(venc_vpss_chn->venc_chn[i]);
        }
    }
}

static hi_s32 sample_venc_fmu_one_grp(hi_void)
{
    hi_s32 ret;
    sample_sns_type sns_type = SENSOR0_TYPE;
    sample_vi_cfg vi_cfg = { 0 };
    const hi_vi_pipe vi_pipe = 0;
    const hi_vi_chn vi_chn = 0;
    const hi_vpss_grp vpss_grp = 0;
    sample_venc_vb_attr vb_attr = { 0 };
    hi_size enc_size[CHN_NUM_MAX], vi_size;
    sample_venc_vpss_chn venc_vpss_chn = { 0 };
    sample_venc_vpss_chn_attr grp_chn_attr[GRP_NUM_MAX] = { 0 };

    sample_comm_vi_get_default_vi_cfg(sns_type, &vi_cfg);
    /* vpss fmu direct场景下VI输出tile格式给vpss */
    sample_venc_get_vi_cfg_by_fmu_direct(&vi_cfg);
    sample_comm_vi_get_size_by_sns_type(sns_type, &vi_size);

    /* *****************************************
      step 0: related parameter ready
    ***************************************** */
    ret = sample_venc_grp_fmu_init_param(enc_size, CHN_NUM_MAX, &vi_size, grp_chn_attr, 1);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    sample_set_venc_vpss_fmu_chn(&venc_vpss_chn, CHN_NUM_MAX, grp_chn_attr, 1);

    /******************************************
      step 1: init sys alloc common vb
    ******************************************/
    get_fmu_vb_attr(&vi_size, &vb_attr);
    if ((ret = sample_venc_sys_init(&vb_attr)) != HI_SUCCESS) {
        sample_print("Init SYS err for %#x!\n", ret);
        return ret;
    }

    if ((ret = sample_venc_fmu_wrap_init(grp_chn_attr, 1, &vi_size)) != HI_SUCCESS) {
        sample_print("Init SYS err for %#x!\n", ret);
        return ret;
    }

    if ((ret = sample_venc_vi_init(&vi_cfg)) != HI_SUCCESS) {
        sample_print("Init VI err for %#x!\n", ret);
        goto EXIT_SYS_STOP;
    }

    if ((ret = sample_venc_vpss_grp_fmu_init(grp_chn_attr, 1)) != HI_SUCCESS) {
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
    ret = sample_venc_fmu_grp_start_encode(enc_size, CHN_NUM_MAX, 1, &venc_vpss_chn);
    if (ret != HI_SUCCESS) {
        goto EXIT_VI_VPSS_UNBIND;
    }

    /* *****************************************
     exit process
    ***************************************** */
    sample_venc_exit_process();
    sample_venc_unbind_vpss_fmu_stop(1, &venc_vpss_chn);

EXIT_VI_VPSS_UNBIND:
    sample_comm_vi_un_bind_vpss(vi_pipe, vi_chn, vpss_grp, 0);
EXIT_VPSS_STOP:
    sample_venc_vpss_deinit(vpss_grp, &grp_chn_attr[vpss_grp]);
EXIT_VI_STOP:
    sample_venc_vi_deinit(&vi_cfg);
EXIT_SYS_STOP:
    sample_comm_sys_exit();

    return ret;
}

static hi_s32 sample_venc_fmu_two_grp(hi_void)
{
    hi_s32 i, ret;
    sample_sns_type sns_type = SENSOR0_TYPE;
    sample_vi_cfg vi_cfg = { 0 };
    const hi_vi_pipe vi_pipe = 0;
    const hi_vi_chn vi_chn = 0;
    sample_venc_vb_attr vb_attr = { 0 };
    hi_size enc_size[CHN_NUM_MAX], vi_size;
    sample_venc_vpss_chn venc_vpss_chn = { 0 };
    sample_venc_vpss_chn_attr grp_chn_attr[GRP_NUM_MAX] = { 0 };

    sample_comm_vi_get_default_vi_cfg(sns_type, &vi_cfg);

    /* vpss fmu direct场景下VI输出tile格式给vpss */
    sample_venc_get_vi_cfg_by_fmu_direct(&vi_cfg);
    sample_comm_vi_get_size_by_sns_type(sns_type, &vi_size);

    /******************************************
      step 0: related parameter ready
    ******************************************/
    ret = sample_venc_grp_fmu_init_param(enc_size, CHN_NUM_MAX, &vi_size, grp_chn_attr, GRP_NUM_MAX);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    sample_set_venc_vpss_fmu_chn(&venc_vpss_chn, CHN_NUM_MAX, grp_chn_attr, GRP_NUM_MAX);

    /* *****************************************
      step 1: init sys alloc common vb
    ***************************************** */
    get_fmu_vb_attr(&vi_size, &vb_attr);
    if ((ret = sample_venc_sys_init(&vb_attr)) != HI_SUCCESS) {
        sample_print("Init SYS err for %#x!\n", ret);
        return ret;
    }

    if ((ret = sample_venc_fmu_wrap_init(grp_chn_attr, GRP_NUM_MAX, &vi_size)) != HI_SUCCESS) {
        sample_print("Init SYS err for %#x!\n", ret);
        return ret;
    }

    if ((ret = sample_venc_vi_init(&vi_cfg)) != HI_SUCCESS) {
        sample_print("Init VI err for %#x!\n", ret);
        goto EXIT_SYS_STOP;
    }

    if ((ret = sample_venc_vpss_grp_fmu_init(grp_chn_attr, GRP_NUM_MAX)) != HI_SUCCESS) {
        sample_print("Init VPSS err for %#x!\n", ret);
        goto EXIT_VI_STOP;
    }

    if ((ret = sample_venc_vi_bind_vpss_init(vi_pipe, vi_chn)) != HI_SUCCESS) {
        sample_print("Init vi bind vpss err for %#x!\n", ret);
        goto EXIT_VPSS_STOP;
    }

    /* *****************************************
    start stream venc
    ***************************************** */
    ret = sample_venc_fmu_grp_start_encode(enc_size, CHN_NUM_MAX, GRP_NUM_MAX, &venc_vpss_chn);
    if (ret != HI_SUCCESS) {
        goto EXIT_VI_VPSS_UNBIND;
    }

    /* *****************************************
     exit process
    ***************************************** */
    sample_venc_exit_process();
    sample_venc_unbind_vpss_fmu_stop(GRP_NUM_MAX, &venc_vpss_chn);

EXIT_VI_VPSS_UNBIND:
    for (i = 0; i < GRP_NUM_MAX; i++) {
        sample_comm_vi_un_bind_vpss(vi_pipe, vi_chn, i, 0);
    }
EXIT_VPSS_STOP:
    for (i = 0; i < GRP_NUM_MAX; i++) {
        sample_venc_vpss_deinit(i, &grp_chn_attr[i]);
    }
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
            ret = sample_venc_qpmap();
            break;

        case 2: /* 2: mode 2 */
            ret = sample_venc_intra_refresh();
            break;

        case 3: /* 3: mode 3 */
            ret = sample_venc_roi_bg();
            break;

        case 4: /* 4: mode 4 */
            ret = sample_venc_debreath_effect();
            break;

        case 5: /* 5: mode 5 */
            ret = sample_venc_mjpeg_roi_set();
            break;

        case 6: /* 6: mode 6 */
            ret = sample_venc_mjpeg_roimap();
            break;

        case 7: /* 7: mode 7 */
            ret = sample_venc_mosaic_map();
            break;

        case 8: /* 8: mode 8 */
            ret = sample_venc_mjpeg_mosaic_map();
            break;

        case 9: /* 9: mode 9 */
            ret = sample_venc_watermark();
            break;

        case 10: /* 10: mode 10 */
            ret = sample_venc_pvc();
            break;

        case 11: /* 11: mode 11 */
            ret = sample_venc_fmu_one_grp();
            break;

        case 12: /* 12: mode 12 */
            ret = sample_venc_fmu_two_grp();
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

    if (argc == 0x3) {
        hi_s32 save_heif = strtoul(argv[0x2], &end_ptr, 10); /* 10: numberbase */
        if (end_ptr == argv[0x2] || *end_ptr !='\0' || (save_heif != 0 && save_heif != 1)) {
            sample_venc_usage(argv[0]);
            return HI_FAILURE;
        }
        sample_comm_venc_set_save_heif((save_heif == 1) ? HI_TRUE : HI_FALSE);
    }

    index = strtoul(argv[1], &end_ptr, 10); /* 10： numberbase */
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

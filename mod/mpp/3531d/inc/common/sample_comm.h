/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: Common definition for all sample.
 * Author: Hisilicon multimedia software group
 * Create: 2019/11/29
 */

#ifndef __SAMPLE_COMM_H__
#define __SAMPLE_COMM_H__

#include <pthread.h>

#include "hi_common.h"
#include "hi_math.h"
#include "hi_buffer.h"
#include "hi_defines.h"
#include "securec.h"
#include "hi_common_sys.h"
#include "hi_common_vb.h"
#include "hi_common_vi.h"
#include "hi_common_vo.h"
#include "hi_common_venc.h"
#include "hi_common_vdec.h"
#include "hi_common_vpss.h"
#include "hi_common_region.h"
#include "hi_common_adec.h"
#include "hi_common_aenc.h"
#include "hi_common_aio.h"
#include "hi_common_hdmi.h"
#include "hi_common_vgs.h"
#include "hi_common_vda.h"

#include "hi_mpi_sys.h"
#include "hi_mpi_vb.h"
#include "hi_mpi_vi.h"
#include "hi_mpi_vo.h"
#include "hi_mpi_venc.h"
#include "hi_mpi_vdec.h"
#include "hi_mpi_vpss.h"
#include "hi_mpi_region.h"
#include "hi_mpi_audio.h"
#include "hi_mpi_hdmi.h"
#include "hi_mpi_vgs.h"
#include "hi_mpi_vda.h"

#ifdef __cplusplus
extern "C" {
#endif /* end of #ifdef __cplusplus */

/* macro define */
#define MINOR_CHN(vi_chn) ((vi_chn) + 1)

#define FILE_NAME_LEN 128
#define FILE_PATH_LEN 128

#define CHECK_CHN_RET(express,chn,name)\
    do{\
        hi_s32 ret;\
        ret = express;\
        if (HI_SUCCESS != ret)\
        {\
            printf("\033[0;31m%s chn %d failed at %s: LINE: %d with %#x!\033[0;39m\n", name, chn, __FUNCTION__, __LINE__, ret);\
            fflush(stdout);\
            return ret;\
        }\
    } while (0)

#define CHECK_RET(express,name)\
    do{\
        hi_s32 ret;\
        ret = express;\
        if (HI_SUCCESS != ret)\
        {\
            printf("\033[0;31m%s failed at %s: LINE: %d with %#x!\033[0;39m\n", name, __FUNCTION__, __LINE__, ret);\
            return ret;\
        }\
    } while (0)
#define SAMPLE_PIXEL_FORMAT         HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420

#define COLOR_RGB_RED      0xFF0000
#define COLOR_RGB_GREEN    0x00FF00
#define COLOR_RGB_BLUE     0x0000FF
#define COLOR_RGB_BLACK    0x000000
#define COLOR_RGB_YELLOW   0xFFFF00
#define COLOR_RGB_CYN      0x00ffff
#define COLOR_RGB_WHITE    0xffffff

#define SAMPLE_VO_DEV_DHD0 0                  /* VO's device HD0 */
#define SAMPLE_VO_DEV_DHD1 1                  /* VO's device HD1 */
#define SAMPLE_VO_DEV_DSD0 2                  /* VO's device SD0 */
#define SAMPLE_VO_DEV_UHD  SAMPLE_VO_DEV_DHD0 /* VO's ultra HD device:HD0 */
#define SAMPLE_VO_DEV_HD   SAMPLE_VO_DEV_DHD1 /* VO's HD device:HD1 */
#define SAMPLE_VO_LAYER_VHD0 0
#define SAMPLE_VO_LAYER_VHD1 1
#define SAMPLE_VO_LAYER_VHD2 2
#define SAMPLE_VO_LAYER_PIP  SAMPLE_VO_LAYER_VHD2

#define SAMPLE_AUDIO_EXTERN_AI_DEV 0
#define SAMPLE_AUDIO_EXTERN_AO_DEV 0
#define SAMPLE_AUDIO_INNER_AI_DEV 0
#define SAMPLE_AUDIO_INNER_AO_DEV 0
#define SAMPLE_AUDIO_INNER_HDMI_AO_DEV 1

#define SAMPLE_AUDIO_POINT_NUM_PER_FRAME 480

#define WDR_MAX_PIPE_NUM 4

#define CHN_NUM_PRE_DEV            4
#define SECOND_CHN_OFFSET_2MUX     2

#define D1_WIDTH            720
#define D1_HEIGHT_PAL       576
#define D1_HEIGHT_NTSC      480

#define _960H_WIDTH         960
#define _960H_HEIGHT_PAL    576
#define _960H_HEIGHT_NTSC   480

#define HD_WIDTH            1280
#define HD_HEIGHT           720

#define FHD_WIDTH           1920
#define FHD_HEIGHT          1080

#define _4K_WIDTH           3840
#define _4K_HEIGHT          2160

#define AD_NVP6158 0
#define AD_TP2854B 1
#define AD_TP2856  2
#define AD_LT8619C 3

#define SAMPLE_AD_TYPE AD_LT8619C //maohw AD_TP2854B

#define NVP6158_FILE "/dev/nc_vdec"
#define TP2854B_FILE "/dev/tp2802dev"
#define TP2856_FILE "/dev/tp2802dev"
#define TP2828_FILE "/dev/tp2823dev"

#define ES8388_FILE "/dev/es8388"
#define ES8388_CHIP_ID 0

#define VO_LT8618SX 1
#define LT8618SX_DEV_NAME "/dev/lt8618sx"

#define SAMPLE_FRAME_BUF_RATIO_MAX 100
#define SAMPLE_FRAME_BUF_RATIO_MIN 70

#define PAUSE()  do {\
        printf("---------------press enter key to exit!---------------\n");\
        getchar();\
    } while (0)

#define SAMPLE_PRT(fmt...)   \
    do {\
        printf("[%s]-%d: ", __FUNCTION__, __LINE__);\
        printf(fmt);\
    } while (0)

#define check_null_ptr_return(ptr)\
    do {\
        if ((ptr) == HI_NULL) {\
            printf("func:%s,line:%d, NULL pointer\n", __FUNCTION__, __LINE__);\
            return HI_FAILURE;\
        }\
    } while (0)


/* enum define */
typedef enum {
    SAMPLE_VI_MODE_960H_4X4 = 0,
    SAMPLE_VI_MODE_720P_1X4,
    SAMPLE_VI_MODE_720P_2X2,
    SAMPLE_VI_MODE_720P_4X4,
    SAMPLE_VI_MODE_1080P_2X2,
    SAMPLE_VI_MODE_1080P_2X4,
    SAMPLE_VI_MODE_4M_1X8,
    SAMPLE_VI_MODE_5M_1X8,
    SAMPLE_VI_MODE_4K_1X8,
    SAMPLE_VI_MODE_1080P_1X8,
    SAMPLE_VI_MODE_BUTT
} sample_vi_mode;

typedef enum {
    SAMPLE_VIDEO_ENCODING_PAL = 0,
    SAMPLE_VIDEO_ENCODING_NTSC,
    SAMPLE_VIDEO_ENCODING_AUTO,
    SAMPLE_VIDEO_ENCODING_BUTT
} sample_video_encoding;

typedef enum {
    PIC_CIF,
    PIC_360P,    /* 640 * 360 */
    PIC_D1_PAL,  /* 720 * 576 */
    PIC_D1_NTSC, /* 720 * 480 */
    PIC_960H,      /* 960 * 576 */
    PIC_720P,    /* 1280 * 720 */
    PIC_1080P,   /* 1920 * 1080 */
    PIC_480P,
    PIC_576P,
    PIC_800x600,
    PIC_1024x768,
    PIC_1280x1024,
    PIC_1366x768,
    PIC_1440x900,
    PIC_1280x800,
    PIC_1600x1200,
    PIC_1680x1050,
    PIC_1920x1200,
    PIC_640x480,
    PIC_1920x2160,
    PIC_2560x1440,
    PIC_2560x1600,
    PIC_2592x1520,
    PIC_2592x1944,
    PIC_3840x2160,
    PIC_4096x2160,
    PIC_3000x3000,
    PIC_4000x3000,
    PIC_7680x4320,
    PIC_3840x8640,
    PIC_BUTT
} hi_pic_size;

typedef enum {
    SONY_IMX290_MIPI_2M_30FPS_12BIT,
    SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1,
    SONY_IMX290_MIPI_1M_30FPS_12BIT_WDR2TO1,
    SONY_IMX290_SLAVE_MIPI_2M_60FPS_10BIT,
    SONY_IMX334_MIPI_8M_30FPS_12BIT,
    SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1,
    SONY_IMX377_MIPI_8M_30FPS_10BIT,
    NVP6324_MIPI_2M_30FPS_8BIT,
    SAMPLE_SNS_TYPE_BUTT,
} sample_sns_type;

typedef enum {
    VO_MODE_1MUX = 0,
    VO_MODE_2MUX,
    VO_MODE_4MUX,
    VO_MODE_8MUX,
    VO_MODE_9MUX,
    VO_MODE_16MUX,
    VO_MODE_25MUX,
    VO_MODE_36MUX,
    VO_MODE_49MUX,
    VO_MODE_64MUX,
    VO_MODE_2X4,
    VO_MODE_BUTT
} sample_vo_mode;

typedef enum {
    SAMPLE_RC_CBR = 0,
    SAMPLE_RC_VBR,
    SAMPLE_RC_AVBR,
    SAMPLE_RC_CVBR,
    SAMPLE_RC_QVBR,
    SAMPLE_RC_QPMAP,
    SAMPLE_RC_FIXQP
} sample_rc;

/* structure define */
typedef struct {
    hi_bool thread_start;
    hi_venc_chn venc_chn[HI_VENC_MAX_CHN_NUM];
    hi_s32 cnt;
    //maohw add callback;
    void *uargs;
    int (*cb)(hi_venc_chn chn, hi_payload_type pt, hi_venc_stream* stream, void* uargs);
} sample_venc_getstream_para;

typedef struct {
    hi_bool thread_start;
    hi_venc_chn venc_chn[HI_VENC_MAX_CHN_NUM];
    hi_s32  cnt;
    hi_vpss_grp vpss_grp;
    hi_vpss_chn      vpss_chn[HI_VENC_MAX_CHN_NUM];
} sample_venc_rateauto_para;
typedef struct {
    hi_bool  thread_start;
    hi_vpss_grp vpss_grp;
    hi_venc_chn venc_chn[HI_VENC_MAX_CHN_NUM];
    hi_vpss_chn vpss_chn[HI_VPSS_MAX_PHYS_CHN_NUM];
    hi_size size[HI_VENC_MAX_CHN_NUM];
    hi_s32 cnt;
} sample_venc_qpmap_sendframe_para;

typedef struct {
    hi_bool thread_start;
    hi_vpss_grp vpss_grp;
    hi_venc_chn venc_chn[HI_VENC_MAX_CHN_NUM];
    hi_vpss_chn vpss_chn[HI_VPSS_MAX_PHYS_CHN_NUM];
    hi_size size[HI_VENC_MAX_CHN_NUM];
    hi_venc_jpeg_roi_attr roi_attr[HI_VENC_MAX_CHN_NUM];
    hi_s32 cnt;
} sample_venc_roimap_frame_para;

typedef struct {
    hi_vi_pipe vi_pipe;
    hi_s32 cnt;
    hi_bool dump;
    hi_char asz_name[FILE_NAME_LEN];
    pthread_t thread_id;
} sample_vi_dump_thread_info;

typedef struct {
    sample_sns_type sns_type;
    hi_s32 sns_id;
    hi_s32 bus_id;
} sample_sensor_info;

typedef struct {
    hi_bool snap;
    hi_bool double_pipe;
    hi_vi_pipe video_pipe;
    hi_vi_pipe snap_pipe;
} sample_snap_info;

typedef struct {
    hi_vi_dev vi_dev;
} sample_dev_info;

typedef struct {
    hi_vi_pipe a_pipe[WDR_MAX_PIPE_NUM];
    hi_bool multi_pipe;
    hi_bool vc_num_cfged;
    hi_u32 vc_num[WDR_MAX_PIPE_NUM];
} sample_pipe_info;

typedef struct {
    hi_vi_chn vi_chn;
    hi_pixel_format pix_format;
    hi_dynamic_range dynamic_range;
    hi_video_format video_format;
    hi_compress_mode compress_mode;
} sample_chn_info;

typedef struct {
    sample_sensor_info sns_info;
    sample_dev_info dev_info;
    sample_pipe_info pipe_info;
    sample_chn_info chn_info;
    sample_snap_info snap_info;
} sample_vi_info;

typedef struct {
    sample_vi_mode vi_mode;
    sample_video_encoding video_encoding;
} sample_vi_config;

typedef struct {
    hi_s32 vi_dev_cnt;
    hi_s32 vi_dev_interval;
    hi_s32 vi_chn_cnt;
    hi_s32 vi_chn_interval;
    hi_vi_scan_mode scan_mode;
    hi_bool minor_attr_en;
    hi_size minor_dst_size;
} sample_vi_param;

typedef struct {
    hi_u32 width;
    hi_u32 height;
    hi_u32 byte_align;
    hi_pixel_format hi_pixel_format;
    hi_video_format video_format;
    hi_compress_mode compress_mode;
    hi_dynamic_range dynamic_range;
} sample_vi_frame_config;

typedef struct {
    hi_vb_blk vb_blk;
    hi_u32 size;
    hi_video_frame_info video_frame_info;
} sample_vi_frame_info;

typedef struct {
    hi_u32 threshold;
    hi_u32 frame_num;
    hi_pixel_format hi_pixel_format;
    hi_compress_mode compress_mode;
} sample_vi_fpn_calibrate_info;

typedef struct {
    hi_u32 strength;
    hi_pixel_format hi_pixel_format;
    hi_compress_mode compress_mode;
    sample_vi_frame_info vi_frame_info;
} sample_vi_fpn_correction_info;

typedef struct {
    hi_vo_intf_sync intf_sync;
    hi_u32 width;
    hi_u32 height;
    hi_u32 frame_rate;
} sample_vo_sync_info;

typedef struct {
    sample_vo_mode mode;
    hi_u32 wnd_num;
    hi_u32 square;
    hi_u32 row;
    hi_u32 col;
} sample_vo_wnd_info;

typedef struct {
    hi_vo_wbc_src_type source_type;
    hi_dynamic_range dynamic_range;
    hi_compress_mode compress_mode;
    hi_s32 depth;

    hi_s32 vo_wbc;
    hi_vo_wbc_attr wbc_attr;
    hi_vo_wbc_src wbc_source;
    hi_vo_wbc_mode wbc_mode;
} sample_vo_wbc_config;

typedef struct {
    /* for layer */
    hi_vo_layer vo_layer;
    hi_vo_intf_sync intf_sync;
    hi_rect display_rect;
    hi_size image_size;
    hi_pixel_format pix_format;

    hi_u32 dis_buf_len;
    hi_dynamic_range dst_dynamic_range;

    /* for chn */
    sample_vo_mode vo_mode;
} sample_comm_vo_layer_config;

typedef struct {
    /* for device */
    hi_vo_dev vo_dev;
    hi_vo_intf_type vo_intf_type;
    hi_vo_intf_sync intf_sync;
    hi_pic_size pic_size;
    hi_u32 bg_color;

    /* for layer */
    hi_pixel_format pix_format;
    hi_rect disp_rect;
    hi_size image_size;
    hi_vo_partition_mode vo_part_mode;
    hi_compress_mode compress_mode;

    hi_u32 dis_buf_len;
    hi_dynamic_range dst_dynamic_range;

    /* for chnnel */
    sample_vo_mode vo_mode;
} sample_vo_config;

typedef enum {
    THREAD_CTRL_START,
    THREAD_CTRL_PAUSE,
    THREAD_CTRL_STOP,
} thread_contrl;

typedef struct {
    hi_pic_size pic_size;
    hi_vo_intf_sync intf_sync;
    hi_vo_intf_type intf_type;
} vdec_display_cfg;

typedef struct {
    hi_s32 chn_id;
    hi_payload_type type;
    hi_char c_file_path[FILE_PATH_LEN];
    hi_char c_file_name[FILE_NAME_LEN];
    hi_s32 stream_mode;
    hi_s32 milli_sec;
    hi_s32 min_buf_size;
    hi_s32 interval_time;
    thread_contrl e_thread_ctrl;
    hi_u64 pts_init;
    hi_u64 pts_increase;
    hi_bool circle_send;
} vdec_thread_param;

typedef struct {
    hi_u32 pic_buf_size;
    hi_u32 tmv_buf_size;
    hi_bool pic_buf_alloc;
    hi_bool tmv_buf_alloc;
} sample_vdec_buf;

typedef struct {
    hi_video_dec_mode dec_mode;
    hi_u32 ref_frame_num;
    hi_data_bit_width bit_width;
} sample_vdec_video_attr;

typedef struct {
    hi_pixel_format pixel_format;
    hi_u32 alpha;
} sample_vdec_pic_attr;

typedef struct {
    hi_payload_type type;
    hi_vdec_send_mode mode;
    hi_u32 width;
    hi_u32 height;
    hi_u32 frame_buf_cnt;
    hi_u32 display_frame_num;
    union {
        sample_vdec_video_attr sapmle_vdec_video; /* structure with video (h265/h264) */
        sample_vdec_pic_attr sapmle_vdec_picture; /* structure with picture (jpeg/mjpeg) */
    };
} sample_vdec_attr;

typedef struct {
    hi_video_format video_format;
    hi_pixel_format pixel_format;
    hi_u32 width;
    hi_u32 height;
    hi_u32 align;
    hi_compress_mode compress_mode;
} sample_vb_base_info;

typedef struct {
    hi_u32 vb_size;
    hi_u32 head_stride;
    hi_u32 head_size;
    hi_u32 head_y_size;
    hi_u32 main_stride;
    hi_u32 main_size;
    hi_u32 main_y_size;
    hi_u32 ext_stride;
    hi_u32 ext_y_size;
} sample_vb_cal_config;

typedef struct {
    hi_u32 frame_rate;
    hi_u32 stats_time;
    hi_u32 gop;
    hi_size venc_size;
    hi_pic_size size;
    hi_u32 profile;
    hi_bool is_rcn_ref_share_buf;
    hi_venc_gop_attr gop_attr;
    hi_payload_type type;
    sample_rc rc_mode;
} sample_comm_venc_chnl_param;

typedef struct {
    hi_audio_sample_rate out_sample_rate;
    hi_bool resample_en;
    hi_void *ai_vqe_attr;
    hi_u32 ai_vqe_type;
} sample_comm_ai_vqe_param;

/* function announce */
hi_void *sample_sys_io_mmap(hi_u64 phy_addr, hi_u32 size);
hi_s32 sample_sys_munmap(hi_void *vir_addr, hi_u32 size);
hi_s32 sample_sys_set_reg(hi_u64 addr, hi_u32 value);
hi_s32 sample_sys_get_reg(hi_u64 addr, hi_u32 *value);

hi_s32 sample_comm_sys_get_pic_size(hi_pic_size pic_size, hi_size *size);
hi_s32 sample_comm_sys_mem_config(hi_void);
hi_void sample_comm_sys_exit(hi_void);
hi_s32 sample_comm_sys_init(hi_vb_cfg *vb_cfg);
hi_s32 sample_comm_sys_init_with_vb_supplement(hi_vb_cfg* vb_cfg, hi_u32 supplement_config);

hi_s32 sample_comm_vi_bind_vo(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_vo_layer vo_layer, hi_vo_chn vo_chn);
hi_s32 sample_comm_vi_un_bind_vo(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_vo_layer vo_layer, hi_vo_chn vo_chn);
hi_s32 sample_comm_vi_bind_vpss(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn);
hi_s32 sample_comm_vi_un_bind_vpss(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn);
hi_s32 sample_comm_vi_bind_venc(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_venc_chn venc_chn);
hi_s32 sample_comm_vi_un_bind_venc(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_venc_chn venc_chn);
hi_s32 sample_comm_vpss_bind_vo(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn, hi_vo_layer vo_layer, hi_vo_chn vo_chn);
hi_s32 sample_comm_vpss_un_bind_vo(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn, hi_vo_layer vo_layer, hi_vo_chn vo_chn);
hi_s32 sample_comm_vpss_bind_venc(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn, hi_venc_chn venc_chn);
hi_s32 sample_comm_vpss_un_bind_venc(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn, hi_venc_chn venc_chn);
hi_s32 sample_comm_vdec_bind_vpss(hi_vdec_chn vdec_chn, hi_vpss_grp vpss_grp);
hi_s32 sample_comm_vdec_un_bind_vpss(hi_vdec_chn vdec_chn, hi_vpss_grp vpss_grp);
hi_s32 sample_comm_vo_bind_vo(hi_vo_layer src_vo_layer, hi_vo_chn src_vo_chn,
                              hi_vo_layer dst_vo_layer, hi_vo_chn dst_vo_chn);
hi_s32 sample_comm_vo_un_bind_vo(hi_vo_layer dst_vo_layer, hi_vo_chn dst_vo_chn);

hi_s32 sample_common_vi_mode_to_param(sample_vi_mode vi_mode, sample_vi_param *vi_param);
hi_s32 sample_common_vi_get_size_by_mode(sample_vi_mode vi_mode, hi_pic_size *pic_size);
hi_s32 sample_common_vi_mode_to_param(sample_vi_mode vi_mode, sample_vi_param *vi_param);
hi_s32 sample_common_vi_start_dev(hi_vi_dev vi_dev, sample_vi_mode vi_mode);
hi_s32 sample_common_vi_start_chn(hi_s32 vi_chn, hi_rect *capture_rect, hi_size *dst_size, sample_vi_param *vi_param);
hi_s32 sample_common_vi_start_mipi(hi_rect *capture_rect, sample_vi_param *vi_param);
hi_s32 sample_common_vi_start_vi(sample_vi_mode vi_mode, sample_video_encoding video_encoding, sample_vi_param *vi_param);
hi_s32 sample_common_vi_stop(sample_vi_mode vi_mode, sample_vi_param *vi_param);

hi_s32 sample_common_vpss_start(hi_vpss_grp grp, const hi_bool *chn_enable,
    const hi_vpss_grp_attr *grp_attr, const hi_vpss_chn_attr *chn_attr, hi_u32 chn_array_size);
hi_s32 sample_common_vpss_stop(hi_vpss_grp grp, const hi_bool *chn_enable, hi_u32 chn_array_size);

hi_s32 sample_comm_vo_get_width_height(hi_vo_intf_sync intf_sync, hi_u32 *width, hi_u32 *height,
    hi_u32 *frame_rate);
hi_s32 sample_comm_vo_mem_config(hi_vo_dev vo_dev, hi_char *pc_mmz_name);
hi_s32 sample_comm_vo_start_dev(hi_vo_dev vo_dev, hi_vo_pub_attr *pub_attr);
hi_s32 sample_comm_vo_stop_dev(hi_vo_dev vo_dev);
hi_s32 sample_comm_vo_start_layer(hi_vo_layer vo_layer, const hi_vo_video_layer_attr *layer_attr);
hi_s32 sample_comm_vo_stop_layer(hi_vo_layer vo_layer);
hi_s32 sample_comm_vo_get_wnd_info(sample_vo_mode mode, sample_vo_wnd_info *wnd_info);
hi_s32 sample_comm_vo_get_chn_attr(sample_vo_wnd_info *wnd_info, hi_vo_video_layer_attr *layer_attr,
    hi_s32 chn, hi_vo_chn_attr *chn_attr);
hi_s32 sample_comm_vo_start_chn(hi_vo_layer vo_layer, sample_vo_mode mode);
hi_s32 sample_comm_vo_stop_chn(hi_vo_layer vo_layer, sample_vo_mode mode);
hi_s32 sample_comm_vo_start_wbc(sample_vo_wbc_config *wbc_config);
hi_s32 sample_comm_vo_stop_wbc(sample_vo_wbc_config *wbc_config);
hi_s32 sample_comm_vo_get_def_wbc_config(sample_vo_wbc_config *wbc_config);
hi_s32 sample_comm_vo_bind_vi(hi_vo_layer vo_layer, hi_vo_chn vo_chn, hi_vi_chn vi_chn);
hi_s32 sample_comm_vo_un_bind_vi(hi_vo_layer vo_layer, hi_vo_chn vo_chn);
hi_s32 sample_comm_vo_bt1120_start(hi_vo_pub_attr *pub_attr);
hi_s32 sample_comm_vo_hdmi_start(hi_vo_intf_sync intf_sync);
hi_s32 sample_comm_vo_hdmi_stop(hi_void);
hi_s32 sample_comm_vo_get_def_config(sample_vo_config *vo_config);
hi_s32 sample_comm_vo_stop_vo(sample_vo_config *vo_config);
hi_void sample_comm_vo_set_hdmi_rgb_mode(hi_bool enable);
hi_s32 sample_comm_vo_start_vo(sample_vo_config *vo_config);
hi_s32 sample_comm_vo_stop_pip(sample_vo_config *vo_config);
hi_s32 sample_comm_vo_start_pip(sample_vo_config *vo_config);
hi_s32 sample_comm_vo_get_def_layer_config(sample_comm_vo_layer_config *vo_layer_config);
hi_s32 sample_comm_vo_start_layer_chn(sample_comm_vo_layer_config *vo_layer_config);
hi_s32 sample_comm_vo_stop_layer_chn(sample_comm_vo_layer_config *vo_layer_config);

hi_s32 sample_comm_venc_mem_config(hi_void);
hi_s32 sample_comm_venc_create(hi_venc_chn venc_chn, sample_comm_venc_chnl_param *venc_create_param);
hi_s32 sample_comm_venc_start(hi_venc_chn venc_chn, sample_comm_venc_chnl_param *venc_create_param);
hi_s32 sample_comm_venc_stop(hi_venc_chn venc_chn);
hi_s32 sample_comm_venc_snap_start(hi_venc_chn venc_chn, hi_size* size, hi_bool support_dcf);
hi_s32 sample_comm_venc_snap_process(hi_venc_chn venc_chn, hi_u32 snap_cnt, hi_bool save_jpg, hi_bool save_thm);
hi_s32 sample_comm_venc_save_jpeg(hi_venc_chn venc_chn, hi_u32 snap_cnt);
hi_s32 sample_comm_venc_snap_stop(hi_venc_chn venc_chn);

hi_s32 sample_comm_venc_start_get_stream(hi_venc_chn ve_chn[], hi_s32 cnt);
hi_s32 sample_comm_venc_start_get_streamCB(hi_venc_chn *venc_chn, hi_s32 cnt, int (*cb)(hi_venc_chn chn, hi_payload_type pt, hi_venc_stream* stream, void* uargs), void *uargs);

hi_s32 sample_comm_venc_stop_get_stream(hi_s32 chn_num);
hi_s32 sample_comm_venc_start_get_stream_svc_t(hi_s32 cnt);
hi_s32 sample_comm_venc_get_gop_attr(hi_venc_gop_mode gop_mode, hi_venc_gop_attr *gop_attr);
hi_s32 sample_comm_venc_qpmap_send_frame(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn[], hi_venc_chn venc_chn[], hi_s32 cnt, hi_size size[]);
hi_s32 sample_comm_venc_stop_send_qpmap_frame(void);
hi_s32 sample_comm_venc_rateauto_start(hi_venc_chn ve_chn[], hi_s32 cnt, hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn[]);
hi_s32 sample_comm_venc_stop_rateauto(hi_venc_chn ve_chn[], hi_s32 cnt);

hi_s32 sample_comm_venc_send_roimap_frame(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn[], hi_venc_chn venc_chn[],
    hi_s32 cnt, hi_size size[], hi_venc_jpeg_roi_attr roi_attr[]);
hi_s32 sample_comm_venc_stop_send_roimap_frame(void);
hi_s32 sample_comm_venc_save_stream(FILE *fd, hi_venc_stream *stream);

hi_s32 sample_comm_region_create(hi_s32 handle_num, hi_rgn_type type);
hi_s32 sample_comm_region_destroy(hi_s32 handle_num, hi_rgn_type type);
hi_s32 sample_comm_region_attach_to_chn(hi_s32 handle_num, hi_rgn_type type, hi_mpp_chn *mpp_chn);
hi_s32 sample_comm_region_detach_frm_chn(hi_s32 handle_num, hi_rgn_type type, hi_mpp_chn *mpp_chn);
hi_s32 sample_comm_region_set_bit_map(hi_rgn_handle handle, const hi_char *bmp_path);
hi_s32 sample_comm_region_get_up_canvas(hi_rgn_handle handle, const hi_char *bmp_path);
hi_s32 sample_comm_region_get_min_handle(hi_rgn_type type);

hi_s32 sample_comm_audio_creat_trd_ai_ao(hi_audio_dev ai_dev, hi_ai_chn ai_chn, hi_audio_dev ao_dev, hi_ao_chn ao_chn);
hi_s32 sample_comm_audio_creat_trd_ai_aenc(hi_audio_dev ai_dev, hi_ai_chn ai_chn, hi_aenc_chn ae_chn);
hi_s32 sample_comm_audio_creat_trd_aenc_adec(hi_aenc_chn ae_chn, hi_adec_chn ad_chn, FILE *aenc_fd);
hi_s32 sample_comm_audio_creat_trd_file_adec(hi_adec_chn ad_chn, FILE *adec_fd);
hi_s32 sample_comm_audio_creat_trd_ao_vol_ctrl(hi_audio_dev ao_dev);
hi_s32 sample_comm_audio_destory_trd_ai(hi_audio_dev ai_dev, hi_ai_chn ai_chn);
hi_s32 sample_comm_audio_destory_trd_aenc_adec(hi_aenc_chn ae_chn);
hi_s32 sample_comm_audio_destory_trd_file_adec(hi_adec_chn ad_chn);
hi_s32 sample_comm_audio_destory_trd_ao_vol_ctrl(hi_audio_dev ao_dev);
hi_s32 sample_comm_audio_destory_all_trd(void);
hi_s32 sample_comm_audio_ao_bind_adec(hi_audio_dev ao_dev, hi_ao_chn ao_chn, hi_adec_chn ad_chn);
hi_s32 sample_comm_audio_ao_unbind_adec(hi_audio_dev ao_dev, hi_ao_chn ao_chn, hi_adec_chn ad_chn);
hi_s32 sample_comm_audio_ao_bind_ai(hi_audio_dev ai_dev, hi_ai_chn ai_chn, hi_audio_dev ao_dev, hi_ao_chn ao_chn);
hi_s32 sample_comm_audio_ao_unbind_ai(hi_audio_dev ai_dev, hi_ai_chn ai_chn, hi_audio_dev ao_dev, hi_ao_chn ao_chn);
hi_s32 sample_comm_audio_aenc_bind_ai(hi_audio_dev ai_dev, hi_ai_chn ai_chn, hi_aenc_chn ae_chn);
hi_s32 sample_comm_audio_aenc_unbind_ai(hi_audio_dev ai_dev, hi_ai_chn ai_chn, hi_aenc_chn ae_chn);
hi_s32 sample_comm_audio_start_ai(hi_audio_dev ai_dev_id, hi_u32 ai_chn_cnt, hi_aio_attr *aio_attr,
    const sample_comm_ai_vqe_param *ai_vqe_param, hi_audio_dev ao_dev_id);
hi_s32 sample_comm_audio_stop_ai(hi_audio_dev ai_dev_id, hi_u32 ai_chn_cnt, hi_bool resample_en, hi_bool vqe_en);
hi_s32 sample_comm_audio_start_ao(hi_audio_dev ao_dev_id, hi_u32 ao_chn_cnt,
                                  hi_aio_attr *aio_attr, hi_audio_sample_rate in_sample_rate, hi_bool resample_en);
hi_s32 sample_comm_audio_stop_ao(hi_audio_dev ao_dev_id, hi_u32 ao_chn_cnt, hi_bool resample_en);
hi_s32 sample_comm_audio_start_aenc(hi_u32 aenc_chn_cnt, hi_aio_attr *aio_attr, hi_payload_type type);
hi_s32 sample_comm_audio_stop_aenc(hi_u32 aenc_chn_cnt);
hi_s32 sample_comm_audio_start_adec(hi_u32 adec_chn_cnt, hi_payload_type type);
hi_s32 sample_comm_audio_stop_adec(hi_adec_chn ad_chn);
hi_s32 sample_comm_audio_cfg_acodec(hi_aio_attr *aio_attr);

hi_s32 sample_comm_vdec_init_vb_pool(hi_u32 chn_num, sample_vdec_attr *past_sample_vdec, hi_u32 arr_len);
hi_void sample_comm_vdec_exit_vb_pool(hi_void);
hi_void sample_comm_vdec_cmd_ctrl(hi_s32 chn_num, vdec_thread_param *vdec_send, pthread_t *vdec_thread,
    hi_u32 send_arr_len, hi_u32 thread_arr_len);
hi_void sample_comm_vdec_start_send_stream(hi_s32 chn_num, vdec_thread_param *vdec_send, pthread_t *vdec_thread,
    hi_u32 send_arr_len, hi_u32 thread_arr_len);
hi_void sample_comm_vdec_stop_send_stream(hi_s32 chn_num, vdec_thread_param *vdec_send, pthread_t *vdec_thread,
    hi_u32 send_arr_len, hi_u32 thread_arr_len);
hi_void* sample_comm_vdec_send_stream(hi_void *args);
hi_s32 sample_comm_vdec_start(hi_s32 chn_num, sample_vdec_attr *past_sample_vdec, hi_u32 arr_len);
hi_s32 sample_comm_vdec_stop(hi_s32 chn_num);
hi_bool sample_comm_vdec_get_progressive_en(hi_void);
hi_void sample_comm_vdec_set_progressive_en(hi_bool enable);

hi_void sample_common_vda_handle_sig(void);
hi_s32 sample_common_vda_md_start(hi_vda_chn vda_chn, hi_u32 chn, hi_size *size);
hi_void sample_common_vda_md_stop(hi_vda_chn vda_chn, hi_u32 chn);
hi_s32 sample_common_vda_od_start(hi_vda_chn vda_chn, hi_u32 chn, hi_size *size);
hi_void sample_common_vda_od_stop(hi_vda_chn vda_chn, hi_u32 chn);




//maohw // for sample_venc.c;
#define CHN_NUM_MAX 2
typedef struct {
    hi_size            max_size;
    hi_pixel_format    pixel_format;
    hi_size            output_size[HI_VPSS_MAX_PHYS_CHN_NUM];
    hi_compress_mode   compress_mode[HI_VPSS_MAX_PHYS_CHN_NUM];
    hi_bool            enable[HI_VPSS_MAX_PHYS_CHN_NUM];
} sample_venc_vpss_chn_attr;

typedef struct {
    hi_u32 valid_num;
    hi_u64 blk_size[HI_VB_MAX_COMMON_POOLS];
    hi_u32 blk_cnt[HI_VB_MAX_COMMON_POOLS];
    hi_u32 supplement_config;
} sample_venc_vb_attr;
#include "mpp.h"
////////////


#ifdef __cplusplus
}
#endif /* end of #ifdef __cplusplus */

#endif /* end of #ifndef __SAMPLE_COMMON_H__ */

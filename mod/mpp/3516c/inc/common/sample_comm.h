/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef SAMPLE_COMM_H
#define SAMPLE_COMM_H

#include <pthread.h>

#include "hi_common.h"
#include "hi_math.h"
#include "hi_buffer.h"
#include "hi_defines.h"
#include "securec.h"
#include "hi_mipi_rx.h"
#include "hi_common_sys.h"
#include "hi_common_vb.h"
#include "hi_common_isp.h"
#include "hi_common_vi.h"
#include "hi_common_venc.h"
#include "hi_common_vpss.h"
#include "hi_common_region.h"
#include "hi_common_adec.h"
#include "hi_common_aenc.h"
#include "hi_common_aio.h"
#include "hi_common_vgs.h"

#include "hi_mpi_sys.h"
#include "hi_mpi_sys_bind.h"
#include "hi_mpi_sys_mem.h"
#include "hi_mpi_vb.h"
#include "hi_mpi_vi.h"
#include "hi_mpi_isp.h"
#include "hi_mpi_venc.h"
#include "hi_mpi_vpss.h"
#include "hi_mpi_region.h"
#include "hi_mpi_audio.h"
#include "hi_mpi_vgs.h"
#ifdef __cplusplus
extern "C" {
#endif /* end of #ifdef __cplusplus */

/* macro define */
#define FILE_NAME_LEN 128
#define FILE_PATH_LEN 128

#define SAMPLE_PIXEL_FORMAT HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420

#define COLOR_RGB_RED      0xFF0000
#define COLOR_RGB_GREEN    0x00FF00
#define COLOR_RGB_BLUE     0x0000FF
#define COLOR_RGB_BLACK    0x000000
#define COLOR_RGB_YELLOW   0xFFFF00
#define COLOR_RGB_CYN      0x00ffff
#define COLOR_RGB_WHITE    0xffffff

#define SAMPLE_RGN_HANDLE_NUM_MAX 16
#define SAMPLE_RGN_HANDLE_NUM_MIN 1

#define SAMPLE_AUDIO_EXTERN_AI_DEV 0
#define SAMPLE_AUDIO_EXTERN_AO_DEV 0
#define SAMPLE_AUDIO_INNER_AI_DEV 0
#define SAMPLE_AUDIO_INNER_AO_DEV 0

#define SAMPLE_AUDIO_POINT_NUM_PER_FRAME 480
#define SAMPLE_AUDIO_AI_USER_FRAME_DEPTH 5

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

#define WIDTH_2688          2688
#define WIDTH_2592          2592
#define HEIGHT_1520         1520

#define WIDTH_2560          2560
#define HEIGHT_1440         1440

#define WIDTH_2880          2880
#define HEIGHT_1620         1620

#define WIDTH_2304          2304
#define HEIGHT_1296         1296

#define AD_NVP6158 0
#define AD_TP2856 1

#define SAMPLE_VIE_MAX_ROUTE_NUM 4
#define SAMPLE_VIE_POOL_NUM 3

#define SAMPLE_AD_TYPE AD_TP2856

#define TP2856_FILE "/dev/tp2802dev"
#define TP2828_FILE "/dev/tp2823dev"

#define ACODEC_FILE "/dev/acodec"

#define ES8388_FILE "/dev/es8388"
#define ES8388_CHIP_ID 0

#define SAMPLE_FRAME_BUF_RATIO_MAX 100
#define SAMPLE_FRAME_BUF_RATIO_MIN 70
#define SAMPLE_FRAME_BUF_RATIO_DEFAULT 75

#define minor_chn(vi_chn) ((vi_chn) + 1)

#define sample_pause() \
    do { \
        printf("---------------press enter key to exit!---------------\n"); \
        getchar(); \
    } while (0)

#define sample_print(fmt...) \
    do { \
        printf("[%s]-%d: ", __FUNCTION__, __LINE__); \
        printf(fmt); \
    } while (0)

#define sample_print_pause(fmt...) \
    do { \
        printf(fmt); \
        getchar(); \
    } while (0)

#define check_null_ptr_return(ptr) \
    do { \
        if ((ptr) == HI_NULL) { \
            printf("func:%s,line:%d, NULL pointer\n", __FUNCTION__, __LINE__); \
            return HI_FAILURE; \
        } \
    } while (0)

#define check_chn_return(express, chn, name) \
    do { \
        hi_s32 ret_ = (express); \
        if (ret_ != HI_SUCCESS) { \
            printf("\033[0;31m%s chn %d failed at %s: LINE: %d with %#x!\033[0;39m\n", \
                   (name), (chn), __FUNCTION__, __LINE__, ret_); \
            fflush(stdout); \
            return ret_; \
        } \
    } while (0)

#define check_return(express, name) \
    do { \
        hi_s32 ret_ = (express); \
        if (ret_ != HI_SUCCESS) { \
            printf("\033[0;31m%s failed at %s: LINE: %d with %#x!\033[0;39m\n", \
                   (name), __FUNCTION__, __LINE__, ret_); \
            return ret_; \
        } \
    } while (0)

#define sample_check_eok_return(ret, err_code) \
    do { \
        if ((ret) != EOK) { \
            printf("%s:%d:strncpy_s failed.\n", __FUNCTION__, __LINE__); \
            return (err_code); \
        } \
    } while (0)

#define rgn_check_handle_num_return(handle_num) \
    do { \
        if (((handle_num) < SAMPLE_RGN_HANDLE_NUM_MIN) || ((handle_num) > SAMPLE_RGN_HANDLE_NUM_MAX)) { \
            sample_print("handle_num(%d) should be in [%d, %d].\n", \
                (handle_num), SAMPLE_RGN_HANDLE_NUM_MIN, SAMPLE_RGN_HANDLE_NUM_MAX); \
            return HI_FAILURE; \
        } \
    } while (0)

#define check_digit(x) ((x) >= '0' && (x) <= '9')

/* structure define */
typedef enum {
    PIC_CIF,
    PIC_360P,    /* 640 * 360 */
    #define HAVE_PIC_512P
    PIC_512P,    /* 640 * 512 maohw */
    #define HAVE_PIC_640P
    PIC_640P,    /*640 * 640 maohw */
    PIC_D1_PAL,  /* 720 * 576 */
    PIC_D1_NTSC, /* 720 * 480 */
    PIC_960H,      /* 960 * 576 */
    PIC_720P,    /* 1280 * 720 */
    PIC_1080P,   /* 1920 * 1080 */
    PIC_480P,
    PIC_576P,
    PIC_800X600,
    PIC_1024X768,
    PIC_1280X1024,
    PIC_1366X768,
    PIC_1440X900,
    PIC_1280X800,
    PIC_1600X1200,
    PIC_1680X1050,
    PIC_1920X1200,
    PIC_640X480,
    PIC_1920X2160,
    PIC_2304X1296,
    PIC_2560X1440,
    PIC_2560X1600,
    PIC_2592X1520,
    PIC_2688X1520,
    PIC_2592X1944,
    PIC_3840X2160,
    PIC_4096X2160,
    PIC_3000X3000,
    PIC_4000X3000,
    PIC_6080X2800,
    PIC_7680X4320,
    PIC_3840X8640,
    PIC_2880X1620,
    PIC_BUTT
} hi_pic_size;

//maohw;
enum {
    PIC_800x600   = PIC_800X600,
    PIC_1024x768  = PIC_1024X768,
    PIC_1280x1024 = PIC_1280X1024,
    PIC_1366x768  = PIC_1366X768,
    PIC_1440x900  = PIC_1440X900,
    PIC_1280x800  = PIC_1280X800,
    PIC_1600x1200 = PIC_1600X1200,
    PIC_1680x1050 = PIC_1680X1050,
    PIC_1920x1200 = PIC_1920X1200,
    PIC_640x480   = PIC_640X480,
    PIC_1920x2160 = PIC_1920X2160,
    PIC_2560x1440 = PIC_2560X1440,
    PIC_2560x1600 = PIC_2560X1600,
    PIC_2592x1520 = PIC_2592X1520,
    PIC_2592x1944 = PIC_2592X1944,
	  PIC_2688x1520 = PIC_2688X1520,
    PIC_3840x2160 = PIC_3840X2160,
    PIC_4096x2160 = PIC_4096X2160,
    PIC_3000x3000 = PIC_3000X3000,
    PIC_4000x3000 = PIC_4000X3000,
    PIC_6080x2800 = PIC_6080X2800,
    PIC_7680x4320 = PIC_7680X4320,
    PIC_3840x8640 = PIC_3840X8640,
    PIC_2880x1620 = PIC_2880X1620,
};


typedef enum {
    SC4336P_MIPI_4M_30FPS_10BIT,
    OS04D10_MIPI_4M_30FPS_10BIT,
    GC4023_MIPI_4M_30FPS_10BIT,
    SC431HAI_MIPI_4M_30FPS_10BIT,
    SC431HAI_MIPI_4M_30FPS_10BIT_WDR2TO1,
    SC450AI_MIPI_4M_30FPS_10BIT,
    SC450AI_MIPI_4M_30FPS_10BIT_WDR2TO1,
    SC500AI_MIPI_5M_30FPS_10BIT,
    SC500AI_MIPI_5M_30FPS_10BIT_WDR2TO1,
    SC4336P_MIPI_3M_30FPS_10BIT,
    OV_OS04A10_MIPI_4M_30FPS_12BIT,
	  OV_OS04A10_MIPI_4M_30FPS_12BIT_WDR2TO1,
    OV_OS04A10_SLAVE_MIPI_4M_30FPS_12BIT,
    SNS_TYPE_BUTT,
} sample_sns_type;

typedef enum {
    DMEB_QFN,
    DMEB_BGA,
    BOARD_TYPE_BUTT,
} sample_board_type;

typedef struct {
    hi_size          vb_size[SAMPLE_VIE_POOL_NUM];
    hi_pixel_format  pixel_format[SAMPLE_VIE_POOL_NUM];
    hi_compress_mode compress_mode[SAMPLE_VIE_POOL_NUM];
    hi_video_format  video_format[SAMPLE_VIE_POOL_NUM];
    hi_s32           blk_num[SAMPLE_VIE_POOL_NUM];
} sample_vb_param;

typedef struct {
    sample_sns_type sns_type;
    hi_u32          sns_clk_src;
    hi_u32          sns_rst_src;
    hi_u32          bus_id;
    hi_bool         sns_clk_rst_en;
} sample_sns_info;

typedef struct {
    hi_s32             mipi_dev;
    lane_divide_mode_t divide_mode;
    combo_dev_attr_t   combo_dev_attr;
    ext_data_type_t    ext_data_type_attr;
} sample_mipi_info;

typedef struct {
    hi_vi_dev      vi_dev;
    hi_vi_dev_attr dev_attr;
    hi_vi_bas_attr bas_attr;
} sample_vi_dev_info;

typedef struct {
    hi_isp_pub_attr isp_pub_attr;
} sample_isp_info;

typedef struct {
    hi_u32                    grp_num;
    hi_vi_grp                 fusion_grp[HI_VI_MAX_WDR_FUSION_GRP_NUM];
    hi_vi_wdr_fusion_grp_attr fusion_grp_attr[HI_VI_MAX_WDR_FUSION_GRP_NUM];
} sample_vi_grp_info;

typedef struct {
    hi_vi_chn      vi_chn;
    hi_vi_chn_attr chn_attr;
    hi_fmu_mode    fmu_mode;
} sample_vi_chn_info;

typedef struct {
    hi_vi_pipe_attr    pipe_attr;

    hi_bool            pipe_need_start;
    hi_bool            isp_need_run;
    sample_isp_info    isp_info;
    hi_bool            isp_be_end_trigger;
    hi_bool            isp_quick_start;

    hi_u32             chn_num;
    sample_vi_chn_info chn_info[HI_VI_MAX_PHYS_CHN_NUM];
    hi_3dnr_attr       nr_attr;
    hi_bool            vc_change_en;
    hi_u32             vc_number;
    hi_bool            is_master_pipe;
    hi_u32             bnr_bnf_num;
    hi_vi_pipe_buf_wrap_attr wrap_attr;
    hi_u32             pixel_rate;
} sample_vi_pipe_info;

typedef struct {
    sample_sns_info     sns_info;
    sample_mipi_info    mipi_info;
    sample_vi_dev_info  dev_info;
    hi_vi_bind_pipe     bind_pipe;
    sample_vi_grp_info  grp_info;
    sample_vi_pipe_info pipe_info[HI_VI_MAX_PHYS_PIPE_NUM];
} sample_vi_cfg;

typedef struct {
    hi_vb_blk           vb_blk;
    hi_u32              blk_size;
    hi_video_frame_info frame_info;
} sample_vi_user_frame_info;

typedef struct {
    hi_size          size;
    hi_pixel_format  pixel_format;
    hi_video_format  video_format;
    hi_compress_mode compress_mode;
    hi_dynamic_range dynamic_range;
} sample_vi_get_frame_vb_cfg;

typedef struct {
    hi_u32           threshold;
    hi_u32           frame_num;
    hi_isp_fpn_type  fpn_type;
    hi_pixel_format  pixel_format;
    hi_compress_mode compress_mode;
} sample_vi_fpn_calibration_cfg;

typedef struct {
    hi_op_mode                op_mode;
    hi_bool                   aibnr_mode;
    hi_isp_fpn_type           fpn_type;
    hi_u32                    strength;
    hi_pixel_format           pixel_format;
    hi_compress_mode          compress_mode;
    sample_vi_user_frame_info user_frame_info;
} sample_vi_fpn_correction_cfg;

typedef struct {
    hi_u32      offset;
} sample_scene_fpn_offset_cfg;

typedef struct {
    hi_u32           pipe_num;
    hi_vi_vpss_mode_type mode_type;
    hi_vi_aiisp_mode aiisp_mode;
    hi_size          in_size;
    hi_vb_cfg        vb_cfg;
    hi_u32           supplement_cfg;
    hi_3dnr_pos_type  nr_pos;
    hi_vi_pipe       vi_pipe;
    hi_vi_chn        vi_chn;
    sample_vi_cfg    vi_cfg;
    hi_vpss_grp_attr grp_attr;
    hi_bool          chn_en[HI_VPSS_MAX_PHYS_CHN_NUM];
    hi_vpss_chn_attr chn_attr[HI_VPSS_MAX_PHYS_CHN_NUM];
    hi_vpss_grp      vpss_grp[SAMPLE_VIE_MAX_ROUTE_NUM];
    hi_fmu_mode      vi_fmu[SAMPLE_VIE_MAX_ROUTE_NUM];
    hi_fmu_mode      vpss_fmu[SAMPLE_VIE_MAX_ROUTE_NUM];
    hi_bool          is_direct[SAMPLE_VIE_MAX_ROUTE_NUM];
} sample_comm_cfg;

typedef struct {
    hi_s32               route_num;
    hi_vi_vpss_mode_type mode_type;
    hi_fmu_mode          vi_fmu[SAMPLE_VIE_MAX_ROUTE_NUM];
    hi_fmu_mode          vpss_fmu[SAMPLE_VIE_MAX_ROUTE_NUM];
    td_bool              vpss_wrap_en;
    hi_u32               vpss_wrap_size;
} sampe_sys_cfg;

typedef struct {
    hi_vpss_grp                 vpss_grp;
    hi_vpss_grp_attr            grp_attr;
    hi_3dnr_attr                nr_attr;
    hi_bool                     chn_en[HI_VPSS_MAX_PHYS_CHN_NUM];
    hi_vpss_chn_attr            chn_attr[HI_VPSS_MAX_PHYS_CHN_NUM];
    hi_vpss_chn_buf_wrap_attr   wrap_attr[HI_VPSS_MAX_PHYS_CHN_NUM];
} sample_vpss_cfg;

typedef enum {
    VI_USER_PIC_FRAME = 0,
    VI_USER_PIC_BGCOLOR,
} sample_vi_user_pic_type;

typedef enum {
    SAMPLE_RC_ABR = 0,
    SAMPLE_RC_CBR,
    SAMPLE_RC_VBR,
    SAMPLE_RC_AVBR,
    SAMPLE_RC_CVBR,
    SAMPLE_RC_QVBR,
    SAMPLE_RC_QPMAP,
    SAMPLE_RC_FIXQP
} sample_rc;

typedef struct {
    volatile hi_bool thread_start;
    hi_venc_chn venc_chn[HI_VENC_MAX_CHN_NUM];
    hi_s32 cnt;
    //maohw add callback;
    void *uargs;
    int (*cb)(hi_venc_chn chn, hi_payload_type pt, hi_venc_stream* stream, void* uargs);
} sample_venc_getstream_para;

typedef struct {
    hi_bool thread_start;
    hi_venc_chn venc_chn[HI_VENC_MAX_CHN_NUM];
    hi_u32  cnt;
    hi_vpss_grp vpss_grp;
    hi_vpss_chn      vpss_chn[HI_VENC_MAX_CHN_NUM];
} sample_venc_rateauto_para;

typedef struct {
    hi_bool  thread_start;
    hi_vpss_grp vpss_grp;
    hi_venc_chn venc_chn[HI_VENC_MAX_CHN_NUM];
    hi_vpss_chn vpss_chn[HI_VPSS_MAX_PHYS_CHN_NUM];
    hi_size size[HI_VENC_MAX_CHN_NUM];
    hi_u32 cnt;
} sample_venc_qpmap_sendframe_para;

typedef struct {
    hi_bool thread_start;
    hi_vpss_grp vpss_grp;
    hi_venc_chn venc_chn[HI_VENC_MAX_CHN_NUM];
    hi_vpss_chn vpss_chn[HI_VPSS_MAX_PHYS_CHN_NUM];
    hi_size size[HI_VENC_MAX_CHN_NUM];
    hi_venc_jpeg_roi_attr roi_attr[HI_VENC_MAX_CHN_NUM];
    hi_u32 cnt;
} sample_venc_roimap_frame_para;

#define REGION_OP_CHN               (0x01L << 0)
#define REGION_OP_DEV               (0x01L << 1)
#define REGION_DESTROY                  (0x01L << 2)
typedef hi_u32 region_op_flag;

typedef enum {
    THREAD_CTRL_START,
    THREAD_CTRL_PAUSE,
    THREAD_CTRL_STOP,
} thread_contrl;

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
    hi_u32 bitrate;//maohw;
} sample_comm_venc_chn_param;

typedef struct {
    hi_vpss_chn *vpss_chn;
    hi_venc_chn *venc_chn;
    hi_s32 cnt;
} sample_venc_roimap_chn_info;

typedef enum {
    SAMPLE_AUDIO_VQE_TYPE_NONE = 0,
    SAMPLE_AUDIO_VQE_TYPE_RECORD,
    SAMPLE_AUDIO_VQE_TYPE_TALK,
    SAMPLE_AUDIO_VQE_TYPE_TALKV2,
    SAMPLE_AUDIO_VQE_TYPE_MAX,
} sample_audio_vqe_type;

typedef struct {
    hi_audio_sample_rate out_sample_rate;
    hi_bool resample_en;
    hi_void *ai_vqe_attr;
    sample_audio_vqe_type ai_vqe_type;
} sample_comm_ai_vqe_param;

typedef struct {
    hi_wdr_mode wdr_mode;                         /* RW; WDR mode. */
    hi_u32     pipe_num;                         /* RW; Range [1,OT_VI_MAX_PHYS_PIPE_NUM] */
    hi_vi_pipe pipe_id[OT_VI_MAX_WDR_FRAME_NUM]; /* RW; Array of pipe ID */
} sample_run_be_bind_pipe;

typedef struct {
    hi_vpss_chn_attr chn_attr[HI_VPSS_MAX_PHYS_CHN_NUM];
    hi_bool chn_enable[HI_VPSS_MAX_PHYS_CHN_NUM];
    hi_vpss_chn_buf_wrap_attr wrap_attr[HI_VPSS_MAX_PHYS_CHN_NUM];
    hi_u32 chn_array_size;
} sample_vpss_chn_attr;

typedef struct {
    hi_point point1;
    hi_point point2;
    hi_u32 thick;
    hi_u32 color;
    hi_bool is_display;
} sample_osd_line;

typedef struct {
    hi_pixel_format pixel_format;
    hi_u8 *data;
    hi_size size;
    sample_osd_line *line;
    hi_u32 stride; /* get canvas interface need */
    hi_bool is_set_bmp; /* use set_bmp or get_canvas interface */
} sample_drawline_param;

typedef struct {
    hi_s32 start_x;
    hi_s32 start_y;
    hi_s32 end_x;
    hi_s32 end_y;
} sample_line;

/* function announce */
#ifndef __LITEOS__
hi_void sample_sys_signal(void (*func)(int));
#endif
hi_void *sample_sys_io_mmap(hi_u64 phy_addr, hi_u32 size);
hi_s32 sample_sys_munmap(hi_void *vir_addr, hi_u32 size);
hi_s32 sample_sys_set_reg(hi_u64 addr, hi_u32 value);
hi_s32 sample_sys_get_reg(hi_u64 addr, hi_u32 *value);

hi_void sample_comm_sys_get_default_vb_cfg(sample_vb_param *input_cfg, hi_vb_cfg *vb_cfg);
hi_void sample_comm_sys_get_default_cfg(hi_u32 pipe_num, sample_comm_cfg *comm_cfg);
hi_s32 sample_comm_sys_get_pic_size(hi_pic_size pic_size, hi_size *size);
hi_pic_size sample_comm_sys_get_pic_enum(const hi_size *size);
hi_s32 sample_comm_sys_mem_config(hi_void);
hi_s32 sample_comm_sys_init(sample_comm_cfg *comm_cfg);
hi_void sample_comm_sys_exit(hi_void);
hi_s32 sample_comm_sys_vb_init(const hi_vb_cfg *vb_cfg);
hi_s32 sample_comm_sys_init_with_vb_supplement(const hi_vb_cfg *vb_cfg, hi_u32 supplement_config);

hi_s32 sample_comm_vi_set_mipi_hs_mode(lane_divide_mode_t hs_mode);
hi_s32 sample_comm_vi_mipi_ctrl_cmd(hi_u32 devno, hi_u32 cmd);
hi_s32 sample_comm_vi_bind_vpss(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn);
hi_s32 sample_comm_vi_un_bind_vpss(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn);
hi_s32 sample_comm_vi_bind_venc(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_venc_chn venc_chn);
hi_s32 sample_comm_vi_un_bind_venc(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_venc_chn venc_chn);

hi_s32 sample_comm_vi_switch_isp_mode(const sample_vi_cfg *vi_cfg);
hi_s32 sample_comm_vi_switch_isp_resolution(const sample_vi_cfg *vi_cfg, const hi_size *size);
hi_void sample_comm_vi_mode_switch_stop_vi(const sample_vi_cfg *vi_cfg);
hi_s32 sample_comm_vi_mode_switch_start_vi(const sample_vi_cfg *vi_cfg, hi_bool chg_resolution, const hi_size *size);

hi_s32 sample_comm_vpss_bind_venc(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn, hi_venc_chn venc_chn);
hi_s32 sample_comm_vpss_un_bind_venc(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn, hi_venc_chn venc_chn);

hi_s32 sample_comm_isp_sensor_regiter_callback(hi_isp_dev isp_dev, sample_sns_type sns_type);
hi_s32 sample_comm_isp_sensor_unregiter_callback(hi_isp_dev isp_dev);
hi_s32 sample_comm_isp_bind_sns(hi_isp_dev isp_dev, sample_sns_type sns_type, hi_s8 sns_dev);
hi_s32 sample_comm_isp_thermo_lib_callback(hi_isp_dev isp_dev);
hi_s32 sample_comm_isp_thermo_lib_uncallback(hi_isp_dev isp_dev);
hi_s32 sample_comm_isp_ae_lib_callback(hi_isp_dev isp_dev);
hi_s32 sample_comm_isp_ae_lib_uncallback(hi_isp_dev isp_dev);
hi_s32 sample_comm_isp_awb_lib_callback(hi_isp_dev isp_dev);
hi_s32 sample_comm_isp_awb_lib_uncallback(hi_isp_dev isp_dev);
hi_s32 sample_comm_isp_run(hi_isp_dev isp_dev);
hi_void sample_comm_isp_stop(hi_isp_dev isp_dev);
hi_void sample_comm_all_isp_stop(hi_void);
hi_s32 sample_comm_isp_get_pub_attr_by_sns(sample_sns_type sns_type, hi_isp_pub_attr *pub_attr);
hi_isp_sns_type sample_comm_get_sns_bus_type(sample_sns_type sns_type);
hi_void sample_comm_vi_get_size_by_sns_type(sample_sns_type sns_type, hi_size *size);
hi_void sample_comm_vi_get_default_vi_cfg(sample_sns_type sns_type, sample_vi_cfg *vi_cfg);
hi_void sample_comm_vi_get_vi_cfg_by_fmu_mode(sample_sns_type sns_type, hi_fmu_mode fum_mode, sample_vi_cfg *vi_cfg);
hi_void sample_comm_vi_init_vi_cfg(sample_sns_type sns_type, hi_size *size, sample_vi_cfg *vi_cfg);
hi_s32 sample_comm_vi_set_vi_vpss_mode(hi_vi_vpss_mode_type mode_type, hi_vi_aiisp_mode aiisp_mode);
hi_s32 sample_comm_vi_get_isp_run_state(hi_bool *isp_states, hi_u32 size);
hi_s32 sample_comm_vi_start_sensor(const sample_sns_info *sns_info, const sample_mipi_info *mipi_info);
hi_void sample_comm_vi_stop_mipi_rx(const sample_sns_info *sns_info, const sample_mipi_info *mipi_info);
hi_s32 sample_comm_vi_start_isp(const sample_vi_cfg *vi_cfg);
hi_void sample_comm_vi_stop_isp(const sample_vi_cfg *vi_cfg);
hi_s32 sample_comm_vi_start_vi(const sample_vi_cfg *vi_cfg);
hi_void sample_comm_vi_stop_vi(const sample_vi_cfg *vi_cfg);
hi_void sample_comm_vi_stop_four_vi(const sample_vi_cfg *vi_cfg, hi_s32 route_num);
hi_s32 sample_comm_vi_fpn_calibrate(hi_vi_pipe vi_pipe, sample_vi_fpn_calibration_cfg *calibration_cfg);
hi_s32 sample_comm_vi_enable_fpn_correction(hi_vi_pipe vi_pipe, sample_vi_fpn_correction_cfg *correction_cfg);
hi_s32 sample_comm_vi_disable_fpn_correction(hi_vi_pipe vi_pipe, sample_vi_fpn_correction_cfg *correction_cfg);

hi_s32 sample_comm_vi_fpn_calibrate_for_thermo(hi_vi_pipe vi_pipe, sample_vi_fpn_calibration_cfg *calibration_cfg);
hi_s32 sample_comm_vi_enable_fpn_correction_for_thermo(hi_vi_pipe vi_pipe,
    sample_vi_fpn_correction_cfg *correction_cfg);
hi_s32 sample_comm_vi_disable_fpn_correction_for_thermo(hi_vi_pipe vi_pipe,
    sample_vi_fpn_correction_cfg *correction_cfg);

hi_s32 sample_comm_vi_enable_fpn_correction_for_scene(hi_vi_pipe vi_pipe, sample_vi_fpn_correction_cfg *correction_cfg,
    hi_u32 iso, sample_scene_fpn_offset_cfg *scene_fpn_offset_cfg, const hi_char *dir_name);

hi_s32 sample_comm_vi_start_virt_pipe(const sample_vi_cfg *vi_cfg);
hi_void sample_comm_vi_stop_virt_pipe(const sample_vi_cfg *vi_cfg);
hi_s32 sample_common_vi_load_user_pic(hi_vi_pipe vi_pipe, sample_vi_user_pic_type user_pic_type,
    sample_vi_user_frame_info *user_frame_info);
hi_void sample_common_vi_unload_user_pic(sample_vi_user_frame_info *user_frame_info);

hi_void sample_comm_vpss_get_default_grp_attr(hi_vpss_grp_attr *grp_attr);
hi_void sample_comm_vpss_get_default_3dnr_attr(hi_3dnr_attr *nr_attr);
hi_void sample_comm_vpss_get_default_chn_attr(hi_vpss_chn_attr *chn_attr);
hi_s32 sample_comm_vpss_get_wrap_cfg(hi_vpss_chn_attr chn_attr[HI_VPSS_MAX_PHYS_CHN_NUM],
    hi_vi_vpss_mode_type mode, hi_u32 full_lines_std, hi_vpss_chn_buf_wrap_attr *wrap_attr);
hi_void sample_comm_vpss_get_default_vpss_cfg(sample_vpss_cfg *vpss_cfg, td_bool vpss_wrap_en);
hi_s32 sample_common_vpss_start(hi_vpss_grp grp, const hi_vpss_grp_attr *grp_attr,
    const sample_vpss_chn_attr *vpss_chn_attr);
hi_s32 sample_common_vpss_stop(hi_vpss_grp grp, const hi_bool *chn_enable, hi_u32 chn_array_size);

hi_s32 sample_comm_venc_mem_config(hi_void);
hi_s32 sample_comm_venc_create(hi_venc_chn venc_chn, sample_comm_venc_chn_param *chn_param);
hi_s32 sample_comm_venc_start(hi_venc_chn venc_chn, sample_comm_venc_chn_param *chn_param);
hi_s32 sample_comm_venc_stop(hi_venc_chn venc_chn);
hi_s32 sample_comm_venc_snap_start(hi_venc_chn venc_chn, hi_size *size, hi_bool support_dcf);
hi_s32 sample_comm_venc_photo_start(hi_venc_chn venc_chn, hi_size *size, hi_bool support_dcf);
hi_s32 sample_comm_venc_snap_process(hi_venc_chn venc_chn, hi_u32 snap_cnt, hi_bool save_jpg, hi_bool save_thm);

//maohw
hi_s32 sample_comm_venc_snap_processCB(hi_venc_chn venc_chn, hi_u32 snap_cnt, int(*cb)(int i, hi_venc_stream* stream, void* u), void* u);

hi_s32 sample_comm_venc_save_jpeg(hi_venc_chn venc_chn, hi_u32 snap_cnt);
hi_s32 sample_comm_venc_snap_stop(hi_venc_chn venc_chn);
hi_s32 sample_comm_venc_start_get_stream(hi_venc_chn ve_chn[], hi_s32 cnt);

//maohw
hi_s32 sample_comm_venc_start_get_streamCB(hi_venc_chn *venc_chn, hi_s32 cnt, int (*cb)(hi_venc_chn chn, hi_payload_type pt, hi_venc_stream* stream, void* uargs), void *uargs);


hi_s32 sample_comm_venc_stop_get_stream(hi_s32 chn_num);
hi_s32 sample_comm_venc_start_get_stream_svc_t(hi_s32 cnt);
hi_s32 sample_comm_venc_get_gop_attr(hi_venc_gop_mode gop_mode, hi_venc_gop_attr *gop_attr);
hi_s32 sample_comm_venc_qpmap_send_frame(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn[],
                                         hi_venc_chn venc_chn[], hi_s32 cnt, hi_size size[]);
hi_s32 sample_comm_venc_stop_send_qpmap_frame(hi_void);
hi_s32 sample_comm_venc_rateauto_start(hi_venc_chn ve_chn[], hi_s32 cnt, hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn[]);
hi_s32 sample_comm_venc_stop_rateauto(hi_venc_chn ve_chn[], hi_s32 cnt);

hi_s32 sample_comm_venc_get_file_postfix(hi_payload_type payload, hi_char *file_postfix, hi_u8 len);
hi_s32 sample_comm_venc_send_roimap_frame(hi_vpss_grp vpss_grp, sample_venc_roimap_chn_info roimap_chn_info,
    hi_size size[], hi_venc_jpeg_roi_attr roi_attr[]);
hi_s32 sample_comm_venc_stop_send_roimap_frame(hi_void);
hi_s32 sample_comm_venc_save_stream(FILE *fd, hi_venc_stream *stream);
hi_s32 sample_comm_venc_mosaic_map_send_frame(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn[], hi_venc_chn venc_chn[],
    hi_s32 cnt, hi_size size[]);
hi_s32 sample_comm_venc_stop_send_frame(hi_void);
hi_s32 sample_comm_venc_mini_buf_en(sample_comm_venc_chn_param *chn_param, hi_s32 venc_chn_num);

hi_s32 sample_comm_region_create(hi_s32 handle_num, hi_rgn_type type);
hi_s32 sample_comm_region_destroy(hi_s32 handle_num, hi_rgn_type type);
hi_s32 sample_comm_region_attach(hi_s32 handle_num, hi_rgn_type type, hi_mpp_chn *mpp_chn,
    region_op_flag op_flag);
hi_s32 sample_comm_check_min(hi_s32 min_handle);
hi_s32 sample_comm_region_detach(hi_s32 handle_num, hi_rgn_type type, hi_mpp_chn *mpp_chn,
    region_op_flag op_flag);
hi_s32 sample_comm_region_set_bit_map(hi_rgn_handle handle, const hi_char *bmp_path);
hi_s32 sample_comm_region_get_up_canvas(hi_rgn_handle handle, const hi_char *bmp_path);
hi_s32 sample_comm_region_get_min_handle(hi_rgn_type type);

hi_s32 sample_comm_audio_init(hi_void);
hi_void sample_comm_audio_exit(hi_void);
hi_s32 sample_comm_audio_create_thread_ai_ao(hi_audio_dev ai_dev, hi_ai_chn ai_chn,
    hi_audio_dev ao_dev, hi_ao_chn ao_chn);
hi_s32 sample_comm_audio_create_thread_ai_aenc(hi_audio_dev ai_dev, hi_ai_chn ai_chn, hi_aenc_chn ae_chn);
hi_s32 sample_comm_audio_create_thread_aenc_adec(hi_aenc_chn ae_chn, hi_adec_chn ad_chn, FILE *aenc_fd);
hi_s32 sample_comm_audio_create_thread_file_adec(hi_adec_chn ad_chn, FILE *adec_fd);
hi_s32 sample_comm_audio_create_thread_ao_vol_ctrl(hi_audio_dev ao_dev);
hi_s32 sample_comm_audio_destroy_thread_ai(hi_audio_dev ai_dev, hi_ai_chn ai_chn);
hi_s32 sample_comm_audio_destroy_thread_aenc_adec(hi_aenc_chn ae_chn);
hi_s32 sample_comm_audio_destroy_thread_file_adec(hi_adec_chn ad_chn);
hi_s32 sample_comm_audio_destroy_thread_ao_vol_ctrl(hi_audio_dev ao_dev);
hi_s32 sample_comm_audio_destroy_all_thread(hi_void);
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
hi_s32 sample_comm_audio_start_aenc(hi_u32 aenc_chn_cnt, const hi_aio_attr *aio_attr, hi_payload_type type);
hi_s32 sample_comm_audio_stop_aenc(hi_u32 aenc_chn_cnt);
hi_s32 sample_comm_audio_start_adec(hi_u32 adec_chn_cnt, const hi_aio_attr *aio_attr, hi_payload_type type);
hi_s32 sample_comm_audio_stop_adec(hi_adec_chn ad_chn);
hi_s32 sample_comm_audio_cfg_acodec(const hi_aio_attr *aio_attr);

hi_void sample_comm_vi_get_default_sns_info(sample_sns_type sns_type, sample_sns_info *sns_info);
hi_void sample_comm_vi_get_default_pipe_info(sample_sns_type sns_type, hi_vi_bind_pipe *bind_pipe,
                                             sample_vi_pipe_info pipe_info[]);

hi_void sample_comm_vi_get_default_mipi_info(sample_sns_type sns_type, sample_mipi_info *mipi_info);
hi_void sample_comm_vi_get_default_dev_info(sample_sns_type sns_type, sample_vi_dev_info *dev_info);
hi_void sample_comm_vi_get_mipi_info_by_dev_id(sample_sns_type sns_type, hi_vi_dev vi_dev,
                                               sample_mipi_info *mipi_info);

hi_s32 sample_comm_vi_send_run_be_frame(sample_run_be_bind_pipe *bind_pipe);

hi_u32 sample_comm_vi_get_raw_stride(hi_pixel_format pixel_format, hi_u32 width, hi_u32 byte_align, hi_u32 align);
hi_s32 sample_comm_vi_read_raw_frame(hi_char *frame_file,
                                     sample_vi_user_frame_info user_frame_info[], hi_u32 frame_cnt);

hi_void sample_comm_set_osd_drawline(hi_bool osd_drawline);
hi_s32 sample_comm_osd_drawline(sample_drawline_param *drawline_param);
hi_s32 sample_comm_region_drawline_set_bit_map(hi_rgn_handle handle, sample_osd_line *line, hi_u32 line_num);
hi_s32 sample_comm_region_drawline_get_canvas(hi_rgn_handle handle, sample_osd_line *line, hi_u32 line_num);


//// maohw ////

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


#include "mpp.h"

///////////////

#ifdef __cplusplus
}
#endif /* end of #ifdef __cplusplus */

#endif /* end of #ifndef SAMPLE_COMMON_H */

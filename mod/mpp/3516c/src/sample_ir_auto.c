/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#include <sys/time.h>
#include <sys/prctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "sample_comm.h"
#include "hi_mpi_isp.h"
#include "hi_common_isp.h"
#include "hi_mpi_ae.h"
#include "hi_mpi_awb.h"
#include "hi_mpi_sys.h"

#ifdef __LITEOS__
#include <asm/io.h>
#endif

#include <fcntl.h>
#include <sys/mman.h>
#include "ir_auto_prev.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */
static volatile sig_atomic_t g_sig_flag = 0;

typedef struct {
    hi_bool thread_flag;
    pthread_t thread;
}hi_ir_auto_thread;
static hi_ir_auto_thread g_ast_ir_thread[HI_ISP_MAX_PIPE_NUM] = {{0}};

static hi_isp_ir_auto_attr g_ast_ir_attr[HI_ISP_MAX_PIPE_NUM] = {{0}};
static hi_vi_pipe g_vi_pipe = 0;

//maohw
static pthread_mutex_t g_ir_mutex = PTHREAD_MUTEX_INITIALIZER;
static gsf_mpp_ir_t g_ir_switch = {0,};
#define SAMPLE_IR_CALIBRATION_MODE  (0)
#define SAMPLE_IR_AUTO_MODE         (1)
#define GAIN_MAX_COEF               (280)
#define GAIN_MIN_COEF               (190)
#define SHIFT_8BIT                  (8)
#define SAMPLE_IR_AUTO_RETURN            (2)

static sampe_sys_cfg g_ir_sys_cfg = {
    .route_num = 1,
    .mode_type = HI_VI_ONLINE_VPSS_ONLINE,
    .vpss_wrap_en = HI_TRUE,
    .vpss_wrap_size = 3200 * 128 *1.5,
};

/* default sc4336p x 2 */
static hi_vi_wrap_in_param g_vi_wrap_param = {
    .is_slave = HI_FALSE,
    .pipe_num = 2,
    .wrap_param[0] = {
        .pipe_size = {2560, 1440},
        .frame_rate_100x = 3000,
        .full_lines_std = 1500,
        .slave_param = {
            .vi_vpss_online = HI_TRUE,
            .vpss_chn0_size = {2560, 1440},
            .vpss_venc_wrap = HI_TRUE,
            .large_stream_size = {2560, 1440},
            .small_stream_size = {720, 480},
        }
    },
    .wrap_param[1] = {
        .pipe_size = {2560, 1440},
        .frame_rate_100x = 3000,
        .full_lines_std = 1500,
        .slave_param.vi_vpss_online = HI_TRUE,
        .slave_param.vpss_chn0_size = {2560, 1440},
        .slave_param.vpss_venc_wrap = HI_TRUE,
        .slave_param.large_stream_size = {2560, 1440},
        .slave_param.small_stream_size = {720, 480},
    }
};

static hi_s32 sample_ir_auto_stop_vi(sample_vi_cfg *vi_config)
{
    sample_comm_vi_stop_vi(vi_config);

    return HI_SUCCESS;
}

hi_s32 sample_ir_auto_start_vi(sample_vi_cfg *vi_config)
{
    hi_s32  ret;

    ret = sample_comm_vi_start_vi(vi_config);
    if (HI_SUCCESS != ret) {
        sample_print("start vi failed!\n");
        return ret;
    }

    return ret;
}

static hi_void sample_ir_auto_get_default_vb_config(hi_size *size, hi_vb_cfg *vb_cfg)
{
    hi_vb_calc_cfg calc_cfg;
    hi_pic_buf_attr buf_attr;

    (hi_void)memset_s(vb_cfg, sizeof(hi_vb_cfg), 0, sizeof(hi_vb_cfg));
    vb_cfg->max_pool_cnt = 128; /* 128 pool limit */

    buf_attr.width         = size->width;
    buf_attr.height        = size->height;
    buf_attr.align         = HI_DEFAULT_ALIGN;
    buf_attr.bit_width     = HI_DATA_BIT_WIDTH_8;
    buf_attr.pixel_format  = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    buf_attr.compress_mode = HI_COMPRESS_MODE_LINE;
    buf_attr.video_format = HI_VIDEO_FORMAT_LINEAR;
    hi_common_get_pic_buf_cfg(&buf_attr, &calc_cfg);

    vb_cfg->common_pool[0].blk_size = calc_cfg.vb_size;
    vb_cfg->common_pool[0].blk_cnt  = 1; /* 2 pool cnt */
}

static hi_s32 sample_ir_auto_sys_init(hi_vi_vpss_mode_type mode_type, hi_vi_aiisp_mode aiisp_mode)
{
    hi_s32 ret;
    hi_size size;
    hi_vb_cfg vb_cfg;
    hi_u32 supplement_config;

    sample_comm_vi_get_size_by_sns_type(SENSOR0_TYPE, &size);
    sample_ir_auto_get_default_vb_config(&size, &vb_cfg);

    supplement_config = HI_VB_SUPPLEMENT_BNR_MOT_MASK;
    ret = sample_comm_sys_init_with_vb_supplement(&vb_cfg, supplement_config);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    ret = sample_comm_vi_set_vi_vpss_mode(mode_type, aiisp_mode);
    if (ret != HI_SUCCESS) {
        sample_comm_sys_exit();

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void sample_ir_auto_get_default_venc_param(sample_sns_type sns_type, sample_comm_venc_chn_param *venc_param)
{
    hi_size size;

    sample_comm_vi_get_size_by_sns_type(sns_type, &size);

    venc_param->frame_rate = 30;    // 30: frame rate
    venc_param->stats_time = 2;     // 2: default stats_time
    venc_param->gop = 60;           // 60: default gop
    venc_param->venc_size.width = size.width;
    venc_param->venc_size.height = size.height;
    venc_param->size = -1;
    venc_param->profile = 0;
    venc_param->is_rcn_ref_share_buf = HI_FALSE;
    venc_param->gop_attr.gop_mode = HI_VENC_GOP_MODE_NORMAL_P;
    venc_param->gop_attr.normal_p.ip_qp_delta = 2;  // 2: default delta
    venc_param->type = HI_PT_H265;
    venc_param->rc_mode = SAMPLE_RC_CBR;
}

static hi_s32 sample_ir_auto_vpss_set_wrap_grp_int_attr(hi_vi_vpss_mode_type mode_type, hi_bool wrap_en,
    hi_u32 max_height)
{
    hi_s32 ret;
    if (mode_type == HI_VI_ONLINE_VPSS_ONLINE && wrap_en) {
        hi_frame_interrupt_attr frame_interrupt_attr;
        frame_interrupt_attr.interrupt_type = HI_FRAME_INTERRUPT_EARLY_END;
        frame_interrupt_attr.early_line = max_height / 2; /* 2 half */
        ret = hi_mpi_vpss_set_grp_frame_interrupt_attr(0, &frame_interrupt_attr);
        if (ret != HI_SUCCESS) {
            printf("hi_mpi_vpss_set_grp_frame_interrupt_attr failed!\n");
            return ret;
        }
    }
    return HI_SUCCESS;
}

static hi_s32 sample_ir_auto_start_vpss(hi_vpss_grp grp, sample_vpss_cfg *vpss_cfg)
{
    hi_s32 ret;
    sample_vpss_chn_attr vpss_chn_attr = {0};

    if (grp == 0) {
        ret = sample_ir_auto_vpss_set_wrap_grp_int_attr(g_ir_sys_cfg.mode_type, vpss_cfg->wrap_attr[0].enable,
            vpss_cfg->grp_attr.max_height);
        if (ret != HI_SUCCESS) {
            return ret;
        }
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

static hi_s32 sample_ir_auto_stop_vpss(hi_vpss_grp grp)
{
    hi_bool chn_enable[HI_VPSS_MAX_PHYS_CHN_NUM] = {HI_TRUE, HI_TRUE, HI_FALSE};

    return sample_common_vpss_stop(grp, chn_enable, HI_VPSS_MAX_PHYS_CHN_NUM);
}

static td_void sample_ir_auto_get_pipe_wrap_line(sample_vi_cfg vi_cfg[], td_u32 route_num)
{
    hi_vi_wrap_out_param out_param;
    hi_vi_wrap_in_param in_param;
    hi_u32 i;

    if (g_ir_sys_cfg.mode_type == HI_VI_ONLINE_VPSS_ONLINE || g_ir_sys_cfg.mode_type == HI_VI_ONLINE_VPSS_OFFLINE) {
        return;
    }

    (hi_void)memcpy_s(&in_param, sizeof(hi_vi_wrap_in_param), &g_vi_wrap_param, sizeof(hi_vi_wrap_in_param));
    if (g_ir_sys_cfg.mode_type == HI_VI_OFFLINE_VPSS_OFFLINE) {
        in_param.wrap_param[0].slave_param.vi_vpss_online = HI_FALSE;
        in_param.wrap_param[1].slave_param.vi_vpss_online = HI_FALSE;
    }
    in_param.pipe_num = route_num;
#ifdef OT_FPGA
    in_param.wrap_param[0].frame_rate_100x = 1500; /* 1500: 15fps*100 */
    in_param.wrap_param[1].frame_rate_100x = 1500; /* 1500: 15fps*100 */
    in_param.wrap_param[0].slave_param.vpss_venc_wrap = HI_FALSE;
    in_param.wrap_param[1].slave_param.vpss_venc_wrap = HI_FALSE;
#endif
    sample_comm_vi_get_size_by_sns_type(SENSOR0_TYPE, &in_param.wrap_param[0].pipe_size);
    sample_comm_vi_get_size_by_sns_type(SENSOR0_TYPE, &in_param.wrap_param[0].slave_param.vpss_chn0_size);
    sample_comm_vi_get_size_by_sns_type(SENSOR0_TYPE, &in_param.wrap_param[0].slave_param.large_stream_size);
    sample_comm_vi_get_size_by_sns_type(SENSOR1_TYPE, &in_param.wrap_param[1].pipe_size);
    sample_comm_vi_get_size_by_sns_type(SENSOR1_TYPE, &in_param.wrap_param[1].slave_param.vpss_chn0_size);
    sample_comm_vi_get_size_by_sns_type(SENSOR1_TYPE, &in_param.wrap_param[1].slave_param.large_stream_size);
    in_param.wrap_param[0].full_lines_std = in_param.wrap_param[0].pipe_size.height;
    in_param.wrap_param[1].full_lines_std = in_param.wrap_param[1].pipe_size.height;
    if (hi_mpi_sys_get_vi_wrap_buffer_line(&in_param, &out_param) != HI_SUCCESS) {
        sample_print("sample_vie get wrap line fail, pipe_wrap not enable!\n");
        return;
    }

    for (i = 0; i < route_num; i++) {
        vi_cfg[i].pipe_info[0].wrap_attr.buf_line = out_param.buf_line[i];
        vi_cfg[i].pipe_info[0].wrap_attr.enable = HI_TRUE;
    }
}

static hi_void sample_ir_auto_vpss_get_wrap_cfg(sampe_sys_cfg *g_ir_sys_cfg, sample_vpss_cfg *vpss_cfg)
{
    hi_u32 full_lines_std;
    if (g_ir_sys_cfg->vpss_wrap_en) {
        if (SENSOR0_TYPE == SC4336P_MIPI_4M_30FPS_10BIT || SENSOR0_TYPE == OS04D10_MIPI_4M_30FPS_10BIT ||
            SENSOR0_TYPE == GC4023_MIPI_4M_30FPS_10BIT || SENSOR0_TYPE == SC431HAI_MIPI_4M_30FPS_10BIT ||
            SENSOR0_TYPE == SC431HAI_MIPI_4M_30FPS_10BIT_WDR2TO1 || SENSOR0_TYPE == SC4336P_MIPI_3M_30FPS_10BIT) {
            full_lines_std = 1500; /* full_lines_std: 1500 */
        } else if (SENSOR0_TYPE == SC450AI_MIPI_4M_30FPS_10BIT || SENSOR0_TYPE == SC450AI_MIPI_4M_30FPS_10BIT_WDR2TO1) {
            full_lines_std = 1585; /* full_lines_std: 1585 */
        } else if (SENSOR0_TYPE == SC500AI_MIPI_5M_30FPS_10BIT || SENSOR0_TYPE == SC500AI_MIPI_5M_30FPS_10BIT_WDR2TO1) {
            full_lines_std = 1700; /* full_lines_std: 1700 */
        } else {
            g_ir_sys_cfg->vpss_wrap_en = HI_FALSE;
            vpss_cfg->wrap_attr[0].enable = HI_FALSE;
            return;
        }
        (hi_void)sample_comm_vpss_get_wrap_cfg(vpss_cfg->chn_attr, g_ir_sys_cfg->mode_type, full_lines_std,
            &vpss_cfg->wrap_attr[0]);
        g_ir_sys_cfg->vpss_wrap_size = vpss_cfg->wrap_attr[0].buf_size;
    }
}

hi_s32 sample_ir_auto_start_prev(ir_auto_prev *ir_auto_prev)
{
    hi_s32 ret;
    sample_sns_type sns_type;
    hi_size in_size;
    hi_vi_aiisp_mode aiisp_mode = HI_VI_AIISP_MODE_DEFAULT;
    sample_vpss_cfg vpss_cfg;

    ir_auto_prev->vi_pipe = 0;
    ir_auto_prev->vi_chn = 0;
    ir_auto_prev->vpss_grp = 0;
    ir_auto_prev->venc_chn = 0;

    /* ***********************************************
    step1:  get all sensors information
    ************************************************ */

    /* ***********************************************
    step2:  get  input size
    ************************************************ */
    ret = sample_ir_auto_sys_init(g_ir_sys_cfg.mode_type, aiisp_mode);
    if (ret != HI_SUCCESS) {
        goto sys_init_failed;
    }

    sns_type = SENSOR0_TYPE;

    sample_comm_vi_get_size_by_sns_type(sns_type, &in_size);
    sample_comm_vi_get_default_vi_cfg(sns_type, &ir_auto_prev->vi_config);
    sample_ir_auto_get_pipe_wrap_line(&ir_auto_prev->vi_config, 1);
    sample_comm_vpss_get_default_vpss_cfg(&vpss_cfg, g_ir_sys_cfg.vpss_wrap_en);
    vpss_cfg.chn_en[1] = HI_FALSE;
    sample_ir_auto_vpss_get_wrap_cfg(&g_ir_sys_cfg, &vpss_cfg);
    sample_ir_auto_get_default_venc_param(sns_type, &ir_auto_prev->venc_param);
    /* ***********************************************
    step4:  init VI and VENC
    ************************************************ */
    ret = sample_ir_auto_start_vi(&ir_auto_prev->vi_config);
    if (HI_SUCCESS != ret) {
        sample_print("sample_ir_auto_start_vi failed witfh %d\n", ret);
        goto EXIT;
    }

    ret = sample_ir_auto_start_vpss(ir_auto_prev->vpss_grp, &vpss_cfg);
    if (HI_SUCCESS != ret) {
        sample_print("sample_ir_auto_start_vpss failed witfh %d\n", ret);
        goto EXIT1;
    }

    ret = sample_comm_venc_start(ir_auto_prev->venc_chn, &ir_auto_prev->venc_param);
    if (HI_SUCCESS != ret) {
        sample_print("sample_ir_auto_start_vi failed witfh %d\n", ret);
        goto EXIT2;
    }

    /* ***********************************************
    step5:  bind VI
    ************************************************ */
    ret = sample_comm_vi_bind_vpss(ir_auto_prev->vi_pipe, ir_auto_prev->vi_chn, ir_auto_prev->vpss_grp, 0);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vi_bind_vpss failed with %#x!\n", ret);
        goto EXIT3;
    }

    ret = sample_comm_vpss_bind_venc(ir_auto_prev->vpss_grp, 0, ir_auto_prev->venc_chn);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vpss_bind_venc failed with %#x!\n", ret);
        goto EXIT4;
    }

    /* *****************************************
     stream save process
    ***************************************** */
    if ((ret = sample_comm_venc_start_get_stream(&ir_auto_prev->venc_chn, 1)) != HI_SUCCESS) {    // 1: chn num
        sample_print("sample_comm_venc_start_get_stream failed!\n");
        goto EXIT5;
    }

    return ret;

EXIT5:
    sample_comm_vpss_un_bind_venc(ir_auto_prev->vpss_grp, 0, ir_auto_prev->venc_chn);
EXIT4:
    sample_comm_vi_un_bind_vpss(ir_auto_prev->vi_pipe, ir_auto_prev->vi_chn, ir_auto_prev->vpss_grp, 0);
EXIT3:
    sample_comm_venc_stop(ir_auto_prev->venc_chn);
EXIT2:
    sample_ir_auto_stop_vpss(ir_auto_prev->vpss_grp);
EXIT1:
    sample_ir_auto_stop_vi(&ir_auto_prev->vi_config);
EXIT:
    sample_comm_sys_exit();
sys_init_failed:
    return ret;
}

hi_void sample_ir_auto_stop_prev(ir_auto_prev *ir_auto_prev)
{
    hi_s32 ret;

    ret = sample_comm_venc_stop_get_stream(1);    // 1: chn num
    if (ret != HI_SUCCESS) {
        return;
    }

    ret = sample_comm_vpss_un_bind_venc(ir_auto_prev->vpss_grp, 0, ir_auto_prev->venc_chn);
    if (ret != HI_SUCCESS) {
        return;
    }

    ret = sample_comm_venc_stop(ir_auto_prev->venc_chn);
    if (ret != HI_SUCCESS) {
        return;
    }

    ret = sample_ir_auto_stop_vpss(ir_auto_prev->vpss_grp);
    if (ret != HI_SUCCESS) {
        return;
    }

    ret = sample_comm_vi_un_bind_vpss(ir_auto_prev->vi_pipe, ir_auto_prev->vi_chn, ir_auto_prev->vpss_grp, 0);
    if (ret != HI_SUCCESS) {
        return;
    }

    ret = sample_ir_auto_stop_vi(&ir_auto_prev->vi_config);
    if (ret != HI_SUCCESS) {
        return;
    }

    sample_comm_sys_exit();

    return;
}

#define sample_ir_cut_check_fp_return(fp, file_name)                                   \
    do {                                                       \
        if ((fp) == NULL) {    \
            printf("Cannot open %s.\n", file_name); \
            return;                   \
        }                                                      \
    } while (0)

#define LEN 10
#define IR_CUT_FILE_NAME_LEN 50
hi_void sample_ir_cut_gpio_export(hi_u32 gpio_chip_num, hi_u32 gpio_offset)
{
    hi_s32 ret;
    FILE *fp = HI_NULL;
    char file_name[IR_CUT_FILE_NAME_LEN];
    hi_u32 gpio_num = gpio_chip_num * 8 + gpio_offset; // 8 pins

    ret = sprintf_s(file_name, IR_CUT_FILE_NAME_LEN, "/sys/class/gpio/export");
    if (ret < 0) {
        printf("export sprintf_s failed with %d.\n", ret);
        return;
    }
    fp = fopen(file_name, "w");
    sample_ir_cut_check_fp_return(fp, file_name);
    ret = fprintf(fp, "%u", gpio_num);
    if (ret < 0) {
        printf("print gpio_num failed!!!\n");
    }

    ret = fclose(fp);
    if (ret != 0) {
        printf("export close fp err ret:%d\n", ret);
    }
    return;
}

hi_void sample_ir_cut_gpio_dir(hi_u32 gpio_chip_num, hi_u32 gpio_offset)
{
    hi_s32 ret;
    FILE *fp = HI_NULL;
    char file_name[IR_CUT_FILE_NAME_LEN];
    hi_u32 gpio_num = gpio_chip_num * 8 + gpio_offset; // 8 pins

    ret = sprintf_s(file_name, IR_CUT_FILE_NAME_LEN, "/sys/class/gpio/gpio%u/direction", gpio_num);
    if (ret < 0) {
        printf("dir sprintf_s failed with %d.\n", ret);
        return;
    }
    fp = fopen(file_name, "rb+");
    sample_ir_cut_check_fp_return(fp, file_name);
    ret = fprintf(fp, "out");
    if (ret < 0) {
        printf("print out failed!!!\n");
    }
    ret = fclose(fp);
    if (ret != 0) {
        printf("dir close fp err, ret:%d\n", ret);
    }

    return;
}

hi_void sample_ir_cut_gpio_write(hi_u32 gpio_chip_num, hi_u32 gpio_offset, hi_u32 gpio_out_val)
{
    hi_s32 ret;
    FILE *fp = HI_NULL;
    char buf[LEN];
    char file_name[IR_CUT_FILE_NAME_LEN];
    hi_u32 gpio_num = gpio_chip_num * 8 + gpio_offset; // 8 pins

    ret = sprintf_s(file_name, IR_CUT_FILE_NAME_LEN, "/sys/class/gpio/gpio%u/value", gpio_num);
    if (ret < 0) {
        printf("write sprintf_s failed with %d.\n", ret);
        return;
    }

    fp = fopen(file_name, "rb+");
    sample_ir_cut_check_fp_return(fp, file_name);

    if (gpio_out_val) {
        ret = strcpy_s(buf, LEN, "1");
        if (ret != EOK) {
            printf("strcpy_s buf 1 failed,pls check it!!!\n");
        }
    } else {
        ret = strcpy_s(buf, LEN, "0");
        if (ret != EOK) {
            printf("strcpy_s buf 0 failed,pls check it!!!\n");
        }
    }
    ret = (hi_s32)fwrite(buf, sizeof(char), sizeof(buf) - 1, fp);
    if (ret != (sizeof(buf) - 1)) {
        printf("%s: gpio%u_%u = %s, fwrite err ret:%d\n", __func__, gpio_chip_num, gpio_offset, buf, ret);
    }
    printf("%s: gpio%u_%u = %s\n", __func__, gpio_chip_num, gpio_offset, buf);
    ret = fclose(fp);
    if (ret != 0) {
        printf("write close fp err, ret:%d\n", ret);
    }

    return;
}

hi_void sample_ir_cut_gpio_unexport(hi_u32 gpio_chip_num, hi_u32 gpio_offset)
{
    hi_s32 ret;
    FILE *fp = HI_NULL;
    char file_name[IR_CUT_FILE_NAME_LEN];
    hi_u32 gpio_num = gpio_chip_num * 8 + gpio_offset; // 8 pins

    ret = sprintf_s(file_name, IR_CUT_FILE_NAME_LEN, "/sys/class/gpio/unexport");
    if (ret < 0) {
        printf("unexport sprintf_s failed with %d.\n", ret);
        return;
    }

    fp = fopen(file_name, "w");
    sample_ir_cut_check_fp_return(fp, file_name);
    ret = fprintf(fp, "%u", gpio_num);
    if (ret < 0) {
        printf("print unexport gpio_num failed!!!\n");
    }
    ret = fclose(fp);
    if (ret != 0) {
        printf("unexport close fp err, ret:%d\n", ret);
    }

    return;
}

// normal mode
void sample_ir_cut_enable(hi_vi_pipe vi_pipe)
{
    pthread_mutex_lock(&g_ir_mutex);
    if(g_ir_switch.cb)
    {
      g_ir_switch.cb(vi_pipe, HI_ISP_IR_STATUS_NORMAL, g_ir_switch.uargs);
    }
    pthread_mutex_unlock(&g_ir_mutex);
    
    g_ast_ir_attr[vi_pipe].ir_status = HI_ISP_IR_STATUS_NORMAL;
	return;
	
	
    hi_u32 gpio_chip_num7 = 7; // gpio7
    hi_u32 gpio_chip_num1 = 1; // gpio1
    hi_u32 gpio_offset6 = 6;  // gpio7_6
    hi_u32 gpio_offset1 = 1;  // gpio1_1
    hi_u32 gpio_offset7 = 7;  // gpio7_7
    hi_u32 gpio_offset0 = 0;  // gpio1_0
    if (BOARD_TYPE == DMEB_QFN) {
        // export
        sample_ir_cut_gpio_export(gpio_chip_num7, gpio_offset7);
        sample_ir_cut_gpio_export(gpio_chip_num1, gpio_offset0);
        // dir
        sample_ir_cut_gpio_dir(gpio_chip_num7, gpio_offset7);
        sample_ir_cut_gpio_dir(gpio_chip_num1, gpio_offset0);
        // data
        sample_ir_cut_gpio_write(gpio_chip_num7, gpio_offset7, 0); // set gpio7_7 0
        sample_ir_cut_gpio_write(gpio_chip_num1, gpio_offset0, 1); // set gpio1_0 1
        /* 1000000 1000ms */
        usleep(1000000);
        // back to original
        sample_ir_cut_gpio_write(gpio_chip_num7, gpio_offset7, 0); // set gpio7_7 0
        sample_ir_cut_gpio_write(gpio_chip_num1, gpio_offset0, 0); // set gpio1_0 0
        sample_ir_cut_gpio_unexport(gpio_chip_num7, gpio_offset7);
        sample_ir_cut_gpio_unexport(gpio_chip_num1, gpio_offset0);
    } else {
        // export
        sample_ir_cut_gpio_export(gpio_chip_num1, gpio_offset1);
        sample_ir_cut_gpio_export(gpio_chip_num7, gpio_offset6);
        // dir
        sample_ir_cut_gpio_dir(gpio_chip_num1, gpio_offset1);
        sample_ir_cut_gpio_dir(gpio_chip_num7, gpio_offset6);
        // data
        sample_ir_cut_gpio_write(gpio_chip_num1, gpio_offset1, 0); // set gpio1_1 0
        sample_ir_cut_gpio_write(gpio_chip_num7, gpio_offset6, 1); // set gpio7_6 1
        /* 1000000 1000ms */
        usleep(1000000);
        // back to original
        sample_ir_cut_gpio_write(gpio_chip_num1, gpio_offset1, 0); // set gpio1_1 0
        sample_ir_cut_gpio_write(gpio_chip_num7, gpio_offset6, 0); // set gpio7_6 0
        sample_ir_cut_gpio_unexport(gpio_chip_num1, gpio_offset1);
        sample_ir_cut_gpio_unexport(gpio_chip_num7, gpio_offset6);
    }
    return;
}

void sample_ir_cut_disable(hi_vi_pipe vi_pipe)
{
    pthread_mutex_lock(&g_ir_mutex);
    if(g_ir_switch.cb)
    {
      g_ir_switch.cb(vi_pipe, HI_ISP_IR_STATUS_IR, g_ir_switch.uargs);
    }
    pthread_mutex_unlock(&g_ir_mutex);
    g_ast_ir_attr[vi_pipe].ir_status = HI_ISP_IR_STATUS_IR;
	return;
	
    hi_u32 gpio_chip_num7 = 7; // gpio7
    hi_u32 gpio_chip_num1 = 1; // gpio1
    hi_u32 gpio_offset7 = 7;  // gpio7_7
    hi_u32 gpio_offset0 = 0;  // gpio1_0
    hi_u32 gpio_offset6 = 6;  // gpio7_6
    hi_u32 gpio_offset1 = 1;  // gpio1_1
    if (BOARD_TYPE == DMEB_QFN) {
        // export
        sample_ir_cut_gpio_export(gpio_chip_num7, gpio_offset7);
        sample_ir_cut_gpio_export(gpio_chip_num1, gpio_offset0);
        // dir
        sample_ir_cut_gpio_dir(gpio_chip_num7, gpio_offset7);
        sample_ir_cut_gpio_dir(gpio_chip_num1, gpio_offset0);
        // data
        sample_ir_cut_gpio_write(gpio_chip_num7, gpio_offset7, 1); // set gpio7_7 1
        sample_ir_cut_gpio_write(gpio_chip_num1, gpio_offset0, 0); // set gpio1_0 0
        /* 1000000 1000ms */
        usleep(1000000);
        // back to original
        sample_ir_cut_gpio_write(gpio_chip_num7, gpio_offset7, 0); // set gpio7_7 0
        sample_ir_cut_gpio_write(gpio_chip_num1, gpio_offset0, 0); // set gpio1_1 0
        sample_ir_cut_gpio_unexport(gpio_chip_num7, gpio_offset7);
        sample_ir_cut_gpio_unexport(gpio_chip_num1, gpio_offset0);
    } else {
        // export
        sample_ir_cut_gpio_export(gpio_chip_num1, gpio_offset1);
        sample_ir_cut_gpio_export(gpio_chip_num7, gpio_offset6);
        // dir
        sample_ir_cut_gpio_dir(gpio_chip_num1, gpio_offset1);
        sample_ir_cut_gpio_dir(gpio_chip_num7, gpio_offset6);
        // data
        sample_ir_cut_gpio_write(gpio_chip_num1, gpio_offset1, 1); // set gpio1_1 1
        sample_ir_cut_gpio_write(gpio_chip_num7, gpio_offset6, 0); // set gpio7_6 0
        /* 1000000 1000ms */
        usleep(1000000);
        // back to original
        sample_ir_cut_gpio_write(gpio_chip_num1, gpio_offset1, 0); // set gpio1_1 0
        sample_ir_cut_gpio_write(gpio_chip_num7, gpio_offset6, 0); // set gpio7_6 0
        sample_ir_cut_gpio_unexport(gpio_chip_num1, gpio_offset1);
        sample_ir_cut_gpio_unexport(gpio_chip_num7, gpio_offset6);
    }
    return;
}

void sample_ir_auto_usage(const char* s_prg_nm)
{
    printf("usage : %s <mode> <normal2_ir_exp_thr> <ir2_normal_exp_thr>"
        " <rg_max> <rg_min> <bg_max> <bg_min> <ir_status>\n", s_prg_nm);

    printf("mode:\n");
    printf("\t 0) SAMPLE_IR_CALIBRATION_MODE.\n");
    printf("\t 1) SAMPLE_IR_AUTO_MODE.\n");

    printf("normal2_ir_exp_thr:\n");
    printf("\t ISO threshold of switching from normal to IR mode.\n");

    printf("ir2_normal_exp_thr:\n");
    printf("\t ISO threshold of switching from IR to normal mode.\n");

    printf("rg_max/rg_min/bg_max/bg_min:\n");
    printf("\t maximum(minimum) value of R/G(B/G) in IR scene.\n");

    printf("ir_status:\n");
    printf("\t current IR status. 0: normal mode; 1: IR mode.\n");

    printf("e.g : %s 0 (SAMPLE_IR_CALIBRATION_MODE)\n", s_prg_nm);
    printf("e.g : %s 1 (SAMPLE_IR_AUTO_MODE, default parameters)\n", s_prg_nm);
    printf("e.g : %s 1 16000 400 280 190 280 190 1 (SAMPLE_IR_AUTO_MODE, user_define parameters)\n", s_prg_nm);

    return;
}

hi_s32 isp_ir_switch_to_normal(hi_vi_pipe vi_pipe)
{
    hi_s32 ret;
    hi_isp_saturation_attr isp_saturation_attr;
    hi_isp_wb_attr isp_wb_attr;
    hi_isp_color_matrix_attr isp_ccm_attr;

    /* switch ir-cut to normal */
    sample_ir_cut_enable(vi_pipe);
    /* switch isp config to normal */
    ret = hi_mpi_isp_get_saturation_attr(vi_pipe, &isp_saturation_attr);
    if (HI_SUCCESS != ret) {
        printf("hi_mpi_isp_get_saturation_attr failed\n");
        return ret;
    }
    isp_saturation_attr.op_type = HI_OP_MODE_AUTO;
    ret = hi_mpi_isp_set_saturation_attr(vi_pipe, &isp_saturation_attr);
    if (HI_SUCCESS != ret) {
        printf("hi_mpi_isp_set_saturation_attr failed\n");
        return ret;
    }

    ret = hi_mpi_isp_get_wb_attr(vi_pipe, &isp_wb_attr);
    if (HI_SUCCESS != ret) {
        printf("hi_mpi_isp_get_wb_attr failed\n");
        return ret;
    }
    isp_wb_attr.op_type = HI_OP_MODE_AUTO;
    ret = hi_mpi_isp_set_wb_attr(vi_pipe, &isp_wb_attr);
    if (HI_SUCCESS != ret) {
        printf("hi_mpi_isp_set_wb_attr failed\n");
        return ret;
    }

    ret = hi_mpi_isp_get_ccm_attr(vi_pipe, &isp_ccm_attr);
    if (HI_SUCCESS != ret) {
        printf("hi_mpi_isp_get_ccm_attr failed\n");
        return ret;
    }
    isp_ccm_attr.op_type = HI_OP_MODE_AUTO;
    ret = hi_mpi_isp_set_ccm_attr(vi_pipe, &isp_ccm_attr);
    if (HI_SUCCESS != ret) {
        printf("hi_mpi_isp_set_ccm_attr failed\n");
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 isp_ir_change_ccm_attr(hi_vi_pipe vi_pipe)
{
    hi_s32 ret;
    hi_isp_color_matrix_attr isp_ccm_attr;
    ret = hi_mpi_isp_get_ccm_attr(vi_pipe, &isp_ccm_attr);
    if (HI_SUCCESS != ret) {
        printf("hi_mpi_isp_get_ccm_attr failed\n");
        return ret;
    }
    isp_ccm_attr.op_type = HI_OP_MODE_MANUAL;
    isp_ccm_attr.manual_attr.ccm[0] = 0x100;
    isp_ccm_attr.manual_attr.ccm[1] = 0x0;
    isp_ccm_attr.manual_attr.ccm[2] = 0x0; // idx 2
    isp_ccm_attr.manual_attr.ccm[3] = 0x0; // idx 3
    isp_ccm_attr.manual_attr.ccm[4] = 0x100; // idx 4
    isp_ccm_attr.manual_attr.ccm[5] = 0x0; // idx 5
    isp_ccm_attr.manual_attr.ccm[6] = 0x0; // idx 6
    isp_ccm_attr.manual_attr.ccm[7] = 0x0; // idx 7
    isp_ccm_attr.manual_attr.ccm[8] = 0x100; // idx 8
    ret = hi_mpi_isp_set_ccm_attr(vi_pipe, &isp_ccm_attr);
    if (HI_SUCCESS != ret) {
        printf("hi_mpi_isp_set_ccm_attr failed\n");
        return ret;
    }
    return HI_SUCCESS;
}

hi_s32 isp_ir_switch_to_ir(hi_vi_pipe vi_pipe)
{
    hi_s32 ret;
    hi_isp_saturation_attr isp_saturation_attr;
    hi_isp_wb_attr isp_wb_attr;

    /* switch isp config to ir */
    ret = hi_mpi_isp_get_saturation_attr(vi_pipe, &isp_saturation_attr);
    if (HI_SUCCESS != ret) {
        printf("hi_mpi_isp_get_saturation_attr failed\n");
        return ret;
    }
    isp_saturation_attr.op_type = HI_OP_MODE_MANUAL;
    isp_saturation_attr.manual_attr.saturation = 0;
    ret = hi_mpi_isp_set_saturation_attr(vi_pipe, &isp_saturation_attr);
    if (HI_SUCCESS != ret) {
        printf("hi_mpi_isp_set_saturation_attr failed\n");
        return ret;
    }

    ret = hi_mpi_isp_get_wb_attr(vi_pipe, &isp_wb_attr);
    if (HI_SUCCESS != ret) {
        printf("hi_mpi_isp_get_wb_attr failed\n");
        return ret;
    }
    isp_wb_attr.op_type = HI_OP_MODE_MANUAL;
    isp_wb_attr.manual_attr.b_gain  = 0x100;
    isp_wb_attr.manual_attr.gb_gain = 0x100;
    isp_wb_attr.manual_attr.gr_gain = 0x100;
    isp_wb_attr.manual_attr.r_gain  = 0x100;
    ret = hi_mpi_isp_set_wb_attr(vi_pipe, &isp_wb_attr);
    if (HI_SUCCESS != ret) {
        printf("hi_mpi_isp_set_wb_attr failed\n");
        return ret;
    }

    ret = isp_ir_change_ccm_attr(vi_pipe);
    if (HI_SUCCESS != ret) {
        return ret;
    }

    usleep(1000000); // sleep 1000000 us

    /* switch ir-cut to ir */
    sample_ir_cut_disable(vi_pipe);

    return HI_SUCCESS;
}

hi_s32 sample_isp_ir_auto_run(hi_vi_pipe vi_pipe)
{
    hi_s32 ret = HI_SUCCESS;
    hi_isp_ir_auto_attr ir_attr;

    ir_attr.enable            = g_ast_ir_attr[vi_pipe].enable;
    ir_attr.normal_to_ir_iso_threshold = g_ast_ir_attr[vi_pipe].normal_to_ir_iso_threshold;
    ir_attr.ir_to_normal_iso_threshold = g_ast_ir_attr[vi_pipe].ir_to_normal_iso_threshold;
    ir_attr.rg_max           = g_ast_ir_attr[vi_pipe].rg_max;
    ir_attr.rg_min           = g_ast_ir_attr[vi_pipe].rg_min;
    ir_attr.bg_max           = g_ast_ir_attr[vi_pipe].bg_max;
    ir_attr.bg_min           = g_ast_ir_attr[vi_pipe].bg_min;
    ir_attr.ir_status         = g_ast_ir_attr[vi_pipe].ir_status;
    ir_attr.ir_switch         = g_ast_ir_attr[vi_pipe].ir_switch;
    while (HI_TRUE == g_ast_ir_thread[vi_pipe].thread_flag) {
      
        //printf("sample_isp_ir_auto_run g_ir_switch.cb:%p, g_ir_switch.cds:%p\n", g_ir_switch.cb, g_ir_switch.cds);
        
        /* run_interval: 40 ms */
        usleep(40000 * 4); // sleep 40000 * 4 us

        if(!g_ir_switch.cb)
          continue;
        else
          ir_attr.ir_status = g_ast_ir_attr[vi_pipe].ir_status;
        
        if(g_ir_switch.cds)
        {
          int night = 0; 
          pthread_mutex_lock(&g_ir_mutex);
          if(g_ir_switch.cds)
          {
            night = g_ir_switch.cds(vi_pipe, g_ir_switch.uargs);
          }
          pthread_mutex_unlock(&g_ir_mutex);
          if(ir_attr.ir_status != night)
          {
            ir_attr.ir_switch = (night)?HI_ISP_IR_SWITCH_TO_IR:HI_ISP_IR_SWITCH_TO_NORMAL;
          }
          else 
          {
            ir_attr.ir_switch = HI_ISP_IR_SWITCH_NONE;
          }
        }
        else
        {
          ret = hi_mpi_isp_ir_auto(vi_pipe, &ir_attr);
          if (HI_SUCCESS != ret) {
              printf("hi_mpi_isp_ir_auto_run_once failed\n");
              return ret;
          }
        }
 
        if (HI_ISP_IR_SWITCH_TO_IR == ir_attr.ir_switch) { /* normal to IR */
            printf("\n[normal -> IR]\n");

            ret = isp_ir_switch_to_ir(vi_pipe);
            if (HI_SUCCESS != ret) {
                printf("isp_ir_switch_to_ir failed\n");
                return ret;
            }

            g_ast_ir_attr[vi_pipe].ir_status = ir_attr.ir_status = HI_ISP_IR_STATUS_IR;
        } else if (HI_ISP_IR_SWITCH_TO_NORMAL == ir_attr.ir_switch) { /* IR to normal */
            printf("\n[IR -> normal]\n");

            ret = isp_ir_switch_to_normal(vi_pipe);
            if (HI_SUCCESS != ret) {
                printf("isp_ir_switch_to_normal failed\n");
                return ret;
            }

            g_ast_ir_attr[vi_pipe].ir_status = ir_attr.ir_status = HI_ISP_IR_STATUS_NORMAL;
        }
    }

    return HI_SUCCESS;
}

static void* sample_isp_ir_auto_thread(void* param)
{
    hi_vi_pipe vi_pipe;

    vi_pipe = *(hi_vi_pipe *)param;

    prctl(PR_SET_NAME, "isp_ir_auto", 0, 0, 0);

    sample_isp_ir_auto_run(vi_pipe);

    return NULL;
}

hi_s32 sample_isp_ir_auto_exit(hi_vi_pipe vi_pipe)
{
    if (g_ast_ir_thread[vi_pipe].thread) {
        g_ast_ir_thread[vi_pipe].thread_flag = HI_FALSE;
        pthread_join(g_ast_ir_thread[vi_pipe].thread, NULL);
        g_ast_ir_thread[vi_pipe].thread = 0;
    }

    return HI_SUCCESS;
}
void sample_ir_auto_handle_sig(hi_s32 signo)
{
    int ret;

    if (SIGINT == signo || SIGTERM == signo) {
        ret = sample_isp_ir_auto_exit(g_vi_pipe);
        if (ret != 0) {
            printf("sample_isp_ir_auto_exit failed\n");
            return;
        }
    }
    return;
}

hi_s32 sample_ir_calibration(hi_void)
{
    hi_isp_wb_stats stat;
    hi_u32 rg, bg;
    hi_s32 ret;

    /* 1. infrared scene without visible light */

    /* 2. switch to IR */
    ret = isp_ir_switch_to_ir(g_vi_pipe);
    if (HI_SUCCESS != ret) {
        printf("isp_ir_switch_to_ir failed\n");
        return ret;
    }

    /* 3. waiting for AE to stabilize */
    usleep(1000000); // sleep 1000000 us

    /* 4. calculate rg_max/rg_min/bg_max/bg_min */
    ret = hi_mpi_isp_get_wb_stats(g_vi_pipe, &stat);
    if (HI_SUCCESS != ret) {
        printf("hi_mpi_isp_get_statistics failed\n");
        return ret;
    }

    rg = ((hi_u32)stat.global_r << SHIFT_8BIT) / div_0_to_1(stat.global_g);
    bg = ((hi_u32)stat.global_b << SHIFT_8BIT) / div_0_to_1(stat.global_g);
    printf("rg_max:%u, rg_min:%u, bg_max:%u, bg_min:%u\n",
           (rg * GAIN_MAX_COEF) >> SHIFT_8BIT, (rg * GAIN_MIN_COEF) >> SHIFT_8BIT,
           (bg * GAIN_MAX_COEF) >> SHIFT_8BIT, (bg * GAIN_MIN_COEF) >> SHIFT_8BIT);

    return HI_SUCCESS;
}


int sample_ir_auto_thread(int argc, char* argv[])
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 mode = SAMPLE_IR_AUTO_MODE;
    hi_char *para_stop;

    g_vi_pipe = 0;
    mode = (hi_u32)strtol(argv[1], &para_stop, 10); /* idx 2 10 dec */
    /* SAMPLE_IR_CALIBRATION_MODE */
    if (mode == SAMPLE_IR_CALIBRATION_MODE) {
        sample_ir_calibration();
    }
    /* SAMPLE_IR_AUTO_MODE */
    if (mode == SAMPLE_IR_AUTO_MODE && argc > 8) { // argc num 8
        printf("[user_config] normal_to_ir_iso_threshold:%u, ir_to_normal_iso_threshold:%u,"
        "RG:[%u,%u], BG:[%u,%u], ir_status:%d\n",
            g_ast_ir_attr[g_vi_pipe].normal_to_ir_iso_threshold, g_ast_ir_attr[g_vi_pipe].ir_to_normal_iso_threshold,
            g_ast_ir_attr[g_vi_pipe].rg_min, g_ast_ir_attr[g_vi_pipe].rg_max,
            g_ast_ir_attr[g_vi_pipe].bg_min, g_ast_ir_attr[g_vi_pipe].bg_max,
            g_ast_ir_attr[g_vi_pipe].ir_status);
    }
    g_ast_ir_thread[g_vi_pipe].thread_flag = HI_TRUE;

    ret = pthread_create(&g_ast_ir_thread[g_vi_pipe].thread, HI_NULL, sample_isp_ir_auto_thread, (hi_void *)&g_vi_pipe);
    if (ret != 0) {
        printf("%s: create isp ir_auto thread failed!, error: %d, %s\r\n", __FUNCTION__, ret, strerror(ret));
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_ir_parse_auto_args(int argc, char *argv[])
{
    hi_char *para_stop;
    g_ast_ir_attr[g_vi_pipe].normal_to_ir_iso_threshold =
        (hi_u32)strtol(argv[2], &para_stop, 10); /* idx 2, 10 dec */
    if (*para_stop != '\0' || para_stop == argv[2]) {
        return SAMPLE_IR_AUTO_RETURN;
    }
    g_ast_ir_attr[g_vi_pipe].ir_to_normal_iso_threshold =
        (hi_u32)strtol(argv[3], &para_stop, 10); /* idx 3, 10 dec */
    if (*para_stop != '\0' || para_stop == argv[3]) {
        return SAMPLE_IR_AUTO_RETURN;
    }
    g_ast_ir_attr[g_vi_pipe].rg_max = (hi_u32)strtol(argv[4], &para_stop, 10); /* idx 4, 10 dec */
    if (*para_stop != '\0' || para_stop == argv[4]) {
        return SAMPLE_IR_AUTO_RETURN;
    }
    g_ast_ir_attr[g_vi_pipe].rg_min = (hi_u32)strtol(argv[5], &para_stop, 10); /* idx 5, 10 dec */
    if (*para_stop != '\0' || para_stop == argv[5]) {
        return SAMPLE_IR_AUTO_RETURN;
    }
    g_ast_ir_attr[g_vi_pipe].bg_max = (hi_u32)strtol(argv[6], &para_stop, 10); /* idx 6, 10 dec */
    if (*para_stop != '\0' || para_stop == argv[6]) {
        return SAMPLE_IR_AUTO_RETURN;
    }
    g_ast_ir_attr[g_vi_pipe].bg_min = (hi_u32)strtol(argv[7], &para_stop, 10); /* idx 7, 10 dec */
    if (*para_stop != '\0' || para_stop == argv[7]) {
        return SAMPLE_IR_AUTO_RETURN;
    }
    g_ast_ir_attr[g_vi_pipe].ir_status = (hi_u32)strtol(argv[8], &para_stop, 10); /* idx 8, 10 dec */
    if (*para_stop != '\0' || para_stop == argv[8]) {
        return SAMPLE_IR_AUTO_RETURN;
    }
    if ((g_ast_ir_attr[g_vi_pipe].ir_status != HI_ISP_IR_STATUS_NORMAL)
        && (g_ast_ir_attr[g_vi_pipe].ir_status != HI_ISP_IR_STATUS_IR)) {
        return SAMPLE_IR_AUTO_RETURN;
    }

    return HI_SUCCESS;
}

hi_s32 sample_ir_parse_args(int argc, char *argv[])
{
    hi_u32 mode = SAMPLE_IR_AUTO_MODE;

    if (argc < 2) { // argc num is 2
        sample_ir_auto_usage(argv[0]);
        return SAMPLE_IR_AUTO_RETURN;
    }
    if (!strncmp(argv[1], "-h", 2)) { // size 2
        sample_ir_auto_usage(argv[0]);
        return SAMPLE_IR_AUTO_RETURN;
    }

    if (strlen(argv[1]) != 1  || !check_digit(argv[1][0])) {
        sample_ir_auto_usage(argv[0]);
        return HI_FAILURE;
    }

    mode = (hi_u32)atoi(argv[1]); // idx 1
    if ((mode != SAMPLE_IR_AUTO_MODE) && (mode != SAMPLE_IR_CALIBRATION_MODE)) {
        printf("the mode is invalid!\n");
        sample_ir_auto_usage(argv[0]);
        return SAMPLE_IR_AUTO_RETURN;
    }

    if ((mode == SAMPLE_IR_CALIBRATION_MODE) && (argc != 2)) {  // num of argc 2
        printf("SAMPLE_IR_CALIBRATION_MODE, input parameter error\n");
        sample_ir_auto_usage(argv[0]);
        return SAMPLE_IR_AUTO_RETURN;
    }

    if ((mode == SAMPLE_IR_AUTO_MODE) && (argc != 9) && (argc != 2)) { // num of argc 9, 2
        printf("SAMPLE_IR_AUTO_MODE, input parameter error\n");
        sample_ir_auto_usage(argv[0]);
        return SAMPLE_IR_AUTO_RETURN;
    }

    /* SAMPLE_IR_AUTO_MODE */
    if ((mode == SAMPLE_IR_AUTO_MODE) && (argc > 8)) { // argc num 8
        if (sample_ir_parse_auto_args(argc, argv)) {
            printf("SAMPLE_IR_AUTO_MODE, input parameter error\n");
            sample_ir_auto_usage(argv[0]);
            return SAMPLE_IR_AUTO_RETURN;
        }
    }

    return HI_SUCCESS;
}

static hi_void sample_ir_cut_handle_sig(hi_s32 signo)
{
    if (signo == SIGINT || signo == SIGTERM) {
        g_sig_flag = 1;
    }
}

static hi_void sample_register_sig_handler(hi_void (*sig_handle)(hi_s32))
{
    struct sigaction sa;

    (hi_void)memset_s(&sa, sizeof(struct sigaction), 0, sizeof(struct sigaction));
    sa.sa_handler = sig_handle;
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, HI_NULL);
    sigaction(SIGTERM, &sa, HI_NULL);
}

static hi_void ir_auto_getchar(hi_void)
{
    if (g_sig_flag == 1) {
        return;
    }

    (hi_void)getchar();

    if (g_sig_flag == 1) {
        return;
    }
}


 
hi_s32 isp_ir_mode(gsf_mpp_ir_t *ir)
{
  pthread_mutex_lock(&g_ir_mutex);
  if(ir)
    g_ir_switch = *ir;
  else
    g_ir_switch.cb = NULL;
  pthread_mutex_unlock(&g_ir_mutex);
  
  printf("isp_ir_mode switch.cb:%p, switch.cds:%p\n", g_ir_switch.cb, g_ir_switch.cds);
  return 0;
}


hi_s32 isp_ir_switch_to_auto(hi_vi_pipe vi_pipe)
{
  if(g_ast_ir_thread[vi_pipe].thread)
    return 0;
    
  g_vi_pipe = vi_pipe;
  #define RATIO 1.5
    
  /* need to modified when sensor/lens/IR-cut/infrared light changed */
  g_ast_ir_attr[g_vi_pipe].enable = HI_TRUE;
  g_ast_ir_attr[g_vi_pipe].normal_to_ir_iso_threshold= 16000; // normal_to_ir_iso_threshold 16000
  g_ast_ir_attr[g_vi_pipe].ir_to_normal_iso_threshold = 400; // ir_to_normal_iso_threshold 400
  g_ast_ir_attr[g_vi_pipe].rg_max = 280 * RATIO; // max 280
  g_ast_ir_attr[g_vi_pipe].rg_min = 190 / RATIO; // 190
  g_ast_ir_attr[g_vi_pipe].bg_max = 280 * RATIO; // 280
  g_ast_ir_attr[g_vi_pipe].bg_min = 190 / RATIO; // 190
  g_ast_ir_attr[g_vi_pipe].ir_status = HI_ISP_IR_STATUS_NORMAL;
  
  g_ast_ir_thread[g_vi_pipe].thread_flag = HI_TRUE;

  int ret = pthread_create(&g_ast_ir_thread[g_vi_pipe].thread, HI_NULL, sample_isp_ir_auto_thread, (hi_void *)&g_vi_pipe);
  if (ret != 0) {
      printf("%s: create isp ir_auto thread failed!, error: %d, %s\r\n", __FUNCTION__, ret, strerror(ret));
      return HI_FAILURE;
  }
  
  return HI_SUCCESS;
}

/******************************************************************************
* function    : main()
* description : video preview sample
******************************************************************************/
#ifdef __LITEOS__
int app_main(int argc, char *argv[])
#else
int sample_ir_auto_main(int argc, char* argv[])
#endif
{
    hi_s32 ret = HI_FAILURE;
    g_vi_pipe = 0;
    /* need to modified when sensor/lens/IR-cut/infrared light changed */
    g_ast_ir_attr[g_vi_pipe].enable = HI_TRUE;
    g_ast_ir_attr[g_vi_pipe].normal_to_ir_iso_threshold= 16000; // normal_to_ir_iso_threshold 16000
    g_ast_ir_attr[g_vi_pipe].ir_to_normal_iso_threshold = 400; // ir_to_normal_iso_threshold 400
    g_ast_ir_attr[g_vi_pipe].rg_max = 280; // max 280
    g_ast_ir_attr[g_vi_pipe].rg_min = 190; // 190
    g_ast_ir_attr[g_vi_pipe].bg_max = 280; // 280
    g_ast_ir_attr[g_vi_pipe].bg_min = 190; // 190
    g_ast_ir_attr[g_vi_pipe].ir_status = HI_ISP_IR_STATUS_IR;
    ret = sample_ir_parse_args(argc, argv);
    if (ret != HI_SUCCESS) {
        return ret;
    }
#ifndef __LITEOS__
    sample_register_sig_handler(sample_ir_cut_handle_sig);
#endif
    ir_auto_prev ir_auto_prev;
    hi_s32 s32ret;
    s32ret = sample_ir_auto_start_prev(&ir_auto_prev);
    if (HI_SUCCESS == s32ret) {
        sample_print("ISP is now running normally\n");
    } else {
        sample_print("ISP is not running normally!please check it\n");
        return -1;
    }
    printf("input anything to continue....\n");
    ir_auto_getchar();

    sleep(2); // sleep 2s
    ret = sample_ir_auto_thread(argc, argv);
    if (ret != HI_SUCCESS) {
        goto EXIT;
    }
    ir_auto_getchar();
    sample_isp_ir_auto_exit(g_vi_pipe);
EXIT:
    printf("input anything to continue....\n");
    ir_auto_getchar();
    sample_ir_auto_stop_prev(&ir_auto_prev);
    return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

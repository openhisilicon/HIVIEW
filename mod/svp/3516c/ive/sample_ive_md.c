/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#include "sample_common_ive.h"
#include "hi_ivs_md.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/prctl.h>

#define HI_SAMPLE_IVE_MD_IMAGE_NUM          2
#define HI_SAMPLE_IVE_MD_MILLIC_SEC         20000
#define HI_SAMPLE_IVE_MD_ADD_X_VAL          32768
#define HI_SAMPLE_IVE_MD_ADD_Y_VAL          32768
#define HI_SAMPLE_IVE_MD_THREAD_NAME_LEN    16
#define HI_SAMPLE_IVE_MD_AREA_THR_STEP      8
#define HI_SAMPLE_IVE_MD_VPSS_CHN           2
#define HI_SAMPLE_IVE_MD_NUM_TWO            2
#define HI_SAMPLE_IVE_SAD_THRESHOLD         100

typedef struct {
    hi_svp_src_img img[HI_SAMPLE_IVE_MD_IMAGE_NUM];
    hi_svp_dst_mem_info blob;
    hi_md_attr md_attr;
    hi_sample_svp_rect_info region;
} hi_sample_ivs_md_info;

typedef struct {
    hi_md_chn md_chn;
    hi_venc_chn venc_chn;
    hi_s32 vpss_grp;
} hi_sample_md_vpss_hld;

static hi_bool g_stop_signal = HI_FALSE;
static pthread_t g_md_thread;
static hi_sample_ivs_md_info g_md_info;
static hi_sample_svp_switch g_md_switch = { HI_TRUE };
static sample_vi_cfg g_vi_config;
static hi_sample_src_dst_size g_src_dst;

static hi_void sample_ivs_md_uninit(hi_sample_ivs_md_info *md_info_ptr)
{
    hi_s32 i;
    hi_s32 ret;

    sample_svp_check_exps_return_void(md_info_ptr == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR, "md_inf_ptr can't be null\n");

    for (i = 0; i < HI_SAMPLE_IVE_MD_IMAGE_NUM; i++) {
        sample_svp_mmz_free(md_info_ptr->img[i].phys_addr[0], md_info_ptr->img[i].virt_addr[0]);
    }

    sample_svp_mmz_free(md_info_ptr->blob.phys_addr, md_info_ptr->blob.virt_addr);

    ret = hi_ivs_md_exit();
    if (ret != HI_SUCCESS) {
        sample_svp_trace_err("hi_ivs_md_exit fail,Error(%#x)\n", ret);
        return;
    }
}

static hi_s32 sample_ivs_md_init(hi_sample_ivs_md_info *md_inf_ptr, hi_u32 width, hi_u32 height)
{
    hi_s32 ret = HI_ERR_IVE_NULL_PTR;
    hi_s32 i;
    hi_u32 size, sad_mode;
    hi_u8 wnd_size;

    sample_svp_check_exps_return(md_inf_ptr == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "md_inf_ptr can't be null\n");

    for (i = 0; i < HI_SAMPLE_IVE_MD_IMAGE_NUM; i++) {
        ret = sample_common_ive_create_image(&md_inf_ptr->img[i], HI_SVP_IMG_TYPE_U8C1, width, height);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, md_init_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),Create img[%d] image failed!\n", ret, i);
    }
    size = (hi_u32)sizeof(hi_ive_ccblob);
    ret = sample_common_ive_create_mem_info(&md_inf_ptr->blob, size);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, md_init_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create blob mem info failed!\n", ret);

    /* Set attr info */
    md_inf_ptr->md_attr.alg_mode = HI_MD_ALG_MODE_BG;
    md_inf_ptr->md_attr.sad_mode = HI_IVE_SAD_MODE_MB_4X4;
    md_inf_ptr->md_attr.sad_out_ctrl = HI_IVE_SAD_OUT_CTRL_THRESHOLD;
    md_inf_ptr->md_attr.sad_threshold = HI_SAMPLE_IVE_SAD_THRESHOLD * (1 << 1);
    md_inf_ptr->md_attr.width = width;
    md_inf_ptr->md_attr.height = height;
    md_inf_ptr->md_attr.add_ctrl.x = HI_SAMPLE_IVE_MD_ADD_X_VAL;
    md_inf_ptr->md_attr.add_ctrl.y = HI_SAMPLE_IVE_MD_ADD_Y_VAL;
    md_inf_ptr->md_attr.ccl_ctrl.mode = HI_IVE_CCL_MODE_4C;
    sad_mode = (hi_u32)md_inf_ptr->md_attr.sad_mode;
    wnd_size = (1 << (HI_SAMPLE_IVE_MD_NUM_TWO + sad_mode));
    md_inf_ptr->md_attr.ccl_ctrl.init_area_threshold = wnd_size * wnd_size;
    md_inf_ptr->md_attr.ccl_ctrl.step = wnd_size;

    ret = hi_ivs_md_init();
    sample_svp_check_exps_goto(ret != HI_SUCCESS,  md_init_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),hi_ivs_md_init failed!\n", ret);

md_init_fail:
    if (ret != HI_SUCCESS) {
        sample_ivs_md_uninit(md_inf_ptr);
    }
    return ret;
}

static hi_void sample_ivs_set_src_dst_size(hi_sample_src_dst_size *src_dst, hi_u32 src_width,
    hi_u32 src_height, hi_u32 dst_width, hi_u32 dst_height)
{
    src_dst->src.width = src_width;
    src_dst->src.height = src_height;
    src_dst->dst.width = dst_width;
    src_dst->dst.height = dst_height;
}

/* first frame just init reference frame, if not, change the frame idx */
static hi_s32 sample_ivs_md_dma_data(hi_u32 cur_idx, hi_video_frame_info *frm,
    hi_sample_ivs_md_info *md_ptr, hi_bool *is_first_frm)
{
    hi_s32 ret;
    hi_bool is_instant = HI_TRUE;
    if (*is_first_frm != HI_TRUE) {
            ret = sample_common_ive_dma_image(frm, &md_ptr->img[cur_idx], is_instant);
            sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
                "sample_ive_dma_image fail,Err(%#x)\n", ret);
        } else {
            ret = sample_common_ive_dma_image(frm, &md_ptr->img[1 - cur_idx], is_instant);
            sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
                "sample_ive_dma_image fail,Err(%#x)\n", ret);

            *is_first_frm = HI_FALSE;
        }
    return HI_SUCCESS;
}

static hi_void *sample_ivs_md_proc(hi_void *args)
{
    hi_s32 ret;
    hi_sample_ivs_md_info *md_ptr = (hi_sample_ivs_md_info *)(args);
    hi_video_frame_info frm[HI_SAMPLE_IVE_MD_VPSS_CHN]; /* 0:base_frm, 1:ext_frm */
    hi_sample_md_vpss_hld hld = {0};
    hi_s32 vpss_chn[] = { HI_VPSS_CHN0, HI_VPSS_CHN1 };
    hi_s32 cur_idx = 0;
    hi_bool is_first_frm = HI_TRUE;

    sample_svp_check_exps_return(md_ptr == HI_NULL, HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR, "md_inf_ptr can't be null\n");

    /* Create chn */
    ret = hi_ivs_md_create_chn(hld.md_chn, &(md_ptr->md_attr));
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR, "hi_ivs_md_create_chn fail\n");

    while (g_stop_signal == HI_FALSE) {
        ret = hi_mpi_vpss_get_chn_frame(hld.vpss_grp, vpss_chn[1], &frm[1], HI_SAMPLE_IVE_MD_MILLIC_SEC);
        sample_svp_check_exps_continue(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Err(%#x),vpss_get_chn_frame failed, vpss_grp(%d), vpss_chn(%d)!\n", ret, hld.vpss_grp, vpss_chn[1]);

        ret = hi_mpi_vpss_get_chn_frame(hld.vpss_grp, vpss_chn[0], &frm[0], HI_SAMPLE_IVE_MD_MILLIC_SEC);
        sample_svp_check_failed_goto(ret, ext_free, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),vpss_get_chn_frame failed, VPSS_GRP(%d), VPSS_CHN(%d)!\n", ret, hld.vpss_grp, vpss_chn[0]);

        ret = sample_ivs_md_dma_data(cur_idx, &frm[1], md_ptr, &is_first_frm);
        sample_svp_check_failed_goto(ret, base_free, SAMPLE_SVP_ERR_LEVEL_ERROR, "dma data failed, Err(%#x)\n", ret);

        /* change idx */
        if (is_first_frm == HI_TRUE) {
            goto change_idx;
        }

        ret = hi_ivs_md_proc(hld.md_chn, &md_ptr->img[cur_idx], &md_ptr->img[1 - cur_idx], HI_NULL, &md_ptr->blob);
        sample_svp_check_failed_goto(ret, base_free, SAMPLE_SVP_ERR_LEVEL_ERROR, "ivs_md_proc fail,Err(%#x)\n", ret);

        sample_ivs_set_src_dst_size(&g_src_dst, md_ptr->md_attr.width, md_ptr->md_attr.height,
            frm[0].video_frame.width, frm[0].video_frame.height);
        ret = sample_common_ive_blob_to_rect(sample_svp_convert_addr_to_ptr(hi_ive_ccblob, md_ptr->blob.virt_addr),
            &(md_ptr->region), HI_SVP_RECT_NUM, HI_SAMPLE_IVE_MD_AREA_THR_STEP, &g_src_dst);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, base_free, SAMPLE_SVP_ERR_LEVEL_ERROR, "blob to rect failed!\n");

        /* Draw rect */
        ret = sample_common_svp_vgs_fill_rect(&frm[0], &md_ptr->region, 0x0000FF00);
        sample_svp_check_failed_err_level_goto(ret, base_free, "sample_svp_vgs_fill_rect fail,Err(%#x)\n", ret);

        ret = hi_mpi_venc_send_frame(hld.venc_chn, &frm[0], HI_SAMPLE_IVE_MD_MILLIC_SEC);
        sample_svp_check_failed_err_level_goto(ret, base_free, "sample_venc_send_frame fail,Error(%#x)\n", ret);

change_idx:
        /* Change reference and current frame index */
        cur_idx = 1 - cur_idx;
base_free:
        ret = hi_mpi_vpss_release_chn_frame(hld.vpss_grp, vpss_chn[0], &frm[0]);
        sample_svp_check_exps_trace(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Err(%#x),release_frame failed,grp(%d) chn(%d)!\n", ret, hld.vpss_grp, vpss_chn[0]);

ext_free:
        ret = hi_mpi_vpss_release_chn_frame(hld.vpss_grp, vpss_chn[1], &frm[1]);
        sample_svp_check_exps_trace(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Err(%#x),release_frame failed,grp(%d) chn(%d)!\n", ret, hld.vpss_grp, vpss_chn[1]);
    }

    /* destroy */
    ret = hi_ivs_md_destroy_chn(hld.md_chn);
    sample_svp_check_failed_trace(ret, SAMPLE_SVP_ERR_LEVEL_ERROR,  "hi_ivs_md_destroy_chn fail,Err(%#x)\n", ret);

    return HI_NULL;
}

static hi_s32 sample_ive_md_pause(hi_void)
{
    printf("---------------press Enter key to exit!---------------\n");
    if (g_stop_signal == HI_TRUE) {
        if (g_md_thread != 0) {
            pthread_join(g_md_thread, HI_NULL);
            g_md_thread = 0;
        }
        sample_ivs_md_uninit(&(g_md_info));
        (hi_void)memset_s(&g_md_info, sizeof(g_md_info), 0, sizeof(g_md_info));

        sample_common_svp_stop_vi_vpss_venc(&g_vi_config, &g_md_switch);
        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
        return HI_TRUE;
    }

    (void)getchar();

    if (g_stop_signal == HI_TRUE) {
        if (g_md_thread != 0) {
            pthread_join(g_md_thread, HI_NULL);
            g_md_thread = 0;
        }
        sample_ivs_md_uninit(&(g_md_info));
        (hi_void)memset_s(&g_md_info, sizeof(g_md_info), 0, sizeof(g_md_info));

        sample_common_svp_stop_vi_vpss_venc(&g_vi_config, &g_md_switch);
        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
        return HI_TRUE;
    }
    return HI_FALSE;
}

hi_void sample_ive_md(hi_void)
{
    hi_size pic_size;
    hi_pic_size pic_type = PIC_720P;
    hi_s32 ret;

    (hi_void)memset_s(&g_md_info, sizeof(g_md_info), 0, sizeof(g_md_info));
    /*
     * step 1: start vi vpss venc
     */
    ret = sample_common_svp_start_vi_vpss_venc(&g_vi_config, &g_md_switch, &pic_type);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_md_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_common_svp_start_vi_vpss_venc failed!\n", ret);

    pic_type = PIC_720P;
    ret = sample_comm_sys_get_pic_size(pic_type, &pic_size);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_md_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_comm_sys_get_pic_size failed!\n", ret);
    /*
     * step 2: Init Md
     */
    ret = sample_ivs_md_init(&g_md_info, pic_size.width, pic_size.height);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_md_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        " Error(%#x),sample_ivs_md_init failed!\n", ret);
    g_stop_signal = HI_FALSE;
    /*
     * step 3: Create work thread
     */
    ret = prctl(PR_SET_NAME, "ive_md_proc", 0, 0, 0);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_md_0, SAMPLE_SVP_ERR_LEVEL_ERROR, "set thread name failed!\n");
    ret = pthread_create(&g_md_thread, 0, sample_ivs_md_proc, (hi_void *)&g_md_info);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_md_0, SAMPLE_SVP_ERR_LEVEL_ERROR, "pthread_create failed!\n");

    ret = sample_ive_md_pause();
    sample_svp_check_exps_return_void(ret == HI_TRUE, SAMPLE_SVP_ERR_LEVEL_ERROR, "md exist!\n");
    g_stop_signal = HI_TRUE;
    pthread_join(g_md_thread, HI_NULL);
    g_md_thread = 0;

    sample_ivs_md_uninit(&(g_md_info));
    (hi_void)memset_s(&g_md_info, sizeof(g_md_info), 0, sizeof(g_md_info));

end_md_0:
    g_md_thread = 0;
    g_stop_signal = HI_TRUE;
    sample_common_svp_stop_vi_vpss_venc(&g_vi_config, &g_md_switch);
    return;
}

/*
 * function : Md sample signal handle
 */
hi_void sample_ive_md_handle_sig(hi_void)
{
    g_stop_signal = HI_TRUE;
}

/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "sample_aibnr.h"
#include "sample_aiisp_common.h"
#include "hi_mpi_aibnr.h"

#define VB_AIBNR_LINE_CNT  7
#define VB_AIBNR_WDR_CNT   7
#define VB_AIBNR_COMMON_VB_CNT 12

typedef struct {
    hi_vi_pipe video_pipe;
    hi_vi_pipe snap_pipe;
    pthread_t  thread_id;
    hi_bool    start;
} sample_aibnr_snap_thread_info;

static sample_aibnr_snap_thread_info g_snap_thread_info;
static hi_vb_pool g_aibnr_vb_pool = HI_VB_INVALID_POOL_ID;

//maohw
static hi_char g_model_file[256] = {0};

hi_void aibnr_dir_name(const hi_char *dir_name)
{
  snprintf_truncated_s(g_model_file, 256, "%s", dir_name);
}

static hi_s32 sample_aibnr_set_attr(hi_vi_pipe vi_pipe, sample_aibnr_param *aibnr_param)
{
    hi_s32 ret;
    hi_aibnr_attr aibnr_attr;

    ret = hi_mpi_aibnr_get_attr(vi_pipe, &aibnr_attr);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_aibnr_get_attr error\n");
        return ret;
    }

    aibnr_attr.enable = HI_TRUE;
    aibnr_attr.bnr_bypass = HI_FALSE;
    aibnr_attr.blend = aibnr_param->is_blend;
    aibnr_attr.op_type = HI_OP_MODE_MANUAL;
    aibnr_attr.manual_attr.sfs = 31; /* sfs: 31 */
    aibnr_attr.manual_attr.tfs = 31; /* tfs: 31 */
    ret = hi_mpi_aibnr_set_attr(vi_pipe, &aibnr_attr);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_aibnr_set_attr error\n");
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_aibnr_load_model(hi_size in_size, hi_aibnr_model *model_info, hi_s32 *model_id)
{
    hi_s32 ret;

    hi_aiisp_model *model = &model_info->model;

	//maohw
    if (in_size.width > 3072) { // 5.3M :3072x1728
        strcat(g_model_file, "/aibnr_model_4K.bin");
    }
    else 
    {
      strcat(g_model_file, "/aibnr_model_4M.bin");
    }

    printf("%s => model_file:[%s]\n", __func__, g_model_file);

    if (model->mem_info.virt_addr == TD_NULL) {
        ret = sample_aiisp_load_mem((hi_aiisp_mem_info *)&(model->mem_info), g_model_file);
        if (ret != HI_SUCCESS) {
            sample_print("sample_aiisp_load_mem error\n");
            return ret;
        }
    }

    ret = hi_mpi_aibnr_load_model(model_info, model_id);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_aibnr_load_model error(%#x)\n", ret);
        goto unload_mem;
    }

    return ret;

unload_mem:
    sample_aiisp_unload_mem((hi_aiisp_mem_info *)&(model->mem_info));
    return ret;
}

static hi_s32 sample_aibnr_load(hi_size in_size, hi_aibnr_model *model_info, hi_s32 *model_id,
    sample_aibnr_param *aibnr_param)
{
    hi_s32 ret;

    model_info->model.preempted_en = HI_FALSE;
    model_info->model.image_size.width  = in_size.width;
    model_info->model.image_size.height = in_size.height;
    model_info->is_wdr_mode = aibnr_param->is_wdr_mode;
    model_info->ref_mode = aibnr_param->ref_mode;

    if (model_info->ref_mode == HI_AIBNR_REF_MODE_NONE) {
        ret = sample_aibnr_load_model(in_size, model_info, &model_id[0]);
        if (ret != HI_SUCCESS) {
            return ret;
        }
    } else {
        model_info->is_wdr_mode = HI_FALSE;
        ret = sample_aibnr_load_model(in_size, model_info, &model_id[0]);
        if (ret != HI_SUCCESS) {
            return ret;
        }

        model_info->is_wdr_mode = HI_TRUE;
        ret = sample_aibnr_load_model(in_size, model_info, &model_id[1]);
        if (ret != HI_SUCCESS) {
            goto unload_cfg;
        }
    }

    return HI_SUCCESS;

unload_cfg:
    if (model_id[0] != -1) {
        (hi_void)hi_mpi_aibnr_unload_model(model_id[0]);
    }

    sample_aiisp_unload_mem((hi_aiisp_mem_info *)&(model_info->model.mem_info));

    return ret;
}

/*maohw static*/ hi_s32 sample_aibnr_start(hi_vi_pipe vi_pipe, hi_size in_size, hi_aibnr_model *model_info, hi_s32 *model_id,
    hi_s32 model_size, sample_aibnr_param *aibnr_param)
{
    hi_s32 ret = hi_mpi_aibnr_init();
    hi_aibnr_cfg cfg;
    hi_s32 i;

    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_aibnr_init error(%#x)\n", ret);
        return ret;
    }

    ret = sample_aibnr_load(in_size, model_info, model_id, aibnr_param);
    if (ret != HI_SUCCESS) {
        sample_print("sample_aibnr_load error(%#x)\n", ret);
        goto deinit;
    }

    cfg.ref_mode = aibnr_param->ref_mode; /* ref mode NORM: need reference frame; NONE: no reference frame */
    ret = hi_mpi_aibnr_set_cfg(vi_pipe, &cfg);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_aibnr_set_alg_cfg error(%#x)\n", ret);
        goto unload_model;
    }

    //maohw sample_get_char("enable");

    ret = hi_mpi_aibnr_enable(vi_pipe);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_aibnr_enable error(%#x)\n", ret);
        goto unload_model;
    }

    ret = sample_aibnr_set_attr(vi_pipe, aibnr_param);
    if (ret != HI_SUCCESS) {
        sample_print("sample_aibnr_set_attr error(%#x)\n", ret);
        goto aibnr_disable;
    }

    return ret;

aibnr_disable:
    hi_mpi_aibnr_disable(vi_pipe);
unload_model:
    for (i = 0;i < model_size; i++) {
        if (model_id[i] != -1) {
            (hi_void)hi_mpi_aibnr_unload_model(model_id[i]);
        }
    }
    sample_aiisp_unload_mem((hi_aiisp_mem_info *)&(model_info->model.mem_info));
deinit:
    hi_mpi_aibnr_exit();

    return ret;
}


static hi_s32 sample_aibnr_unload_exit(hi_vi_pipe vi_pipe, hi_aibnr_model *model, hi_s32 *model_id,
    hi_s32 model_size)
{
    hi_s32 return_value = HI_SUCCESS;
    hi_s32 ret;
    hi_s32 i;
    for (i = 0; i < model_size; i++) {
        if (model_id[i] != -1) {
            ret = hi_mpi_aibnr_unload_model(model_id[i]);
            if (ret != HI_SUCCESS) {
                return_value = HI_FAILURE;
                sample_print("hi_mpi_aibnr_unload_cfg error(%#x)\n", ret);
            }
        }
    }
    sample_aiisp_unload_mem((hi_aiisp_mem_info *)&(model->model.mem_info));
    hi_mpi_aibnr_exit();
    return return_value;
}


/*maohw static*/ hi_s32 sample_aibnr_stop(hi_vi_pipe vi_pipe, hi_aibnr_model *model, hi_s32 *model_id, hi_s32 model_size)
{
    hi_s32 ret1;
    hi_s32 ret2;

    ret1 = hi_mpi_aibnr_disable(vi_pipe);
    if (ret1 != HI_SUCCESS) {
        sample_print("hi_mpi_aibnr_disable false error(%#x)\n", ret1);
    }

    ret2 = sample_aibnr_unload_exit(vi_pipe, model, model_id, model_size);
    if (ret2 != HI_SUCCESS) {
        sample_print("sample_aibnr_unload_exit false error(%#x)\n", ret2);
    }

    if ((ret1 != HI_SUCCESS) || (ret2 != HI_SUCCESS)) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


static hi_void sample_aibnr_detach_vb(hi_vi_pipe vi_pipe)
{
    hi_s32 ret;

    ret = hi_mpi_vi_detach_aiisp_vb_pool(vi_pipe, HI_AIISP_TYPE_AIBNR);
    if (ret != HI_SUCCESS) {
        sample_print("vi aiisp detach vb pool failed.\n");
    }

    if (g_aibnr_vb_pool != HI_VB_INVALID_POOL_ID) {
        hi_mpi_vb_destroy_pool(g_aibnr_vb_pool);
        g_aibnr_vb_pool = HI_VB_INVALID_POOL_ID;
    }
}

/*maohw static*/ hi_void sample_aibnr_stop_vi(hi_vi_pipe vi_pipe, const sample_vi_cfg *vi_cfg)
{
    sample_aibnr_detach_vb(vi_pipe);
    sample_comm_vi_stop_vi(vi_cfg);
    sample_comm_sys_exit();
}

static hi_s32 sample_aibnr_attach_vb(hi_vi_pipe vi_pipe, hi_size *in_size, hi_u32 vb_cnt)
{
    hi_s32 ret;
    hi_s32 blk_size;
    hi_vb_pool vb_pool;
    hi_vb_pool_cfg vb_pool_cfg = {0};
    hi_aiisp_pool pool_attr = {0};

    blk_size = hi_aibnr_get_pic_buf_size(in_size->width, in_size->height);
    vb_pool_cfg.blk_size = blk_size;
    vb_pool_cfg.blk_cnt = vb_cnt;
    vb_pool_cfg.remap_mode = HI_VB_REMAP_MODE_NONE;
    vb_pool = hi_mpi_vb_create_pool(&vb_pool_cfg);
    if (vb_pool == HI_VB_INVALID_POOL_ID) {
        sample_print("aibnr create user pool failed!\n");
        return HI_FAILURE;
    }

#ifdef SAMPLE_MEM_SHARE_ENABLE
    hi_mpi_vb_pool_share_all(vb_pool);
#endif

    pool_attr.aiisp_type = HI_AIISP_TYPE_AIBNR;
    pool_attr.aibnr_pool.vb_pool = vb_pool;

    ret = hi_mpi_vi_attach_aiisp_vb_pool(vi_pipe, &pool_attr);
    if (ret != HI_SUCCESS) {
        hi_mpi_vb_destroy_pool(vb_pool);
        sample_print("vi aiisp attach vb pool failed.\n");
        return ret;
    }

    g_aibnr_vb_pool = vb_pool;

    return HI_SUCCESS;
}

/*maohw static*/ hi_s32 sample_aibnr_start_vi(hi_vi_pipe vi_pipe, const sample_vi_cfg *vi_cfg, hi_size *in_size,
    hi_u32 vb_cnt)
{
    hi_s32 ret;

    if (sample_aiisp_sys_init(in_size, VB_AIBNR_COMMON_VB_CNT) != HI_SUCCESS) {
        sample_print("sample_aiisp_sys_init failed.\n");
        return HI_FAILURE;
    }

    ret = sample_comm_vi_start_vi(vi_cfg);
    if (ret != HI_SUCCESS) {
        sample_print("start vi failed.\n");
        goto sys_exit;
    }

    ret = sample_aibnr_attach_vb(vi_pipe, in_size, vb_cnt);
    if (ret != HI_SUCCESS) {
        sample_print("attach vb failed.\n");
        goto stop_vi;
    }

    return HI_SUCCESS;

stop_vi:
    sample_comm_vi_stop_vi(vi_cfg);
sys_exit:
    sample_comm_sys_exit();

    return HI_FAILURE;
}

static hi_s32 sample_aibnr_switch_start_vi(hi_vi_pipe vi_pipe, const sample_vi_cfg *vi_cfg, hi_size *in_size)
{
    hi_s32 ret;
    hi_s32 blk_size;
    hi_vb_pool vb_pool;
    hi_vb_pool_cfg vb_pool_cfg = {0};
    hi_aiisp_pool pool_attr = {0};

    blk_size = hi_aibnr_get_pic_buf_size(in_size->width, in_size->height);
    vb_pool_cfg.blk_size = blk_size;
    vb_pool_cfg.blk_cnt = VB_AIBNR_WDR_CNT;
    vb_pool_cfg.remap_mode = HI_VB_REMAP_MODE_NONE;
    vb_pool = hi_mpi_vb_create_pool(&vb_pool_cfg);
    if (vb_pool == HI_VB_INVALID_POOL_ID) {
        sample_print("create user pool failed!\n");
        return HI_FAILURE;
    }

#ifdef SAMPLE_MEM_SHARE_ENABLE
    hi_mpi_vb_pool_share_all(vb_pool);
#endif

    ret = sample_comm_vi_mode_switch_start_vi(vi_cfg, HI_FALSE, (const hi_size *)&in_size);
    if (ret != HI_SUCCESS) {
        sample_print("start vi failed.\n");
        return ret;
    }
    pool_attr.aiisp_type = HI_AIISP_TYPE_AIBNR;
    pool_attr.aibnr_pool.vb_pool = vb_pool;

    ret = hi_mpi_vi_attach_aiisp_vb_pool(vi_pipe, &pool_attr);
    if (ret != HI_SUCCESS) {
        sample_print("vi aiisp attach vb pool failed.\n");
        return ret;
    }
    g_aibnr_vb_pool = vb_pool;
    return HI_SUCCESS;
}

/*maohw static*/ hi_s32 sample_aibnr_set_blc(hi_vi_pipe vi_pipe, sample_sns_type sns_type)
{
    hi_s32 i, j, ret;
    hi_isp_black_level_attr black_level_attr;

    ret = hi_mpi_isp_get_black_level_attr(vi_pipe, &black_level_attr);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_isp_get_black_level_attr failed.\n");
        return HI_FAILURE;
    }

    black_level_attr.user_black_level_en = HI_TRUE;

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        for (j = 0; j < HI_ISP_BAYER_CHN_NUM; j++) {
            black_level_attr.user_black_level[i][j] = 1200; /* user_black_level of aibnr default as 1200 */
        }
    }

    ret = hi_mpi_isp_set_black_level_attr(vi_pipe, &black_level_attr);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_isp_set_black_level_attr failed.\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*maohw static*/ hi_void sample_aibnr_update_cfg(hi_vi_pipe vi_pipe, sample_vi_cfg *vi_cfg)
{
    vi_cfg->pipe_info[vi_pipe].pipe_attr.compress_mode = HI_COMPRESS_MODE_NONE;
}

static hi_void sample_aibnr_update_snap_cfg(hi_vi_pipe vi_pipe, sample_vi_cfg *vi_cfg)
{
    vi_cfg->pipe_info[vi_pipe].pipe_attr.compress_mode = HI_COMPRESS_MODE_NONE;
    vi_cfg->pipe_info[vi_pipe].isp_need_run = HI_FALSE;
}

static hi_s32 sample_aibnr_switch_first_route_line(sample_sns_type sns_type, hi_aibnr_model *model_info,
    hi_s32 *model_id, hi_s32 model_size)
{
    const hi_vi_pipe vi_pipe = 0;
    const hi_vi_chn vi_chn = 0;
    hi_vpss_grp vpss_grp[1] = {0};
    const hi_u32 grp_num = 1;
    const hi_vpss_chn vpss_chn = 0;
    sample_vi_cfg vi_cfg;
    hi_size in_size;
    hi_s32 ret = HI_FAILURE;
    sample_aibnr_param aibnr_param = {HI_FALSE, HI_AIBNR_REF_MODE_NORM, HI_TRUE};

    sample_aiisp_get_default_cfg(sns_type, vi_pipe, &in_size, &vi_cfg);
    sample_aibnr_update_cfg(vi_pipe, &vi_cfg);

    if (sample_aibnr_start_vi(vi_pipe, &vi_cfg, &in_size, VB_AIBNR_WDR_CNT) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    sample_aiisp_sensor_cfg(vi_pipe, sns_type);

    sample_comm_vi_bind_vpss(vi_pipe, vi_chn, vpss_grp[0], vpss_chn);

    if (sample_aiisp_start_vpss(vpss_grp[0], &in_size) != HI_SUCCESS) {
        goto stop_vi;
    }

    if (sample_aiisp_start_venc_and_vo(vpss_grp, grp_num, &in_size) != HI_SUCCESS) {
        goto stop_vpss;
    }

    if (sample_aibnr_set_blc(vi_pipe, sns_type) != HI_SUCCESS) {
        goto stop_venc_and_vo;
    }

    if (sample_aibnr_start(vi_pipe, in_size, model_info, model_id, model_size, &aibnr_param) != HI_SUCCESS) {
        goto stop_venc_and_vo;
    }

    sample_get_char("exit line mode");

    ret = hi_mpi_aibnr_disable(vi_pipe);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_aibnr_disable false error(%#x)\n", ret);
    }

stop_venc_and_vo:
    sample_aiisp_stop_venc_and_vo(vpss_grp, grp_num);
stop_vpss:
    sample_aiisp_stop_vpss(vpss_grp[0]);
stop_vi:
    sample_aibnr_detach_vb(vi_pipe);
    sample_comm_vi_un_bind_vpss(vi_pipe, vi_chn, vpss_grp[0], vpss_chn);
    if (ret == HI_SUCCESS) {
        sample_comm_vi_mode_switch_stop_vi(&vi_cfg);
    } else {
        sample_comm_vi_stop_vi(&vi_cfg);
    }

    return ret;
}

static hi_s32 sample_aibnr_switch_second_route_wdr(sample_sns_type sns_type, hi_aibnr_model *model_info,
    hi_s32 *model_id, hi_s32 model_size)
{
    const hi_vi_pipe vi_pipe = 1;
    const hi_vi_pipe master_pipe = 0;
    const hi_vi_chn vi_chn = 0;
    hi_vpss_grp vpss_grp[1] = {0};
    const hi_vpss_chn vpss_chn = 0;
    hi_size in_size;
    sample_vi_cfg vi_cfg;
    hi_s32 ret = HI_FAILURE;
    sample_aibnr_param aibnr_param = {HI_TRUE, HI_AIBNR_REF_MODE_NORM, HI_FALSE};

    sample_comm_vi_get_size_by_sns_type(sns_type, &in_size);
    sample_comm_vi_get_default_vi_cfg(sns_type, &vi_cfg);

    sample_aiisp_get_default_cfg(sns_type, vi_pipe, &in_size, &vi_cfg);
    sample_aibnr_update_cfg(vi_pipe, &vi_cfg);

    if (sample_aibnr_switch_start_vi(vi_pipe, &vi_cfg, &in_size) != HI_SUCCESS) {
        goto unload_exit;
    }

    sample_aiisp_sensor_cfg(vi_pipe, sns_type);
    sample_comm_vi_bind_vpss(master_pipe, vi_chn, vpss_grp[0], vpss_chn);

    if (sample_aiisp_start_vpss(vpss_grp[0], &in_size) != HI_SUCCESS) {
        goto stop_vi;
    }

    if (sample_aiisp_start_venc_and_vo(vpss_grp, 1, &in_size) != HI_SUCCESS) {
        goto stop_vpss;
    }

    if (sample_aibnr_set_blc(master_pipe, sns_type) != HI_SUCCESS) {
        goto stop_venc_and_vo;
    }

    ret = hi_mpi_aibnr_enable(vi_pipe);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_aibnr_enable error(%#x)\n", ret);
        goto disable_aibnr;
    }

    sample_aibnr_set_attr(vi_pipe, &aibnr_param);

    sample_get_char("exit wdr mode");

disable_aibnr:
    hi_mpi_aibnr_disable(vi_pipe);
stop_venc_and_vo:
    sample_aiisp_stop_venc_and_vo(vpss_grp, 1);
stop_vpss:
    sample_aiisp_stop_vpss(vpss_grp[0]);
stop_vi:
    sample_aibnr_detach_vb(vi_pipe);
    sample_comm_vi_un_bind_vpss(master_pipe, vi_chn, vpss_grp[0], vpss_chn);
    sample_comm_vi_stop_vi(&vi_cfg);
unload_exit:
    sample_aibnr_unload_exit(vi_pipe, model_info, model_id, model_size);

    return ret;
}

hi_s32 sample_aibnr_line_switch_to_wdr(hi_void)
{
    hi_s32 ret;
    sample_vi_cfg vi_cfg;
    hi_size in_size;
    const hi_vi_pipe vi_pipe = 0;
    sample_sns_type sns_type = SENSOR0_TYPE;
    sample_sns_type wdr_sns_type = sample_aiisp_get_wdr_sns_type(SENSOR0_TYPE);
    hi_aibnr_model model_info = {0};
    hi_s32 model_id[2] = {-1, -1};
    hi_s32 model_size = (hi_s32)(sizeof(model_id) / sizeof(hi_s32));

    if (wdr_sns_type == SNS_TYPE_BUTT) {
        sample_print("unsupport sensor type:%d\n", sns_type);
        return HI_FAILURE;
    }

    sample_aiisp_get_default_cfg(sns_type, vi_pipe, &in_size, &vi_cfg);
    sample_aibnr_update_cfg(vi_pipe, &vi_cfg);

    if (sample_aiisp_sys_init(&in_size, VB_AIBNR_LINE_CNT) != HI_SUCCESS) {
        sample_print("sys init failed!\n");
        return HI_FAILURE;
    }

    ret = sample_aibnr_switch_first_route_line(sns_type, &model_info, model_id, model_size);
    if (ret != HI_SUCCESS) {
        sample_comm_sys_exit();
        sample_print("start first route line failed!\n");
        return ret;
    }

    printf("switch to wdr mode\n");

    ret = sample_aibnr_switch_second_route_wdr(wdr_sns_type, &model_info, model_id, model_size);
    sample_comm_sys_exit();

    return ret;
}

static hi_s32 sample_aibnr_check_support(hi_vi_pipe vi_pipe, sample_aibnr_param *aibnr_param)
{
    if (aibnr_param->is_wdr_mode == TD_TRUE) {
        if (aibnr_param->is_blend != HI_FALSE) {
            sample_print("normal_blend must be false in wdr mode\n");
            return HI_FAILURE;
        }

        if (vi_pipe >= HI_VI_MAX_PHYS_PIPE_NUM) {
            sample_print("vi_pipe must be phy pipe in wdr mode\n");
            return HI_FAILURE;
        }
    }

    /* bnr_bypass must be false when attr->enable is false */
    /* blend must be false when bnr_bypass is true */
    /* vi must be offline */

    return HI_SUCCESS;
}

hi_s32 sample_aibnr(sample_aibnr_param *aibnr_param)
{
    hi_s32 ret = HI_FAILURE;
    sample_vi_cfg vi_cfg = {0};
    sample_sns_type sns_type = aibnr_param->is_wdr_mode ? sample_aiisp_get_wdr_sns_type(SENSOR0_TYPE) : SENSOR0_TYPE;
    const hi_u32 vb_cnt = aibnr_param->is_wdr_mode ? VB_AIBNR_WDR_CNT : VB_AIBNR_LINE_CNT;
    const hi_vi_pipe vi_pipe = aibnr_param->is_wdr_mode ? 1 : 0;
    hi_vi_pipe master_pipe = 0;
    const hi_vi_chn vi_chn = 0;
    hi_vpss_grp vpss_grp[1] = {0};
    hi_size in_size = {0};
    hi_aibnr_model model_info = {0};
    hi_s32 model_id[2] = {-1, -1};
    hi_s32 model_size = (hi_s32)(sizeof(model_id) / sizeof(hi_s32));

    if (sample_aibnr_check_support(vi_pipe, aibnr_param) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    sample_aiisp_get_default_cfg(sns_type, vi_pipe, &in_size, &vi_cfg);
    sample_aibnr_update_cfg(vi_pipe, &vi_cfg);

    if (sample_aibnr_start_vi(vi_pipe, &vi_cfg, &in_size, vb_cnt) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    sample_aiisp_sensor_cfg(vi_pipe, sns_type);

    sample_comm_vi_bind_vpss(master_pipe, vi_chn, vpss_grp[0], 0);
    if (sample_aiisp_start_vpss(vpss_grp[0], &in_size) != HI_SUCCESS) {
        goto stop_vi;
    }

    if (sample_aiisp_start_venc_and_vo(vpss_grp, sizeof(vpss_grp) / sizeof(vpss_grp[0]), &in_size) != HI_SUCCESS) {
        goto stop_vpss;
    }

    if (sample_aibnr_set_blc(master_pipe, sns_type) != HI_SUCCESS) {
        goto stop_venc_and_vo;
    }

    if (sample_aibnr_start(vi_pipe, in_size, &model_info, model_id, model_size, aibnr_param) != HI_SUCCESS) {
        goto stop_venc_and_vo;
    }

    if (sample_aiisp_set_long_frame_mode(master_pipe, aibnr_param->is_wdr_mode) != HI_SUCCESS) {
        goto stop_aibnr;
    }

    sample_get_char("disable");

    ret = HI_SUCCESS;

stop_aibnr:
    sample_aibnr_stop(vi_pipe, &model_info, model_id, model_size);
    sample_get_char("exit");
stop_venc_and_vo:
    sample_aiisp_stop_venc_and_vo(vpss_grp, sizeof(vpss_grp) / sizeof(vpss_grp[0]));
stop_vpss:
    sample_aiisp_stop_vpss(vpss_grp[0]);
stop_vi:
    sample_comm_vi_un_bind_vpss(master_pipe, vi_chn, vpss_grp[0], 0);
    sample_aibnr_stop_vi(vi_pipe, &vi_cfg);
    return ret;
}

static hi_void *sample_aibnr_snap_thread(hi_void *param)
{
    hi_s32 ret;
    const hi_s32 milli_sec = 1000; /* 1000: wait 1000ms */
    hi_video_frame_info get_frame_info;
    hi_vi_frame_dump_attr dump_attr;
    sample_aibnr_snap_thread_info *thread_info = (sample_aibnr_snap_thread_info *)param;
    const hi_video_frame_info *send_frame_info[1];

    dump_attr.enable = HI_TRUE;
    dump_attr.depth = 3; /* 3: dump depth is 3 */
    ret = hi_mpi_vi_set_pipe_frame_dump_attr(thread_info->video_pipe, &dump_attr);
    if (ret != HI_SUCCESS) {
        sample_print("set pipe frame dump attr failed! ret:0x%x\n", ret);
        return HI_NULL;
    }

    ret = hi_mpi_vi_set_pipe_frame_source(thread_info->snap_pipe, HI_VI_PIPE_FRAME_SOURCE_USER);
    if (ret != HI_SUCCESS) {
        sample_print("vi set pipe frame source failed!\n");
        return HI_NULL;
    }

    while (thread_info->start == HI_TRUE) {
        ret = hi_mpi_vi_get_pipe_frame(thread_info->video_pipe, &get_frame_info, milli_sec);
        if (ret != HI_SUCCESS) {
            break;
        }

        ret = hi_mpi_isp_run_once(thread_info->snap_pipe);
        if (ret != HI_SUCCESS) {
            sample_print("isp run once failed!\n");
            goto exit0;
        }

        send_frame_info[0] = &get_frame_info;
        ret = hi_mpi_vi_send_pipe_raw(thread_info->snap_pipe, send_frame_info, 1, milli_sec);
        if (ret != HI_SUCCESS) {
            sample_print("vi send pipe bas dump frame failed!\n");
        }

exit0:
        ret = hi_mpi_vi_release_pipe_frame(thread_info->video_pipe, &get_frame_info);
        if (ret != HI_SUCCESS) {
            sample_print("release pipe frame failed!\n");
            return HI_NULL;
        }
    }

    return HI_NULL;
}

static hi_s32 sample_aibnr_snap_create_thread(hi_vi_pipe video_pipe, hi_vi_pipe capture_pipe)
{
    hi_s32 ret;

    g_snap_thread_info.video_pipe   = video_pipe;
    g_snap_thread_info.snap_pipe = capture_pipe;
    ret = pthread_create(&g_snap_thread_info.thread_id, HI_NULL, sample_aibnr_snap_thread, &g_snap_thread_info);
    if (ret != 0) {
        sample_print("create capture thread failed!\n");
        return HI_FAILURE;
    }
    g_snap_thread_info.start = HI_TRUE;

    return HI_SUCCESS;
}

static hi_void sample_aibnr_snap_set_vi_cfg(sample_sns_type sns_type, hi_vi_pipe vi_pipe,
    hi_size *size, sample_vi_cfg *vi_cfg)
{
    sample_aiisp_get_default_cfg(sns_type, vi_pipe, size, vi_cfg);
    sample_aibnr_update_snap_cfg(vi_pipe, vi_cfg);
}

static hi_void sample_aibnr_snap_destroy_thread(hi_void)
{
    if (g_snap_thread_info.start == HI_TRUE) {
        g_snap_thread_info.start = HI_FALSE;
        pthread_join(g_snap_thread_info.thread_id, NULL);
    }
}

hi_s32 sample_aibnr_snap(sample_aibnr_param *aibnr_param)
{
    hi_s32 ret = HI_FAILURE;
    sample_vi_cfg vi_cfg;
    sample_sns_type sns_type =  SENSOR0_TYPE;
    const hi_vi_pipe vi_pipe = 0;
    const hi_vi_chn vi_chn = 0;
    hi_vpss_grp vpss_grp[1] = {0};
    hi_size in_size;
    hi_aibnr_model model_info = {0};
    hi_s32 model_id[2] = {-1, -1};
    hi_s32 model_size = (hi_s32)(sizeof(model_id) / sizeof(hi_s32));
    const hi_u32 vb_cnt = aibnr_param->is_wdr_mode ? VB_AIBNR_WDR_CNT : VB_AIBNR_LINE_CNT;

    sample_aibnr_snap_set_vi_cfg(sns_type, vi_pipe, &in_size, &vi_cfg);

    if (sample_aibnr_check_support(vi_pipe, aibnr_param) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (sample_aibnr_start_vi(vi_pipe, &vi_cfg, &in_size, vb_cnt) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    sample_aiisp_sensor_cfg(vi_pipe, sns_type);

    sample_comm_vi_bind_vpss(vi_pipe, vi_chn, vpss_grp[0], 0);
    if (sample_aiisp_start_vpss(vpss_grp[0], &in_size) != HI_SUCCESS) {
        goto stop_vi;
    }

    if (sample_aiisp_start_venc_and_vo(vpss_grp, sizeof(vpss_grp) / sizeof(vpss_grp[0]), &in_size) != HI_SUCCESS) {
        goto stop_vpss;
    }

    if (sample_aibnr_set_blc(vi_pipe, sns_type) != HI_SUCCESS) {
        goto stop_venc_and_vo;
    }

    if (sample_aibnr_start(vi_pipe, in_size, &model_info, model_id, model_size, aibnr_param) != HI_SUCCESS) {
        goto stop_venc_and_vo;
    }

    if (sample_aibnr_snap_create_thread(vi_pipe, vi_pipe) != HI_SUCCESS) {
        goto stop_aibnr;
    }

    sample_get_char("exit");

    sample_aibnr_snap_destroy_thread();

    ret = HI_SUCCESS;

stop_aibnr:
    sample_aibnr_stop(vi_pipe, &model_info, model_id, model_size);
stop_venc_and_vo:
    sample_aiisp_stop_venc_and_vo(vpss_grp, sizeof(vpss_grp) / sizeof(vpss_grp[0]));
stop_vpss:
    sample_aiisp_stop_vpss(vpss_grp[0]);
stop_vi:
    sample_comm_vi_un_bind_vpss(vi_pipe, vi_chn, vpss_grp[0], 0);
    sample_aibnr_stop_vi(vi_pipe, &vi_cfg);
    return ret;
}

/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#include "sample_aidrc.h"
#include "sample_aiisp_common.h"
#include "hi_mpi_aidrc.h"

#define VB_AIDRC_COMMON_CNT   14
#define VB_AIDRC_IN_CNT   3
#define VB_AIDRC_OUT_CNT   3

static hi_vb_pool g_aidrc_out_vb_pool = HI_VB_INVALID_POOL_ID;
static hi_vb_pool g_aidrc_in_vb_pool = HI_VB_INVALID_POOL_ID;
static hi_char g_model_file[256] = {0}; 

static hi_s32 sample_aidrc_set_attr(hi_vi_pipe vi_pipe, sample_aidrc_param *aidrc_param)
{
    hi_s32 ret;
    hi_aidrc_attr aidrc_attr;

    ret = hi_mpi_aidrc_get_attr(vi_pipe, &aidrc_attr);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_aidrc_get_attr error\n");
        return ret;
    }

    aidrc_attr.enable = HI_TRUE;
    aidrc_attr.param.strength = 7; /* strength: 7 */
    aidrc_attr.param.threshold = 1; /* threshold: 1 */

    ret = hi_mpi_aidrc_set_attr(vi_pipe, &aidrc_attr);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_aidrc_set_attr error\n");
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_aidrc_load_model(hi_size *in_size, hi_aidrc_model *model_info, hi_s32 *model_id)
{
    hi_s32 ret;
    hi_aiisp_model *model = &model_info->model;

    model_info->model.preempted_en = HI_FALSE;
    model_info->model.image_size.width  = in_size->width;
    model_info->model.image_size.height = in_size->height;

    if (model->mem_info.virt_addr == TD_NULL) {
        ret = sample_aiisp_load_mem((hi_aiisp_mem_info *)&(model->mem_info), g_model_file);
        if (ret != HI_SUCCESS) {
            sample_print("sample_aiisp_load_mem error\n");
            return ret;
        }
    }

    ret = hi_mpi_aidrc_load_model(model_info, model_id);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_aidrc_load_model error(%#x)\n", ret);
        goto unload_mem;
    }

    return ret;

unload_mem:
    sample_aiisp_unload_mem((hi_aiisp_mem_info *)&(model->mem_info));
    return ret;
}

/*maohw static*/  hi_s32 sample_aidrc_start(hi_vi_pipe vi_pipe, hi_size in_size, hi_aidrc_model *model_info,
    hi_s32 *model_id, sample_aidrc_param *aidrc_param)
{
    hi_s32 ret = hi_mpi_aidrc_init();
    hi_aidrc_cfg cfg;

    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_aidrc_init error(%#x)\n", ret);
        return ret;
    }
    
  	//maohw
  	strcat(g_model_file, sample_aiisp_dir_name(NULL));
  	strcat(g_model_file, "/aidrc_model.bin");
  	printf("%s => model_file:[%s]\n", __func__, g_model_file);

    model_info->mode = aidrc_param->mode;

    ret = sample_aidrc_load_model(&in_size, model_info, &model_id[0]);
    if (ret != HI_SUCCESS) {
        goto task_exit;
    }

    ret = hi_mpi_aidrc_get_cfg(vi_pipe, &cfg);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_aidrc_get_cfg error(%#x)\n", ret);
        goto unload_cfg;
    }

    cfg.mode = aidrc_param->mode;
    ret = hi_mpi_aidrc_set_cfg(vi_pipe, &cfg);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_aidrc_set_alg_cfg error(%#x)\n", ret);
        goto unload_cfg;
    }

    ret = hi_mpi_aidrc_enable(vi_pipe);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_aidrc_enable error(%#x)\n", ret);
        goto unload_cfg;
    }

    ret = sample_aidrc_set_attr(vi_pipe, aidrc_param);
    if (ret != HI_SUCCESS) {
        sample_print("sample_aidrc_set_attr error(%#x)\n", ret);
        goto aidrc_disable;
    }

    return ret;

aidrc_disable:
    hi_mpi_aidrc_disable(vi_pipe);
unload_cfg:
    hi_mpi_aidrc_unload_model(model_id[0]);
    sample_aiisp_unload_mem((hi_aiisp_mem_info *)&(model_info->model.mem_info));
task_exit:
    hi_mpi_aidrc_exit();

    return ret;
}

/*maohw static*/  hi_s32 sample_aidrc_stop(hi_vi_pipe vi_pipe, hi_aidrc_model *model, hi_s32 *model_id, hi_s32 model_size)
{
    hi_s32 return_value = HI_SUCCESS;
    hi_s32 ret;
    hi_s32 i;

    ret = hi_mpi_aidrc_disable(vi_pipe);
    if (ret != HI_SUCCESS) {
        return_value = HI_FAILURE;
        sample_print("hi_mpi_aidrc_disable false error(%#x)\n", ret);
    }
    for (i = 0; i < model_size; i++) {
        if (model_id[i] != -1) {
            ret = hi_mpi_aidrc_unload_model(model_id[i]);
            if (ret != HI_SUCCESS) {
                return_value = HI_FAILURE;
                sample_print("hi_mpi_aidrc_unload_cfg error(%#x)\n", ret);
            }
        }
    }

    sample_aiisp_unload_mem((hi_aiisp_mem_info *)&(model->model.mem_info));

    hi_mpi_aidrc_exit();

    return return_value;
}

static hi_s32 sample_aidrc_create_in_vb(hi_size *in_size)
{
    hi_vb_pool in_vb_pool;
    hi_vb_pool_cfg vb_pool_cfg = {0};

    vb_pool_cfg.blk_size = hi_aidrc_get_in_buf_size(in_size->width, in_size->height);
    vb_pool_cfg.blk_cnt = VB_AIDRC_IN_CNT;
    vb_pool_cfg.remap_mode = HI_VB_REMAP_MODE_NONE;
    in_vb_pool = hi_mpi_vb_create_pool(&vb_pool_cfg);
    if (in_vb_pool == HI_VB_INVALID_POOL_ID) {
        sample_print("create user pool failed!\n");
        return HI_FAILURE;
    }

#ifdef SAMPLE_MEM_SHARE_ENABLE
    hi_mpi_vb_pool_share_all(in_vb_pool);
#endif

    g_aidrc_in_vb_pool = in_vb_pool;

    return HI_SUCCESS;
}

static hi_void sample_adrc_destroy_in_vb(hi_void)
{
    if (g_aidrc_in_vb_pool != HI_VB_INVALID_POOL_ID) {
        hi_mpi_vb_destroy_pool(g_aidrc_in_vb_pool);
        g_aidrc_in_vb_pool = HI_VB_INVALID_POOL_ID;
    }
}

static hi_s32 sample_aidrc_create_out_vb(hi_vi_pipe vi_pipe, hi_size *in_size, hi_aidrc_mode mode)
{
    hi_s32 blk_size;
    hi_vb_pool vb_pool;
    hi_vb_pool_cfg vb_pool_cfg = {0};
    hi_bool advanced_mode = (mode == HI_AIDRC_MODE_ADVANCED) ? TD_TRUE : TD_FALSE;

    if (g_aidrc_out_vb_pool != HI_VB_INVALID_POOL_ID) {
        sample_print("pipe(%d) has been attached with pool(%u)\n", vi_pipe, g_aidrc_out_vb_pool);
        return HI_SUCCESS;
    }

    blk_size = hi_aidrc_get_out_buf_size(in_size->width, in_size->height, advanced_mode);
    vb_pool_cfg.blk_size = blk_size;
    vb_pool_cfg.blk_cnt = VB_AIDRC_OUT_CNT;
    vb_pool_cfg.remap_mode = HI_VB_REMAP_MODE_NONE;
    vb_pool = hi_mpi_vb_create_pool(&vb_pool_cfg);
    if (vb_pool == HI_VB_INVALID_POOL_ID) {
        sample_print("create user pool failed!\n");
        return HI_FAILURE;
    }

#ifdef SAMPLE_MEM_SHARE_ENABLE
    hi_mpi_vb_pool_share_all(vb_pool);
#endif

    g_aidrc_out_vb_pool = vb_pool;

    return HI_SUCCESS;
}

static hi_void sample_adrc_destroy_out_vb(hi_void)
{
    if (g_aidrc_out_vb_pool != HI_VB_INVALID_POOL_ID) {
        hi_mpi_vb_destroy_pool(g_aidrc_out_vb_pool);
        g_aidrc_out_vb_pool = HI_VB_INVALID_POOL_ID;
    }
}

static hi_s32 sample_aidrc_attach_vb(hi_vi_pipe vi_pipe, const sample_vi_cfg *vi_cfg, hi_size *in_size,
    hi_aidrc_mode mode)
{
    hi_s32 ret;
    hi_aiisp_pool pool_attr = {0};

    ret = sample_aidrc_create_in_vb(in_size);
    if (ret != HI_SUCCESS) {
        sample_print("create in vb pool failed.\n");
        return ret;
    }

    ret = sample_aidrc_create_out_vb(vi_pipe, in_size, mode);
    if (ret != HI_SUCCESS) {
        sample_adrc_destroy_in_vb();
        sample_print("create in vb pool failed.\n");
        return ret;
    }

    pool_attr.aiisp_type = HI_AIISP_TYPE_AIDRC;
    pool_attr.aidrc_pool.in_vb_pool = g_aidrc_in_vb_pool;
    pool_attr.aidrc_pool.out_vb_pool = g_aidrc_out_vb_pool;
    ret = hi_mpi_vi_attach_aiisp_vb_pool(vi_pipe, &pool_attr);
    if (ret != HI_SUCCESS) {
        sample_adrc_destroy_in_vb();
        sample_adrc_destroy_out_vb();
        sample_print("vi aiisp attach vb pool failed.\n");
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_aidrc_detach_vb(hi_vi_pipe vi_pipe)
{
    hi_s32 ret;

    ret = hi_mpi_vi_detach_aiisp_vb_pool(vi_pipe, HI_AIISP_TYPE_AIDRC);
    if (ret != HI_SUCCESS) {
        sample_print("vi aiisp detach vb pool failed.\n");
    }

    sample_adrc_destroy_in_vb();
    sample_adrc_destroy_out_vb();

    return HI_SUCCESS;
}
/*maohw static */hi_s32 sample_aidrc_start_vi(hi_vi_pipe vi_pipe, const sample_vi_cfg *vi_cfg, hi_size *in_size,
    hi_u32 vb_cnt, hi_aidrc_mode mode)
{
    hi_s32 ret;

    if (sample_aiisp_sys_init(in_size, vb_cnt) != HI_SUCCESS) {
        sample_print("sample_aiisp_sys_init failed.\n");
        return HI_FAILURE;
    }

    ret = sample_comm_vi_start_vi(vi_cfg);
    if (ret != HI_SUCCESS) {
        sample_print("start vi failed.\n");
        goto sys_exit;
    }

    ret = sample_aidrc_attach_vb(vi_pipe, vi_cfg, in_size, mode);
    if (ret != HI_SUCCESS) {
        sample_print("sample_aidrc_attach_vb failed.\n");
        goto stop_vi;
    }

    return HI_SUCCESS;

stop_vi:
    sample_comm_vi_stop_vi(vi_cfg);
sys_exit:
    sample_comm_sys_exit();

    return HI_FAILURE;
}

/*maohw static*/ hi_void sample_adrc_stop_vi(hi_vi_pipe vi_pipe, const sample_vi_cfg *vi_cfg)
{
    sample_aidrc_detach_vb(vi_pipe);
    sample_comm_vi_stop_vi(vi_cfg);
    sample_comm_sys_exit();
}

hi_s32 sample_aidrc(sample_aidrc_param *aidrc_param)
{
    hi_s32 ret = HI_FAILURE;
    sample_vi_cfg vi_cfg = {0};
    sample_sns_type sns_type = aidrc_param->is_wdr_mode ? sample_aiisp_get_wdr_sns_type(SENSOR0_TYPE) : SENSOR0_TYPE;
    const hi_u32 vb_cnt = VB_AIDRC_COMMON_CNT;
    const hi_vi_pipe vi_pipe = 0;
    const hi_vi_chn vi_chn = 0;
    hi_vpss_grp vpss_grp[1] = {0};
    hi_size in_size = {0};
    hi_aidrc_model  model_info = {0};
    hi_s32 model_id[2] = {-1, -1};
    hi_s32 model_size = (hi_s32)(sizeof(model_id) / sizeof(hi_s32));

    sample_aiisp_get_default_cfg(sns_type, vi_pipe, &in_size, &vi_cfg);
    vi_cfg.pipe_info[0].bnr_bnf_num = 3; /* 3: aidrc should set bnr buf_num 3 */

    if (sample_aidrc_start_vi(vi_pipe, &vi_cfg, &in_size, vb_cnt, aidrc_param->mode) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    sample_comm_vi_bind_vpss(vi_pipe, vi_chn, vpss_grp[0], 0);
    if (sample_aiisp_start_vpss(vpss_grp[0], &in_size) != HI_SUCCESS) {
        goto stop_vi;
    }

    if (sample_aiisp_start_venc_and_vo(vpss_grp, sizeof(vpss_grp) / sizeof(vpss_grp[0]), &in_size) != HI_SUCCESS) {
        goto stop_vpss;
    }
    sample_get_char("enable aidrc");

    if (sample_aidrc_start(vi_pipe, in_size, &model_info, model_id, aidrc_param) != HI_SUCCESS) {
        goto stop_venc_and_vo;
    }

    sample_get_char("disable");

    ret = HI_SUCCESS;

    sample_aidrc_stop(vi_pipe, &model_info, model_id, model_size);

    sample_get_char("exit");

stop_venc_and_vo:
    sample_aiisp_stop_venc_and_vo(vpss_grp, sizeof(vpss_grp) / sizeof(vpss_grp[0]));
stop_vpss:
    sample_aiisp_stop_vpss(vpss_grp[0]);
stop_vi:
    sample_comm_vi_un_bind_vpss(vi_pipe, vi_chn, vpss_grp[0], 0);
    sample_adrc_stop_vi(vi_pipe, &vi_cfg);
    return ret;
}


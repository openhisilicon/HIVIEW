/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "sample_common_svp_npu_model.h"
#include <limits.h>
#include "svp_acl_rt.h"
#include "svp_acl_ext.h"
#include "sample_common_svp.h"

#define SAMPLE_SVP_NPU_EVEN 2
#define SAMPLE_SVP_NPU_H_DIM_IDX 2
#define SAMPLE_SVP_NPU_W_DIM_IDX 3
#define SAMPLE_SVP_NPU_DIM_NUM 4
#define SAMPLE_SVP_NPU_JOINT_COORDS_NUM 17
#define SAMPLE_SVP_NPU_LINE_THICK 6

static sample_svp_npu_model_info g_svp_npu_model[SAMPLE_SVP_NPU_MAX_MODEL_NUM] = {0};

static hi_s32 sample_svp_npu_get_model_base_info(hi_u32 model_index)
{
    svp_acl_error ret;

    g_svp_npu_model[model_index].input_num = svp_acl_mdl_get_num_inputs(g_svp_npu_model[model_index].model_desc);
    sample_svp_check_exps_return(g_svp_npu_model[model_index].input_num < SAMPLE_SVP_NPU_EXTRA_INPUT_NUM + 1,
        HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get input num failed!\n");

    g_svp_npu_model[model_index].output_num = svp_acl_mdl_get_num_outputs(g_svp_npu_model[model_index].model_desc);
    sample_svp_check_exps_return(g_svp_npu_model[model_index].output_num < 1,
        HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get output num failed!\n");

    ret = svp_acl_mdl_get_input_index_by_name(g_svp_npu_model[model_index].model_desc,
        SVP_ACL_DYNAMIC_TENSOR_NAME, &g_svp_npu_model[model_index].dynamic_batch_idx);
    sample_svp_check_exps_return(ret != SVP_ACL_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get dynamic batch idx failed, model id is %u, error code is %d!\n", model_index, ret);

    return HI_SUCCESS;
}

static hi_s32 sample_svp_npu_get_line_num_and_line_byte_num(const sample_svp_npu_task_info *task, hi_u32 idx,
    hi_bool is_input, hi_u32 *total_line_num, hi_u32 *line_byte_num)
{
    hi_s32 ret;
    hi_u32 i;
    svp_acl_mdl_io_dims dims;
    svp_acl_data_type data_type;
    size_t data_size;
    svp_acl_aipp_info aipp_info = { 0 };

    if (is_input == HI_TRUE) {
        ret = svp_acl_mdl_get_input_dims(g_svp_npu_model[task->cfg.model_idx].model_desc, idx, &dims);
    } else {
        ret = svp_acl_mdl_get_output_dims(g_svp_npu_model[task->cfg.model_idx].model_desc, idx, &dims);
    }
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get %u-th input/output dims failed!\n", idx);

    if (is_input == HI_TRUE) {
        data_type = svp_acl_mdl_get_input_data_type(g_svp_npu_model[task->cfg.model_idx].model_desc, idx);
    } else {
        data_type = svp_acl_mdl_get_output_data_type(g_svp_npu_model[task->cfg.model_idx].model_desc, idx);
    }
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get %u-th input/output data type failed!\n", idx);

    data_size = svp_acl_data_type_size(data_type);
    sample_svp_check_exps_return(data_size == 0, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get data size failed!\n");

    *line_byte_num = dims.dims[dims.dim_count - 1] *
        ((data_size + SAMPLE_SVP_NPU_BYTE_BIT_NUM - 1) / SAMPLE_SVP_NPU_BYTE_BIT_NUM);

    *total_line_num = 1;
    for (i = 0; i < dims.dim_count - 1; i++) {
        *total_line_num *= dims.dims[i];
    }

    if (is_input == HI_TRUE) {
        ret = svp_acl_mdl_get_first_aipp_info(g_svp_npu_model[task->cfg.model_idx].model_id, idx, &aipp_info);
        sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get %u-th input aipp_info failed!\n", idx);

        if (aipp_info.aipp_format == SVP_ACL_YUV420SP_U8 || aipp_info.aipp_format == SVP_ACL_YVU420SP_U8) {
            *total_line_num /= SAMPLE_SVP_NPU_EVEN;
        }
    }

    /* lstm xt line num */
    if ((task->cfg.total_t != 0) && (idx == 0)) {
        sample_svp_check_exps_return(task->cfg.total_t > dims.dims[0], HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "total t(%u) can't be greater than max total t(%ld)!\n", task->cfg.total_t, dims.dims[0]);
        *total_line_num /= dims.dims[0];
        *total_line_num *= task->cfg.total_t;
    }
    return HI_SUCCESS;
}

static hi_s32 sample_svp_check_task_cfg(const sample_svp_npu_task_info *task)
{
    sample_svp_check_exps_return(task == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "task is NULL!\n");

    sample_svp_check_exps_return(task->cfg.max_batch_num == 0, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "max_batch_num(%u) is 0!\n", task->cfg.max_batch_num);

    sample_svp_check_exps_return(task->cfg.dynamic_batch_num == 0, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "dynamic_batch_num(%u) is 0!\n", task->cfg.dynamic_batch_num);

    sample_svp_check_exps_return(task->cfg.total_t != 0 && task->cfg.dynamic_batch_num != 1, HI_FAILURE,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "dynamic_batch_num(%u) should be 1 when total_t(%u) is not 0!\n",
        task->cfg.dynamic_batch_num, task->cfg.total_t);

    sample_svp_check_exps_return((task->cfg.is_cached != HI_TRUE && task->cfg.is_cached != HI_FALSE), HI_FAILURE,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "is_cached(%u) should be [%u, %u]!\n", task->cfg.is_cached, HI_FALSE, HI_TRUE);

    sample_svp_check_exps_return(task->cfg.model_idx >= SAMPLE_SVP_NPU_MAX_MODEL_NUM, HI_FAILURE,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "model_idx(%u) should be less than %u!\n",
        task->cfg.model_idx, SAMPLE_SVP_NPU_MAX_MODEL_NUM);

    sample_svp_check_exps_return(g_svp_npu_model[task->cfg.model_idx].model_desc == HI_NULL, HI_FAILURE,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "%u-th model_desc is NULL!\n", task->cfg.model_idx);
    return HI_SUCCESS;
}

static hi_s32 sample_common_svp_npu_check_input_src_file_num(sample_svp_npu_model_info *model_info,
    hi_u32 file_num)
{
    size_t input_num, attached_index;
    svp_acl_mdl_aipp_type type;
    hi_u32 aipp_num = 0;
    hi_u32 i;
    hi_s32 ret;

    input_num = model_info->input_num - SAMPLE_SVP_NPU_EXTRA_INPUT_NUM;
    for (i = 0; i < input_num; i++) {
        ret = svp_acl_mdl_get_input_aipp_type(model_info->model_id, i, &type, &attached_index);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, TEST_PICO_ERR_LEVEL_ERROR, "ret:%d\n", ret);

        if (type == SVP_ACL_DATA_WITH_DYNAMIC_AIPP) {
            aipp_num++;
        }
    }

    input_num -= aipp_num;
    sample_svp_check_exps_return(input_num != file_num, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "input file num(%u) should be equal to %zu!\n", file_num, input_num);

    return HI_SUCCESS;
}

hi_s32 sample_common_svp_npu_get_input_data(const hi_char *src[], hi_u32 file_num,
    const sample_svp_npu_task_info *task)
{
    hi_s32 ret;
    hi_u32 i, line, total_line_num, line_byte_num;
    hi_char path[PATH_MAX + 1] = { 0 };
    size_t stride, size;
    FILE *fp = HI_NULL;
    hi_void *data = HI_NULL;
    svp_acl_data_buffer *data_buffer = HI_NULL;

    if (sample_svp_check_task_cfg(task) != HI_SUCCESS) {
        sample_svp_trace_err("check task cfg failed!\n");
        return HI_FAILURE;
    }
    sample_svp_check_exps_return(src == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "src is NULL!\n");

    if (sample_common_svp_npu_check_input_src_file_num(&g_svp_npu_model[task->cfg.model_idx], file_num) != HI_SUCCESS) {
        sample_svp_trace_err("check file num failed!\n");
        return HI_FAILURE;
    }

    for (i = 0; i < file_num; i++) {
        sample_svp_check_exps_return(realpath(src[i], path) == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Invalid file!\n");

        fp = fopen(path, "rb");
        sample_svp_check_exps_return(fp == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "open file failed!\n");

        ret = sample_svp_npu_get_line_num_and_line_byte_num(task, i, HI_TRUE, &total_line_num, &line_byte_num);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, end, SAMPLE_SVP_ERR_LEVEL_ERROR, "get line num failed!\n");

        data_buffer = svp_acl_mdl_get_dataset_buffer(task->input_dataset, i);
        sample_svp_check_exps_goto(data_buffer == HI_NULL, end, SAMPLE_SVP_ERR_LEVEL_ERROR, "get data buffer NULL!\n");

        data = svp_acl_get_data_buffer_addr(data_buffer);
        sample_svp_check_exps_goto(data == HI_NULL, end, SAMPLE_SVP_ERR_LEVEL_ERROR, "get data addr NULL!\n");

        stride = svp_acl_get_data_buffer_stride(data_buffer);
        sample_svp_check_exps_goto(stride == 0, end, SAMPLE_SVP_ERR_LEVEL_ERROR, "get data stride failed!\n");

        size = svp_acl_get_data_buffer_size(data_buffer);
        sample_svp_check_exps_goto(size < (hi_u64)task->cfg.dynamic_batch_num * total_line_num * stride, end,
            SAMPLE_SVP_ERR_LEVEL_ERROR, "%u-th data buffer size(%zu) is less than needed(%llu)!\n",
            i, size, (hi_u64)task->cfg.dynamic_batch_num * total_line_num * stride);

        for (line = 0; line < task->cfg.dynamic_batch_num * total_line_num; line++) {
            ret = fread(data + line * stride, line_byte_num, 1, fp);
            sample_svp_check_exps_goto(ret != 1, end, SAMPLE_SVP_ERR_LEVEL_ERROR, "Read file failed!\n");
        }
        if (task->cfg.is_cached == HI_TRUE) {
            (hi_void)svp_acl_rt_mem_flush(data, task->cfg.dynamic_batch_num * total_line_num * stride);
        }
        (hi_void)fclose(fp);
    }
    return HI_SUCCESS;

end:
    if (fp != HI_NULL) {
        (hi_void)fclose(fp);
    }
    return HI_FAILURE;
}

static hi_s32 sample_svp_npu_read_model(const hi_char *model_path, hi_u32 model_index, hi_bool is_cached)
{
    FILE *fp = HI_NULL;
    hi_s32 ret;

    /* Get model file size */
    fp = fopen(model_path, "rb");
    sample_svp_check_exps_return(fp == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "open model file failed, model file is %s!\n", model_path);

    ret = fseek(fp, 0L, SEEK_END);
    sample_svp_check_exps_goto(ret == -1, end_0, SAMPLE_SVP_ERR_LEVEL_ERROR, "fseek failed!\n");

    g_svp_npu_model[model_index].model_mem_size = ftell(fp);
    sample_svp_check_exps_goto(g_svp_npu_model[model_index].model_mem_size <= 0, end_0,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "ftell failed!\n");

    ret = fseek(fp, 0L, SEEK_SET);
    sample_svp_check_exps_goto(ret == -1, end_0, SAMPLE_SVP_ERR_LEVEL_ERROR, "fseek failed!\n");

    /* malloc model file mem */
    if (is_cached == HI_TRUE) {
        ret = svp_acl_rt_malloc_cached(&g_svp_npu_model[model_index].model_mem_ptr,
            g_svp_npu_model[model_index].model_mem_size, SVP_ACL_MEM_MALLOC_NORMAL_ONLY);
    } else {
        ret = svp_acl_rt_malloc(&g_svp_npu_model[model_index].model_mem_ptr,
            g_svp_npu_model[model_index].model_mem_size, SVP_ACL_MEM_MALLOC_NORMAL_ONLY);
    }
    sample_svp_check_exps_goto(ret != SVP_ACL_SUCCESS, end_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "malloc mem failed, erroe code %d!\n", ret);

    ret = fread(g_svp_npu_model[model_index].model_mem_ptr, g_svp_npu_model[model_index].model_mem_size, 1, fp);
    sample_svp_check_exps_goto(ret != 1, end_1, SAMPLE_SVP_ERR_LEVEL_ERROR, "read model file failed!\n");

    if (is_cached == HI_TRUE) {
        ret = svp_acl_rt_mem_flush(g_svp_npu_model[model_index].model_mem_ptr,
            g_svp_npu_model[model_index].model_mem_size);
        sample_svp_check_exps_goto(ret != SVP_ACL_SUCCESS, end_1, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "flush mem failed!, error code is %d\n", ret);
    }
    (hi_void)fclose(fp);
    return HI_SUCCESS;
end_1:
    (hi_void)svp_acl_rt_free(g_svp_npu_model[model_index].model_mem_ptr);
end_0:
    (hi_void)fclose(fp);
    return HI_FAILURE;
}

static hi_s32 sample_svp_npu_create_desc(hi_u32 model_index)
{
    svp_acl_error ret;

    g_svp_npu_model[model_index].model_desc = svp_acl_mdl_create_desc();
    sample_svp_check_exps_return(g_svp_npu_model[model_index].model_desc == HI_NULL, HI_FAILURE,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "create model description failed!\n");

    ret = svp_acl_mdl_get_desc(g_svp_npu_model[model_index].model_desc, g_svp_npu_model[model_index].model_id);
    sample_svp_check_exps_return(ret != SVP_ACL_SUCCESS, HI_FAILURE,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "get model description failed, error code is %d!\n", ret);

    sample_svp_trace_info("create model description success!\n");

    return HI_SUCCESS;
}

hi_s32 sample_common_svp_npu_load_model(const hi_char *model_path, hi_u32 model_index, hi_bool is_cached)
{
    hi_s32 ret;

    sample_svp_check_exps_return(model_path == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "path is null!\n");
    sample_svp_check_exps_return(model_index >= SAMPLE_SVP_NPU_MAX_MODEL_NUM, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "model_index(%u) should in [0,%d) !\n", model_index, SAMPLE_SVP_NPU_MAX_MODEL_NUM);

    sample_svp_check_exps_return(g_svp_npu_model[model_index].is_load_flag == HI_TRUE, HI_FAILURE,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "%u-th node has already loaded a model!\n", model_index);

    ret = sample_svp_npu_read_model(model_path, model_index, is_cached);
    sample_svp_check_exps_return(ret != SVP_ACL_SUCCESS, HI_FAILURE,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "query model failed, model file is %s!\n", model_path);

    ret = svp_acl_mdl_load_from_mem(g_svp_npu_model[model_index].model_mem_ptr,
        g_svp_npu_model[model_index].model_mem_size, &g_svp_npu_model[model_index].model_id);
    sample_svp_check_exps_goto(ret != SVP_ACL_SUCCESS, end_0,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "load model from mem failed, error code is %d!\n", ret);

    ret = sample_svp_npu_create_desc(model_index);
    sample_svp_check_exps_goto(ret != SVP_ACL_SUCCESS, end_0,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "create desc failed, model file is %s!\n", model_path);

    ret = sample_svp_npu_get_model_base_info(model_index);
    sample_svp_check_exps_goto(ret != SVP_ACL_SUCCESS, end_1,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "get model base info failed, model file is %s!\n", model_path);

    sample_svp_trace_info("load mem_size:%lu, id:%u!\n", g_svp_npu_model[model_index].model_mem_size,
        g_svp_npu_model[model_index].model_id);

    g_svp_npu_model[model_index].is_load_flag = HI_TRUE;
    sample_svp_trace_info("load model %s success!\n", model_path);

    return HI_SUCCESS;
end_1:
    (hi_void)svp_acl_mdl_destroy_desc(g_svp_npu_model[model_index].model_desc);
    g_svp_npu_model[model_index].model_desc = HI_NULL;
end_0:
    (hi_void)svp_acl_rt_free(g_svp_npu_model[model_index].model_mem_ptr);
    g_svp_npu_model[model_index].model_mem_ptr = HI_NULL;
    g_svp_npu_model[model_index].model_mem_size = 0;
    return HI_FAILURE;
}

hi_void sample_common_svp_npu_unload_model(hi_u32 model_index)
{
    svp_acl_error ret;

    sample_svp_check_exps_return_void(g_svp_npu_model[model_index].is_load_flag != HI_TRUE,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "%u-th node has not loaded a model!\n", model_index);

    ret = svp_acl_mdl_unload(g_svp_npu_model[model_index].model_id);
    if (ret != SVP_ACL_SUCCESS) {
        sample_svp_trace_err("unload model failed, model_id is %u, error code is %d!\n",
            g_svp_npu_model[model_index].model_id, ret);
    }

    if (g_svp_npu_model[model_index].model_desc != HI_NULL) {
        (hi_void)svp_acl_mdl_destroy_desc(g_svp_npu_model[model_index].model_desc);
        g_svp_npu_model[model_index].model_desc = HI_NULL;
    }

    if (g_svp_npu_model[model_index].model_mem_ptr != HI_NULL) {
        (hi_void)svp_acl_rt_free(g_svp_npu_model[model_index].model_mem_ptr);
        g_svp_npu_model[model_index].model_mem_ptr = HI_NULL;
        g_svp_npu_model[model_index].model_mem_size = 0;
    }

    if (g_svp_npu_model[model_index].handle != HI_NULL) {
        (hi_void)svp_acl_mdl_destroy_config_handle(g_svp_npu_model[model_index].handle);
        g_svp_npu_model[model_index].handle = HI_NULL;
    }

    g_svp_npu_model[model_index].is_load_flag = HI_FALSE;
    sample_svp_trace_info("unload model SUCCESS, model id is %u!\n", g_svp_npu_model[model_index].model_id);
}

static hi_s32 sample_common_svp_npu_set_model_config(hi_u32 model_index)
{
    size_t value = SVP_ACL_MDL_LOAD_FROM_MEM;
    hi_s32 ret;

    ret = svp_acl_mdl_set_config_opt(g_svp_npu_model[model_index].handle, SVP_ACL_MDL_LOAD_TYPE_SIZET,
        &(value), sizeof(size_t));
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, TEST_PICO_ERR_LEVEL_ERROR, "set_load_type failed!\n");

    ret = svp_acl_mdl_set_config_opt(g_svp_npu_model[model_index].handle, SVP_ACL_MDL_MEM_ADDR_PTR,
        (hi_void *)(&(g_svp_npu_model[model_index].model_mem_ptr)), sizeof(hi_void *));
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, TEST_PICO_ERR_LEVEL_ERROR, "set_mem_ptr failed!\n");

    ret = svp_acl_mdl_set_config_opt(g_svp_npu_model[model_index].handle, SVP_ACL_MDL_MEM_SIZET,
        &(g_svp_npu_model[model_index].model_mem_size), sizeof(size_t));
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, TEST_PICO_ERR_LEVEL_ERROR, "set_mem_size failed!\n");

    ret = svp_acl_mdl_set_config_opt(g_svp_npu_model[model_index].handle, SVP_ACL_MDL_PRIORITY_INT32,
        &(g_svp_npu_model[model_index].prior_cfg.priority), sizeof(hi_s32));
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, TEST_PICO_ERR_LEVEL_ERROR, "set_mem_size failed!\n");

    ret = svp_acl_mdl_set_config_opt(g_svp_npu_model[model_index].handle, SVP_ACL_MDL_PRIORITY_PREEMP_EN_BOOL,
        &g_svp_npu_model[model_index].prior_cfg.prior_preemp_en, sizeof(hi_bool));
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, TEST_PICO_ERR_LEVEL_ERROR, "prior_preemp_en failed!\n");

    ret = svp_acl_mdl_set_config_opt(g_svp_npu_model[model_index].handle, SVP_ACL_MDL_PRIORITY_UP_STEP_TIMEOUT_UINT32,
        &g_svp_npu_model[model_index].prior_cfg.prior_up_step_timeout, sizeof(hi_u32));
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, TEST_PICO_ERR_LEVEL_ERROR,
        "prior_up_step_timeout failed!\n");

    ret = svp_acl_mdl_set_config_opt(g_svp_npu_model[model_index].handle, SVP_ACL_MDL_PRIORITY_UP_TOP_TIMEOUT_UINT32,
        &g_svp_npu_model[model_index].prior_cfg.prior_up_top_timeout, sizeof(hi_u32));
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, TEST_PICO_ERR_LEVEL_ERROR,
        "prior_up_top_timeout failed!\n");

    return HI_SUCCESS;
}

hi_s32 sample_common_svp_npu_load_model_with_config(const hi_char *model_path, hi_u32 model_index,
    hi_bool is_cached)
{
    hi_s32 ret;

    sample_svp_check_exps_return(model_path == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "path is null!\n");
    sample_svp_check_exps_return(model_index >= SAMPLE_SVP_NPU_MAX_MODEL_NUM, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "model_index(%u) should in [0,%d) !\n", model_index, SAMPLE_SVP_NPU_MAX_MODEL_NUM);

    sample_svp_check_exps_return(g_svp_npu_model[model_index].is_load_flag == HI_TRUE, HI_FAILURE,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "%u-th node has already loaded a model!\n", model_index);

    ret = sample_svp_npu_read_model(model_path, model_index, is_cached);
    sample_svp_check_exps_return(ret != SVP_ACL_SUCCESS, HI_FAILURE,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "query model failed, model file is %s!\n", model_path);

    g_svp_npu_model[model_index].handle = svp_acl_mdl_create_config_handle();
    sample_svp_check_exps_goto(ret != SVP_ACL_SUCCESS, end_0, SAMPLE_SVP_ERR_LEVEL_ERROR, "create handle failed!\n");

    ret = sample_common_svp_npu_set_model_config(model_index);
    sample_svp_check_exps_goto(ret != SVP_ACL_SUCCESS, end_1, SAMPLE_SVP_ERR_LEVEL_ERROR, "set handle failed!\n");

    ret = svp_acl_mdl_load_with_config(g_svp_npu_model[model_index].handle, &g_svp_npu_model[model_index].model_id);
    sample_svp_check_exps_goto(ret != SVP_ACL_SUCCESS, end_1,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "load model from mem failed, error code is %d!\n", ret);

    ret = sample_svp_npu_create_desc(model_index);
    sample_svp_check_exps_goto(ret != SVP_ACL_SUCCESS, end_1,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "create desc failed, model file is %s!\n", model_path);

    ret = sample_svp_npu_get_model_base_info(model_index);
    sample_svp_check_exps_goto(ret != SVP_ACL_SUCCESS, end_2,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "get model base info failed, model file is %s!\n", model_path);

    sample_svp_trace_info("load mem_size:%lu, id:%u!\n", g_svp_npu_model[model_index].model_mem_size,
        g_svp_npu_model[model_index].model_id);

    g_svp_npu_model[model_index].is_load_flag = HI_TRUE;
    sample_svp_trace_info("load model %s success!\n", model_path);

    return HI_SUCCESS;

end_2:
    (hi_void)svp_acl_mdl_destroy_desc(g_svp_npu_model[model_index].model_desc);
    g_svp_npu_model[model_index].model_desc = HI_NULL;

end_1:
    (hi_void)svp_acl_mdl_destroy_config_handle(g_svp_npu_model[model_index].handle);

end_0:
    (hi_void)svp_acl_rt_free(g_svp_npu_model[model_index].model_mem_ptr);
    g_svp_npu_model[model_index].model_mem_ptr = HI_NULL;
    g_svp_npu_model[model_index].model_mem_size = 0;
    return HI_FAILURE;
}

hi_void sample_svp_npu_set_model_prior_info(hi_u32 model_index, sample_svp_npu_model_prior_cfg *prior_cfg)
{
    if (model_index >= SAMPLE_SVP_NPU_MAX_MODEL_NUM) {
        sample_svp_trace_err("model index[%u] is out of range [0 %u)!\n", model_index, SAMPLE_SVP_NPU_MAX_MODEL_NUM);
        return;
    }

    g_svp_npu_model[model_index].prior_cfg.priority = prior_cfg->priority;
    g_svp_npu_model[model_index].prior_cfg.prior_preemp_en = prior_cfg->prior_preemp_en;
    g_svp_npu_model[model_index].prior_cfg.prior_up_step_timeout = prior_cfg->prior_up_step_timeout;
    g_svp_npu_model[model_index].prior_cfg.prior_up_top_timeout = prior_cfg->prior_up_top_timeout;
}

static hi_s32 sample_svp_npu_malloc_mem(hi_void **buffer, hi_u32 buffer_size, hi_bool is_cached)
{
    svp_acl_error ret;

    if (is_cached == HI_TRUE) {
        ret = svp_acl_rt_malloc_cached(buffer, buffer_size, SVP_ACL_MEM_MALLOC_NORMAL_ONLY);
    } else {
        ret = svp_acl_rt_malloc(buffer, buffer_size, SVP_ACL_MEM_MALLOC_NORMAL_ONLY);
    }
    sample_svp_check_exps_return(ret != SVP_ACL_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "can't malloc buffer, size is %u, error code is %d!\n", buffer_size, ret);

    (hi_void)memset_s(*buffer, buffer_size, 0, buffer_size);
    if (is_cached == HI_TRUE) {
        (hi_void)svp_acl_rt_mem_flush(*buffer, buffer_size);
    }

    return ret;
}

static hi_void sample_svp_npu_destroy_data_buffer(svp_acl_data_buffer *input_data)
{
    hi_void *data = svp_acl_get_data_buffer_addr(input_data);
    (hi_void)svp_acl_rt_free(data);
    (hi_void)svp_acl_destroy_data_buffer(input_data);
}

static svp_acl_data_buffer *sample_common_svp_npu_create_input_data_buffer(sample_svp_npu_task_info *task, hi_u32 idx)
{
    size_t buffer_size, stride;
    hi_void *input_buffer = HI_NULL;
    svp_acl_data_buffer *input_data = HI_NULL;

    stride = svp_acl_mdl_get_input_default_stride(g_svp_npu_model[task->cfg.model_idx].model_desc, idx);
    sample_svp_check_exps_return(stride == 0, input_data, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get %u-th input stride failed!\n", idx);

    buffer_size = svp_acl_mdl_get_input_size_by_index(g_svp_npu_model[task->cfg.model_idx].model_desc, idx) *
        (hi_u64)task->cfg.max_batch_num;
    sample_svp_check_exps_return((buffer_size == 0 || buffer_size > SAMPLE_SVP_NPU_MAX_MEM_SIZE), input_data,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "buffer_size(%zu) can't be 0 and should be less than %u!\n",
        buffer_size, SAMPLE_SVP_NPU_MAX_MEM_SIZE);

    if (sample_svp_npu_malloc_mem(&input_buffer, (hi_u32)buffer_size, task->cfg.is_cached) != HI_SUCCESS) {
        sample_svp_trace_err("%u-th input malloc mem failed!\n", idx);
        return input_data;
    }

    input_data = svp_acl_create_data_buffer(input_buffer, buffer_size, stride);
    if (input_data == HI_NULL) {
        sample_svp_trace_err("can't create %u-th input data buffer!\n", idx);
        (hi_void)svp_acl_rt_free(input_buffer);
        return input_data;
    }
    if (idx == g_svp_npu_model[task->cfg.model_idx].input_num - SAMPLE_SVP_NPU_EXTRA_INPUT_NUM) {
        task->task_buf_ptr = input_buffer;
        task->task_buf_size = buffer_size;
        task->task_buf_stride = stride;
    } else if (idx == g_svp_npu_model[task->cfg.model_idx].input_num - 1) {
        task->work_buf_ptr = input_buffer;
        task->work_buf_size = buffer_size;
        task->work_buf_stride = stride;
    }
    return input_data;
}

static svp_acl_data_buffer *sample_common_svp_npu_create_output_data_buffer(const sample_svp_npu_task_info *task,
    hi_u32 idx)
{
    size_t buffer_size, stride;
    hi_void *output_buffer = HI_NULL;
    svp_acl_data_buffer *output_data = HI_NULL;

    stride = svp_acl_mdl_get_output_default_stride(g_svp_npu_model[task->cfg.model_idx].model_desc, idx);
    sample_svp_check_exps_return(stride == 0, output_data, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get %u-th output stride failed!\n", idx);

    buffer_size = svp_acl_mdl_get_output_size_by_index(g_svp_npu_model[task->cfg.model_idx].model_desc, idx) *
        (hi_u64)task->cfg.max_batch_num;
    sample_svp_check_exps_return((buffer_size == 0 || buffer_size > SAMPLE_SVP_NPU_MAX_MEM_SIZE), output_data,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "buffer_size(%zu) can't be 0 and should be less than %u!\n",
        buffer_size, SAMPLE_SVP_NPU_MAX_MEM_SIZE);

    if (sample_svp_npu_malloc_mem(&output_buffer, buffer_size, task->cfg.is_cached) != HI_SUCCESS) {
        sample_svp_trace_err("%u-th output malloc mem failed!\n", idx);
        return output_data;
    }

    output_data = svp_acl_create_data_buffer(output_buffer, buffer_size, stride);
    if (output_data == HI_NULL) {
        sample_svp_trace_err("can't create %u-th output data buffer!\n", idx);
        (hi_void)svp_acl_rt_free(output_buffer);
        return output_data;
    }
    return output_data;
}

hi_s32 sample_common_svp_npu_create_input(sample_svp_npu_task_info *task)
{
    svp_acl_error ret;
    hi_u32 i;
    svp_acl_data_buffer *input_data = HI_NULL;

    if (sample_svp_check_task_cfg(task) != HI_SUCCESS) {
        sample_svp_trace_err("check task cfg failed!\n");
        return HI_FAILURE;
    }

    task->input_dataset = svp_acl_mdl_create_dataset();
    sample_svp_check_exps_return(task->input_dataset == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "create input dataset failed!\n");

    for (i = 0; i < g_svp_npu_model[task->cfg.model_idx].input_num - SAMPLE_SVP_NPU_EXTRA_INPUT_NUM; i++) {
        input_data = sample_common_svp_npu_create_input_data_buffer(task, i);
        if (input_data == HI_NULL) {
            sample_svp_trace_err("create %u-th input data buffer failed!\n", i);
            (hi_void)sample_common_svp_npu_destroy_input(task);
            return HI_FAILURE;
        }

        ret = svp_acl_mdl_add_dataset_buffer(task->input_dataset, input_data);
        if (ret != SVP_ACL_SUCCESS) {
            sample_svp_trace_err("add %u-th input data buffer failed!\n", i);
            (hi_void)sample_svp_npu_destroy_data_buffer(input_data);
            (hi_void)sample_common_svp_npu_destroy_input(task);
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

hi_s32 sample_common_svp_npu_create_output(sample_svp_npu_task_info *task)
{
    svp_acl_error ret;
    hi_u32 i;
    svp_acl_data_buffer *output_data = HI_NULL;

    if (sample_svp_check_task_cfg(task) != HI_SUCCESS) {
        sample_svp_trace_err("check task cfg failed!\n");
        return HI_FAILURE;
    }

    task->output_dataset = svp_acl_mdl_create_dataset();
    sample_svp_check_exps_return(task->input_dataset == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "create output dataset failed!\n");

    for (i = 0; i < g_svp_npu_model[task->cfg.model_idx].output_num; i++) {
        output_data = sample_common_svp_npu_create_output_data_buffer(task, i);
        if (output_data == HI_NULL) {
            sample_svp_trace_err("create %u-th output data buffer failed!\n", i);
            (hi_void)sample_common_svp_npu_destroy_input(task);
            return HI_FAILURE;
        }
        ret = svp_acl_mdl_add_dataset_buffer(task->output_dataset, output_data);
        if (ret != SVP_ACL_SUCCESS) {
            sample_svp_trace_err("add %u-th output data buffer failed!\n", i);
            (hi_void)sample_svp_npu_destroy_data_buffer(output_data);
            (hi_void)sample_common_svp_npu_destroy_output(task);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

hi_void sample_common_svp_npu_destroy_input(sample_svp_npu_task_info *task)
{
    hi_u32 i;
    size_t input_num;
    svp_acl_data_buffer *data_buffer = HI_NULL;
    hi_void *data = HI_NULL;

    if (sample_svp_check_task_cfg(task) != HI_SUCCESS) {
        sample_svp_trace_err("check task cfg failed!\n");
        return;
    }

    if (task->input_dataset == HI_NULL) {
        return;
    }

    input_num = svp_acl_mdl_get_dataset_num_buffers(task->input_dataset);
    for (i = 0; i < input_num; i++) {
        data_buffer = svp_acl_mdl_get_dataset_buffer(task->input_dataset, i);
        if (i < g_svp_npu_model[task->cfg.model_idx].input_num - SAMPLE_SVP_NPU_EXTRA_INPUT_NUM) {
            data = svp_acl_get_data_buffer_addr(data_buffer);
            (hi_void)svp_acl_rt_free(data);
        }
        (hi_void)svp_acl_destroy_data_buffer(data_buffer);
    }
    (hi_void)svp_acl_mdl_destroy_dataset(task->input_dataset);
    task->input_dataset = HI_NULL;
}

hi_void sample_common_svp_npu_destroy_output(sample_svp_npu_task_info *task)
{
    hi_u32 i;
    size_t output_num;
    svp_acl_data_buffer *data_buffer = HI_NULL;
    hi_void *data = HI_NULL;

    if (sample_svp_check_task_cfg(task) != HI_SUCCESS) {
        sample_svp_trace_err("check task cfg failed!\n");
        return;
    }

    if (task->output_dataset == HI_NULL) {
        return;
    }

    output_num = svp_acl_mdl_get_dataset_num_buffers(task->output_dataset);
    printf("%s => output_num:%d\n", __func__, output_num);
    for (i = 0; i < output_num; i++) {
        data_buffer = svp_acl_mdl_get_dataset_buffer(task->output_dataset, i);
        data = svp_acl_get_data_buffer_addr(data_buffer);
        (hi_void)svp_acl_rt_free(data);
        (hi_void)svp_acl_destroy_data_buffer(data_buffer);
    }

    (hi_void)svp_acl_mdl_destroy_dataset(task->output_dataset);
    task->output_dataset = HI_NULL;
}

hi_void sample_common_svp_npu_destroy_task_buf(sample_svp_npu_task_info *task)
{
    if (sample_svp_check_task_cfg(task) != HI_SUCCESS) {
        sample_svp_trace_err("check task cfg failed!\n");
        return;
    }

    if (task->task_buf_ptr == HI_NULL) {
        return;
    }
    (hi_void)svp_acl_rt_free(task->task_buf_ptr);
    task->task_buf_ptr = HI_NULL;
    task->task_buf_stride = 0;
    task->task_buf_size = 0;
}

hi_void sample_common_svp_npu_destroy_work_buf(sample_svp_npu_task_info *task)
{
    if (sample_svp_check_task_cfg(task) != HI_SUCCESS) {
        sample_svp_trace_err("check task cfg failed!\n");
        return;
    }

    if (task->work_buf_ptr == HI_NULL) {
        return;
    }
    (hi_void)svp_acl_rt_free(task->work_buf_ptr);
    task->work_buf_ptr = HI_NULL;
    task->work_buf_stride = 0;
    task->work_buf_size = 0;
}

hi_s32 sample_common_svp_npu_create_task_buf(sample_svp_npu_task_info *task)
{
    size_t num;
    svp_acl_data_buffer *task_buf = HI_NULL;
    svp_acl_error ret;

    if (sample_svp_check_task_cfg(task) != HI_SUCCESS) {
        sample_svp_trace_err("check task cfg failed!\n");
        return HI_FAILURE;
    }
    sample_svp_check_exps_return(task->input_dataset == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "input_dataset is NULL!\n");

    num = svp_acl_mdl_get_dataset_num_buffers(task->input_dataset);
    sample_svp_check_exps_return(num != g_svp_npu_model[task->cfg.model_idx].input_num - SAMPLE_SVP_NPU_EXTRA_INPUT_NUM,
        HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "num of data buffer(%zu) should be %lu when create task buf!\n",
        num, g_svp_npu_model[task->cfg.model_idx].input_num - SAMPLE_SVP_NPU_EXTRA_INPUT_NUM);

    task_buf = sample_common_svp_npu_create_input_data_buffer(task,
        g_svp_npu_model[task->cfg.model_idx].input_num - SAMPLE_SVP_NPU_EXTRA_INPUT_NUM);
    sample_svp_check_exps_return(task_buf == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "create task buf failed!\n");

    ret = svp_acl_mdl_add_dataset_buffer(task->input_dataset, task_buf);
    if (ret != SVP_ACL_SUCCESS) {
        sample_svp_trace_err("add task buf failed!\n");
        (hi_void)sample_svp_npu_destroy_data_buffer(task_buf);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 sample_common_svp_npu_create_work_buf(sample_svp_npu_task_info *task)
{
    size_t num;
    svp_acl_data_buffer *work_buf = HI_NULL;
    svp_acl_error ret;

    if (sample_svp_check_task_cfg(task) != HI_SUCCESS) {
        sample_svp_trace_err("check task cfg failed!\n");
        return HI_FAILURE;
    }

    sample_svp_check_exps_return(task->input_dataset == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "input_dataset is NULL!\n");

    num = svp_acl_mdl_get_dataset_num_buffers(task->input_dataset);
    sample_svp_check_exps_return(num != g_svp_npu_model[task->cfg.model_idx].input_num - 1,
        HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "num of data buffer(%zu) should be %lu when create work buf!\n",
        num, g_svp_npu_model[task->cfg.model_idx].input_num - 1);

    work_buf = sample_common_svp_npu_create_input_data_buffer(task, g_svp_npu_model[task->cfg.model_idx].input_num - 1);
    sample_svp_check_exps_return(work_buf == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "create work buf failed!\n");

    ret = svp_acl_mdl_add_dataset_buffer(task->input_dataset, work_buf);
    if (ret != SVP_ACL_SUCCESS) {
        sample_svp_trace_err("add work buf failed!\n");
        (hi_void)sample_svp_npu_destroy_data_buffer(work_buf);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 sample_common_svp_npu_get_work_buf_info(const sample_svp_npu_task_info *task,
    hi_u32 *work_buf_size, hi_u32 *work_buf_stride)
{
    if (work_buf_size == HI_NULL || work_buf_stride == HI_NULL) {
        sample_svp_trace_err("nullptr!\n");
        return HI_FAILURE;
    }

    if (sample_svp_check_task_cfg(task) != HI_SUCCESS) {
        sample_svp_trace_err("check task cfg failed!\n");
        return HI_FAILURE;
    }

    *work_buf_stride = (hi_u32)svp_acl_mdl_get_input_default_stride(g_svp_npu_model[task->cfg.model_idx].model_desc,
        g_svp_npu_model[task->cfg.model_idx].input_num - 1);
    if (*work_buf_stride == 0) {
        sample_svp_trace_err("get work buf stride failed!\n");
        return HI_FAILURE;
    }

    *work_buf_size = (hi_u32)svp_acl_mdl_get_input_size_by_index(g_svp_npu_model[task->cfg.model_idx].model_desc,
        g_svp_npu_model[task->cfg.model_idx].input_num - 1);
    if (*work_buf_size == 0) {
        sample_svp_trace_err("get work buf size failed!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 sample_common_svp_npu_share_work_buf(const sample_svp_npu_shared_work_buf *shared_work_buf,
    const sample_svp_npu_task_info *task)
{
    svp_acl_error ret;
    svp_acl_data_buffer *work_buf = HI_NULL;

    if (sample_svp_check_task_cfg(task) != HI_SUCCESS) {
        sample_svp_trace_err("check task cfg failed!\n");
        return HI_FAILURE;
    }

    sample_svp_check_exps_return(shared_work_buf == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "shared_work_buf is NULL!\n");

    sample_svp_check_exps_return(shared_work_buf->work_buf_ptr == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "work_buf_ptr is NULL!\n");

    sample_svp_check_exps_return(task->work_buf_ptr != HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "task has created work buf!\n");

    work_buf = svp_acl_create_data_buffer(shared_work_buf->work_buf_ptr, shared_work_buf->work_buf_size,
        shared_work_buf->work_buf_stride);
    sample_svp_check_exps_return(work_buf == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "create work buf failed!\n");

    ret = svp_acl_mdl_add_dataset_buffer(task->input_dataset, work_buf);
    if (ret != SVP_ACL_SUCCESS) {
        sample_svp_trace_err("add work buf failed!\n");
        (hi_void)svp_acl_destroy_data_buffer(work_buf);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_common_svp_npu_update_input_info_after_aippset(
    sample_svp_npu_model_info *model, const sample_svp_npu_task_info *task, hi_u32 index)
{
    svp_acl_data_buffer *data_buffer = HI_NULL;
    void *data = HI_NULL;
    hi_s32 ret = SVP_ACL_ERROR_INVALID_PARAM;
    size_t stride, size;

    data_buffer = svp_acl_mdl_get_dataset_buffer(task->input_dataset, index);
    sample_svp_check_exps_return(data_buffer == HI_NULL, ret, TEST_PICO_ERR_LEVEL_ERROR, "ret:%d\n", ret);
    data = svp_acl_get_data_buffer_addr(data_buffer);
    sample_svp_check_exps_return(data == HI_NULL, ret, TEST_PICO_ERR_LEVEL_ERROR, "ret:%d\n", ret);

    size = svp_acl_mdl_get_input_size_by_index(model->model_desc, index);
    stride = svp_acl_mdl_get_input_default_stride(model->model_desc, index);

    return svp_acl_update_data_buffer(data_buffer, data, size, stride);
}

static hi_s32 sample_common_svp_npu_set_input_aipp_dynamic_param(svp_acl_mdl_aipp *aipp_set,
    const sample_aipp_dynamic_param *dynamic_param)
{
    hi_s32 ret;

    ret = svp_acl_mdl_set_aipp_input_format(aipp_set, dynamic_param->input_format);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, TEST_PICO_ERR_LEVEL_ERROR, "ret:%d\n", ret);

    ret = svp_acl_mdl_set_aipp_src_image_size(aipp_set, dynamic_param->image_width, dynamic_param->image_height);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, TEST_PICO_ERR_LEVEL_ERROR, "ret:%d\n", ret);

    if (dynamic_param->csc_switch != 0) {
        ret = svp_acl_mdl_set_aipp_csc_params(aipp_set, 1,
            dynamic_param->csc_matrix_r0c0, dynamic_param->csc_matrix_r0c1, dynamic_param->csc_matrix_r0c2,
            dynamic_param->csc_matrix_r1c0, dynamic_param->csc_matrix_r1c1, dynamic_param->csc_matrix_r1c2,
            dynamic_param->csc_matrix_r2c0, dynamic_param->csc_matrix_r2c1, dynamic_param->csc_matrix_r2c2,
            dynamic_param->csc_output_bias_r0, dynamic_param->csc_output_bias_r1, dynamic_param->csc_output_bias_r2,
            dynamic_param->csc_input_bias_r0, dynamic_param->csc_input_bias_r1, dynamic_param->csc_input_bias_r2);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, TEST_PICO_ERR_LEVEL_ERROR, "ret:%d\n", ret);
    }

    if (dynamic_param->rbuv_swap_switch != 0) {
        ret = svp_acl_mdl_set_aipp_rbuv_swap_switch(aipp_set, 1);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, TEST_PICO_ERR_LEVEL_ERROR, "ret:%d\n", ret);
    }

    if (dynamic_param->ax_swap_switch != 0) {
        ret = svp_acl_mdl_set_aipp_ax_swap_switch(aipp_set, 1);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, TEST_PICO_ERR_LEVEL_ERROR, "ret:%d\n", ret);
    }

    return HI_SUCCESS;
}

static hi_s32 sample_common_svp_npu_set_input_aipp_param(svp_acl_mdl_aipp *aipp_set,
    const sample_svp_npu_model_aipp *model_aipp)
{
    hi_s32 ret;

    ret = sample_common_svp_npu_set_input_aipp_dynamic_param(aipp_set, &model_aipp->dynamic_param);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, TEST_PICO_ERR_LEVEL_ERROR, "ret:%d\n", ret);

    if (model_aipp->dynamic_batch_param.crop_switch != 0) {
        ret = svp_acl_mdl_set_aipp_crop_params(aipp_set, 1,
            model_aipp->dynamic_batch_param.crop_pos_x, model_aipp->dynamic_batch_param.crop_pos_y,
            model_aipp->dynamic_batch_param.crop_width, model_aipp->dynamic_batch_param.crop_height, 0);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, TEST_PICO_ERR_LEVEL_ERROR, "ret:%d\n", ret);
    }

    if (model_aipp->dynamic_batch_param.padding_switch != 0) {
        ret = svp_acl_mdl_set_aipp_padding_params(aipp_set, model_aipp->dynamic_batch_param.padding_switch,
            model_aipp->dynamic_batch_param.padding_top, model_aipp->dynamic_batch_param.padding_bottom,
            model_aipp->dynamic_batch_param.padding_left, model_aipp->dynamic_batch_param.padding_right, 0);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, TEST_PICO_ERR_LEVEL_ERROR, "ret:%d\n", ret);
    }

    if (model_aipp->dynamic_batch_param.scf_switch != 0) {
        ret = svp_acl_mdl_set_aipp_scf_params(aipp_set, model_aipp->dynamic_batch_param.scf_switch,
            model_aipp->dynamic_batch_param.scf_input_width, model_aipp->dynamic_batch_param.scf_input_height,
            model_aipp->dynamic_batch_param.scf_output_width, model_aipp->dynamic_batch_param.scf_output_height, 0);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, TEST_PICO_ERR_LEVEL_ERROR, "ret:%d\n", ret);
    }

    if (model_aipp->dynamic_batch_param.preproc_switch != 0) {
        ret = svp_acl_mdl_set_aipp_pixel_var_reci(aipp_set,
            model_aipp->dynamic_batch_param.reci_chn0, model_aipp->dynamic_batch_param.reci_chn1,
            model_aipp->dynamic_batch_param.reci_chn2, model_aipp->dynamic_batch_param.reci_chn3, 0);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, TEST_PICO_ERR_LEVEL_ERROR, "ret:%d\n", ret);

        ret = svp_acl_mdl_set_aipp_dtc_pixel_mean(aipp_set,
            model_aipp->dynamic_batch_param.mean_chn0, model_aipp->dynamic_batch_param.mean_chn1,
            model_aipp->dynamic_batch_param.mean_chn2, model_aipp->dynamic_batch_param.mean_chn3, 0);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, TEST_PICO_ERR_LEVEL_ERROR, "ret:%d\n", ret);

        ret = svp_acl_mdl_set_aipp_dtc_pixel_min(aipp_set,
            model_aipp->dynamic_batch_param.min_chn0, model_aipp->dynamic_batch_param.min_chn1,
            model_aipp->dynamic_batch_param.min_chn2, model_aipp->dynamic_batch_param.min_chn3, 0);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, TEST_PICO_ERR_LEVEL_ERROR, "ret:%d\n", ret);
    }

    return HI_SUCCESS;
}

hi_s32 sample_common_svp_npu_set_input_aipp_info(td_u32 model_index, td_u32 index,
    const sample_svp_npu_model_aipp *model_aipp, const sample_svp_npu_task_info *task)
{
    svp_acl_mdl_aipp *aipp_set = TD_NULL;
    svp_acl_mdl_aipp_type aipp_type;
    size_t attached_index;
    hi_s32 ret;

    if (model_index >= SAMPLE_SVP_NPU_MAX_MODEL_NUM || model_aipp == HI_NULL) {
        sample_svp_trace_err("model_index[%u] is out of range[0, %d) or model_aipp nullptr!\n",
            model_index, SAMPLE_SVP_NPU_MAX_MODEL_NUM);
        return HI_FAILURE;
    }

    sample_svp_check_exps_return(g_svp_npu_model[model_index].is_load_flag != HI_TRUE, HI_FAILURE,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "%u-th node has not loaded a model!\n", model_index);

    if (sample_svp_check_task_cfg(task) != HI_SUCCESS) {
        sample_svp_trace_err("check task cfg failed!\n");
        return HI_FAILURE;
    }

    ret = svp_acl_mdl_get_input_aipp_type(g_svp_npu_model[model_index].model_id, index, &aipp_type, &attached_index);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, TEST_PICO_ERR_LEVEL_ERROR, "ret:%d\n", ret);
    if (aipp_type != SVP_ACL_DATA_WITH_DYNAMIC_AIPP) {
        sample_svp_trace_err("input without dynamic aipp!\n");
        return HI_FAILURE;
    }

    aipp_set = svp_acl_mdl_create_aipp(1);
    sample_svp_check_exps_return(aipp_set == HI_NULL, HI_FAILURE, TEST_PICO_ERR_LEVEL_ERROR, "create aipp failure\n");

    ret = sample_common_svp_npu_set_input_aipp_param(aipp_set, model_aipp);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, TEST_PICO_ERR_LEVEL_ERROR, "ret:%d\n", ret);

    ret = svp_acl_mdl_set_input_aipp(g_svp_npu_model[model_index].model_id, task->input_dataset,
        attached_index, aipp_set);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, TEST_PICO_ERR_LEVEL_ERROR, "ret:%d\n", ret);

    return sample_common_svp_npu_update_input_info_after_aippset(&g_svp_npu_model[model_index], task, index);

fail:
    (hi_void)svp_acl_mdl_destroy_aipp(aipp_set);
    return ret;
}

/* print the top n confidence values with indexes */
static hi_void sample_svp_npu_sort_output_result(const hi_float *src, hi_u32 src_len,
    sample_svp_npu_top_n_result *dst, hi_u32 dst_len)
{
    hi_u32 i, j, index;
    hi_bool charge;

    for (i = 0; i < src_len; i++) {
        charge = HI_FALSE;

        for (j = 0; j < dst_len; j++) {
            if (src[i] > dst[j].score) {
                index = j;
                charge = HI_TRUE;
                break;
            }
        }

        if (charge == HI_TRUE) {
            for (j = dst_len - 1; j > index; j--) {
                dst[j].score = dst[j - 1].score;
                dst[j].class_id = dst[j - 1].class_id;
            }
            dst[index].score = src[i];
            dst[index].class_id = i;
        }
    }
}

hi_void sample_common_svp_npu_output_classification_result(const sample_svp_npu_task_info *task)
{
    svp_acl_data_buffer *data_buffer = HI_NULL;
    hi_void *data = HI_NULL;
    hi_u32 i, j, n;
    svp_acl_error ret;
    size_t stride;
    svp_acl_mdl_io_dims dims;
    sample_svp_npu_top_n_result top[SAMPLE_SVP_NPU_SHOW_TOP_NUM] = { 0 };

    if (sample_svp_check_task_cfg(task) != HI_SUCCESS) {
        sample_svp_trace_err("check task cfg failed!\n");
        return;
    }

    sample_svp_check_exps_return_void(task->output_dataset == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "task->output_dataset is NULL!\n");

    for (i = 0; i < svp_acl_mdl_get_dataset_num_buffers(task->output_dataset); i++) {
        data_buffer = svp_acl_mdl_get_dataset_buffer(task->output_dataset, i);
        sample_svp_check_exps_return_void(data_buffer == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get %u-th data buffer is NULL!\n", i);

        data = svp_acl_get_data_buffer_addr(data_buffer);
        sample_svp_check_exps_return_void(data == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get %u-th data addr is NULL!\n", i);

        stride = svp_acl_get_data_buffer_stride(data_buffer);
        sample_svp_check_exps_return_void(data == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get %u-th data stride is 0!\n", i);

        ret = svp_acl_mdl_get_output_dims(g_svp_npu_model[task->cfg.model_idx].model_desc, i, &dims);
        sample_svp_check_exps_return_void(data == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get %u-th output dims failed, error code is %d!\n", i, ret);

        for (n = 0; n < task->cfg.dynamic_batch_num; n++) {
            (hi_void)sample_svp_npu_sort_output_result(data, (hi_u32)dims.dims[dims.dim_count - 1],
                top, SAMPLE_SVP_NPU_SHOW_TOP_NUM);
            sample_svp_trace_info("%u-th batch result:\n", n);
            for (j = 0; j < SAMPLE_SVP_NPU_SHOW_TOP_NUM; j++) {
                sample_svp_trace_info("top %u: value[%lf], class_id[%u]!\n", j, top[j].score, top[j].class_id);
            }
            data += stride;
            (hi_void)memset_s(top, sizeof(top), 0, sizeof(top));
        }
    }

    sample_svp_trace_info("output data success!\n");
}

hi_s32 sample_common_svp_npu_set_dynamic_batch(const sample_svp_npu_task_info *task)
{
    svp_acl_error ret;

    if (sample_svp_check_task_cfg(task) != HI_SUCCESS) {
        sample_svp_trace_err("check task cfg failed!\n");
        return HI_FAILURE;
    }
    ret = svp_acl_mdl_set_dynamic_batch_size(g_svp_npu_model[task->cfg.model_idx].model_id,
        task->input_dataset, g_svp_npu_model[task->cfg.model_idx].dynamic_batch_idx, task->cfg.dynamic_batch_num);
    sample_svp_check_exps_return(ret != SVP_ACL_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "set dynamic batch size failed, model id is %u, error code is %d!\n",
        g_svp_npu_model[task->cfg.model_idx].model_id, ret);

    if (task->cfg.total_t != 0) {
        ret = svp_acl_mdl_set_total_t(g_svp_npu_model[task->cfg.model_idx].model_id,
        task->input_dataset, task->cfg.total_t);
        sample_svp_check_exps_return(ret != SVP_ACL_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "set total_t failed, model id is %u, error code is %d!\n",
            g_svp_npu_model[task->cfg.model_idx].model_id, ret);
    }

    return HI_SUCCESS;
}

hi_s32 sample_common_svp_npu_model_execute(const sample_svp_npu_task_info *task)
{
    svp_acl_data_buffer *data_buffer = HI_NULL;
    hi_void *data = HI_NULL;
    size_t size;
    hi_u32 i;
    svp_acl_error ret;

    if (sample_svp_check_task_cfg(task) != HI_SUCCESS) {
        sample_svp_trace_err("check task cfg failed!\n");
        return HI_FAILURE;
    }
    ret = svp_acl_mdl_execute(g_svp_npu_model[task->cfg.model_idx].model_id, task->input_dataset, task->output_dataset);
    sample_svp_check_exps_return(ret != SVP_ACL_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "svp_acl_mdl_execute failed, model_id is %u, error code is %d!\n",
        g_svp_npu_model[task->cfg.model_idx].model_id, ret);

    if (task->cfg.is_cached == HI_TRUE) {
        for (i = 0; i < g_svp_npu_model[task->cfg.model_idx].output_num; i++) {
            data_buffer = svp_acl_mdl_get_dataset_buffer(task->output_dataset, i);
            sample_svp_check_exps_return(data_buffer == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
                "get %u-th output data_buffer is NULL!\n", i);

            data = svp_acl_get_data_buffer_addr(data_buffer);
            sample_svp_check_exps_return(data == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
                "get %u-th output data is NULL!\n", i);

            size = svp_acl_get_data_buffer_size(data_buffer) / task->cfg.max_batch_num * task->cfg.dynamic_batch_num;
            sample_svp_check_exps_return(size == 0, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
                "get %u-th output data size is 0!\n", i);

            ret = svp_acl_rt_mem_flush(data, size);
            sample_svp_check_exps_return(data_buffer == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
                "flush %u-th output data failed, error code is %d!\n", i, ret);
        }
    }
    return ret;
}

hi_s32 sample_common_svp_npu_model_execute_async(const sample_svp_npu_task_info *task,
    svp_acl_rt_stream stream)
{
    if (sample_svp_check_task_cfg(task) != HI_SUCCESS) {
        sample_svp_trace_err("check task cfg failed!\n");
        return HI_FAILURE;
    }

    return svp_acl_mdl_execute_async(g_svp_npu_model[task->cfg.model_idx].model_id, task->input_dataset,
        task->output_dataset, stream);
}

static hi_void sample_svp_npu_dump_dataset(const sample_svp_npu_task_info *task, const hi_char *file_name,
    hi_bool is_input)
{
    hi_u32 i, line, total_line_num, line_byte_num, data_num;
    svp_acl_error ret;
    size_t size, stride;
    svp_acl_data_buffer *data_buffer = HI_NULL;
    svp_acl_mdl_dataset *dataset = HI_NULL;
    hi_void *data = HI_NULL;
    FILE *fp = HI_NULL;
    hi_char name[SAMPLE_SVP_NPU_MAX_NAME_LEN] = {'\0'};

    data_num = is_input == HI_TRUE ? g_svp_npu_model[task->cfg.model_idx].input_num - SAMPLE_SVP_NPU_EXTRA_INPUT_NUM :
        g_svp_npu_model[task->cfg.model_idx].output_num;
    dataset = is_input == HI_TRUE ? task->input_dataset : task->output_dataset;

    for (i = 0; i < data_num; i++) {
        ret = sample_svp_npu_get_line_num_and_line_byte_num(task, i, is_input, &total_line_num, &line_byte_num);
        sample_svp_check_exps_return_void(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get line num and line byte num failed!\n");

        data_buffer = svp_acl_mdl_get_dataset_buffer(dataset, i);
        sample_svp_check_exps_return_void(data_buffer == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get %u-th data_buffer failed!\n", i);

        data = svp_acl_get_data_buffer_addr(data_buffer);
        sample_svp_check_exps_return_void(data == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get %u-th data addr failed!\n", i);

        stride = svp_acl_get_data_buffer_stride(data_buffer);
        sample_svp_check_exps_return_void(stride == 0, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get %u-th data stride failed!\n", i);

        size = svp_acl_get_data_buffer_size(data_buffer);
        sample_svp_check_exps_return_void(size == 0, SAMPLE_SVP_ERR_LEVEL_ERROR, "get %u-th data size failed!\n", i);

        sample_svp_check_exps_return_void(size < (hi_u64)task->cfg.dynamic_batch_num * total_line_num * stride,
            SAMPLE_SVP_ERR_LEVEL_ERROR, "%u-th data buffer size(%zu) is less than needed(%llu)!\n",
            i, size, (hi_u64)task->cfg.dynamic_batch_num * total_line_num * stride);

        ret = snprintf_s(name, SAMPLE_SVP_NPU_MAX_NAME_LEN - 1, SAMPLE_SVP_NPU_MAX_NAME_LEN - 1, file_name, i);
        sample_svp_check_exps_return_void(ret < 0, SAMPLE_SVP_ERR_LEVEL_ERROR, "create file name failed!\n");

        fp = fopen(name, "wb");
        sample_svp_check_exps_return_void(fp == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR, "open file failed!\n");

        for (line = 0; line < task->cfg.dynamic_batch_num * total_line_num; line++) {
            ret = fwrite(data, line_byte_num, 1, fp);
            if (ret != 1) {
                fclose(fp);
                sample_svp_check_exps_return_void(1, SAMPLE_SVP_ERR_LEVEL_ERROR, "write file failed!\n");
            }
            data += stride;
        }
        (hi_void)fclose(fp);
    }
}

hi_void sample_common_svp_npu_dump_task_data(const sample_svp_npu_task_info *task)
{
    if (sample_svp_check_task_cfg(task) != HI_SUCCESS) {
        sample_svp_trace_err("check task cfg failed!\n");
        return;
    }

    sample_svp_check_exps_return_void(task->input_dataset == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "task->input_dataset is NULL!\n");
    sample_svp_check_exps_return_void(task->output_dataset == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "task->input_dataset is NULL!\n");

    (hi_void)sample_svp_npu_dump_dataset(task, "input_%u.bin", HI_TRUE);
    (hi_void)sample_svp_npu_dump_dataset(task, "output_%u.bin", HI_FALSE);
}

hi_s32 sample_common_svp_npu_update_input_data_buffer_info(hi_u8 *virt_addr, hi_u32 size, hi_u32 stride, hi_u32 idx,
    const sample_svp_npu_task_info *task)
{
    svp_acl_data_buffer *data_buffer = HI_NULL;
    svp_acl_error ret;

    data_buffer = svp_acl_mdl_get_dataset_buffer(task->input_dataset, idx);
    sample_svp_check_exps_return(data_buffer == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get %u-th data_buffer failed!\n", idx);
    ret = svp_acl_update_data_buffer(data_buffer, (hi_void *)virt_addr, size, stride);
    sample_svp_check_exps_return(ret != SVP_ACL_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "update data buffer failed!\n");
    return HI_SUCCESS;
}

hi_s32 sample_common_svp_npu_get_input_data_buffer_info(const sample_svp_npu_task_info *task, hi_u32 idx,
    hi_u8 **virt_addr, hi_u32 *size, hi_u32 *stride)
{
    svp_acl_data_buffer *data_buffer = HI_NULL;

    if (sample_svp_check_task_cfg(task) != HI_SUCCESS) {
        sample_svp_trace_err("check task cfg failed!\n");
        return HI_FAILURE;
    }
    sample_svp_check_exps_return(virt_addr == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "virt_addr is NULL!\n");
    sample_svp_check_exps_return(size == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "size is NULL!\n");
    sample_svp_check_exps_return(stride == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "size is NULL!\n");

    data_buffer = svp_acl_mdl_get_dataset_buffer(task->input_dataset, idx);
    sample_svp_check_exps_return(data_buffer == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get 0-th data_buffer failed!\n");
    *size = (hi_u32)svp_acl_get_data_buffer_size(data_buffer);
    *stride = (hi_u32)svp_acl_get_data_buffer_stride(data_buffer);
    *virt_addr = (hi_u8 *)svp_acl_get_data_buffer_addr(data_buffer);
    return HI_SUCCESS;
}

static hi_s32 sample_svp_npu_get_roi_num(const sample_svp_npu_task_info *task, sample_svp_npu_detection_info *info,
    hi_sample_svp_rect_info *rect_info)
{
    svp_acl_error ret;
    size_t num_idx;
    svp_acl_mdl_io_dims dims;
    svp_acl_data_buffer *data_buffer = HI_NULL;
    hi_float *data = HI_NULL;
    hi_float total_num = 0.0;
    hi_u32 start = 0;
    hi_u32 i;

    ret = svp_acl_mdl_get_output_index_by_name(g_svp_npu_model[task->cfg.model_idx].model_desc,
        info->num_name, &num_idx);
    sample_svp_check_exps_return(ret != SVP_ACL_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get num idx failed!\n");

    ret = svp_acl_mdl_get_output_dims(g_svp_npu_model[task->cfg.model_idx].model_desc, num_idx, &dims);
    sample_svp_check_exps_return(ret != SVP_ACL_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get num dims failed!\n");

    data_buffer = svp_acl_mdl_get_dataset_buffer(task->output_dataset, num_idx);
    sample_svp_check_exps_return(data_buffer == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get num data_buffer is NULL!\n");

    data = (hi_float *)svp_acl_get_data_buffer_addr(data_buffer);
    sample_svp_check_exps_return(data == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get num data is NULL!\n");

    if (info->has_background == HI_TRUE) {
        info->roi_offset = (hi_u32)(*data);
        start++;
    }

    for (i = start; i < dims.dims[dims.dim_count - 1]; i++) {
        total_num += *(data + i);
    }
    rect_info->num = (hi_u32)total_num;
    return HI_SUCCESS;
}

static hi_s32 sample_svp_npu_get_roi(const sample_svp_npu_task_info *task, const sample_svp_npu_detection_info *info,
    const hi_video_frame_info *proc_frame, const hi_video_frame_info *show_frame, hi_sample_svp_rect_info *rect_info)
{
    svp_acl_error ret;
    size_t roi_idx, stride;
    svp_acl_data_buffer *data_buffer = HI_NULL;
    hi_float *x_min = HI_NULL;
    hi_float *y_min = HI_NULL;
    hi_float *x_max = HI_NULL;
    hi_float *y_max = HI_NULL;
    
    hi_u32 roi_offset = info->roi_offset;
    hi_u32 i;

    ret = svp_acl_mdl_get_output_index_by_name(g_svp_npu_model[task->cfg.model_idx].model_desc,
        info->roi_name, &roi_idx);
    sample_svp_check_exps_return(ret != SVP_ACL_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get roi idx failed!\n");

    data_buffer = svp_acl_mdl_get_dataset_buffer(task->output_dataset, roi_idx);
    sample_svp_check_exps_return(data_buffer == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get roi data_buffer is NULL!\n");

    stride = svp_acl_get_data_buffer_stride(data_buffer);
    sample_svp_check_exps_return(stride == 0, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get roi stride is 0!\n");

    x_min = (hi_float *)svp_acl_get_data_buffer_addr(data_buffer);
    sample_svp_check_exps_return(x_min == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get roi data is NULL!\n");
    y_min = x_min + stride / sizeof(hi_float);
    x_max = y_min + stride / sizeof(hi_float);
    y_max = x_max + stride / sizeof(hi_float);
    
    //maohw
    hi_float *score = y_max + stride / sizeof(hi_float);
    hi_float *id = score + stride / sizeof(hi_float);
    
    for (i = 0; i < rect_info->num; i++) {
      
        rect_info->id[i] = (hi_u32)id[roi_offset];
        rect_info->score[i] = (hi_float)score[roi_offset];
        
        rect_info->rect[i].point[SAMPLE_SVP_NPU_RECT_LEFT_TOP].x = (hi_u32)((hi_float)x_min[roi_offset] /
            proc_frame->video_frame.width * show_frame->video_frame.width) & (~1);
        rect_info->rect[i].point[SAMPLE_SVP_NPU_RECT_LEFT_TOP].y = (hi_u32)((hi_float)y_min[roi_offset] /
            proc_frame->video_frame.height * show_frame->video_frame.height) & (~1);

        rect_info->rect[i].point[SAMPLE_SVP_NPU_RECT_RIGHT_TOP].x = (hi_u32)((hi_float)x_max[roi_offset] /
            proc_frame->video_frame.width * show_frame->video_frame.width) & (~1);
        rect_info->rect[i].point[SAMPLE_SVP_NPU_RECT_RIGHT_TOP].y =
            rect_info->rect[i].point[SAMPLE_SVP_NPU_RECT_LEFT_TOP].y;

        rect_info->rect[i].point[SAMPLE_SVP_NPU_RECT_RIGHT_BOTTOM].x =
            rect_info->rect[i].point[SAMPLE_SVP_NPU_RECT_RIGHT_TOP].x;
        rect_info->rect[i].point[SAMPLE_SVP_NPU_RECT_RIGHT_BOTTOM].y = (hi_u32)((hi_float)y_max[roi_offset] /
            proc_frame->video_frame.height * show_frame->video_frame.height) & (~1);

        rect_info->rect[i].point[SAMPLE_SVP_NPU_RECT_LEFT_BOTTOM].x =
            rect_info->rect[i].point[SAMPLE_SVP_NPU_RECT_LEFT_TOP].x;
        rect_info->rect[i].point[SAMPLE_SVP_NPU_RECT_LEFT_BOTTOM].y =
            rect_info->rect[i].point[SAMPLE_SVP_NPU_RECT_RIGHT_BOTTOM].y;
        roi_offset++;
    }
    return HI_SUCCESS;
}

hi_s32 sample_common_svp_vgs_fill_joint_coords_lines(const hi_video_frame_info *frame_info,
    hi_sample_svp_coords_info *coords_info)
{
    hi_vgs_handle vgs_handle = -1;
    hi_s32 ret = HI_FAILURE;
    hi_u16 i;
    hi_vgs_task_attr vgs_task;
    hi_u32 idx_start, idx_end;
    hi_vgs_line vgs_line[SAMPLE_SVP_NPU_JOINT_COORDS_NUM];
    hi_u32 line[SAMPLE_SVP_NPU_JOINT_COORDS_NUM][2] = {
        {1, 3}, {1, 0}, {2, 4}, {2, 0}, {0, 5}, {0, 6}, {5, 7}, {7, 9}, {6, 8}, {8, 10}, {5, 11}, {6, 12}, {11, 12},
        {11, 13}, {13, 15}, {12, 14}, {14, 16}
    };
    hi_u32 color[SAMPLE_SVP_NPU_JOINT_COORDS_NUM] = {
        0x0000FF, 0x00FFFF, 0xFF00FF, 0x0055FF,
        0x00AAFF, 0x00FFAA, 0x00FF55, 0x00FF00,
        0x55FF00, 0xAAFF00, 0xFFFF00, 0xFFAA00,
        0xFF5500, 0xFF0000, 0xFF0055, 0xFF00AA,
        0xAA00FF
    };

    sample_svp_check_exps_return(frame_info == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "frame_info can't be null\n");
    sample_svp_check_exps_return(coords_info == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "rect can't be null\n");

    ret = hi_mpi_vgs_begin_job(&vgs_handle);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Vgs begin job fail,Error(%#x)\n", ret);

    ret = memcpy_s(&vgs_task.img_in, sizeof(hi_video_frame_info), frame_info, sizeof(hi_video_frame_info));
    sample_svp_check_exps_goto(ret != EOK, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "get img_in failed\n");
    ret = memcpy_s(&vgs_task.img_out, sizeof(hi_video_frame_info), frame_info, sizeof(hi_video_frame_info));
    sample_svp_check_exps_goto(ret != EOK, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "get img_out failed\n");

    for (i = 0; i < SAMPLE_SVP_NPU_JOINT_COORDS_NUM; i++) {
        idx_start = line[i][0];
        idx_end = line[i][1];
        vgs_line[i].start_point.x = (hi_u32)coords_info->coords[idx_start].point[0] & ~1;
        vgs_line[i].start_point.y =  (hi_u32)coords_info->coords[idx_start].point[1] & ~1;
        vgs_line[i].end_point.x =  (hi_u32)coords_info->coords[idx_end].point[0] & ~1;
        vgs_line[i].end_point.y =  (hi_u32)coords_info->coords[idx_end].point[1] & ~1;
        vgs_line[i].thick = SAMPLE_SVP_NPU_LINE_THICK;
        vgs_line[i].color = color[i];
    }
    ret = hi_mpi_vgs_add_draw_line_task(vgs_handle, &vgs_task, vgs_line, SAMPLE_SVP_NPU_JOINT_COORDS_NUM);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "hi_mpi_vgs_draw_line_task fail,Error(%#x)\n", ret);
    ret = hi_mpi_vgs_end_job(vgs_handle);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "hi_mpi_vgs_end_job fail,Error(%#x)\n", ret);

    return ret;
fail:
    hi_mpi_vgs_cancel_job(vgs_handle);
    return ret;
}

static hi_s32 sample_svp_npu_get_output_info(const sample_svp_npu_task_info *task, size_t *stride,
    hi_float **virt_addr, svp_acl_mdl_io_dims *dim)
{
    svp_acl_error ret;
    svp_acl_data_buffer *data_buffer = svp_acl_mdl_get_dataset_buffer(task->output_dataset, 0);
    sample_svp_check_exps_return(data_buffer == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get roi data_buffer is NULL!\n");

    *stride = svp_acl_get_data_buffer_stride(data_buffer) / sizeof(hi_float);
    sample_svp_check_exps_return(*stride == 0, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get stride is 0!\n");

    *virt_addr = (hi_float *)svp_acl_get_data_buffer_addr(data_buffer);
    sample_svp_check_exps_return(*virt_addr == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get addr failed!\n");

    ret = svp_acl_mdl_get_output_dims(g_svp_npu_model[task->cfg.model_idx].model_desc, 0, dim);
    sample_svp_check_exps_return(ret != HI_SUCCESS || dim->dim_count != SAMPLE_SVP_NPU_DIM_NUM ||
        dim->dims[1] > UINT_MAX || dim->dims[SAMPLE_SVP_NPU_H_DIM_IDX] > UINT_MAX ||
        dim->dims[SAMPLE_SVP_NPU_W_DIM_IDX] > UINT_MAX, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get dim failed!\n");
    return HI_SUCCESS;
}

static hi_s32 sample_svp_npu_get_and_calc_joint_coords(const sample_svp_npu_task_info *task,
    const hi_video_frame_info *proc_frame,
    const hi_video_frame_info *show_frame, hi_sample_svp_coords_info *coords_info)
{
    svp_acl_error ret;
    hi_u32 c, h, w, i, j, k;
    size_t stride;
    svp_acl_mdl_io_dims dim;
    hi_float *out = HI_NULL;
    hi_float diff = 0.25;

    ret = sample_svp_npu_get_output_info(task, &stride, &out, &dim);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get output info failed!\n");
    c = dim.dims[1];
    h = dim.dims[SAMPLE_SVP_NPU_H_DIM_IDX];
    w = dim.dims[SAMPLE_SVP_NPU_W_DIM_IDX];

    /* get max value coordinates for each h * w */
    for (i = 0; i < c; i++) {
        hi_float max = 0.0;
        for (j = 0; j < h; j++) {
            for (k = 0; k < w; k++) {
                hi_float temp = out[i * (h * stride) + j * stride + k];
                max = ((temp >= max) ? temp : max);
                coords_info->coords[i].point[0] = (temp == max ? k *1.0 : coords_info->coords[i].point[0]);
                coords_info->coords[i].point[1] = (temp == max ? j *1.0 : coords_info->coords[i].point[1]);
            }
        }
    }
    hi_float affine_zoom = (hi_float)proc_frame->video_frame.width / (hi_float)w;
    hi_float affine_offset = ((hi_float)proc_frame->video_frame.height - (h * affine_zoom)) / 2.0;
    hi_float multiples_w = (hi_float)show_frame->video_frame.width / (hi_float)proc_frame->video_frame.width;
    hi_float multiples_h = (hi_float)show_frame->video_frame.height / (hi_float)proc_frame->video_frame.height;
    /* calc diff */
    for (i = 0; i < c; i++) {
        hi_s32 px = (hi_s32)coords_info->coords[i].point[0];
        hi_s32 py = (hi_s32)coords_info->coords[i].point[1];
        if (px > 1 && px < w - 1 && py > 1 && py < h - 1) {
            double diffx = out[i * (h * stride) + py * stride + px + 1] - out[i * (h * stride) + py * stride + px - 1];
            double diffy = out[i * (h * stride) + (py + 1) * stride + px] -
                out[i * (h * stride) + (py - 1) * stride + px];
            coords_info->coords[i].point[0] += diffx > 0 ? diff : diffx < 0 ? -1 * diff : 0;
            coords_info->coords[i].point[1] += diffy > 0 ? diff : diffy < 0 ? -1 * diff : 0;
        }
        /* AffineTransform from proc fram to show fram */
        coords_info->coords[i].point[0] = affine_zoom * coords_info->coords[i].point[0] * multiples_w;
        coords_info->coords[i].point[1] = (affine_zoom * coords_info->coords[i].point[1] + affine_offset) * multiples_h;
    }
    return HI_SUCCESS;
}

hi_s32 sample_common_svp_npu_get_joint_coords(const sample_svp_npu_task_info *task,
    const hi_video_frame_info *proc_frame, const hi_video_frame_info *show_frame,
    hi_sample_svp_coords_info *coords_info)
{
    hi_s32 ret;

    if (sample_svp_check_task_cfg(task) != HI_SUCCESS) {
        sample_svp_trace_err("check task cfg failed!\n");
        return HI_FAILURE;
    }
    sample_svp_check_exps_return(proc_frame == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "proc_frame is NULL!\n");
    sample_svp_check_exps_return(show_frame == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "show_frame is NULL!\n");
    sample_svp_check_exps_return(coords_info == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "rect_info is NULL!\n");

    ret = sample_svp_npu_get_and_calc_joint_coords(task, proc_frame, show_frame, coords_info);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get calc joint coords failed!\n");

    return HI_SUCCESS;
}

hi_s32 sample_common_svp_npu_roi_to_rect(const sample_svp_npu_task_info *task,
    sample_svp_npu_detection_info *detection_info, const hi_video_frame_info *proc_frame,
    const hi_video_frame_info *show_frame, hi_sample_svp_rect_info *rect_info)
{
    hi_s32 ret;

    if (sample_svp_check_task_cfg(task) != HI_SUCCESS) {
        sample_svp_trace_err("check task cfg failed!\n");
        return HI_FAILURE;
    }
    sample_svp_check_exps_return(detection_info == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "detection_info is NULL!\n");
    sample_svp_check_exps_return(proc_frame == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "proc_frame is NULL!\n");
    sample_svp_check_exps_return(show_frame == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "show_frame is NULL!\n");
    sample_svp_check_exps_return(rect_info == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "rect_info is NULL!\n");

    detection_info->roi_offset = 0;
    ret = sample_svp_npu_get_roi_num(task, detection_info, rect_info);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get roi num failed!\n");

    ret = sample_svp_npu_get_roi(task, detection_info, proc_frame, show_frame, rect_info);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get roi failed!\n");

    return HI_SUCCESS;
}


hi_s32 sample_common_svp_npu_set_threshold(sample_svp_npu_threshold threshold[], hi_u32 threshold_num,
    const sample_svp_npu_task_info *task)
{
    hi_u32 i, n;
    svp_acl_error ret;
    svp_acl_data_buffer *data_buffer = HI_NULL;
    hi_float *data = HI_NULL;
    size_t idx, size;

    if (sample_svp_check_task_cfg(task) != HI_SUCCESS) {
        sample_svp_trace_err("check task cfg failed!\n");
        return HI_FAILURE;
    }
    sample_svp_check_exps_return(threshold == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "threshold is NULL!\n");
    sample_svp_check_exps_return(threshold_num == 0, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "threshold_num is 0!\n");

    for (i = 0; i < threshold_num; i++) {
        ret = svp_acl_mdl_get_input_index_by_name(g_svp_npu_model[task->cfg.model_idx].model_desc,
            threshold[i].name, &idx);
        sample_svp_check_exps_return(ret != SVP_ACL_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get roi idx failed!\n");

        data_buffer = svp_acl_mdl_get_dataset_buffer(task->input_dataset, idx);
        sample_svp_check_exps_return(data_buffer == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get roi data_buffer is NULL!\n");

        size = svp_acl_get_data_buffer_size(data_buffer);
        sample_svp_check_exps_return(size < SAMPLE_SVP_NPU_THRESHOLD_NUM * sizeof(hi_float), HI_FAILURE,
            SAMPLE_SVP_ERR_LEVEL_ERROR, "get size(%zu) is less than %lu!\n",
            size, SAMPLE_SVP_NPU_THRESHOLD_NUM * sizeof(hi_float));

        data = (hi_float *)svp_acl_get_data_buffer_addr(data_buffer);
        sample_svp_check_exps_return(data == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get roi data is NULL!\n");
        n = 0;
        data[n++] = threshold[i].nms_threshold;
        data[n++] = threshold[i].score_threshold;
        data[n++] = threshold[i].min_height;
        data[n++] = threshold[i].min_width;
    }
    return HI_SUCCESS;
}

hi_s32 sample_common_svp_npu_check_has_aicpu_task(const sample_svp_npu_task_info *task, hi_bool *has_aicpu_task)
{
    hi_u32 aicpu_task_num;
    svp_acl_error ret;

    if (sample_svp_check_task_cfg(task) != HI_SUCCESS) {
        sample_svp_trace_err("check task cfg failed!\n");
        return HI_FAILURE;
    }
    sample_svp_check_exps_return(has_aicpu_task == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "has_aicpu_task is NULL!\n");

    ret = svp_acl_ext_get_mdl_aicpu_task_num(g_svp_npu_model[task->cfg.model_idx].model_id, &aicpu_task_num);
    sample_svp_check_exps_return(ret != SVP_ACL_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get aicpu task num failed, error code is %d!\n", ret);

    *has_aicpu_task = (aicpu_task_num == 0) ? HI_FALSE : HI_TRUE;
    return HI_SUCCESS;
}

sample_svp_npu_model_info* sample_common_svp_npu_get_model_info(hi_u32 model_idx)
{
    sample_svp_check_exps_return(model_idx >= SAMPLE_SVP_NPU_MAX_MODEL_NUM, NULL, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "model_idx(%u) must be [0, %u)!\n", model_idx, SAMPLE_SVP_NPU_MAX_MODEL_NUM);
    return &g_svp_npu_model[model_idx];
}

hi_s32 sample_common_svp_npu_get_input_resolution(hi_u32 model_idx, hi_u32 input_idx,
    hi_size *pic_size)
{
    svp_acl_error ret;
    svp_acl_mdl_io_dims dims = {0};

    ret = svp_acl_mdl_get_input_dims(g_svp_npu_model[model_idx].model_desc, input_idx, &dims);
    sample_svp_check_exps_return(ret != SVP_ACL_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get num dims failed!\n");
    pic_size->height = dims.dims[dims.dim_count - SAMPLE_SVP_NPU_H_DIM_IDX]; // NCHW
    pic_size->width = dims.dims[dims.dim_count - 1]; // NCHW
    return HI_SUCCESS;
}

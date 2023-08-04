/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef SAMPLE_NPU_MODEL_H
#define SAMPLE_NPU_MODEL_H
#include <stddef.h>
#include "hi_type.h"
#include "acl.h"
#include "hi_mpi_sys.h"

#define MAX_THREAD_NUM 20
#define MAX_INPUT_NUM 5
#define MAX_OUTPUT_NUM 5

typedef struct npu_acl_model {
    hi_u32 model_id;
    hi_ulong model_mem_size;
    hi_ulong model_weight_size;
    hi_void *model_mem_ptr;
    hi_void *model_weight_ptr;
    hi_phys_addr_t model_mem_phy_addr;
    hi_phys_addr_t model_weight_phy_addr;
    hi_bool is_load_flag;
    aclmdlDesc *model_desc;
    aclmdlDataset *input_dataset;
    aclmdlDataset *output_dataset;
    hi_phys_addr_t output_phy_addr[MAX_OUTPUT_NUM];
    hi_phys_addr_t input_phy_addr[MAX_INPUT_NUM];
} npu_acl_model_t;

typedef struct npu_output{
    int size;
    float *dst;
} npu_output_t;


hi_s32 sample_npu_load_model_with_mem(const char *model_path, hi_u32 model_index);
hi_s32 sample_npu_load_model_with_mem_cached(const char *model_path, hi_u32 model_index);

hi_s32 sample_npu_create_desc(hi_u32 model_index);
hi_void sample_npu_destroy_desc(hi_u32 model_index);

hi_s32 sample_npu_get_input_size_by_index(const hi_u32 index, size_t *inputSize, hi_u32 model_index);
hi_s32 sample_npu_create_input_dataset(hi_u32 model_index);
hi_void sample_npu_destroy_input_dataset(hi_u32 model_index);
hi_s32 sample_npu_create_input_databuf(hi_void *data_buf, size_t data_len, hi_u32 model_index);
hi_void sample_npu_destroy_input_databuf(hi_u32 model_index);

hi_s32 sample_npu_create_output(hi_u32 model_index);
hi_void sample_npu_output_model_result(npu_output_t *output, hi_u32 model_index);
hi_void sample_npu_destroy_output(hi_u32 model_index);

hi_s32 sample_npu_model_execute(hi_u32 model_index);

hi_void sample_npu_unload_model(hi_u32 model_index);
hi_void sample_npu_unload_model_cached(hi_u32 model_index);

hi_s32 sample_npu_create_cached_input(hi_u32 model_index);
hi_s32 sample_npu_create_cached_output(hi_u32 model_index);
hi_void sample_npu_destroy_cached_input(hi_u32 model_index);
hi_void sample_npu_destroy_cached_output(hi_u32 model_index);

hi_s32 sample_svp_npu_loop_execute_dynamicbatch(hi_u32 model_index);
#endif

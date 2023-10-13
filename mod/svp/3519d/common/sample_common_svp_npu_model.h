/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef SAMPLE_COMMON_SVP_NPU_MODEL_H
#define SAMPLE_COMMON_SVP_NPU_MODEL_H
#include "hi_type.h"
#include "svp_acl_mdl.h"
#include "sample_common_svp.h"
#include "sample_common_svp_npu.h"

hi_s32 sample_common_svp_npu_get_input_data(const hi_char *src[], hi_u32 file_num,
    const sample_svp_npu_task_info *task);

hi_s32 sample_common_svp_npu_load_model(const hi_char *model_path, hi_u32 model_index, hi_bool is_cached);
hi_s32 sample_common_svp_npu_load_model_with_config(const hi_char *model_path, hi_u32 model_index,
    hi_bool is_cached);
hi_void sample_common_svp_npu_unload_model(hi_u32 model_index);

hi_void sample_svp_npu_set_model_prior_info(hi_u32 model_index, sample_svp_npu_model_prior_cfg *prior_cfg);

hi_s32 sample_common_svp_npu_create_input(sample_svp_npu_task_info *task);
hi_s32 sample_common_svp_npu_create_output(sample_svp_npu_task_info *task);
hi_void sample_common_svp_npu_destroy_input(sample_svp_npu_task_info *task);
hi_void sample_common_svp_npu_destroy_output(sample_svp_npu_task_info *task);

hi_s32 sample_common_svp_npu_create_task_buf(sample_svp_npu_task_info *task);
hi_s32 sample_common_svp_npu_create_work_buf(sample_svp_npu_task_info *task);
hi_void sample_common_svp_npu_destroy_task_buf(sample_svp_npu_task_info *task);
hi_void sample_common_svp_npu_destroy_work_buf(sample_svp_npu_task_info *task);

hi_s32 sample_common_svp_npu_get_work_buf_info(const sample_svp_npu_task_info *task,
    hi_u32 *work_buf_size, hi_u32 *work_buf_stride);
hi_s32 sample_common_svp_npu_share_work_buf(const sample_svp_npu_shared_work_buf *shared_work_buf,
    const sample_svp_npu_task_info *task);

hi_s32 sample_common_svp_npu_set_input_aipp_info(td_u32 model_index, td_u32 index,
    const sample_svp_npu_model_aipp *model_aipp, const sample_svp_npu_task_info *task);

hi_s32 sample_common_svp_npu_set_dynamic_batch(const sample_svp_npu_task_info *task);
hi_s32 sample_common_svp_npu_model_execute(const sample_svp_npu_task_info *task);
hi_s32 sample_common_svp_npu_model_execute_async(const sample_svp_npu_task_info *task,
    svp_acl_rt_stream stream);
hi_void sample_common_svp_npu_output_classification_result(const sample_svp_npu_task_info *task);

hi_void sample_common_svp_npu_dump_task_data(const sample_svp_npu_task_info *task);

hi_s32 sample_common_svp_npu_set_threshold(sample_svp_npu_threshold threshold[], hi_u32 threshold_num,
    const sample_svp_npu_task_info *task);

hi_s32 sample_common_svp_npu_roi_to_rect(const sample_svp_npu_task_info *task,
    sample_svp_npu_detection_info *detection_info, const hi_video_frame_info *proc_frame,
    const hi_video_frame_info *show_frame, hi_sample_svp_rect_info *rect_info);

hi_s32 sample_common_svp_npu_get_joint_coords(const sample_svp_npu_task_info *task,
    const hi_video_frame_info *proc_frame, const hi_video_frame_info *show_frame,
    hi_sample_svp_coords_info *coords_info);

hi_s32 sample_common_svp_vgs_fill_joint_coords_lines(const hi_video_frame_info *frame_info,
    hi_sample_svp_coords_info *coords_info);

hi_s32 sample_common_svp_npu_update_input_data_buffer_info(hi_u8 *virt_addr, hi_u32 size, hi_u32 stride, hi_u32 idx,
    const sample_svp_npu_task_info *task);

hi_s32 sample_common_svp_npu_get_input_data_buffer_info(const sample_svp_npu_task_info *task, hi_u32 idx,
    hi_u8 **virt_addr, hi_u32 *size, hi_u32 *stride);

hi_s32 sample_common_svp_npu_check_has_aicpu_task(const sample_svp_npu_task_info *task, hi_bool *has_aicpu_task);

sample_svp_npu_model_info* sample_common_svp_npu_get_model_info(hi_u32 model_idx);

hi_s32 sample_common_svp_npu_get_input_resolution(hi_u32 model_idx, hi_u32 input_idx,
    hi_size *pic_size);
#endif

/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: Function of mpi_pq_ai.h
 * Author: ISP SW
 * Create: 2012/06/28
 */

#ifndef __MPI_PQ_AI_ADAPT_H__
#define __MPI_PQ_AI_ADAPT_H__

#include "hi_type.h"
#include "hi_common_adapt.h"
#include "hi_comm_pq_ai.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/* PQ for AI */
hi_s32 hi_mpi_pq_ai_set_group_attr(hi_vi_pipe ai_pipe, const hi_pq_ai_group_attr *ai_group_attr);
hi_s32 hi_mpi_pq_ai_get_group_attr(hi_vi_pipe ai_pipe, hi_pq_ai_group_attr *ai_group_attr);

hi_s32 hi_mpi_pq_ai_set_color_attr(hi_vi_pipe ai_pipe, const hi_pq_ai_color_attr *color_attr);
hi_s32 hi_mpi_pq_ai_get_color_attr(hi_vi_pipe ai_pipe, hi_pq_ai_color_attr *color_attr);

hi_s32 hi_mpi_pq_ai_set_sharpness_attr(hi_vi_pipe ai_pipe, const hi_pq_ai_sharpness_attr *shp_attr);
hi_s32 hi_mpi_pq_ai_get_sharpness_attr(hi_vi_pipe ai_pipe, hi_pq_ai_sharpness_attr *shp_attr);

hi_s32 hi_mpi_pq_ai_set_noiseness_pre_attr(hi_vi_pipe ai_pipe, const hi_pq_ai_noiseness_pre_attr *noise_attr);
hi_s32 hi_mpi_pq_ai_get_noiseness_pre_attr(hi_vi_pipe ai_pipe, hi_pq_ai_noiseness_pre_attr *noise_attr);

hi_s32 hi_mpi_pq_ai_set_noiseness_post_attr(hi_vi_pipe ai_pipe, const hi_pq_ai_noiseness_post_attr *noise_attr);
hi_s32 hi_mpi_pq_ai_get_noiseness_post_attr(hi_vi_pipe ai_pipe, hi_pq_ai_noiseness_post_attr *noise_attr);

hi_s32 hi_mpi_pq_ai_set_brightness_attr(hi_vi_pipe ai_pipe, const hi_pq_ai_brightness_attr *bright_attr);
hi_s32 hi_mpi_pq_ai_get_brightness_attr(hi_vi_pipe ai_pipe, hi_pq_ai_brightness_attr *bright_attr);

hi_s32 hi_mpi_pq_ai_set_contrast_attr(hi_vi_pipe ai_pipe, const hi_pq_ai_contrast_attr *contrast_attr);
hi_s32 hi_mpi_pq_ai_get_contrast_attr(hi_vi_pipe ai_pipe, hi_pq_ai_contrast_attr *contrast_attr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
#endif /* __MPI_PQ_AI_ADAPT_H__ */

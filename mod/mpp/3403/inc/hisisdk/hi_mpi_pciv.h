/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_PCIV_H__
#define __HI_MPI_PCIV_H__

#include "hi_type.h"
#include "hi_common_pciv.h"


#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_pciv_malloc_chn_buf(hi_pciv_chn chn, hi_u32 blk_size, hi_u32 blk_cnt, hi_phys_addr_t phys_addr[]);
hi_s32 hi_mpi_pciv_free_chn_buf(hi_pciv_chn chn, hi_u32 blk_cnt);
hi_s32 hi_mpi_pciv_create_chn(hi_pciv_chn chn, const hi_pciv_attr *attr);
hi_s32 hi_mpi_pciv_destroy_chn(hi_pciv_chn chn);
hi_s32 hi_mpi_pciv_start_chn(hi_pciv_chn chn);
hi_s32 hi_mpi_pciv_stop_chn(hi_pciv_chn chn);
hi_s32 hi_mpi_pciv_set_chn_attr(hi_pciv_chn chn, const hi_pciv_attr *attr);
hi_s32 hi_mpi_pciv_get_chn_attr(hi_pciv_chn chn, hi_pciv_attr *attr);
hi_s32 hi_mpi_pciv_show_chn(hi_pciv_chn chn);
hi_s32 hi_mpi_pciv_hide_chn(hi_pciv_chn chn);

hi_s32 hi_mpi_pciv_create_window_vb(const hi_pciv_window_vb_cfg *cfg);
hi_s32 hi_mpi_pciv_destroy_window_vb(hi_void);
hi_s32 hi_mpi_pciv_malloc_window_buf(hi_u32 blk_size, hi_u32 blk_cnt, hi_phys_addr_t phys_addr[]);
hi_s32 hi_mpi_pciv_free_window_buf(hi_u32 blk_cnt, const hi_phys_addr_t phys_addr[]);
hi_s32 hi_mpi_pciv_dma_task(const hi_pciv_dma_task *task);

hi_s32 hi_mpi_pciv_get_local_id(hi_s32 *id);
hi_s32 hi_mpi_pciv_enum_chip(hi_pciv_enum_chip *chips);
hi_s32 hi_mpi_pciv_get_window_base(hi_s32 chip_id, hi_pciv_window_base *base);

#ifdef __cplusplus
}
#endif
#endif /* __HI_MPI_PCIV_H__ */

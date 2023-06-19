/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_VB_H__
#define __HI_MPI_VB_H__

#include "hi_common_vb.h"
#include "hi_common_video.h"


#ifdef __cplusplus
extern "C" {
#endif

hi_vb_pool hi_mpi_vb_create_pool(const hi_vb_pool_cfg *vb_pool_cfg);
hi_vb_pool hi_mpi_vb_create_ext_pool(const hi_vb_pool_cfg *vb_pool_cfg);
hi_s32 hi_mpi_vb_destroy_pool(hi_vb_pool pool);

hi_vb_blk hi_mpi_vb_get_blk(hi_vb_pool pool, hi_u64 blk_size, const hi_char *mmz_name);
hi_s32 hi_mpi_vb_release_blk(hi_vb_blk vb_blk);

hi_vb_blk hi_mpi_vb_insert_buf_to_ext_pool(hi_vb_pool pool, hi_phys_addr_t phys_addr, hi_u64 size, hi_bool is_filled);
hi_s32 hi_mpi_vb_delete_buf_from_ext_pool(hi_vb_blk block, hi_s32 milli_sec);

hi_vb_blk hi_mpi_vb_phys_addr_to_handle(hi_phys_addr_t phys_addr);
hi_phys_addr_t hi_mpi_vb_handle_to_phys_addr(hi_vb_blk vb_blk);
hi_vb_pool hi_mpi_vb_handle_to_pool_id(hi_vb_blk vb_blk);

hi_s32 hi_mpi_vb_inquire_user_cnt(hi_vb_blk vb_blk);

hi_s32 hi_mpi_vb_get_supplement_addr(hi_vb_blk vb_blk, hi_video_supplement *supplement);

hi_s32 hi_mpi_vb_set_supplement_cfg(const hi_vb_supplement_cfg *supplement_cfg);
hi_s32 hi_mpi_vb_get_supplement_cfg(hi_vb_supplement_cfg *supplement_cfg);

hi_s32 hi_mpi_vb_init(hi_void);
hi_s32 hi_mpi_vb_exit(hi_void);

hi_s32 hi_mpi_vb_set_cfg(const hi_vb_cfg *vb_cfg);
hi_s32 hi_mpi_vb_get_cfg(hi_vb_cfg *vb_cfg);

hi_s32 hi_mpi_vb_get_pool_info(hi_vb_pool pool, hi_vb_pool_info *pool_info);

hi_s32 hi_mpi_vb_init_mod_common_pool(hi_vb_uid vb_uid);
hi_s32 hi_mpi_vb_exit_mod_common_pool(hi_vb_uid vb_uid);

hi_s32 hi_mpi_vb_set_mod_pool_cfg(hi_vb_uid vb_uid, const hi_vb_cfg *vb_cfg);
hi_s32 hi_mpi_vb_get_mod_pool_cfg(hi_vb_uid vb_uid, hi_vb_cfg *vb_cfg);

#ifdef __cplusplus
}
#endif
#endif /* __HI_MPI_VB_H__ */

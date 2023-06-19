/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_SYS_H__
#define __HI_MPI_SYS_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_common_sys.h"
#include "hi_debug.h"


#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_log_set_level_cfg(const hi_log_level_cfg *level_cfg);
hi_s32 hi_mpi_log_get_level_cfg(hi_log_level_cfg *level_cfg);

hi_s32 hi_mpi_log_set_wait_flag(hi_bool flag);
hi_s32 hi_mpi_log_read(hi_char *buf, hi_u32 size);
hi_void hi_mpi_log_close(hi_void);

hi_s32 hi_mpi_sys_init(hi_void);
hi_s32 hi_mpi_sys_exit(hi_void);

hi_s32 hi_mpi_sys_set_cfg(const hi_mpp_sys_cfg *sys_cfg);
hi_s32 hi_mpi_sys_get_cfg(hi_mpp_sys_cfg *sys_cfg);

hi_s32 hi_mpi_sys_bind(const hi_mpp_chn *src_chn, const hi_mpp_chn *dst_chn);
hi_s32 hi_mpi_sys_unbind(const hi_mpp_chn *src_chn, const hi_mpp_chn *dst_chn);

hi_s32 hi_mpi_sys_get_bind_by_dst(const hi_mpp_chn *dst_chn, hi_mpp_chn *src_chn);
hi_s32 hi_mpi_sys_get_bind_by_src(const hi_mpp_chn *src_chn, hi_mpp_bind_dst *bind_dst);

hi_s32 hi_mpi_sys_get_version(hi_mpp_version *version);

hi_s32 hi_mpi_sys_get_cur_pts(hi_u64 *cur_pts);
hi_s32 hi_mpi_sys_init_pts_base(hi_u64 pts_base);
hi_s32 hi_mpi_sys_sync_pts(hi_u64 pts_base);

hi_void *hi_mpi_sys_mmap(hi_phys_addr_t phys_addr, hi_u32 size);
hi_s32 hi_mpi_sys_munmap(const hi_void *virt_addr, hi_u32 size);
hi_void *hi_mpi_sys_mmap_cached(hi_phys_addr_t phys_addr, hi_u32 size);

hi_s32 hi_mpi_sys_close_fd(hi_void);

/* alloc mmz memory in user context */
hi_s32 hi_mpi_sys_mmz_alloc(hi_phys_addr_t *phys_addr, hi_void **virt_addr, const hi_char *mmb, const hi_char *zone,
                            hi_u32 len);
/* alloc mmz memory with cache */
hi_s32 hi_mpi_sys_mmz_alloc_cached(hi_phys_addr_t *phys_addr, hi_void **virt_addr,
                                   const hi_char *mmb, const hi_char *zone, hi_u32 len);
/* free mmz memory in user context */
hi_s32 hi_mpi_sys_mmz_free(hi_phys_addr_t phys_addr, const hi_void *virt_addr);

/* flush cache */
hi_s32 hi_mpi_sys_flush_cache(hi_phys_addr_t phys_addr, hi_void *virt_addr, hi_u32 size);

/* get virtual meminfo according to virtual addr, should be in one process */
hi_s32 hi_mpi_sys_get_virt_mem_info(const void *virt_addr, hi_sys_virt_mem_info *mem_info);

hi_s32 hi_mpi_sys_set_mem_cfg(const hi_mpp_chn *mpp_chn, const hi_char *mmz_name);
hi_s32 hi_mpi_sys_get_mem_cfg(const hi_mpp_chn *mpp_chn, hi_char *mmz_name);

hi_s32 hi_mpi_sys_set_scale_coef_level(const hi_scale_range *scale_range,
    const hi_scale_coef_level *scale_coef_level);
hi_s32 hi_mpi_sys_get_scale_coef_level(const hi_scale_range *scale_range, hi_scale_coef_level *scale_coef_level);

hi_s32 hi_mpi_sys_set_time_zone(hi_s32 time_zone);
hi_s32 hi_mpi_sys_get_time_zone(hi_s32 *time_zone);

hi_s32 hi_mpi_sys_set_gps_info(const hi_gps_info *gps_info);
hi_s32 hi_mpi_sys_get_gps_info(hi_gps_info *gps_info);

hi_s32 hi_mpi_sys_set_vi_vpss_mode(const hi_vi_vpss_mode *vi_vpss_mode);
hi_s32 hi_mpi_sys_get_vi_vpss_mode(hi_vi_vpss_mode *vi_vpss_mode);

hi_s32 hi_mpi_sys_set_vi_video_mode(hi_vi_video_mode video_mode);
hi_s32 hi_mpi_sys_get_vi_video_mode(hi_vi_video_mode *video_mode);

hi_s32 hi_mpi_sys_get_chip_id(hi_u32 *chip_id);
hi_s32 hi_mpi_sys_get_custom_code(hi_u32 *custom_code);
hi_s32 hi_mpi_sys_get_unique_id(hi_unique_id *unique_id);

hi_s32 hi_mpi_sys_set_raw_frame_compress_param(const hi_raw_frame_compress_param *compress_param);
hi_s32 hi_mpi_sys_get_raw_frame_compress_param(hi_raw_frame_compress_param *compress_param);

hi_s32 hi_mpi_sys_set_tuning_connect(hi_s32 connect);
hi_s32 hi_mpi_sys_get_tuning_connect(hi_s32 *connect);

hi_s32 hi_mpi_sys_set_schedule_mode(const hi_schedule_mode *schedule_mode);
hi_s32 hi_mpi_sys_get_schedule_mode(hi_schedule_mode *schedule_mode);


#ifdef __cplusplus
}
#endif
#endif /* __HI_MPI_SYS_H__ */

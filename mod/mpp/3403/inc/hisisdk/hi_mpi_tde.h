/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_TDE_H__
#define __HI_MPI_TDE_H__

#include "hi_common_tde.h"


#ifdef __cplusplus
extern "C" {
#endif

/* API Declaration */
hi_s32 hi_tde_open(hi_void);
hi_void hi_tde_close(hi_void);
hi_s32 hi_tde_begin_job(hi_void);
hi_s32 hi_tde_end_job(hi_s32 handle, hi_bool is_sync, hi_bool is_block, hi_u32 time_out);
hi_s32 hi_tde_cancel_job(hi_s32 handle);
hi_s32 hi_tde_wait_for_done(hi_s32 handle);
hi_s32 hi_tde_wait_all_done(hi_void);
hi_s32 hi_tde_reset(hi_void);
hi_s32 hi_tde_quick_fill(hi_s32 handle, const hi_tde_none_src *none_src, hi_u32 fill_data);
hi_s32 hi_tde_quick_draw_rect(hi_s32 handle, const hi_tde_none_src *none_src,
                              const hi_tde_corner_rect_info *corner_rect_info);
hi_s32 hi_tde_draw_multi_rect(hi_s32 handle, const hi_tde_surface *dst_surface, const hi_tde_corner_rect *corner_rect,
                              hi_u32 num);
hi_s32 hi_tde_draw_line(hi_s32 handle, const hi_tde_surface *dst_surface, const hi_tde_line *line, hi_u32 num);
hi_s32 hi_tde_quick_copy(hi_s32 handle, const hi_tde_single_src *single_src);
hi_s32 hi_tde_quick_resize(hi_s32 handle, const hi_tde_single_src *single_src);
hi_s32 hi_tde_quick_deflicker(hi_s32 handle, const hi_tde_single_src *single_src);
hi_s32 hi_tde_solid_draw(hi_s32 handle, const hi_tde_single_src *single_src, const hi_tde_fill_color *fill_color,
                         const hi_tde_opt *opt);
hi_s32 hi_tde_rotate(hi_s32 handle, const hi_tde_single_src *single_src, hi_tde_rotate_angle rotate);
hi_s32 hi_tde_bit_blit(hi_s32 handle, const hi_tde_double_src *double_src, const hi_tde_opt *opt);
hi_s32 hi_tde_pattern_fill(hi_s32 handle, const hi_tde_double_src *double_src, const hi_tde_pattern_fill_opt *fill_opt);
hi_s32 hi_tde_mb_blit(hi_s32 handle, const hi_tde_mb_src *mb_src, const hi_tde_mb_opt *opt);
hi_s32 hi_tde_bitmap_mask_rop(hi_s32 handle, const hi_tde_triple_src *triple_src,
                              hi_tde_rop_mode rop_code_color, hi_tde_rop_mode rop_code_alpha);
hi_s32 hi_tde_bitmap_mask_blend(hi_s32 handle, const hi_tde_triple_src *triple_src,
                                hi_u8 alpha, hi_tde_alpha_blending blend_mode);
hi_s32 hi_tde_set_deflicker_level(hi_tde_deflicker_level deflicker_level);
hi_s32 hi_tde_get_deflicker_level(hi_tde_deflicker_level *deflicker_level);
hi_s32 hi_tde_set_alpha_threshold_value(hi_u8 threshold_value);
hi_s32 hi_tde_get_alpha_threshold_value(hi_u8 *threshold_value);
hi_s32 hi_tde_set_alpha_threshold_state(hi_bool threshold_en);
hi_s32 hi_tde_get_alpha_threshold_state(hi_bool *threshold_en);
hi_s32 hi_tde_enable_rgn_deflicker(hi_bool rgn_deflicker_en);

#ifdef __cplusplus
}
#endif
#endif /* __HI_MPI_TDE_H__ */

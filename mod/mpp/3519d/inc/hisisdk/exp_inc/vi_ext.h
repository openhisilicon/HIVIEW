/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef VI_EXT_H
#define VI_EXT_H

#include "mod_ext.h"
#include "ot_common_sys.h"
#include "ot_common_vi.h"
#include "ot_common_isp.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    ot_dynamic_range dynamic_range;
} vi_pipe_hdr_attr;

typedef struct {
    ot_vi_dev   vi_dev;
    td_bool     is_mast_pipe;
    ot_wdr_mode wdr_mode;
    td_u32      pipe_num;
    ot_vi_pipe  pipe_id[OT_VI_MAX_PHYS_PIPE_NUM];
} vi_pipe_wdr_attr;

typedef struct {
    ot_vi_vpss_mode_type vi_vpss_mode;
    ot_vi_aiisp_mode     aiisp_mode;
    td_u32               split_num;
    td_u32               overlap;
    ot_rect              rect[OT_VI_MAX_SPLIT_NODE_NUM];
    ot_rect              wch_out_rect;
    ot_pixel_format      pixel_format;
} vi_pipe_split_attr;

typedef struct {
    td_bool stitch_enable;
    td_bool main_pipe;
    td_u8   stitch_pipe_num;
    td_s8   stitch_bind_id[OT_VI_MAX_PHYS_PIPE_NUM];
    td_u64  max_pts_gap;
} vi_stitch_attr;

typedef struct {
    td_bool dsf_en;
    td_u32  buf_size;
    td_u32  buf_num;
    td_bool is_down_sampled;
    ot_size ds_size;
    td_u32  factor;
    td_bool is_lp_filter;
    td_bool is_bit_truncation;
    td_u32  in_crop_x_start;
    td_u32  in_crop_x_step;
    td_u32  in_crop_y_start;
    td_u32  in_crop_y_step;
    td_u32  out_crop_x_start;
    td_u32  out_crop_x_step;
    td_u32  out_crop_y_start;
    td_u32  out_crop_y_step;
} vi_dsf_cfg;

typedef struct {
    td_bool bas_en;
    ot_size bas_size;
    ot_crop_info pre_crop_info;
} vi_blc_crop_info;

typedef struct {
    ot_vi_out_mode out_mode;
    ot_crop_info satgger_crop_info;
} vi_out_mode_info;

#ifdef CONFIG_OT_ISP_DETAIL_STATS_SUPPORT
typedef struct {
    td_phys_addr_t      stt_phys_addr;
    td_void* ATTRIBUTE  stt_virt_addr;
    td_u32              pool_id;
    td_u64              stt_pts;
} vi_stt_buf_info;
#endif

typedef td_s32  fn_vi_down_semaphore(ot_vi_pipe vi_pipe);
typedef td_void fn_vi_up_semaphore(ot_vi_pipe vi_pipe);
typedef td_bool fn_vi_is_pipe_existed(ot_vi_pipe vi_pipe);
typedef td_s32  fn_vi_update_3dnr_pos(ot_3dnr_pos_type pos);
typedef td_s32  fn_vi_update_vi_vpss_mode(const ot_vi_vpss_mode *vi_vpss_mode);
typedef td_s32  fn_vi_update_vi_aiisp_mode(ot_vi_pipe vi_pipe, ot_vi_aiisp_mode aiisp_mode);
typedef td_s32  fn_vi_get_pipe_wdr_attr(ot_vi_pipe vi_pipe, vi_pipe_wdr_attr *wdr_attr);
typedef td_s32  fn_vi_get_pipe_hdr_attr(ot_vi_pipe vi_pipe, vi_pipe_hdr_attr *hdr_attr);
typedef td_s32  fn_vi_get_split_attr(ot_vi_pipe vi_pipe, vi_pipe_split_attr *split_attr);
typedef td_s32  fn_vi_get_pipe_in_size(ot_vi_pipe vi_pipe, ot_size *dev_size);
typedef td_s32  fn_vi_get_vi_frame_interrupt_attr(ot_vi_pipe vi_pipe, ot_frame_interrupt_attr *frame_int_attr);
typedef td_s32  fn_vi_get_vi_pipe_low_delay_en(ot_vi_pipe vi_pipe, td_bool *pipe_low_delay_en);
typedef td_s32  fn_vi_isp_clear_input_queue(ot_vi_pipe vi_pipe);
typedef td_s32  fn_vi_set_pipe_bnr_en(ot_vi_pipe vi_pipe, td_bool bnr_en);
typedef td_s32  fn_vi_set_pipe_disable_bnr_wmot(ot_vi_pipe vi_pipe);
#ifdef IQ_DEBUG
typedef td_s32  fn_vi_set_pipe_bnr_mot_bitw(ot_vi_pipe vi_pipe, td_u8 mot_bitw);
#endif
typedef td_s32  fn_vi_set_pipe_dsf_cfg(ot_vi_pipe vi_pipe, const vi_dsf_cfg *dsf_cfg);
typedef td_bool fn_vi_is_ia_nr_en(ot_vi_pipe vi_pipe);
typedef td_s32  fn_vi_get_pipe_stitch_attr(ot_vi_pipe vi_pipe, vi_stitch_attr *stitch_attr);
typedef td_u64  fn_vi_get_pipe_hw_timestamp(ot_vi_pipe vi_pipe);
typedef td_s32  fn_vi_set_fpn_correct_en(ot_vi_pipe vi_pipe, td_bool enable);
typedef td_s32  fn_vi_get_blc_crop_info(ot_vi_pipe vi_pipe, vi_blc_crop_info *crop_info);
typedef td_s32  fn_vi_set_pro_frame_flag(ot_vi_pipe vi_pipe);
typedef td_s32  fn_vi_get_out_mode(ot_vi_pipe vi_pipe, vi_out_mode_info *out_mode_info);
typedef td_s32  fn_vi_pipe_get_dev_irq_affinity(ot_vi_pipe vi_pipe, td_bool *int0_mask_en);
typedef td_s32  fn_vi_get_chn_cmp_mode(ot_vi_pipe vi_pipe, ot_vi_chn vi_chn, ot_compress_mode *cmp_mode);
typedef td_s32  fn_vi_get_pipe_3dnr_en(ot_vi_pipe vi_pipe, td_bool *nr_en);
typedef td_s32  fn_vi_is_sync_frc_accept(ot_vi_pipe vi_pipe, td_u64 pts, td_bool *accept);
typedef td_s32  fn_vi_set_sync_frc_en(ot_vi_pipe vi_pipe, td_bool enable);
#ifdef CONFIG_OT_SYS_FMU0_SUPPORT
typedef td_s32  fn_vi_get_chn_fmu_mode(ot_vi_pipe vi_pipe, ot_vi_chn vi_chn, ot_fmu_mode *fmu_mode);
#endif
#ifdef CONFIG_OT_VI_PIPE_RGBW
typedef td_s32  fn_vi_set_pipe_rgbw_attr(ot_vi_pipe vi_pipe, const td_void *rgbw_param);
#endif

#ifdef CONFIG_OT_ISP_DETAIL_STATS_SUPPORT
typedef td_s32  fn_vi_get_pipe_stt_info(ot_vi_pipe vi_pipe, vi_stt_buf_info *stt_info);
typedef td_s32  fn_vi_release_pipe_stt_info(ot_vi_pipe vi_pipe, const vi_stt_buf_info *stt_info);
#endif

typedef struct {
    fn_vi_down_semaphore              *pfn_vi_down_semaphore;
    fn_vi_up_semaphore                *pfn_vi_up_semaphore;
    fn_vi_is_pipe_existed             *pfn_vi_is_pipe_existed;
    fn_vi_update_3dnr_pos              *pfn_vi_update_3dnr_pos;
    fn_vi_update_vi_vpss_mode         *pfn_vi_update_vi_vpss_mode;
    fn_vi_update_vi_aiisp_mode        *pfn_vi_update_vi_aiisp_mode;
    fn_vi_get_pipe_wdr_attr           *pfn_vi_get_pipe_wdr_attr;
    fn_vi_get_pipe_hdr_attr           *pfn_vi_get_pipe_hdr_attr;
    fn_vi_get_split_attr              *pfn_vi_get_split_attr;
    fn_vi_get_pipe_in_size            *pfn_vi_get_pipe_in_size;
    fn_vi_get_vi_frame_interrupt_attr *pfn_vi_get_vi_frame_interrupt_attr;
    fn_vi_get_vi_pipe_low_delay_en    *pfn_vi_get_vi_pipe_low_delay_en;
    fn_vi_isp_clear_input_queue       *pfn_vi_isp_clear_input_queue;
    fn_vi_set_pipe_bnr_en             *pfn_vi_set_pipe_bnr_en;
    fn_vi_set_pipe_disable_bnr_wmot   *pfn_vi_set_pipe_disable_bnr_wmot;
#ifdef IQ_DEBUG
    fn_vi_set_pipe_bnr_mot_bitw       *pfn_vi_set_pipe_bnr_mot_bitw;
#endif
    fn_vi_set_pipe_dsf_cfg            *pfn_vi_set_pipe_dsf_cfg;
    fn_vi_is_ia_nr_en                 *pfn_vi_is_ia_nr_en;
    fn_vi_get_pipe_stitch_attr        *pfn_vi_get_pipe_stitch_attr;
    fn_vi_get_pipe_hw_timestamp       *pfn_vi_get_pipe_hw_timestamp;
    fn_vi_set_fpn_correct_en          *pfn_vi_set_fpn_correct_en;
    fn_vi_get_blc_crop_info           *pfn_vi_get_blc_crop_info;
    fn_vi_set_pro_frame_flag          *pfn_vi_set_pro_frame_flag;
    fn_vi_get_out_mode                *pfn_vi_get_out_mode;
    fn_vi_pipe_get_dev_irq_affinity   *pfn_vi_pipe_get_dev_irq_affinity;
#ifdef CONFIG_OT_SYS_FMU0_SUPPORT
    fn_vi_get_chn_fmu_mode            *pfn_vi_get_chn_fmu_mode;
#endif
    fn_vi_get_chn_cmp_mode            *pfn_vi_get_chn_cmp_mode;
    fn_vi_get_pipe_3dnr_en            *pfn_vi_get_pipe_3dnr_en;
#ifdef CONFIG_OT_VI_PIPE_RGBW
    fn_vi_set_pipe_rgbw_attr          *pfn_vi_set_pipe_rgbw_attr;
#endif
#ifdef CONFIG_OT_ISP_DETAIL_STATS_SUPPORT
    fn_vi_get_pipe_stt_info           *pfn_vi_get_pipe_stt_info;
    fn_vi_release_pipe_stt_info       *pfn_vi_release_pipe_stt_info;
#endif
    fn_vi_is_sync_frc_accept          *pfn_vi_is_sync_frc_accept;
    fn_vi_set_sync_frc_en             *pfn_vi_set_sync_frc_en;
} vi_export_func;

td_void *vi_get_export_funcs(td_void);

#define ckfn_vi_down_semaphore() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_down_semaphore != TD_NULL)
#define call_vi_down_semaphore(vi_pipe) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_down_semaphore(vi_pipe)

#define ckfn_vi_up_semaphore() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_up_semaphore != TD_NULL)
#define call_vi_up_semaphore(vi_pipe) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_up_semaphore(vi_pipe)

#define ckfn_vi_is_pipe_existed() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_is_pipe_existed != TD_NULL)
#define call_vi_is_pipe_existed(vi_pipe) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_is_pipe_existed(vi_pipe)

#define ckfn_vi_update_3dnr_pos() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_update_3dnr_pos != TD_NULL)
#define call_vi_update_3dnr_pos(nr_pos) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_update_3dnr_pos(nr_pos)

#define ckfn_vi_update_vi_vpss_mode() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_update_vi_vpss_mode != TD_NULL)
#define call_vi_update_vi_vpss_mode(vi_vpss_mode) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_update_vi_vpss_mode(vi_vpss_mode)

#define ckfn_vi_update_vi_aiisp_mode() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_update_vi_aiisp_mode != TD_NULL)
#define call_vi_update_vi_aiisp_mode(vi_pipe, aiisp_mode) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_update_vi_aiisp_mode(vi_pipe, aiisp_mode)

#define ckfn_vi_get_split_attr() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_split_attr != TD_NULL)
#define call_vi_get_split_attr(vi_pipe, split_attr) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_split_attr(vi_pipe, split_attr)

#define ckfn_vi_get_pipe_wdr_attr() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_pipe_wdr_attr != TD_NULL)
#define call_vi_get_pipe_wdr_attr(vi_pipe, wdr_attr) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_pipe_wdr_attr(vi_pipe, wdr_attr)

#define ckfn_vi_get_pipe_hdr_attr() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_pipe_hdr_attr != TD_NULL)
#define call_vi_get_pipe_hdr_attr(vi_pipe, hdr_attr) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_pipe_hdr_attr(vi_pipe, hdr_attr)

#define ckfn_vi_get_pipe_in_size() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_pipe_in_size != TD_NULL)
#define call_vi_get_pipe_in_size(vi_pipe, dev_size) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_pipe_in_size(vi_pipe, dev_size)

#define ckfn_vi_get_blc_crop_info() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_blc_crop_info != TD_NULL)
#define call_vi_get_blc_crop_info(vi_pipe, crop_info) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_blc_crop_info(vi_pipe, crop_info)

#define ckfn_vi_get_vi_frame_interrupt_attr() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_vi_frame_interrupt_attr != TD_NULL)
#define call_vi_get_vi_frame_interrupt_attr(vi_pipe, frame_int_attr) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_vi_frame_interrupt_attr(vi_pipe, frame_int_attr)

#define ckfn_vi_get_vi_pipe_low_delay_en() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_vi_pipe_low_delay_en != TD_NULL)
#define call_vi_get_vi_pipe_low_delay_en(vi_pipe, pipe_low_delay_en) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_vi_pipe_low_delay_en(vi_pipe, pipe_low_delay_en)

#define ckfn_vi_isp_clear_input_queue() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_isp_clear_input_queue != TD_NULL)
#define call_vi_isp_clear_input_queue(vi_pipe) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_isp_clear_input_queue(vi_pipe)

#define ckfn_vi_set_pipe_bnr_en() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_set_pipe_bnr_en != TD_NULL)
#define call_vi_set_pipe_bnr_en(vi_pipe, bnr_en) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_set_pipe_bnr_en(vi_pipe, bnr_en)

#ifdef IQ_DEBUG
#define ckfn_vi_set_pipe_bnr_mot_bitw() \
            (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_set_pipe_bnr_mot_bitw != TD_NULL)
#define call_vi_set_pipe_bnr_mot_bitw(vi_pipe, bitw) \
            func_entry(vi_export_func, OT_ID_VI)->pfn_vi_set_pipe_bnr_mot_bitw(vi_pipe, bitw)
#endif

#define ckfn_vi_set_pipe_disable_bnr_wmot() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_set_pipe_disable_bnr_wmot != TD_NULL)
#define call_vi_set_pipe_disable_bnr_wmot(vi_pipe) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_set_pipe_disable_bnr_wmot(vi_pipe)

#define ckfn_vi_set_pipe_dsf_cfg() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_set_pipe_dsf_cfg != TD_NULL)
#define call_vi_set_pipe_dsf_cfg(vi_pipe, dsf_cfg) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_set_pipe_dsf_cfg(vi_pipe, dsf_cfg)

#define ckfn_vi_is_ia_nr_en() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_is_ia_nr_en != TD_NULL)
#define call_vi_is_ia_nr_en(vi_pipe) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_is_ia_nr_en(vi_pipe)

#define ckfn_vi_get_pipe_stitch_attr() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_pipe_stitch_attr != TD_NULL)
#define call_vi_get_pipe_stitch_attr(vi_pipe, stitch_attr) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_pipe_stitch_attr(vi_pipe, stitch_attr)

#define ckfn_vi_get_pipe_hw_timestamp() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_pipe_hw_timestamp != TD_NULL)
#define call_vi_get_pipe_hw_timestamp(vi_pipe) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_pipe_hw_timestamp(vi_pipe)

#define ckfn_vi_set_fpn_correct_en() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_set_fpn_correct_en != TD_NULL)
#define call_vi_set_fpn_correct_en(vi_pipe, enable) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_set_fpn_correct_en(vi_pipe, enable)

#define ckfn_vi_set_pro_frame_flag() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_set_pro_frame_flag != TD_NULL)
#define call_vi_set_pro_frame_flag(vi_pipe) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_set_pro_frame_flag(vi_pipe)

#define ckfn_vi_get_out_mode() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_out_mode != TD_NULL)
#define call_vi_get_out_mode(vi_pipe, out_mode_info) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_out_mode(vi_pipe, out_mode_info)

#define ckfn_vi_pipe_get_dev_irq_affinity() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_pipe_get_dev_irq_affinity != TD_NULL)
#define call_vi_pipe_get_dev_irq_affinity(vi_pipe, int0_mask_en) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_pipe_get_dev_irq_affinity(vi_pipe, int0_mask_en)

#ifdef CONFIG_OT_SYS_FMU0_SUPPORT
#define ckfn_vi_get_chn_fmu_mode() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_chn_fmu_mode != TD_NULL)
#define call_vi_get_chn_fmu_mode(vi_pipe, vi_chn, fmu_mode) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_chn_fmu_mode(vi_pipe, vi_chn, fmu_mode)
#endif

#define ckfn_vi_get_chn_cmp_mode() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_chn_cmp_mode != TD_NULL)
#define call_vi_get_chn_cmp_mode(vi_pipe, vi_chn, cmp_mode) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_chn_cmp_mode(vi_pipe, vi_chn, cmp_mode)

#define ckfn_vi_get_pipe_3dnr_en() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_pipe_3dnr_en != TD_NULL)
#define call_vi_get_pipe_3dnr_en(vi_pipe, nr_en) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_pipe_3dnr_en(vi_pipe, nr_en)

#ifdef CONFIG_OT_VI_PIPE_RGBW
#define ckfn_vi_set_pipe_rgbw_attr() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_set_pipe_rgbw_attr != TD_NULL)
#define call_vi_set_pipe_rgbw_attr(vi_pipe, rgbw_attr) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_set_pipe_rgbw_attr(vi_pipe, rgbw_attr)
#endif

#ifdef CONFIG_OT_ISP_DETAIL_STATS_SUPPORT
#define ckfn_vi_get_pipe_stt_info() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_pipe_stt_info != TD_NULL)
#define call_vi_get_pipe_stt_info(vi_pipe, stt_info) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_get_pipe_stt_info(vi_pipe, stt_info)
#define ckfn_vi_release_pipe_stt_info() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_release_pipe_stt_info != TD_NULL)
#define call_vi_release_pipe_stt_info(vi_pipe, stt_info) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_release_pipe_stt_info(vi_pipe, stt_info)
#endif

#define ckfn_vi_is_sync_frc_accept() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_is_sync_frc_accept != TD_NULL)
#define call_vi_is_sync_frc_accept(vi_pipe, pts, discard) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_is_sync_frc_accept(vi_pipe, pts, discard)

#define ckfn_vi_set_sync_frc_en() \
    (func_entry(vi_export_func, OT_ID_VI)->pfn_vi_set_sync_frc_en != TD_NULL)
#define call_vi_set_sync_frc_en(vi_pipe, enable) \
    func_entry(vi_export_func, OT_ID_VI)->pfn_vi_set_sync_frc_en(vi_pipe, enable)

/* mpi interface for other module */
typedef enum {
    FPN_MODE_NONE = 0x0,
    FPN_MODE_CORRECTION,
    FPN_MODE_CALIBRATE,
    FPN_MODE_BUTT
} vi_fpn_work_mode;

typedef struct {
    vi_fpn_work_mode fpn_work_mode;
    union {
        ot_isp_fpn_calibrate_attr calibrate_attr;
        ot_isp_fpn_attr correction_attr;
    };
} vi_fpn_attr;

td_s32 mpi_vi_set_fpn_attr(ot_vi_pipe vi_pipe, vi_fpn_attr *fpn_attr);
td_s32 mpi_vi_get_fpn_attr(ot_vi_pipe vi_pipe, vi_fpn_attr *fpn_attr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* VI_EXT_H */

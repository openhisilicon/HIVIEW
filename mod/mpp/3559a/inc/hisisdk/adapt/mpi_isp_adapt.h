/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author: Hisilicon multimedia software group
* Create: 2011/06/28
*/


#ifndef __MPI_ISP_ADAPT_H__
#define __MPI_ISP_ADAPT_H__

#include "hi_type.h"
#include "hi_common_adapt.h"
#include "hi_comm_isp_adapt.h"
#include "hi_comm_3a_adapt.h"
#include "hi_debug_adapt.h"
#include "hi_comm_sns_adapt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

MPI_STATIC hi_s32 hi_mpi_isp_init(VI_PIPE vi_pipe);
MPI_STATIC hi_s32 hi_mpi_isp_mem_init(VI_PIPE vi_pipe);
MPI_STATIC hi_s32 hi_mpi_isp_run(VI_PIPE vi_pipe);
MPI_STATIC hi_s32 hi_mpi_isp_run_once(VI_PIPE vi_pipe);
MPI_STATIC hi_s32 hi_mpi_isp_exit(VI_PIPE vi_pipe);

MPI_STATIC hi_s32 hi_mpi_isp_sensor_reg_callback(VI_PIPE vi_pipe, hi_isp_sns_attr_info *sns_attr_info, hi_isp_sensor_register *sns_register);
MPI_STATIC hi_s32 hi_mpi_isp_sensor_unreg_callback(VI_PIPE vi_pipe, SENSOR_ID sensor_id);

/* if have registered multy libs, set bind attr to appoint the active lib. */
MPI_STATIC hi_s32 hi_mpi_isp_set_bind_attr(VI_PIPE vi_pipe, const hi_isp_bind_attr *bind_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_bind_attr(VI_PIPE vi_pipe, hi_isp_bind_attr *bind_attr);
MPI_STATIC hi_s32 hi_mpi_isp_ae_lib_reg_callback(VI_PIPE vi_pipe, hi_isp_alg_lib *ae_lib,
                                   hi_isp_ae_register *ae_register);
MPI_STATIC hi_s32 hi_mpi_isp_awb_lib_reg_callback(VI_PIPE vi_pipe, hi_isp_alg_lib *awb_lib,
                                    hi_isp_awb_register *awb_register);
MPI_STATIC hi_s32 hi_mpi_isp_ae_lib_unreg_callback(VI_PIPE vi_pipe, hi_isp_alg_lib *ae_lib);
MPI_STATIC hi_s32 hi_mpi_isp_awb_lib_unreg_callback(VI_PIPE vi_pipe, hi_isp_alg_lib *awb_lib);

MPI_STATIC hi_s32 hi_mpi_isp_get_lightbox_gain(VI_PIPE vi_pipe, hi_isp_awb_calibration_gain *awb_calibration_gain);

MPI_STATIC hi_s32 hi_mpi_isp_set_pub_attr(VI_PIPE vi_pipe, const hi_isp_pub_attr *pub_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_pub_attr(VI_PIPE vi_pipe, hi_isp_pub_attr *pub_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_pipe_differ_attr(VI_PIPE vi_pipe, const hi_isp_pipe_diff_attr *pipe_differ);
MPI_STATIC hi_s32 hi_mpi_isp_get_pipe_differ_attr(VI_PIPE vi_pipe, hi_isp_pipe_diff_attr *pipe_differ);

MPI_STATIC hi_s32 hi_mpi_isp_get_vd_time_out(VI_PIPE vi_pipe, hi_isp_vd_type isp_vd_type, hi_u32 milli_sec);
MPI_STATIC hi_s32 hi_mpi_isp_query_inner_state_info(VI_PIPE vi_pipe, hi_isp_inner_state_info *inner_state_info);
MPI_STATIC hi_s32 hi_mpi_isp_get_ae_statistics(VI_PIPE vi_pipe, hi_isp_ae_statistics *ae_stat);
MPI_STATIC hi_s32 hi_mpi_isp_get_ae_stitch_statistics(VI_PIPE vi_pipe, hi_isp_ae_stitch_statistics *stitch_stat);
MPI_STATIC hi_s32 hi_mpi_isp_get_mg_statistics(VI_PIPE vi_pipe, hi_isp_mg_statistics *mg_stat);

MPI_STATIC hi_s32 hi_mpi_isp_get_wb_statistics(VI_PIPE vi_pipe, hi_isp_wb_statistics *wb_stat);
MPI_STATIC hi_s32 hi_mpi_isp_get_wb_stitch_statistics(VI_PIPE vi_pipe, hi_isp_wb_stitch_statistics *stitch_wb_stat);

MPI_STATIC hi_s32 hi_mpi_isp_get_focus_statistics(VI_PIPE vi_pipe, hi_isp_af_statistics *af_stat);

MPI_STATIC hi_s32 hi_mpi_isp_set_statistics_config(VI_PIPE vi_pipe, const hi_isp_statistics_cfg *stat_cfg);
MPI_STATIC hi_s32 hi_mpi_isp_get_statistics_config(VI_PIPE vi_pipe, hi_isp_statistics_cfg *stat_cfg);

MPI_STATIC hi_s32 hi_mpi_isp_get_isp_reg_attr(VI_PIPE vi_pipe, hi_isp_reg_attr *isp_reg_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_fmw_state(VI_PIPE vi_pipe, const hi_isp_fmw_state state);
MPI_STATIC hi_s32 hi_mpi_isp_get_fmw_state(VI_PIPE vi_pipe, hi_isp_fmw_state *state);

MPI_STATIC hi_s32 hi_mpi_isp_set_debug(VI_PIPE vi_pipe, const hi_isp_debug_info *isp_debug);
MPI_STATIC hi_s32 hi_mpi_isp_get_debug(VI_PIPE vi_pipe, hi_isp_debug_info *isp_debug);

MPI_STATIC hi_s32 hi_mpi_isp_set_mod_param(const hi_isp_mod_param *mod_param);
MPI_STATIC hi_s32 hi_mpi_isp_get_mod_param(hi_isp_mod_param *mod_param);
MPI_STATIC hi_s32 hi_mpi_isp_set_ctrl_param(VI_PIPE vi_pipe, const hi_isp_ctrl_param *isp_ctrl_param);
MPI_STATIC hi_s32 hi_mpi_isp_get_ctrl_param(VI_PIPE vi_pipe, hi_isp_ctrl_param *isp_ctrl_param);

MPI_STATIC hi_s32 hi_mpi_isp_set_fswdr_attr(VI_PIPE vi_pipe, const hi_isp_wdr_fs_attr *fswdr_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_fswdr_attr(VI_PIPE vi_pipe, hi_isp_wdr_fs_attr *fswdr_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_drc_attr(VI_PIPE vi_pipe, const hi_isp_drc_attr *drc);
MPI_STATIC hi_s32 hi_mpi_isp_get_drc_attr(VI_PIPE vi_pipe, hi_isp_drc_attr *drc);

MPI_STATIC hi_s32 hi_mpi_isp_set_dehaze_attr(VI_PIPE vi_pipe, const hi_isp_dehaze_attr *dehaze_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_dehaze_attr(VI_PIPE vi_pipe, hi_isp_dehaze_attr *dehaze_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_ldci_attr(VI_PIPE vi_pipe, const hi_isp_ldci_attr *ldci_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_ldci_attr(VI_PIPE vi_pipe, hi_isp_ldci_attr *ldci_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_sns_slave_attr(SLAVE_DEV slavedev, const hi_isp_slave_sns_sync *sns_sync);
MPI_STATIC hi_s32 hi_mpi_isp_get_sns_slave_attr(SLAVE_DEV slavedev, hi_isp_slave_sns_sync *sns_sync);

MPI_STATIC hi_s32 hi_mpi_isp_set_module_control(VI_PIPE vi_pipe, const hi_isp_module_ctrl *mod_ctrl);
MPI_STATIC hi_s32 hi_mpi_isp_get_module_control(VI_PIPE vi_pipe, hi_isp_module_ctrl *mod_ctrl);

MPI_STATIC hi_s32 hi_mpi_isp_set_dp_calibrate(VI_PIPE vi_pipe, const hi_isp_dp_static_calibrate *dp_calibrate);
MPI_STATIC hi_s32 hi_mpi_isp_get_dp_calibrate(VI_PIPE vi_pipe, hi_isp_dp_static_calibrate *dp_calibrate);

MPI_STATIC hi_s32 hi_mpi_isp_set_dp_static_attr(VI_PIPE vi_pipe, const hi_isp_dp_static_attr *dp_static_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_dp_static_attr(VI_PIPE vi_pipe,  hi_isp_dp_static_attr *dp_static_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_dp_dynamic_attr(VI_PIPE vi_pipe, const hi_isp_dp_dynamic_attr *dp_dynamic_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_dp_dynamic_attr(VI_PIPE vi_pipe, hi_isp_dp_dynamic_attr *dp_dynamic_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_dis_attr(VI_PIPE vi_pipe, const hi_isp_dis_attr *dis_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_dis_attr(VI_PIPE vi_pipe, hi_isp_dis_attr *dis_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_mesh_shading_attr(VI_PIPE vi_pipe, const hi_isp_shading_attr *shading_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_mesh_shading_attr(VI_PIPE vi_pipe, hi_isp_shading_attr *shading_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_mesh_shading_gain_lut_attr(VI_PIPE vi_pipe, const hi_isp_shading_lut_attr *shading_lut_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_mesh_shading_gain_lut_attr(VI_PIPE vi_pipe, hi_isp_shading_lut_attr *shading_lut_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_radial_shading_attr(VI_PIPE vi_pipe, const hi_isp_radial_shading_attr *radial_shading_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_radial_shading_attr(VI_PIPE vi_pipe, hi_isp_radial_shading_attr *radial_shading_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_radial_shading_lut(VI_PIPE vi_pipe, const hi_isp_radial_shading_lut_attr *radial_shading_lut_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_radial_shading_lut(VI_PIPE vi_pipe, hi_isp_radial_shading_lut_attr *radial_shading_lut_attr);

MPI_STATIC hi_s32 hi_mpi_isp_mesh_shading_calibration(VI_PIPE vi_pipe, hi_u16 *src_raw, hi_isp_mlsc_calibration_cfg *mlsc_cali_cfg, hi_isp_mesh_shading_table *mlsc_table);

MPI_STATIC hi_s32 hi_mpi_isp_set_nr_attr(VI_PIPE vi_pipe, const hi_isp_nr_attr *nr_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_nr_attr(VI_PIPE vi_pipe, hi_isp_nr_attr *nr_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_de_attr(VI_PIPE vi_pipe, const hi_isp_de_attr *de_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_de_attr(VI_PIPE vi_pipe, hi_isp_de_attr *de_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_color_tone_attr(VI_PIPE vi_pipe, const hi_isp_color_tone_attr *ct_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_color_tone_attr(VI_PIPE vi_pipe, hi_isp_color_tone_attr *ct_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_gamma_attr(VI_PIPE vi_pipe, const hi_isp_gamma_attr *gamma_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_gamma_attr(VI_PIPE vi_pipe, hi_isp_gamma_attr *gamma_attr);
MPI_STATIC hi_s32 hi_mpi_isp_set_pre_gamma_attr(VI_PIPE vi_pipe, const hi_isp_pregamma_attr *pre_gamma_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_pre_gamma_attr(VI_PIPE vi_pipe, hi_isp_pregamma_attr *pre_gamma_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_pre_log_lut_attr(VI_PIPE vi_pipe, const hi_isp_preloglut_attr *pre_log_lut_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_pre_log_lut_attr(VI_PIPE vi_pipe, hi_isp_preloglut_attr *pre_log_lut_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_log_lut_attr(VI_PIPE vi_pipe, const hi_isp_loglut_attr *log_lut_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_log_lut_attr(VI_PIPE vi_pipe, hi_isp_loglut_attr *log_lut_attr);
MPI_STATIC hi_s32 hi_mpi_isp_set_local_cac_attr(VI_PIPE vi_pipe, const hi_isp_local_cac_attr *local_cac_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_local_cac_attr(VI_PIPE vi_pipe, hi_isp_local_cac_attr *local_cac_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_global_cac_attr(VI_PIPE vi_pipe, const hi_isp_global_cac_attr *global_cac_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_global_cac_attr(VI_PIPE vi_pipe, hi_isp_global_cac_attr *global_cac_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_rc_attr(VI_PIPE vi_pipe, const hi_isp_rc_attr *rc_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_rc_attr(VI_PIPE vi_pipe, hi_isp_rc_attr *rc_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_sharpen_attr(VI_PIPE vi_pipe, const hi_isp_sharpen_attr *isp_shp_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_sharpen_attr(VI_PIPE vi_pipe, hi_isp_sharpen_attr *isp_shp_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_edge_mark_attr(VI_PIPE vi_pipe, const hi_isp_edge_mark_attr *isp_edge_mark_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_edge_mark_attr(VI_PIPE vi_pipe, hi_isp_edge_mark_attr *isp_edge_mark_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_hlc_attr(VI_PIPE vi_pipe, const hi_isp_hlc_attr *isp_hlc_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_hlc_attr(VI_PIPE vi_pipe, hi_isp_hlc_attr *isp_hlc_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_crosstalk_attr(VI_PIPE vi_pipe, const hi_isp_cr_attr *cr_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_crosstalk_attr(VI_PIPE vi_pipe, hi_isp_cr_attr *cr_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_ca_attr(VI_PIPE vi_pipe, const hi_isp_ca_attr *ca_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_ca_attr(VI_PIPE vi_pipe, hi_isp_ca_attr *ca_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_anti_false_color_attr(VI_PIPE vi_pipe, const hi_isp_antifalsecolor_attr *anti_false_color);
MPI_STATIC hi_s32 hi_mpi_isp_get_anti_false_color_attr(VI_PIPE vi_pipe, hi_isp_antifalsecolor_attr *anti_false_color);

MPI_STATIC hi_s32 hi_mpi_isp_set_demosaic_attr(VI_PIPE vi_pipe, const hi_isp_demosaic_attr *demosaic_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_demosaic_attr(VI_PIPE vi_pipe, hi_isp_demosaic_attr *demosaic_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_black_level_attr(VI_PIPE vi_pipe, const hi_isp_black_level *black_level);
MPI_STATIC hi_s32 hi_mpi_isp_get_black_level_attr(VI_PIPE vi_pipe, hi_isp_black_level *black_level);

MPI_STATIC hi_s32 hi_mpi_isp_set_clut_attr(VI_PIPE vi_pipe, const hi_isp_clut_attr *clut_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_clut_attr(VI_PIPE vi_pipe, hi_isp_clut_attr *clut_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_clut_coeff(VI_PIPE vi_pipe, const hi_isp_clut_lut *clut_lut);
MPI_STATIC hi_s32 hi_mpi_isp_get_clut_coeff(VI_PIPE vi_pipe, hi_isp_clut_lut *clut_lut);

MPI_STATIC hi_s32 hi_mpi_isp_set_dcf_info(VI_PIPE vi_pipe, const hi_isp_dcf_info *isp_dcf);
MPI_STATIC hi_s32 hi_mpi_isp_get_dcf_info(VI_PIPE vi_pipe, hi_isp_dcf_info *isp_dcf);

MPI_STATIC hi_s32 hi_mpi_isp_set_csc_attr(VI_PIPE vi_pipe, const hi_isp_csc_attr *csc_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_csc_attr(VI_PIPE vi_pipe, hi_isp_csc_attr *cscf_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_rgbir_attr(VI_PIPE vi_pipe, const hi_isp_rgbir_attr *rgbir_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_rgbir_attr(VI_PIPE vi_pipe, hi_isp_rgbir_attr *rgbir_attr);

MPI_STATIC hi_s32 hi_mpi_isp_set_register(VI_PIPE vi_pipe, hi_u32 addr, hi_u32 value);
MPI_STATIC hi_s32 hi_mpi_isp_get_register(VI_PIPE vi_pipe, hi_u32 addr, hi_u32 *value);

/* block, return fpn frame when this is function is called. */
MPI_STATIC hi_s32 hi_mpi_isp_fpn_calibrate(VI_PIPE vi_pipe, hi_isp_fpn_calibrate_attr *calibrate_attr);
MPI_STATIC hi_s32 hi_mpi_isp_set_fpn_attr(VI_PIPE vi_pipe, const hi_isp_fpn_attr *fpn_attr);
MPI_STATIC hi_s32 hi_mpi_isp_get_fpn_attr(VI_PIPE vi_pipe, hi_isp_fpn_attr *fpn_attr);

MPI_STATIC hi_s32 hi_mpi_isp_get_dng_image_static_info(VI_PIPE vi_pipe, hi_isp_dng_image_static_info *dng_image_static_info);
MPI_STATIC hi_s32 hi_mpi_isp_set_dng_color_param(VI_PIPE vi_pipe, const hi_isp_dng_color_param *dng_color_param);
MPI_STATIC hi_s32 hi_mpi_isp_get_dng_color_param(VI_PIPE vi_pipe, hi_isp_dng_color_param *dng_color_param);
#if 0
MPI_STATIC hi_s32 hi_mpi_isp_ir_auto_run_once(VI_PIPE vi_pipe, hi_isp_ir_auto_attr *ir_attr);
#endif
MPI_STATIC hi_s32 hi_mpi_isp_set_smart_info(VI_PIPE vi_pipe, const hi_isp_smart_info *smart_info);
MPI_STATIC hi_s32 hi_mpi_isp_get_smart_info(VI_PIPE vi_pipe, hi_isp_smart_info *smart_info);

MPI_STATIC hi_s32 hi_mpi_isp_set_frame_info(VI_PIPE vi_pipe, const hi_isp_frame_info *pstIspFrame);
MPI_STATIC hi_s32 hi_mpi_isp_get_frame_info(VI_PIPE vi_pipe, const hi_isp_frame_info *pstIspFrame);
MPI_STATIC hi_s32 hi_mpi_isp_calc_flicker_type(VI_PIPE vi_pipe, hi_isp_calc_flicker_input *input_param,
                                               hi_isp_calc_flicker_output *output_param,
                                               hi_video_frame_info frame[], hi_u32 array_size);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
#endif /* __MPI_ISP_ADAPT_H__ */

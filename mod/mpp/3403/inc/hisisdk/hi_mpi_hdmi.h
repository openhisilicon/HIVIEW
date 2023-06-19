/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_HDMI_H__
#define __HI_MPI_HDMI_H__

#include "hi_common_hdmi.h"


#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_hdmi_init(hi_void);

hi_s32 hi_mpi_hdmi_deinit(hi_void);

hi_s32 hi_mpi_hdmi_open(hi_hdmi_id hdmi);

hi_s32 hi_mpi_hdmi_close(hi_hdmi_id hdmi);

hi_s32 hi_mpi_hdmi_get_sink_capability(hi_hdmi_id hdmi, hi_hdmi_sink_capability *capability);

hi_s32 hi_mpi_hdmi_set_attr(hi_hdmi_id hdmi, const hi_hdmi_attr *attr);

hi_s32 hi_mpi_hdmi_get_attr(hi_hdmi_id hdmi, hi_hdmi_attr *attr);

hi_s32 hi_mpi_hdmi_start(hi_hdmi_id hdmi);

hi_s32 hi_mpi_hdmi_stop(hi_hdmi_id hdmi);

hi_s32 hi_mpi_hdmi_force_get_edid(hi_hdmi_id hdmi, hi_hdmi_edid *edid_data);

hi_s32 hi_mpi_hdmi_set_infoframe(hi_hdmi_id hdmi, const hi_hdmi_infoframe *infoframe);

hi_s32 hi_mpi_hdmi_get_infoframe(hi_hdmi_id hdmi, hi_hdmi_infoframe_type infoframe_type,
    hi_hdmi_infoframe *infoframe);

hi_s32 hi_mpi_hdmi_register_callback(hi_hdmi_id hdmi, const hi_hdmi_callback_func *callback_func);

hi_s32 hi_mpi_hdmi_unregister_callback(hi_hdmi_id hdmi, const hi_hdmi_callback_func *callback_func);

hi_s32 hi_mpi_hdmi_set_hw_spec(hi_hdmi_id hdmi, const hi_hdmi_hw_spec *hw_spec);

hi_s32 hi_mpi_hdmi_get_hw_spec(hi_hdmi_id hdmi, hi_hdmi_hw_spec *hw_spec);

hi_s32 hi_mpi_hdmi_set_avmute(hi_hdmi_id hdmi, hi_bool avmute_en);

hi_s32 hi_mpi_hdmi_set_mod_param(hi_hdmi_id hdmi, const hi_hdmi_mod_param *mod_param);

hi_s32 hi_mpi_hdmi_get_mod_param(hi_hdmi_id hdmi, hi_hdmi_mod_param *mod_param);

#ifdef __cplusplus
}
#endif
#endif /* __HI_MPI_HDMI_H__ */

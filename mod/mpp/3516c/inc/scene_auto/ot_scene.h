/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_SCENE_H
#define HI_SCENE_H

#include "ot_scene_setparam.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* * \addtogroup     SCENE */
/* * @{ */ /* * <!-- [SCENE] */

/* macro define */
#define HI_SCENE_PIPE_MAX_NUM HI_VI_MAX_PIPE_NUM            /* *< pipe max count */
#define HI_SCENE_LONGEXP_AEROUTE_MAX_NUM 4 /* *< LongExp AeRoute Number */
#define HI_SCENE_HDRFRAME_MAX_COUNT 3      /* *< HDR max frame count */

/* error code define */
#define HI_SCENE_EINVAL HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_SCENE, 1)       /* Invalid argument */
#define HI_SCENE_ENOTINIT HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_SCENE, 2)     /* Not inited */
#define HI_SCENE_ENONPTR HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_SCENE, 3)      /* null pointer error */
#define HI_SCENE_EOUTOFRANGE HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_SCENE, 4)  /* out of range */
#define HI_SCENE_EINTER HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_SCENE, 5)       /* Internal error */
#define HI_SCENE_EINITIALIZED HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_SCENE, 6) /* Already initialized */

typedef hi_s32 (*ot_scene_set_static_param)(hi_vi_pipe vi_pipe, hi_u8 index);

/* pipe type */
typedef enum {
    HI_SCENE_PIPE_TYPE_SNAP = 0,
    HI_SCENE_PIPE_TYPE_VIDEO,
    HI_SCENE_PIPE_TYPE_BUTT
} ot_scene_pipe_type;

/* pipe attr */
typedef struct {
    hi_bool enable;
    hi_vi_pipe main_pipe_hdl;
    hi_vi_pipe vcap_pipe_hdl;
    hi_vi_chn pipe_chn_hdl;
    hi_venc_chn venc_hdl;
    hi_vpss_grp vpss_hdl;  /* vpss group hdl */
    hi_vpss_chn vport_hdl; /* vpss chn hdl */
    hi_u8 pipe_param_index;
    ot_scene_pipe_type pipe_type;
} ot_scene_pipe_attr;

typedef enum {
    HI_SCENE_PIPE_MODE_LINEAR = 0,
    HI_SCENE_PIPE_MODE_WDR,
    HI_SCENE_PIPE_MODE_HDR, /* HDR means , WDR existed at same time */
    HI_SCENE_PIPE_MODE_BUTT
} ot_scene_pipe_mode;

/* scene mode, an encapsulation of pipe attr array */
typedef struct {
    ot_scene_pipe_mode pipe_mode;
    ot_scene_pipe_attr pipe_attr[HI_SCENE_PIPE_MAX_NUM]; /* Pipe Attr */
} ot_scene_mode;

typedef struct {
    ot_scene_pipe_param pipe_param[HI_SCENE_PIPETYPE_NUM];
} ot_scene_param;

/* *
 * @brief     Init SCENE module.
 * @return    0 success, non-zero error code.
 * @exception None
 * @author    MobileCam Reference Develop Team
 * @date      2017/12/11
 */
hi_s32 ot_scene_init(const ot_scene_param *scene_param);
/* *
 * @brief     Deinit SCENE module.
 * @return    0 success, non-zero error code.
 * @exception None
 * @author    MobileCam Reference Develop Team
 * @date      2017/12/11
 */
hi_s32 ot_scene_deinit(hi_void);

/* *
 * @brief     SetScenemode, start sceneauto thread.
 * @param[in] scene_mode : pipeCfg
 * @return    0 success, non-zero error code.
 * @exception None
 * @author    MobileCam Reference Develop Team
 * @date      2017/12/11
 */
hi_s32 ot_scene_set_scene_mode(const ot_scene_mode *scene_mode);

/* *
 * @brief GetSceneMode.
 * @param[out]  scene_mode : pipeCfg
 * @return   0 success, non-zero error code.
 * @exception    None
 * @author    MobileCam Reference Develop Team
 * @date      2017/12/11
 */
hi_s32 ot_scene_get_scene_mode(ot_scene_mode *scene_mode);

/* *
 * @brief     Set fps control.
 * @param[in] scene_fps : scenefps config
 * @return    0 success, non-zero error code.
 * @exception None
 * @author    MobileCam Reference Develop Team
 * @date      2017/12/11
 */
hi_s32 ot_scene_set_scene_fps(const ot_scene_fps *scene_fps);

/* *
 * @brief Get fps control.
 * @param[out]  scene_fps : scenefps config
 * @return   0 success, non-zero error code.
 * @exception    None
 * @author    MobileCam Reference Develop Team
 * @date      2017/12/11
 */
hi_s32 ot_scene_get_scene_fps(ot_scene_fps *scene_fps);

/* *
 * @brief     Set init iso and exposure.
 * @param[in] iso : iso
 * @param[in] exp : exposure
 * @return    0 success, non-zero error code.
 * @exception None
 * @author    MobileCam Reference Develop Team
 * @date      2017/12/11
 */
hi_s32 ot_scene_set_scene_init_exp(hi_u32 iso, hi_u64 exp);

/* *
 * @brief Pause sceneauto when isp stopped(in playback state or when reset mediaRoute)
 * Note: when Pause is true, the sceneauto thread can's set param executed
 * @param[in]  enable: pause enable
 * @return   0 success, non-zero error code.
 * @exception    None
 * @author    MobileCam Reference Develop Team
 * @date      2017/12/11
 */
hi_s32 ot_scene_pause(hi_bool enable);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* End of #ifndef HI_SCENE_H */

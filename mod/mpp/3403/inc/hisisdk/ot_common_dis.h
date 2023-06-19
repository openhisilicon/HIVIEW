/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef OT_COMMON_DIS_H
#define OT_COMMON_DIS_H

#include "ot_common_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define OT_ERR_DIS_NO_BUF            OT_DEFINE_ERR(OT_ID_DIS, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
#define OT_ERR_DIS_BUF_EMPTY         OT_DEFINE_ERR(OT_ID_DIS, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)
#define OT_ERR_DIS_NULL_PTR          OT_DEFINE_ERR(OT_ID_DIS, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
#define OT_ERR_DIS_ILLEGAL_PARAM     OT_DEFINE_ERR(OT_ID_DIS, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
#define OT_ERR_DIS_BUF_FULL          OT_DEFINE_ERR(OT_ID_DIS, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_FULL)
#define OT_ERR_DIS_SYS_NOT_READY     OT_DEFINE_ERR(OT_ID_DIS, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
#define OT_ERR_DIS_NOT_SUPPORT       OT_DEFINE_ERR(OT_ID_DIS, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
#define OT_ERR_DIS_NOT_PERM          OT_DEFINE_ERR(OT_ID_DIS, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
#define OT_ERR_DIS_BUSY              OT_DEFINE_ERR(OT_ID_DIS, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
#define OT_ERR_DIS_INVALID_CHN_ID    OT_DEFINE_ERR(OT_ID_DIS, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
#define OT_ERR_DIS_CHN_UNEXIST       OT_DEFINE_ERR(OT_ID_DIS, OT_ERR_LEVEL_ERROR, OT_ERR_UNEXIST)

/* Different mode of DIS */
typedef enum {
    OT_DIS_MODE_4_DOF_GME = 0,       /* Only use with GME in 4 dof. */
    OT_DIS_MODE_6_DOF_GME,           /* Only use with GME in 6 dof. */
    OT_DIS_MODE_GYRO,                /* Only use with gryo in 6 dof. */
    OT_DIS_MODE_BUTT,
} ot_dis_mode;

/* The motion level of camera */
typedef enum {
    OT_DIS_MOTION_LEVEL_LOW = 0,   /* Low motion level. */
    OT_DIS_MOTION_LEVEL_NORM,      /* Normal motion level. */
    OT_DIS_MOTION_LEVEL_HIGH,      /* High motion level. */
    OT_DIS_MOTION_LEVEL_BUTT
} ot_dis_motion_level;

/* Different product type used DIS */
typedef enum {
    OT_DIS_PDT_TYPE_IPC = 0,   /* IPC product type. */
    OT_DIS_PDT_TYPE_DV,        /* DV product type. */
    OT_DIS_PDT_TYPE_DRONE,     /* DRONE product type. */
    OT_DIS_PDT_TYPE_BUTT
} ot_dis_pdt_type;

/* The parameter of DIS */
typedef struct {
    /*
     * RW; [0,100],
     * 0: attenuate large motion most in advance,
     * 100: never attenuate large motion;
     * larger value -> better stability but more likely to crop to the border with large motion.
     */
    td_u32 large_motion_stable_coef;
    /*
     * RW; [0,100],
     * 0: never preserve the low frequency motion,
     * 100: keep all the low frequency motion;
     * small value -> better stability but more likely to crop to the border even with low level motion.
     */
    td_u32 low_freq_motion_preserve;
    /*
     * RW; [0,100],
     * 0: lowest cut frequency,
     * 100: highest cut frequency;
     * small value -> better stability but more likely to crop to the border even with large motion.
     */
    td_u32 low_freq_motion_freq;
} ot_dis_param;

/* The attribute of DIS */
typedef struct {
    td_bool enable;                  /* RW; DIS enable. */
    td_bool gdc_bypass;              /* RW; gdc correction process , DIS = GME&GDC correction. */
    td_u32  moving_subject_level;    /* RW; Range:[0,6]; Moving Subject level. */
    td_s32  rolling_shutter_coef;    /* RW; Range:[0,1000]; Rolling shutter coefficients. */
    td_s32  timelag;                 /* RW; Range:[-2000000,2000000]; Timestamp delay between Gyro and Frame PTS. */
    td_u32  hor_limit;               /* RW; Range:[0,1000]; Parameter to limit horizontal drift by large foreground. */
    td_u32  ver_limit;               /* RW; Range:[0,1000]; Parameter to limit vertical drift by large foreground. */
    td_bool still_crop;              /* RW; The stabilization not working,but the output image still be cropped. */
    td_u32  strength;                /* RW; The DIS strength for different light. */
} ot_dis_attr;

/* The config of DIS */
typedef struct {
    ot_dis_mode         mode;                  /* RW; DIS Mode. */
    ot_dis_motion_level motion_level;          /* RW; DIS Motion level of the camera. */
    ot_dis_pdt_type     pdt_type;              /* RW; DIS product type. */
    td_u32              buf_num;               /* RW; Range:[5,10]; Buf num for DIS. */
    td_u32              crop_ratio;            /* RW; Range:[50,98]; Crop ratio of output image. */

    td_u32              frame_rate;            /* RW; Frame rate. */
    td_bool             camera_steady;         /* RW; The camera is steady or not. */
    td_bool             scale;                 /* RW; Scale output image or not. */
} ot_dis_cfg;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* OT_COMMON_DIS_H */
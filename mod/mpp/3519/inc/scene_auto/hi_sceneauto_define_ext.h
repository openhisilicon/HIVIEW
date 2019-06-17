/******************************************************************************

  Copyright (C), 2013-2023, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_srdk_sceneauto_define_ext.h
  Version       : Initial Draft
  Author        : Hisilicon BVR REF
  Created       : 2014/07/10
  History       :
  1.Date        : 2014/07/10
  Author        : noodle
  Modification: Created file

******************************************************************************/

#ifndef __HI_SRDK_SCENEAUTO_DEFINE_EXT_H__
#define __HI_SRDK_SCENEAUTO_DEFINE_EXT_H__

typedef enum hiSCENEAUTO_SEPCIAL_SCENE_E
{
    SCENEAUTO_SPECIAL_SCENE_NONE = 0,
    SCENEAUTO_SPECIAL_SCENE_IR,
    SCENEAUTO_SPECIAL_SCENE_HLC,
    SCENEAUTO_SPECIAL_SCENE_AUTO_FSWDR,
    SCENEAUTO_SPECIAL_SCENE_MANUAL_LONG_FRAME,
    SCENEAUTO_SPECIAL_SCENE_MANUAL_NORMAL_WDR,
    SCENEAUTO_SPECIAL_SCENE_TRAFFIC,
    SCENEAUTO_SPECIAL_SCENE_FACE,

    SCENEAUTO_SPECIAL_SCENE_BUTT
} SCENEAUTO_SEPCIAL_SCENE_E;

typedef enum hiSCENEAUTO_PRODUCT_TYPE_E
{
	SCENEAUTO_PRODUCT_TYPE_IPC = 0,
	SCENEAUTO_PRODUCT_TYPE_DV,

	SCENEAUTO_PRODUCT_TYPE_BUTT
}SCENEAUTO_PRODUCT_TYPE_E;

typedef enum hiSCENEAUTO_FSWDR_MODE_E
{
    /* Normal FSWDR mode */
    SCENEAUTO_FSWDR_NORMAL_MODE = 0x0,

    /* Long frame mode, only effective in LINE_WDR, when running in this mode FSWDR module only output the long frame data */
    SCENEAUTO_FSWDR_LONG_FRAME_MODE = 0x1,

    SCENEAUTO_FSWDR_MODE_BUTT
}SCENEAUTO_FSWDR_MODE_E;

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

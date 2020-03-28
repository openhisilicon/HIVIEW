/**
* @file    hi_scene.h
* @brief   scene module struct and interface declaration
*
* Copyright (c) 2012 Huawei Tech.Co.,Ltd
*
* @author    HiMobileCam Reference Develop Team
* @date       2017/12/13
* @version   1.0

*/

#ifndef _HI_SCENE_H_
#define _HI_SCENE_H_

#include "hi_scene_setparam.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/** \addtogroup     SCENE */
/** @{ */  /** <!-- [SCENE] */

/** macro define */
#define HI_SCENE_PIPE_MAX_NUM            (8)  /**< pipe max count*/
#define HI_SCENE_LONGEXP_AEROUTE_MAX_NUM (4)  /**< LongExp AeRoute Number*/
#define HI_SCENE_HDRFRAME_MAX_COUNT      (3)  /**< HDR max frame count*/

/** error code define */
#define HI_SCENE_EINVAL         HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_SCENE, 1)/**<Invalid argument */
#define HI_SCENE_ENOTINIT       HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_SCENE, 2)/**<Not inited */
#define HI_SCENE_ENONPTR        HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_SCENE, 3)/**<null pointer error */
#define HI_SCENE_EOUTOFRANGE    HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_SCENE, 4)/**<out ot range */
#define HI_SCENE_EINTER         HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_SCENE, 5)/**<Internal error */
#define HI_SCENE_EINITIALIZED   HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_SCENE, 6)/**<Already initialized */

/**pipe type*/
typedef enum hiSCENE_PIPE_TYPE_E
{
    HI_SCENE_PIPE_TYPE_SNAP = 0,
    HI_SCENE_PIPE_TYPE_VIDEO,
    HI_SCENE_PIPE_TYPE_BUTT
} HI_SCENE_PIPE_TYPE_E;

/**pipe attr*/
typedef struct hiSCENE_PIPE_ATTR_S
{
    HI_BOOL bEnable;
    HI_HANDLE MainPipeHdl;
    HI_HANDLE VcapPipeHdl;
    HI_HANDLE PipeChnHdl;
    HI_HANDLE VencHdl;
    HI_HANDLE VpssHdl;       /**< vpss group hdl */
    HI_HANDLE VPortHdl;      /**< vpss chn hdl */
    HI_U8 u8PipeParamIndex;  /**<The index means this pipe param we set , is from the param array we put to the module by bin. the typeindex is the arrayIndex */
    HI_SCENE_PIPE_TYPE_E enPipeType;  /**<pipe type means the pipe used to snap or video, 0 is snap 1 is video. 3DNR and DIS need*/
} HI_SCENE_PIPE_ATTR_S;

typedef enum hiSCENE_PIPE_MODE_E
{
    HI_SCENE_PIPE_MODE_LINEAR = 0,
    HI_SCENE_PIPE_MODE_WDR,
    HI_SCENE_PIPE_MODE_HDR,              /**HDR means , WDR existed at same time*/
    HI_SCENE_PIPE_MODE_BUTT
} HI_SCENE_PIPE_MODE_E;

/**scene mode, an encapsulation of pipe attr array*/
typedef struct hiSCENE_MODE_S
{
    HI_SCENE_PIPE_MODE_E enPipeMode;
    HI_SCENE_PIPE_ATTR_S astPipeAttr[HI_SCENE_PIPE_MAX_NUM];  /**<Pipe Attr */
} HI_SCENE_MODE_S;

typedef struct hiSCENE_PARAM_S
{
    HI_SCENE_PIPE_PARAM_S astPipeParam[HI_SCENE_PIPETYPE_NUM];
} HI_SCENE_PARAM_S;

/**
 * @brief     Init SCENE module.
 * @return    0 success, non-zero error code.
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/11
 */
HI_S32 HI_SCENE_Init(const HI_SCENE_PARAM_S *pstSceneParam);

/**
 * @brief     Deinit SCENE module.
 * @return    0 success, non-zero error code.
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/11
 */
HI_S32 HI_SCENE_Deinit(HI_VOID);

/**
 * @brief     SetScenemode, start sceneauto thread.
 * @param[in] PstSceneMode : pipeCfg
 * @return    0 success, non-zero error code.
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/11
 */
HI_S32 HI_SCENE_SetSceneMode(const HI_SCENE_MODE_S *pstSceneMode);

/**
 * @brief GetSceneMode.
 * @param[out]  pstSceneMode : pipeCfg
 * @return   0 success, non-zero error code.
 * @exception    None
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/11
 */
HI_S32 HI_SCENE_GetSceneMode(HI_SCENE_MODE_S *pstSceneMode);

/**
 * @brief Pause sceneauto when isp stoped(in playback state or when reset mediaRoute)
 *          Note: when Pause is true, the sceneauto thread can's set param executed
 * @param[in]  bEnable: pause enable
 * @return   0 success, non-zero error code.
 * @exception    None
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/11
 */
HI_S32 HI_SCENE_Pause(HI_BOOL bEnable);

/** @}*/  /** <!-- ==== SCENE End ====*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* End of #ifndef __HI_SCENE_H__ */


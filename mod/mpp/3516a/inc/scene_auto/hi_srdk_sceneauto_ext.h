/******************************************************************************

  Copyright (C), 2013-2023, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_srdk_sceneauto_ext.h
  Version       : Initial Draft
  Author        : Hisilicon BVR REF
  Created       : 2014/09/10
  History       :
  1.Date        : 2014/09/10
  Author        : noodle
  Modification: Created file

******************************************************************************/

#ifndef __HI_SRDK_SCENEAUTO_EXT_H__
#define __HI_SRDK_SCENEAUTO_EXT_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*****************************************************************************
\brief SCENEAUTO init
\attention \n
\param[in]	const HI_CHAR *pszFileName
\retval ::HI_SUCCESS
\retval ::HI_FAILURE
\see \n
:: \n
******************************************************************************/
HI_S32 HI_SRDK_SCENEAUTO_Init(const HI_CHAR* pszFileName);

/*****************************************************************************
\brief SCENEAUTO deinit
\attention \n
\retval ::HI_SUCCESS
\retval ::HI_FAILURE
\see \n
:: \n
******************************************************************************/
HI_S32 HI_SRDK_SCENEAUTO_DeInit();

/*****************************************************************************
\brief SCENEAUTO  start
\attention \n
\retval ::HI_SUCCESS
\retval ::HI_FAILURE
\see \n
:: \n
******************************************************************************/
HI_S32 HI_SRDK_SCENEAUTO_Start();

/*****************************************************************************
\brief SCENEAUTO  stop
\attention \n
\retval ::HI_SUCCESS
\retval ::HI_FAILURE
\see \n
:: \n
******************************************************************************/
HI_S32 HI_SRDK_SCENEAUTO_Stop();

/*****************************************************************************
\brief SCENEAUTO  set special scenemode
\attention \n
\retval ::HI_SUCCESS
\retval ::HI_FAILURE
\see \n
:: \n
******************************************************************************/
HI_S32 HI_SRDK_SCENEAUTO_SetSpecialMode(const SRDK_SCENEAUTO_SEPCIAL_SCENE_E* peSpecialScene);

/*****************************************************************************
\brief SCENEAUTO  get special scenemode
\attention \n
\retval ::HI_SUCCESS
\retval ::HI_FAILURE
\see \n
:: \n
******************************************************************************/
HI_S32 HI_SRDK_SCENEAUTO_GetSpecialMode(SRDK_SCENEAUTO_SEPCIAL_SCENE_E* peSpecialScene);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

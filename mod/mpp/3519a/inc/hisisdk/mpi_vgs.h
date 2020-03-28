/******************************************************************************
  Copyright (C), 2016, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpi_vgs.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2016/07/24
  Last Modified :
  Description   : mpi functions declaration
  Function List :
******************************************************************************/
#ifndef __MPI_VGS_H__
#define __MPI_VGS_H__

#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_vgs.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /*__cplusplus*/

/*****************************************************************************
 Prototype    : HI_MPI_VGS_BeginJob
 Description  : Begin a vgs job,then add task into the job,vgs will finish all the task in the job.
 Input        : VGS_HANDLE *phHandle
 Output       : None
 Return Value :
 Calls        :
 Called By    :
*****************************************************************************/
HI_S32 HI_MPI_VGS_BeginJob(VGS_HANDLE *phHandle);

/*****************************************************************************
 Prototype    : HI_MPI_VGS_EndJob
 Description  : End a job,all tasks in the job will be submmitted to vgs
 Input        : VGS_HANDLE hHandle
 Output       : None
 Return Value :
 Calls        :
 Called By    :
*****************************************************************************/
HI_S32 HI_MPI_VGS_EndJob(VGS_HANDLE hHandle);

/*****************************************************************************
 Prototype    : HI_MPI_VGS_CancelJob
 Description  : Cancel a job,then all tasks in the job will not be submmitted to vgs
 Input        : VGS_HANDLE hHandle
 Output       : None
 Return Value :
 Calls        :
 Called By    :
*****************************************************************************/
HI_S32 HI_MPI_VGS_CancelJob(VGS_HANDLE hHandle);

/*****************************************************************************
 Prototype    : HI_MPI_VGS_AddScaleTask
 Description  : Add a Scale task to a vgs job
 Input        : VGS_HANDLE hHandle
 Output       : None
 Return Value :
 Calls        :
 Called By    :
*****************************************************************************/
HI_S32 HI_MPI_VGS_AddScaleTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask, VGS_SCLCOEF_MODE_E enScaleCoefMode);

/*****************************************************************************
 Prototype    : HI_MPI_VGS_AddDrawLineTask
 Description  : add a draw line task into a job
 Input        : VGS_HANDLE hHandle
 Output       : None
 Return Value :
 Calls        :
 Called By    :
*****************************************************************************/
HI_S32 HI_MPI_VGS_AddDrawLineTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask, const VGS_DRAW_LINE_S *pstVgsDrawLine);

/*****************************************************************************
 Prototype    : HI_MPI_VGS_AddCoverTask
 Description  : add a Cover task into a job
 Input        : VGS_HANDLE hHandle
 Output       : None
 Return Value :
 Calls        :
 Called By    :
*****************************************************************************/
HI_S32 HI_MPI_VGS_AddCoverTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask, const VGS_ADD_COVER_S *pstVgsAddCover);

/*****************************************************************************
 Prototype    : HI_MPI_VGS_AddOsdTask
 Description  : add a Osd task into a job
 Input        : VGS_HANDLE hHandle
 Output       : None
 Return Value :
 Calls        :
 Called By    :
*****************************************************************************/
HI_S32 HI_MPI_VGS_AddOsdTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask, const VGS_ADD_OSD_S *pstVgsAddOsd);

/*****************************************************************************
 Prototype    : HI_MPI_VGS_AddDrawLineTaskArray
 Description  : add draw line tasks into a job
 Input        : VGS_HANDLE hHandle
 Output       : None
 Return Value :
 Calls        :
 Called By    :
*****************************************************************************/
HI_S32 HI_MPI_VGS_AddDrawLineTaskArray(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask, const VGS_DRAW_LINE_S astVgsDrawLine[], HI_U32 u32ArraySize);

/*****************************************************************************
 Prototype    : HI_MPI_VGS_AddCoverTaskArray
 Description  : add cover tasks into a job
 Input        : VGS_HANDLE hHandle
 Output       : None
 Return Value :
 Calls        :
 Called By    :
*****************************************************************************/
HI_S32 HI_MPI_VGS_AddCoverTaskArray(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask, const VGS_ADD_COVER_S astVgsAddCover[], HI_U32 u32ArraySize);

/*****************************************************************************
 Prototype    : HI_MPI_VGS_AddOsdTaskArray
 Description  : add osd tasks into a job
 Input        : VGS_HANDLE hHandle
 Output       : None
 Return Value :
 Calls        :
 Called By    :
*****************************************************************************/
HI_S32 HI_MPI_VGS_AddOsdTaskArray(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask, const VGS_ADD_OSD_S astVgsAddOsd[], HI_U32 u32ArraySize);

/*****************************************************************************
 Prototype    : HI_MPI_VGS_AddRotationTask
 Description  : add a Rotation task into a job
 Input        : VGS_HANDLE hHandle
 Output       : None
 Return Value :
 Calls        :
 Called By    :
*****************************************************************************/
HI_S32 HI_MPI_VGS_AddRotationTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask, ROTATION_E enRotationAngle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /*__cplusplus*/

#endif /*end of __MPI_VGS_H__*/


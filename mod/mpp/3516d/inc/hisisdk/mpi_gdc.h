/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2018. All rights reserved.
 * Description: mpi_gdc.h
 * Author:
 * Create: 2016-10-07
 */

#ifndef __MPI_GDC_H__
#define __MPI_GDC_H__

#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_gdc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*****************************************************************************
 Prototype    : HI_MPI_GDC_BeginJob
 Description  : Begin a gdc job,then add task into the job,gdc will finish all the task in the job.
 Input        : GDC_HANDLE *phHandle
 Output       : None
 Return Value :
 Calls        :
 Called By    :
</$>
  History        :
  1.Date         : 2016/10/07
    Author       :
    Modification : Created function
<$/>
*****************************************************************************/
HI_S32 HI_MPI_GDC_BeginJob(GDC_HANDLE *phHandle);

/*****************************************************************************
 Prototype    : HI_MPI_GDC_EndJob
 Description  : End a job,all tasks in the job will be submmitted to gdc
 Input        : GDC_HANDLE  hHandle
 Output       : None
 Return Value :
 Calls        :
 Called By    :
</$>
  History        :
  1.Date         : 2016/10/07
    Author       :
    Modification : Created function
<$/>
*****************************************************************************/
HI_S32 HI_MPI_GDC_EndJob(GDC_HANDLE hHandle);

/*****************************************************************************
 Prototype    : HI_MPI_GDC_CancelJob
 Description  : Cancel a job ,then all tasks in the job will not be submmitted to gdc
 Input        : GDC_HANDLE  hHandle
 Output       : None
 Return Value :
 Calls        :
 Called By    :
</$>
  History        :
  1.Date         : 2016/10/07
    Author       :
    Modification : Created function
<$/>
*****************************************************************************/
HI_S32 HI_MPI_GDC_CancelJob(GDC_HANDLE hHandle);

/*****************************************************************************
 Prototype    : HI_MPI_GDC_AddCorrectionTask
 Description  : Add a task to a gdc job
 Input        : GDC_HANDLE  hHandle
             GDC_TASK_ATTR_S *pstTask
             FISHEYE_ATTR_S *pstGdcAttr
 Output       : None
 Return Value :
 Calls        :
 Called By    :
</$>
  History        :
  1.Date         : 2016/10/07
    Author       :
    Modification : Created function
<$/>
*****************************************************************************/
HI_S32 HI_MPI_GDC_AddCorrectionTask(GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask,
                                    const FISHEYE_ATTR_S *pstFisheyeAttr);

/*****************************************************************************
 Prototype    : HI_MPI_GDC_AddCorrectionExTask
 Description  : Add a task to a gdc job
 Input        : GDC_HANDLE  hHandle
             GDC_TASK_ATTR_S *pstTask
             FISHEYE_ATTR_EX_S* pstFishEyeAttrEx
             HI_BOOL bCheckMode
 Output       : None
 Return Value :
 Calls        :
 Called By    :
</$>
  History        :
  1.Date         : 2018/12/22
    Author       :
    Modification : Created function
<$/>
*****************************************************************************/
HI_S32 HI_MPI_GDC_AddCorrectionExTask(GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask,
                                      const FISHEYE_ATTR_EX_S *pstFishEyeAttrEx, HI_BOOL bCheckMode);

/*****************************************************************************
 Prototype    : HI_MPI_GDC_SetConfig
 Description  : Set Config
 Input        : GDC_HANDLE  hHandle
             FISHEYE_JOB_CONFIG_S *stJobConfig

 Output       : None
 Return Value :
 Calls        :
 Called By    :
</$>
  History        :
  1.Date         : 2016/09/13
    Author       :
    Modification : Created function
<$/>
*****************************************************************************/

HI_S32 HI_MPI_GDC_SetConfig(GDC_HANDLE hHandle, const FISHEYE_JOB_CONFIG_S *pstJobConfig);

/*****************************************************************************
 Prototype    : HI_MPI_GDC_AddPMFTask
 Description  : Set Config
 Input        :  GDC_HANDLE  hHandle
                   GDC_TASK_ATTR_S* pstTask
                   GDC_PMF_ATTR_S* pstGdcPmfAttr

 Output       : None
 Return Value :
 Calls        :
 Called By    :
</$>
  History        :
  1.Date         : 2018/05/02
    Author       :
    Modification : Created function
<$/>
*****************************************************************************/

HI_S32 HI_MPI_GDC_AddPMFTask(GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask, const GDC_PMF_ATTR_S *pstGdcPmfAttr);


/*****************************************************************************
 Prototype    : HI_MPI_GDC_FisheyePosQueryDst2Src
 Description  : Query source point from correction point
 Input        : GDC_FISHEYE_POINT_QUERY_ATTR_S *pstFisheyePointQueryAttr
             const VIDEO_FRAME_INFO_S *pstVideoInfo
             const POINT_S *pstDstPoint
             POINT_S *pstSrcPoint
 Output       : None
 Return Value :
 Calls        :
 Called By    :
</$>
  History        :
  1.Date         : 2019/05/20
    Author       :
    Modification : Created function
<$/>
*****************************************************************************/

HI_S32 HI_MPI_GDC_FisheyePosQueryDst2Src(const GDC_FISHEYE_POINT_QUERY_ATTR_S *pstFisheyePointQueryAttr,
    const VIDEO_FRAME_INFO_S *pstVideoInfo, const POINT_S *pstDstPoint, POINT_S *pstSrcPoint);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_GDC_H__ */


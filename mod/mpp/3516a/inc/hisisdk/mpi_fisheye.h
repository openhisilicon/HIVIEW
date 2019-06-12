/******************************************************************************

  Copyright (C), 2013-2033, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_fisheye.h
  Version       : Initial Draft
  Author        : Hisilicon Hi35xx MPP Team
  Created       : 20115/06/29
  Last Modified :
  Description   : mpi functions declaration
  Function List :
  History       :
******************************************************************************/
#ifndef __MPI_FISHEYE_H__
#define __MPI_FISHEYE_H__

#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_fisheye.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*****************************************************************************
 Prototype    : HI_MPI_FISHEYE_BeginJob
 Description  : Begin a fisheye job,then add task into the job,fisheye will finish all the task in the job.
 Input        : FISHEYE_HANDLE *phHandle   
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
</$> 
  History        :
  1.Date         : 20115/06/29
    Author       : c00191088
    Modification : Created function
<$/>
*****************************************************************************/
HI_S32 HI_MPI_FISHEYE_BeginJob(FISHEYE_HANDLE *phHandle);

/*****************************************************************************
 Prototype    : HI_MPI_FISHEYE_EndJob
 Description  : End a job,all tasks in the job will be submmitted to fisheye 
 Input        : FISHEYE_HANDLE  hHandle   
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
</$> 
  History        :
  1.Date         : 20115/06/29
    Author       : c00191088
    Modification : Created function
<$/>
*****************************************************************************/
HI_S32 HI_MPI_FISHEYE_EndJob(FISHEYE_HANDLE hHandle);

/*****************************************************************************
 Prototype    : HI_MPI_FISHEYE_CancelJob
 Description  : Cancel a job ,then all tasks in the job will not be submmitted to fisheye 
 Input        : FISHEYE_HANDLE  hHandle   
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
</$> 
  History        :
  1.Date         : 20115/06/29
    Author       : c00191088
    Modification : Created function
<$/>
*****************************************************************************/
HI_S32 HI_MPI_FISHEYE_CancelJob(FISHEYE_HANDLE hHandle);

/*****************************************************************************
 Prototype    : HI_MPI_FISHEYE_AddScaleTask
 Description  : Add a task to a fisheye job 
 Input        : FISHEYE_HANDLE  hHandle   
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
</$> 
  History        :
  1.Date         : 20115/06/29
    Author       : c00191088
    Modification : Created function
<$/>
*****************************************************************************/
	HI_S32 HI_MPI_FISHEYE_AddCorrectionTask(FISHEYE_HANDLE hHandle, FISHEYE_TASK_ATTR_S *pstTask, const FISHEYE_ATTR_S *pstFisheyeAttr);



/*****************************************************************************
 Prototype    : HI_MPI_FISHEYE_AddDrawLineTask
 Description  : add a draw line task into a job
 Input        : FISHEYE_HANDLE  hHandle   
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
</$> 
  History        :
  1.Date         : 2014/01/27
    Author       : 
    Modification : Created function
<$/>
*****************************************************************************/
HI_S32 HI_MPI_FISHEYE_AddLdcTask(FISHEYE_HANDLE hHandle, FISHEYE_TASK_ATTR_S * pstTask, LDC_ATTR_S * pstFisheyeAddLdc);

/*****************************************************************************
 Prototype    : HI_MPI_FISHEYE_AddCoverTask
 Description  : add a draw point task into a job
 Input        : FISHEYE_HANDLE  hHandle   
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
</$> 
  History        :
  1.Date         : 2014/01/27
    Author       : 
    Modification : Created function
<$/>
*****************************************************************************/
	HI_S32 HI_MPI_FISHEYE_AddPmfTask(FISHEYE_HANDLE hHandle, FISHEYE_TASK_ATTR_S *pstTask, FISHEYE_ADD_PMF_S *pstFisheyeAddPmf);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_FISHEYE_H__ */


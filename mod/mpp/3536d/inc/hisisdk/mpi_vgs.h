/******************************************************************************

  Copyright (C), 2013-2033, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_vgs.h
  Version       : Initial Draft
  Author        : Hisilicon Hi35xx MPP Team
  Created       : 2013/07/24
  Last Modified :
  Description   : mpi functions declaration
  Function List :
  History       :
******************************************************************************/
#ifndef __MPI_VGS_H__
#define __MPI_VGS_H__

#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_vgs.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*****************************************************************************
 Prototype    : HI_MPI_VGS_BeginJob
 Description  : Begin a vgs job,then add task into the job,vgs will finish all the task in the job.
 Input        : VGS_HANDLE *phHandle   
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
</$> 
  History        :
  1.Date         : 2013/07/24
    Author       : z00183560
    Modification : Created function
<$/>
*****************************************************************************/
HI_S32  HI_MPI_VGS_BeginJob(VGS_HANDLE *phHandle);

/*****************************************************************************
 Prototype    : HI_MPI_VGS_EndJob
 Description  : End a job,all tasks in the job will be submmitted to vgs 
 Input        : VGS_HANDLE  hHandle   
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
</$> 
  History        :
  1.Date         : 2013/07/24
    Author       : z00183560
    Modification : Created function
<$/>
*****************************************************************************/
HI_S32 HI_MPI_VGS_EndJob(VGS_HANDLE hHandle);

/*****************************************************************************
 Prototype    : HI_MPI_VGS_CancelJob
 Description  : Cancel a job ,then all tasks in the job will not be submmitted to vgs 
 Input        : VGS_HANDLE  hHandle   
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
</$> 
  History        :
  1.Date         : 2013/07/24
    Author       : z00183560
    Modification : Created function
<$/>
*****************************************************************************/
HI_S32 HI_MPI_VGS_CancelJob(VGS_HANDLE hHandle);

/*****************************************************************************
 Prototype    : HI_MPI_VGS_AddScaleTask
 Description  : Add a task to a vgs job 
 Input        : VGS_HANDLE  hHandle   
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
</$> 
  History        :
  1.Date         : 2013/07/24
    Author       : z00183560
    Modification : Created function
<$/>
*****************************************************************************/
HI_S32  HI_MPI_VGS_AddScaleTask(VGS_HANDLE hHandle, VGS_TASK_ATTR_S *pstTask);


/*****************************************************************************
 Prototype    : HI_MPI_VGS_AddDrawLineTask
 Description  : add a draw line task into a job
 Input        : VGS_HANDLE  hHandle   
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
HI_S32 HI_MPI_VGS_AddDrawLineTask(VGS_HANDLE hHandle, VGS_TASK_ATTR_S *pstTask, VGS_LINE_S astVgsDrawLine[], HI_U32 u32ArraySize);

/*****************************************************************************
 Prototype    : HI_MPI_VGS_AddCoverTask
 Description  : add a draw point task into a job
 Input        : VGS_HANDLE  hHandle   
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
HI_S32 HI_MPI_VGS_AddCoverTask(VGS_HANDLE hHandle, VGS_TASK_ATTR_S *pstTask, VGS_COVER_S astVgsAddCover[], HI_U32 u32ArraySize);

/*****************************************************************************
 Prototype    : HI_MPI_VGS_AddOsdTask
 Description  : add a draw point task into a job
 Input        : VGS_HANDLE  hHandle   
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
HI_S32 HI_MPI_VGS_AddOsdTask(VGS_HANDLE hHandle, VGS_TASK_ATTR_S *pstTask, VGS_OSD_S astVgsAddOsd[], HI_U32 u32ArraySize);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VGS_H__ */


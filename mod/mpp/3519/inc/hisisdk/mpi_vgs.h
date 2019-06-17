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
 Description  : Add a Scale task to a vgs job 
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
HI_S32 HI_MPI_VGS_AddDrawLineTask(VGS_HANDLE hHandle, VGS_TASK_ATTR_S *pstTask, VGS_DRAW_LINE_S *pstVgsDrawLine);

/*****************************************************************************
 Prototype    : HI_MPI_VGS_AddCoverTask
 Description  : add a Cover task into a job
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
HI_S32 HI_MPI_VGS_AddCoverTask(VGS_HANDLE hHandle, VGS_TASK_ATTR_S *pstTask, VGS_ADD_COVER_S *pstVgsAddCover);

/*****************************************************************************
 Prototype    : HI_MPI_VGS_AddOsdTask
 Description  : add a Osd task into a job
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
HI_S32 HI_MPI_VGS_AddOsdTask(VGS_HANDLE hHandle, VGS_TASK_ATTR_S *pstTask, VGS_ADD_OSD_S *pstVgsAddOsd);

/*****************************************************************************
 Prototype    : HI_MPI_VGS_AddDrawLineTaskArray
 Description  : add draw line tasks into a job
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
HI_S32 HI_MPI_VGS_AddDrawLineTaskArray(VGS_HANDLE hHandle, VGS_TASK_ATTR_S * pstTask, VGS_DRAW_LINE_S astVgsDrawLine[], HI_U32 u32ArraySize);
/*****************************************************************************
 Prototype    : HI_MPI_VGS_AddCoverTaskArray
 Description  : add cover tasks into a job
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
HI_S32 HI_MPI_VGS_AddCoverTaskArray(VGS_HANDLE hHandle, VGS_TASK_ATTR_S * pstTask, VGS_ADD_COVER_S astVgsAddCover[], HI_U32 u32ArraySize);

/*****************************************************************************
 Prototype    : HI_MPI_VGS_AddOsdTaskArray
 Description  : add osd tasks into a job
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
HI_S32 HI_MPI_VGS_AddOsdTaskArray(VGS_HANDLE hHandle, VGS_TASK_ATTR_S * pstTask, VGS_ADD_OSD_S astVgsAddOsd[], HI_U32 u32ArraySize);

/*****************************************************************************
 Prototype    : HI_MPI_VGS_AddQuickCopyTask
 Description  : add Quick Copy tasks into a job
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
HI_S32 HI_MPI_VGS_AddQuickCopyTask(VGS_HANDLE hHandle, VGS_TASK_ATTR_S * pstTask, VGS_ADD_QUICK_COPY_S * pstAddQuickCopy);


/*****************************************************************************
 Prototype    : HI_MPI_VGS_AddRevertTask
 Description  : add Revert tasks into a job
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
HI_S32 HI_MPI_VGS_AddRevertTask(VGS_HANDLE hHandle, VGS_TASK_ATTR_S * pstTask, VGS_ADD_REVERT_S * pstAddRevert);

/*****************************************************************************
 Prototype    : HI_MPI_VGS_AddRotateTask
 Description  : add Rotate tasks into a job
 Input        : VGS_HANDLE  hHandle   
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
</$> 
  History        :
  1.Date         : 2017/07/07
    Author       : 
    Modification : Created function
<$/>
*****************************************************************************/

HI_S32 HI_MPI_VGS_AddRotateTask(VGS_HANDLE hHandle,VGS_TASK_ATTR_S *pstTask, const ROTATE_E enRotate);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VGS_H__ */


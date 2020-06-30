/******************************************************************************

  Copyright (C), 2017-2018, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_photo.h
  Version       : Initial Draft
  Author        :
  Created       :
  Last Modified :
  Description   : mpi photo function declarations
  Function List :
  History       :
  1.Date        : 20171216
    Author      :
    Modification: Create
******************************************************************************/
#ifndef __MPI_PHOTO_H__
#define __MPI_PHOTO_H__

#include "hi_common.h"
#include "hi_comm_photo.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

HI_S32 HI_MPI_PHOTO_AlgInit(PHOTO_ALG_TYPE_E enAlgType, const PHOTO_ALG_INIT_S *pstPhotoInit);
HI_S32 HI_MPI_PHOTO_AlgDeinit(PHOTO_ALG_TYPE_E enAlgType);

HI_S32 HI_MPI_PHOTO_AlgProcess(PHOTO_ALG_TYPE_E enAlgType, const PHOTO_ALG_ATTR_S *pstPhotoAttr);

HI_S32 HI_MPI_PHOTO_SetAlgCoef(PHOTO_ALG_TYPE_E enAlgType, const PHOTO_ALG_COEF_S *pstAlgCoef);
HI_S32 HI_MPI_PHOTO_GetAlgCoef(PHOTO_ALG_TYPE_E enAlgType, PHOTO_ALG_COEF_S *pstAlgCoef);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_PHOTO_H__ */


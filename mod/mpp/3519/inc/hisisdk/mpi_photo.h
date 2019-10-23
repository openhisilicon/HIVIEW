/******************************************************************************

  Copyright (C), 2016-2036, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_photo.h
  Version       : Initial Draft
  Author        : 
  Created       : 
  Last Modified :
  Description   : mpi photo function declarations
  Function List :
  History       :
  1.Date        : 20160328
    Author      :
    Modification: Create
******************************************************************************/
#ifndef __MPI_PHOTO_H__
#define __MPI_PHOTO_H__

#include "hi_common.h"
#include "hi_comm_photo.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


/*
 * @brief   init algorithms, load alg libs
 * @param   enAlgType, algorithm type
 * @return  return HI_SUCCESS on success, else return HI_FAILURE
 * */
HI_S32 HI_MPI_PHOTO_Init(PHOTO_ALG_TYPE_E enAlgType);


/*
 * @brief   deinit algorithms, unload alg libs
 * @param   enAlgType, algorithm type
 * @return  return HI_SUCCESS on success, else return HI_FAILURE
 * */
HI_S32 HI_MPI_PHOTO_DeInit(PHOTO_ALG_TYPE_E enAlgType);


/*
 * @brief   set alg attr
 * @param   pstAlgAttr
 * @return  return HI_SUCCESS on success, else return HI_FAILURE
 * */
HI_S32 HI_MPI_PHOTO_SetAlgAttr(const PHOTO_ALG_ATTR_S *pstAlgAttr);


/*
 * @brief   get alg attr
 * @param   pstAlgAttr
 * @return  return HI_SUCCESS on success, else return HI_FAILURE
 * */
HI_S32 HI_MPI_PHOTO_GetAlgAttr(PHOTO_ALG_ATTR_S *pstAlgAttr);


/*
 * @brief   process the input image with the giving algorithm type
 * @param   pstInImg,  input images
 * @param   pstOutImg, output image
 * @param   pstAlgAttr, algorithm attrs
 * @return  return HI_SUCCESS on success, else return HI_FAILURE
 * */
HI_S32 HI_MPI_PHOTO_Proc(PHOTO_ALG_TYPE_E enAlgType,
            const PHOTO_SRC_IMG_S *pstInImg, PHOTO_DST_IMG_S *pstOutImg);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_PHOTO_H__ */


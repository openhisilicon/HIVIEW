/******************************************************************************
*
* Copyright (C) 2014 Hisilicon Technologies Co., Ltd.  All rights reserved. 
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon), 
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name	    : hi_jpeg_api.h
Version		    : Initial Draft
Author		    : 
Created		    : 2014/06/20
Description	    : The user will use this api to realize some function
Function List 	: 

			  		  
History       	:
Date				Author        		Modification
2014/06/20		    y00181162  		    Created file      	
******************************************************************************/

#ifndef __HI_JPEG_API_H__
#define __HI_JPEG_API_H__


/*********************************add include here******************************/

#include "jpeglib.h"
#include "hi_type.h"

/*****************************************************************************/


#ifdef __cplusplus
#if __cplusplus
   extern "C" 
{
#endif
#endif /* __cplusplus */


     /***************************** Macro Definition ******************************/
	 /** \addtogroup	  JPEG */
	 /** @{ */	/** <!--[JPEG]*/

	 /** Maximum number of data components */
     #define MAX_PIXEL_COMPONENT_NUM            3
	 /** Maximum column number of hor coef */
     #define MAX_HORCOEF_COL                     8
	 /** Maximum row number of hor coef */
     #define MAX_HORCOEF_ROW                     4
	 /** Maximum column number of ver coef */
	 #define MAX_VERCOEF_COL                     4
	 /** Maximum row number of ver coef */
     #define MAX_VERCOEF_ROW                     4
	 /** Maximum column number of csc coef */
	 #define MAX_CSCCOEF_COL                     3
	 /** Maximum row number of csc coef */
     #define MAX_CSCCOEF_ROW                     3

     /** @} */  /** <!-- ==== Macro Definition end ==== */

	 /*************************** Enum Definition ****************************/
    /****************************************************************************/
	/*							   jpeg enum    					            */
	/****************************************************************************/
	
	/** \addtogroup	  JPEG */
	/** @{ */	/** <!--[JPEG]*/

	/** enum of the input jpeg color space */
	typedef enum hiHI_JPEG_FMT_E
	{
		JPEG_FMT_YUV400	    = 0,   /**< yuv400     */
		JPEG_FMT_YUV420	    = 1,   /**< yuv420     */
		JPEG_FMT_YUV422_21    = 2,   /**< yuv422_21  */
		JPEG_FMT_YUV422_12    = 3,   /**< yuv422_12  */
		JPEG_FMT_YUV444	    = 4,   /**< yuv444     */
		JPEG_FMT_YCCK   	    = 5,   /**< YCCK       */
		JPEG_FMT_CMYK   	    = 6,   /**< CMYK       */
		JPEG_FMT_BUTT	
	}HI_JPEG_FMT_E;

	/** enum of mem check type */
	typedef enum hiHI_JPEG_MEMCHECK_E
	{
		JPEG_MEMCHECK_MEMSIZE    = 0,   /**< mem size   */
		JPEG_MEMCHECK_RESOLUTION = 1,   /**< resolution     */
		JPEG_MEMCHECK_BUTT	
	}HI_JPEG_MEMCHECK_E;

    /** @} */  /** <!-- ==== enum Definition end ==== */
	
	/*************************** Structure Definition ****************************/
    /****************************************************************************/
	/*							   jpeg api structure    					    */
	/****************************************************************************/
	
	/** \addtogroup	  JPEG */
	/** @{ */	/** <!--[JPEG]*/

	/** Structure of Rectangle */
	typedef struct hiJPEG_RECT_S
	{
		HI_S32 x, y;
		HI_S32 w, h;
	}HI_JPEG_RECT_S;

	/** Structure of the output yuvsp/rgb/yuv package message, only support 1/2/4/8/ scale */
	typedef struct hiJPEG_SURFACE_S
	{
		 HI_CHAR* pOutPhy[MAX_PIXEL_COMPONENT_NUM];        /**< output physic address,0 is luminance or rgb buf, 1 is chroma buf     */
		 HI_CHAR* pOutVir[MAX_PIXEL_COMPONENT_NUM];        /**< output virtual address,0 is luminance or rgb buf, 1 is chroma buf   */
		 HI_U32   u32OutStride[MAX_PIXEL_COMPONENT_NUM];   /**< output stride                          */
         HI_BOOL  bUserPhyMem;                             /**< if use user physic memeory           */
	
	}HI_JPEG_SURFACE_S;
	
	/** Structure of the output message */
	typedef struct hiJPEG_SURFACE_DESCRIPTION_S
	{
		 HI_JPEG_SURFACE_S	 stOutSurface; /**< output  surface     */
		 HI_JPEG_RECT_S      stCropRect;   /**< output crop size    */
		 HI_BOOL bCrop;                    /**<  if crop            */
	}HI_JPEG_SURFACE_DESCRIPTION_S;


	/** Structure of output argb sampler and filte message */
    typedef struct hiJPEG_DEC_COEF_S
    {
        HI_BOOL bEnHorMedian;                                  /**< if set hor median filter           */
		HI_BOOL bEnVerMedian;                                  /**< if set ver median filter              */
        HI_BOOL bSetHorSampleCoef;                             /**< if set hor sample coef            */
		HI_BOOL bSetVerSampleCoef;                             /**< if set ver sample coef             */
		HI_BOOL bSetCSCCoef;                                   /**< if set csc coef                       */
		HI_S16  s16HorCoef[MAX_HORCOEF_ROW][MAX_HORCOEF_COL];  /**< set chrome hor sampler coefficient */
		HI_S16  s16VerCoef[MAX_VERCOEF_ROW][MAX_VERCOEF_COL];  /**< set chrome ver sampler coefficient */
		HI_S16  s16CSCCoef[MAX_CSCCOEF_ROW][MAX_CSCCOEF_COL];  /**< set csc coefficient                 */
    }HI_JPEG_DEC_COEF_S;


	/** Structure of the input and output jpeg width and height,input jpeg format */
	typedef struct hiJPEG_INFO_S
	{
	     HI_U32  u32Width[MAX_PIXEL_COMPONENT_NUM];     /**< 0 input and output rgb width and lu width,1 is ch width    */
		 HI_U32  u32Height[MAX_PIXEL_COMPONENT_NUM];    /**< 0 input and output rgb height and lu width,1 is ch height  */
		 HI_U32  u32OutStride[MAX_PIXEL_COMPONENT_NUM]; /**< 0 output rgb stride and lu stride,1 is ch stride              */
		 HI_U32  u32OutSize[MAX_PIXEL_COMPONENT_NUM];   /**< 0 output rgb size and lu size,1 is ch size		           */
		 HI_JPEG_FMT_E enFmt;  /**< the input format  */
		 HI_BOOL bOutInfo;     /**< false is get input information and the other get output information    */
	}HI_JPEG_INFO_S;


	/** Structure of the leave memsize that jpeg decode can used */
	typedef struct hiJPEG_MEMSIZE_INFO_S
	{
	     HI_U32  u32MemSize;      /**< the memsize */
		 HI_U32  u32Width;		   /**< limit width   */
		 HI_U32  u32Height;		   /**< limit height  */
		 HI_JPEG_MEMCHECK_E eCheckType; /**< limit type */
	}HI_JPEG_MEMSIZE_INFO_S;
	
	/** @} */  /** <!-- ==== Structure Definition End ==== */
	
    /********************** Global Variable declaration **************************/
 
    /******************************* API declaration *****************************/


	/** \addtogroup	  JPEG */
	/** @{ */	/** <!--[JPEG]*/
	

	/** 
	\brief Sets dec output message.
	\attention \n
	HI_JPEG_SetOutDesc should have called create jpeg decoder.set the output address \n
	and output stride,set whether crop, set crop rect \n
	

	\param[in]	cinfo. 
	\param[in]	*pstSurfaceDesc. 
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetOutDesc
	*/
	HI_S32	HI_JPEG_SetOutDesc(const struct jpeg_decompress_struct *cinfo, const HI_JPEG_SURFACE_DESCRIPTION_S *pstSurfaceDesc);

	/** 
	\brief Get Jpeg information.
	\attention \n
	if you want to get input format and input width and input height,you should set bOutInfo false.\n
    others you can get the information as follows: output rgb widht/height/stride/size or output \n
    yuvsp lu width/height/stride/size and ch width/height/stride/size.\n
	you call this function should after read header and set the ouput parameter.\n
	

	\param[in]	cinfo.
	\param[out]	pJpegInfo.  
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_GetJpegInfo
	*/
	HI_S32  HI_JPEG_GetJpegInfo(j_decompress_ptr cinfo, HI_JPEG_INFO_S *pJpegInfo);


	/** 
	\brief set jpeg dec inflexion. 
	\attention \n
	HI_JPEG_SetInflexion should have called jpeg_create_decompress.if no call this \n
	function,use the default flexion \n

	\param[in]	cinfo.
	\param[in]	u32flexionSize.
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetInflexion
	*/
	HI_S32 HI_JPEG_SetInflexion(const struct jpeg_decompress_struct *cinfo, const HI_U32 u32flexionSize);
	

	/** 
	\brief get jpeg dec inflexion.
	\attention \n
	HI_JPEG_GetInflexion should have called jpeg_create_decompress.\n
	
	\param[in]	cinfo. 
	\param[out]	pu32flexionSize.
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetInflexion
	*/
	HI_S32 HI_JPEG_GetInflexion(const struct jpeg_decompress_struct *cinfo, HI_U32 *pu32flexionSize);
	

	/** 
	\brief set jpeg dec coef when output argb.
	\attention \n
	HI_JPEG_SetDecCoef should have called jpeg_create_decompress.set whether horizontal \n
	and vertical fliter,whether set horizontal and ver sample, whether set csc coefficient, \n
	and set there coefficient.if no call this function, use the default parameter. \n
	
	\param[in]	cinfo. 
	\param[in]	*pstDecCoef. 
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetDecCoef
	*/
	HI_S32 HI_JPEG_SetDecCoef(const struct jpeg_decompress_struct *cinfo, const HI_JPEG_DEC_COEF_S *pstDecCoef);


	/** 
	\brief get jpeg dec coef when output argb.
	\attention \n
	HI_JPEG_GetDecCoef should have called HI_JPEG_SetDecCoef.\n
	
	\param[in]	cinfo.
	\param[out]	pstOutDecCoef.
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_GetDecCoef
	*/
	HI_S32 HI_JPEG_GetDecCoef(const struct jpeg_decompress_struct *cinfo, HI_JPEG_DEC_COEF_S *pstOutDecCoef);


	/** 
	\brief set alpha value. 
	\attention \n
	HI_JPEG_SetAlpha should have called jpeg_create_decompress.when decode output \n
	argb8888 and argb8888,we can call this function,if no call it,use the default value. \n
	
	\param[in]	cinfo.
	\param[in]	s32Alpha.
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetAlpha
	*/
	HI_S32 HI_JPEG_SetAlpha(const struct jpeg_decompress_struct *cinfo, const HI_U32 u32Alpha);


	/** 
	\brief set stream from flag of use phy mem  or virtual mem.
	\attention \n
	if want to use this function,should call between create decompress and 
	jpeg_stdio_src or jpeg_mem_src.if not call this we should check\n
	
	\param[in]	cinfo. 
	\param[in]	pStreamPhyAddr.
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetStreamPhyMem
	*/
	 HI_S32 HI_JPEG_SetStreamPhyMem(const struct jpeg_decompress_struct *cinfo, HI_CHAR* pStreamPhyAddr);

	/** 
	\brief set stream mem information.
	\attention \n
	if want to use this function,should call before call
	jpeg_stdio_src or jpeg_mem_src.if not call this we should check\n
	
	\param[in]	cinfo. 
	\param[in]	pPhyStremBuf.       
	\param[in]	u32StremBufLen.     
	\param[in]	pVirSaveReturnBuf.  
	\param[in]	s32StrmReturnSize.  
	\param[in]	bStreamBufNeedReturn. 
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetBufInfo
	*/
	HI_S32 HI_JPEG_SetBufInfo(const struct jpeg_decompress_struct *cinfo,HI_CHAR* pPhyStremBuf,HI_U32 u32StremBufLen, HI_CHAR* pVirSaveReturnBuf,HI_S32 s32StrmReturnSize,HI_BOOL bStreamBufNeedReturn);

	/** 
	\brief set if dec output yuv420sp.
	\attention \n
	HI_JPEG_SetYCbCr420spFlag should have called jpeg_create_decompress.\n
	
	\param[in]	cinfo. 
	\param[in]	bOutYCbCr420sp.
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetYCbCr420spFlag
	*/
	HI_S32 HI_JPEG_SetYCbCr420spFlag(const struct jpeg_decompress_struct *cinfo,const HI_BOOL bOutYCbCr420sp);


	/** 
	\brief set if output lu pixle sum value.
	\attention \n
	HI_JPEG_SetLuPixSumFlag should have called jpeg_create_decompress.\n
	
	\param[in]	cinfo. 
	\param[in]	bLuPixSum.
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetLuPixSumFlag
	*/
	HI_S32 HI_JPEG_SetLuPixSumFlag(const struct jpeg_decompress_struct *cinfo,const HI_BOOL bLuPixSum);

	/** 
	\brief get lu pixle sum value. 
	\attention \n
	If you want to get the luminance value, you can call this function, \n
	but you should call it after jpeg_start_decompress and have call HI_JPEG_SetLuPixSumFlag.\n
	
	\param[in]	cinfo. 
	\param[out]	u64LuPixSum. 
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_GetLuPixSum
	*/
	HI_S32 HI_JPEG_GetLuPixSum(const struct jpeg_decompress_struct *cinfo,HI_U64 *u64LuPixSum);


	/** 
	\brief get jpeg dec time.
	\attention \n
	If you want to know how much the decode cost time ,you can call HI_JPEG_GetDecTime, \n
	but should have called it after jpeg_finish_decompress.\n
	
	\param[in]	cinfo. 
	\param[out]	pu32DecTime. 
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_GetDecTime
	*/
	HI_S32 HI_JPEG_GetDecTime(const struct jpeg_decompress_struct *cinfo, HI_U32 *pu32DecTime);


	
	/** 
	\brief set the leave memory size.
	\attention \n
	should have called it after  create jpeg decoder.\n
	
	\param[in]	cinfo. 
	\param[int] sMemSizeInfo. 
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetLeaveMemSize
	*/
	HI_S32 HI_JPEG_SetLeaveMemSize(const struct jpeg_decompress_struct *cinfo, HI_JPEG_MEMSIZE_INFO_S sMemSizeInfo);

	/** @} */  /** <!-- ==== API Declaration End ==== */

    /****************************************************************************/



#ifdef __cplusplus
    
#if __cplusplus
   
}
#endif
#endif /* __cplusplus */

#endif /* __HI_JPEG_API_H__*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_sys.h"
#include "hi_comm_ive.h"
#include "hi_comm_vgs.h"
#include "hi_comm_vi.h"
#include "hi_comm_vo.h"

#include "mpi_vb.h"
#include "mpi_sys.h"
#include "mpi_vi.h"
#include "mpi_vo.h"

#include "mpi_ive.h"
#include "mpi_vgs.h"

#include "sample_comm_ive.h"

static HI_BOOL bMpiInit = HI_FALSE;

HI_U16 SAMPLE_COMM_IVE_CalcStride(HI_U16 u16Width, HI_U8 u8Align)
{
    return (u16Width + (u8Align - u16Width % u8Align) % u8Align);
}

static HI_S32 SAMPLE_IVE_MPI_Init(HI_VOID)
{
    HI_S32 s32Ret;
    VB_CONF_S struVbConf;
    MPP_SYS_CONF_S struSysConf;

    HI_MPI_SYS_Exit();
    HI_MPI_VB_Exit();

    memset(&struVbConf, 0, sizeof(VB_CONF_S));
    struVbConf.u32MaxPoolCnt             = 128;
    struVbConf.astCommPool[0].u32BlkSize = 1920 * 1080 * 2;
    struVbConf.astCommPool[0].u32BlkCnt  = 40;
    s32Ret = HI_MPI_VB_SetConf(&struVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VB_SetConf fail,Error(%#x)\n", s32Ret);
        return s32Ret;
    }
    s32Ret = HI_MPI_VB_Init();
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VB_Init fail,Error(%#x)\n", s32Ret);
        return s32Ret;
    }
    struSysConf.u32AlignWidth = 64;
    s32Ret = HI_MPI_SYS_SetConf(&struSysConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_SYS_SetConf fail,Error(%#x)\n", s32Ret);
        (HI_VOID)HI_MPI_VB_Exit();
        return s32Ret;
    }

    s32Ret = HI_MPI_SYS_Init();
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_SYS_Init fail,Error(%#x)\n", s32Ret);
        (HI_VOID)HI_MPI_VB_Exit();
        return s32Ret;
    }

    return HI_SUCCESS;
}


HI_VOID SAMPLE_COMM_IVE_CheckIveMpiInit(HI_VOID)
{
    if (HI_FALSE == bMpiInit)
    {
        if (SAMPLE_IVE_MPI_Init())
        {
            SAMPLE_PRT("Ive mpi init failed!\n");
            exit(-1);
        }
        bMpiInit = HI_TRUE;
    }
}
HI_S32 SAMPLE_COMM_IVE_IveMpiExit(HI_VOID)
{
    bMpiInit = HI_FALSE;
    if (HI_MPI_SYS_Exit())
    {
        SAMPLE_PRT("Sys exit failed!\n");
        return HI_FAILURE;
    }

    if (HI_MPI_VB_Exit())
    {
        SAMPLE_PRT("Vb exit failed!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VGS_AddDrawRectJob(VGS_HANDLE VgsHandle, IVE_IMAGE_S* pstSrc, IVE_IMAGE_S* pstDst,
                                      RECT_S* pstRect, HI_U16 u16RectNum)
{
    HI_S32 s32Ret = HI_SUCCESS;
    VGS_TASK_ATTR_S stVgsTask;
    VGS_ADD_COVER_S stVgsCover;
    HI_U16 i;

    memset(&stVgsTask, 0, sizeof(VGS_TASK_ATTR_S));

    stVgsTask.stImgIn.stVFrame.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    stVgsTask.stImgIn.stVFrame.enCompressMode = COMPRESS_MODE_NONE;
    stVgsTask.stImgIn.stVFrame.enVideoFormat = VIDEO_FORMAT_LINEAR;
    stVgsTask.stImgIn.stVFrame.u32Field = VIDEO_FIELD_FRAME;
    stVgsTask.stImgIn.stVFrame.u32Width = pstSrc->u16Width;
    stVgsTask.stImgIn.stVFrame.u32Height = pstSrc->u16Height;
    stVgsTask.stImgIn.stVFrame.u32PhyAddr[0] = pstSrc->u32PhyAddr[0];

    stVgsTask.stImgIn.stVFrame.u32PhyAddr[1] = pstSrc->u32PhyAddr[1];
    stVgsTask.stImgIn.stVFrame.pVirAddr[0] = pstSrc->pu8VirAddr[0];
    stVgsTask.stImgIn.stVFrame.pVirAddr[1] = pstSrc->pu8VirAddr[1];

    stVgsTask.stImgIn.stVFrame.u32Stride[0] = pstSrc->u16Stride[0];
    stVgsTask.stImgIn.stVFrame.u32Stride[1] = pstSrc->u16Stride[1];
    stVgsTask.stImgIn.stVFrame.u64pts = 12;
    stVgsTask.stImgIn.stVFrame.u32TimeRef = 24;

    stVgsTask.stImgOut.stVFrame.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    stVgsTask.stImgOut.stVFrame.enCompressMode = COMPRESS_MODE_NONE;
    stVgsTask.stImgOut.stVFrame.enVideoFormat = VIDEO_FORMAT_LINEAR;
    stVgsTask.stImgOut.stVFrame.u32Field = VIDEO_FIELD_FRAME;
    stVgsTask.stImgOut.stVFrame.u32Width = pstDst->u16Width;
    stVgsTask.stImgOut.stVFrame.u32Height = pstDst->u16Height;
    stVgsTask.stImgOut.stVFrame.u32PhyAddr[0] = pstDst->u32PhyAddr[0];

    stVgsTask.stImgOut.stVFrame.u32PhyAddr[1] = pstDst->u32PhyAddr[1];
    stVgsTask.stImgOut.stVFrame.pVirAddr[0] = pstDst->pu8VirAddr[0];
    stVgsTask.stImgOut.stVFrame.pVirAddr[1] = pstDst->pu8VirAddr[1];

    stVgsTask.stImgOut.stVFrame.u32Stride[0] = pstDst->u16Stride[0];
    stVgsTask.stImgOut.stVFrame.u32Stride[1] = pstDst->u16Stride[1];
    stVgsTask.stImgOut.stVFrame.u64pts = 12;
    stVgsTask.stImgOut.stVFrame.u32TimeRef = 24;

    stVgsCover.enCoverType = COVER_RECT;
    for (i = 0; i < u16RectNum; i++)
    {
        stVgsCover.stDstRect.s32X = (pstRect[i].s32X / 2) * 2;
        stVgsCover.stDstRect.s32Y = (pstRect[i].s32Y / 2) * 2;
        stVgsCover.stDstRect.u32Width = (pstRect[i].u32Width / 2) * 2;
        stVgsCover.stDstRect.u32Height = (pstRect[i].u32Height / 2) * 2;
        stVgsCover.u32Color = 0x0000FF00;
        if (stVgsCover.stDstRect.s32X + stVgsCover.stDstRect.u32Width < stVgsTask.stImgOut.stVFrame.u32Width
            && stVgsCover.stDstRect.s32Y + stVgsCover.stDstRect.u32Height < stVgsTask.stImgOut.stVFrame.u32Height)
        {
            s32Ret = HI_MPI_VGS_AddCoverTask(VgsHandle, &stVgsTask, &stVgsCover);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("HI_MPI_VGS_AddCoverTask fail,Error(%#x)\n", s32Ret);
                HI_MPI_VGS_CancelJob(VgsHandle);
                return s32Ret;
            }
        }

    }

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VGS_FillRect(VIDEO_FRAME_INFO_S* pstFrmInfo, SAMPLE_RECT_ARRAY_S* pstRect, HI_U32 u32Color)
{
    VGS_HANDLE VgsHandle = -1;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U16 i;
    VGS_TASK_ATTR_S stVgsTask;
    VGS_ADD_COVER_S stVgsAddCover;

    if (0 == pstRect->u16Num)
    {
        return s32Ret;
    }
    s32Ret = HI_MPI_VGS_BeginJob(&VgsHandle);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Vgs begin job fail,Error(%#x)\n", s32Ret);
        return s32Ret;
    }

    memcpy(&stVgsTask.stImgIn, pstFrmInfo, sizeof(VIDEO_FRAME_INFO_S));
    memcpy(&stVgsTask.stImgOut, pstFrmInfo, sizeof(VIDEO_FRAME_INFO_S));

    stVgsAddCover.enCoverType = COVER_QUAD_RANGLE;
    stVgsAddCover.u32Color = u32Color;
    for (i = 0; i < pstRect->u16Num; i++)
    {
        stVgsAddCover.stQuadRangle.bSolid = HI_FALSE;
        stVgsAddCover.stQuadRangle.u32Thick = 2;
        memcpy(stVgsAddCover.stQuadRangle.stPoint, pstRect->astRect[i].astPoint, sizeof(pstRect->astRect[i].astPoint));
        s32Ret = HI_MPI_VGS_AddCoverTask(VgsHandle, &stVgsTask, &stVgsAddCover);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI_MPI_VGS_AddCoverTask fail,Error(%#x)\n", s32Ret);
            HI_MPI_VGS_CancelJob(VgsHandle);
            return s32Ret;
        }

    }

    s32Ret = HI_MPI_VGS_EndJob(VgsHandle);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VGS_EndJob fail,Error(%#x)\n", s32Ret);
        HI_MPI_VGS_CancelJob(VgsHandle);
        return s32Ret;
    }
    return s32Ret;

}

HI_S32 SAMPLE_COMM_IVE_ReadFile(IVE_IMAGE_S* pstImg, FILE* pFp)
{
    HI_U16 y;
    HI_U8* pU8;
    HI_U16 height;
    HI_U16 width;
    HI_U16 loop;

	(HI_VOID)fgetc(pFp);
	if (feof(pFp))
	{
		SAMPLE_PRT("end of file!\n");
		fseek(pFp, 0 , SEEK_SET );
	}
	else
	{
		fseek(pFp, -1 , SEEK_CUR ); 
	}
	
    //if (feof(pFp))
    //{
    //    SAMPLE_PRT("end of file!\n");
    //    fseek(pFp, 0 , SEEK_SET);
    //}

    height = pstImg->u16Height;
    width = pstImg->u16Width;

    switch (pstImg->enType)
    {
        case  IVE_IMAGE_TYPE_U8C1:
        {
            pU8 = pstImg->pu8VirAddr[0];
            for (y = 0; y < height; y++)
            {
                if ( 1 != fread(pU8, width, 1, pFp))
                {
                    SAMPLE_PRT("Read file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->u16Stride[0];
            }
        }
        break;
        case  IVE_IMAGE_TYPE_YUV420SP:
        {
            pU8 = pstImg->pu8VirAddr[0];
            for (y = 0; y < height; y++)
            {
                if ( 1 != fread(pU8, width, 1, pFp))
                {
                    SAMPLE_PRT("Read file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->u16Stride[0];
            }

            pU8 = pstImg->pu8VirAddr[1];
            for (y = 0; y < height / 2; y++)
            {
                if ( 1 != fread(pU8, width, 1, pFp))
                {
                    SAMPLE_PRT("Read file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->u16Stride[1];
            }
        }
        break;
        case IVE_IMAGE_TYPE_YUV422SP:
        {
            pU8 = pstImg->pu8VirAddr[0];
            for (y = 0; y < height; y++)
            {
                if ( 1 != fread(pU8, width, 1, pFp))
                {
                    SAMPLE_PRT("Read file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->u16Stride[0];
            }

            pU8 = pstImg->pu8VirAddr[1];
            for (y = 0; y < height; y++)
            {
                if ( 1 != fread(pU8, width, 1, pFp))
                {
                    SAMPLE_PRT("Read file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->u16Stride[1];
            }
        }
        break;
        case IVE_IMAGE_TYPE_U8C3_PACKAGE:
        {
            pU8 = pstImg->pu8VirAddr[0];
            for (y = 0; y < height; y++)
            {
                if ( 1 != fread(pU8, width * 3, 1, pFp))
                {
                    SAMPLE_PRT("Read file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->u16Stride[0] * 3;
            }

        }
        break;
        case IVE_IMAGE_TYPE_U8C3_PLANAR:
        {
            for (loop = 0; loop < 3; loop++)
            {
                pU8 = pstImg->pu8VirAddr[loop];
                for (y = 0; y < height; y++)
                {
                    if ( 1 != fread(pU8, width, 1, pFp))
                    {
                        SAMPLE_PRT("Read file fail\n");
                        return HI_FAILURE;
                    }

                    pU8 += pstImg->u16Stride[loop];
                }
            }

        }
        break;
        case IVE_IMAGE_TYPE_S16C1:
        case IVE_IMAGE_TYPE_U16C1:
        {
            pU8 = pstImg->pu8VirAddr[0];
            for ( y = 0; y < height; y++ )
            {
                if ( sizeof(HI_U16) != fread(pU8, width, sizeof(HI_U16), pFp) )
                {
                    SAMPLE_PRT("Read file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->u16Stride[0] * 2;
            }
        }
        break;
        default:
            break;
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_IVE_WriteFile(IVE_IMAGE_S* pstImg, FILE* pFp)
{
    HI_U16 y;
    HI_U8* pU8;
    HI_U16 height;
    HI_U16 width;

    height = pstImg->u16Height;
    width = pstImg->u16Width;

    switch (pstImg->enType)
    {
        case  IVE_IMAGE_TYPE_U8C1:
        case  IVE_IMAGE_TYPE_S8C1:
        {
            pU8 = pstImg->pu8VirAddr[0];
            for (y = 0; y < height; y++)
            {
                if ( 1 != fwrite(pU8, width, 1, pFp))
                {
                    SAMPLE_PRT("Write file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->u16Stride[0];
            }
        }
        break;
        case  IVE_IMAGE_TYPE_YUV420SP:
        {
            pU8 = pstImg->pu8VirAddr[0];
            for (y = 0; y < height; y++)
            {
                if ( width != fwrite(pU8, 1, width, pFp))
                {
                    SAMPLE_PRT("Write file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->u16Stride[0];
            }

            pU8 = pstImg->pu8VirAddr[1];
            for (y = 0; y < height / 2; y++)
            {
                if ( width != fwrite(pU8, 1, width, pFp))
                {
                    SAMPLE_PRT("Write file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->u16Stride[1];
            }
        }
        break;
        case IVE_IMAGE_TYPE_YUV422SP:
        {
            pU8 = pstImg->pu8VirAddr[0];
            for (y = 0; y < height; y++)
            {
                if ( width != fwrite(pU8, 1, width, pFp))
                {
                    SAMPLE_PRT("Write file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->u16Stride[0];
            }

            pU8 = pstImg->pu8VirAddr[1];
            for (y = 0; y < height; y++)
            {
                if ( width != fwrite(pU8, 1, width, pFp))
                {
                    SAMPLE_PRT("Write file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->u16Stride[1];
            }
        }
        break;
#if 0
        case IVE_IMAGE_TYPE_U8C3_PACKAGE:
        {
            pU8 = image->pu8VirAddr[0];
            for (y = 0; y < height; y++)
            {
                if ( 1 != fread(pU8, width * 3, 1, fp))
                {
                    printf("read file error,line:%d\n", __LINE__);
                    return HI_FAILURE;
                }

                pU8 += image->u16Stride[0] * 3;
            }

        }
        break;
        case IVE_IMAGE_TYPE_U8C3_PLANAR:
        {
            for (HI_U16 loop = 0; loop < 3; loop++)
            {
                pU8 = image->pu8VirAddr[loop];
                for (y = 0; y < height; y++)
                {
                    if ( 1 != fread(pU8, width, 1, fp))
                    {
                        printf("read file error,line:%d\n", __LINE__);
                        return HI_FAILURE;
                    }

                    pU8 += image->u16Stride[loop];
                }
            }

        }
        break;
#endif
        case IVE_IMAGE_TYPE_S16C1:
        case  IVE_IMAGE_TYPE_U16C1:
        {
            pU8 = pstImg->pu8VirAddr[0];
            for ( y = 0; y < height; y++ )
            {
                if ( sizeof(HI_U16) != fwrite(pU8, width, sizeof(HI_U16), pFp) )
                {
                    SAMPLE_PRT("Write file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->u16Stride[0] * 2;
            }
        }
        break;
        case IVE_IMAGE_TYPE_U32C1:
        {

            pU8 = pstImg->pu8VirAddr[0];
            for ( y = 0; y < height; y++ )
            {
                if ( width != fwrite(pU8, sizeof(HI_U32), width, pFp) )
                {
                    SAMPLE_PRT("Write file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->u16Stride[0] * 4;
            }
            break;
        }

        default:
            break;
    }

    return HI_SUCCESS;
}

HI_VOID SAMPLE_COMM_IVE_BlobToRect(IVE_CCBLOB_S *pstBlob, SAMPLE_RECT_ARRAY_S *pstRect,
                                            HI_U16 u16RectMaxNum,HI_U16 u16AreaThrStep,
                                            HI_U16 u16SrcWidth, HI_U16 u16SrcHeight,
                                            HI_U16 u16DstWidth,HI_U16 u16DstHeight)
{
    HI_U16 u16Num;
    HI_U16 i,j,k;
    HI_U16 u16Thr= 0;
	HI_BOOL bValid;
	
    if(pstBlob->u8RegionNum > u16RectMaxNum)
    {
     
		u16Thr = pstBlob->u16CurAreaThr;
		do
		{
			u16Num = 0;
			u16Thr += u16AreaThrStep; 
			for(i = 0;i < 254;i++)
			{
				if(pstBlob->astRegion[i].u32Area > u16Thr)
				{
					u16Num++;
				}
			}	  
		}while(u16Num > u16RectMaxNum);  

    }    

   u16Num = 0;
   
   for(i = 0;i < 254;i++)
    {
        if(pstBlob->astRegion[i].u32Area > u16Thr)
        {

            pstRect->astRect[u16Num].astPoint[0].s32X = (HI_U16)((HI_FLOAT)pstBlob->astRegion[i].u16Left / (HI_FLOAT)u16SrcWidth * (HI_FLOAT)u16DstWidth) & (~1) ;
			pstRect->astRect[u16Num].astPoint[0].s32Y = (HI_U16)((HI_FLOAT)pstBlob->astRegion[i].u16Top / (HI_FLOAT)u16SrcHeight * (HI_FLOAT)u16DstHeight) & (~1);

			pstRect->astRect[u16Num].astPoint[1].s32X = (HI_U16)((HI_FLOAT)pstBlob->astRegion[i].u16Right/ (HI_FLOAT)u16SrcWidth * (HI_FLOAT)u16DstWidth) & (~1);
			pstRect->astRect[u16Num].astPoint[1].s32Y = (HI_U16)((HI_FLOAT)pstBlob->astRegion[i].u16Top / (HI_FLOAT)u16SrcHeight * (HI_FLOAT)u16DstHeight) & (~1);

			pstRect->astRect[u16Num].astPoint[2].s32X = (HI_U16)((HI_FLOAT)pstBlob->astRegion[i].u16Right / (HI_FLOAT)u16SrcWidth * (HI_FLOAT)u16DstWidth) & (~1);
			pstRect->astRect[u16Num].astPoint[2].s32Y = (HI_U16)((HI_FLOAT)pstBlob->astRegion[i].u16Bottom / (HI_FLOAT)u16SrcHeight * (HI_FLOAT)u16DstHeight) & (~1);

			pstRect->astRect[u16Num].astPoint[3].s32X = (HI_U16)((HI_FLOAT)pstBlob->astRegion[i].u16Left / (HI_FLOAT)u16SrcWidth * (HI_FLOAT)u16DstWidth) & (~1);
			pstRect->astRect[u16Num].astPoint[3].s32Y = (HI_U16)((HI_FLOAT)pstBlob->astRegion[i].u16Bottom / (HI_FLOAT)u16SrcHeight * (HI_FLOAT)u16DstHeight) & (~1);

			bValid = HI_TRUE;
			for(j = 0; j < 3;j++)
			{
			  for (k = j + 1; k < 4;k++)
			  {
				  if ((pstRect->astRect[u16Num].astPoint[j].s32X == pstRect->astRect[u16Num].astPoint[k].s32X)
				  	 &&(pstRect->astRect[u16Num].astPoint[j].s32Y == pstRect->astRect[u16Num].astPoint[k].s32Y))
				  	{
				  	    bValid = HI_FALSE;
						break;
				     }

			  }
			}
			if (HI_TRUE == bValid)
			{
				u16Num++;
			}
        }
    }

	pstRect->u16Num = u16Num;
}

/******************************************************************************
* function : Create ive image
******************************************************************************/
HI_S32 SAMPLE_COMM_IVE_CreateImage(IVE_IMAGE_S* pstImg, IVE_IMAGE_TYPE_E enType, HI_U16 u16Width, HI_U16 u16Height)
{
    HI_U32 u32Size = 0;
    HI_S32 s32Ret;
    if (NULL == pstImg)
    {
        SAMPLE_PRT("pstImg is null\n");
        return HI_FAILURE;
    }

    pstImg->enType = enType;
    pstImg->u16Width = u16Width;
    pstImg->u16Height = u16Height;
    pstImg->u16Stride[0] = SAMPLE_COMM_IVE_CalcStride(pstImg->u16Width, IVE_ALIGN);

    switch (enType)
    {
        case IVE_IMAGE_TYPE_U8C1:
        case IVE_IMAGE_TYPE_S8C1:
        {
            u32Size = pstImg->u16Stride[0] * pstImg->u16Height;
            s32Ret = HI_MPI_SYS_MmzAlloc(&pstImg->u32PhyAddr[0], (void**)&pstImg->pu8VirAddr[0], NULL, HI_NULL, u32Size);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("Mmz Alloc fail,Error(%#x)\n", s32Ret);
                return s32Ret;
            }
        }
        break;
        case IVE_IMAGE_TYPE_YUV420SP:
        {
            u32Size = pstImg->u16Stride[0] * pstImg->u16Height * 3 / 2;
            s32Ret = HI_MPI_SYS_MmzAlloc(&pstImg->u32PhyAddr[0], (void**)&pstImg->pu8VirAddr[0], NULL, HI_NULL, u32Size);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("Mmz Alloc fail,Error(%#x)\n", s32Ret);
                return s32Ret;
            }
            pstImg->u16Stride[1] = pstImg->u16Stride[0];
            pstImg->u32PhyAddr[1] = pstImg->u32PhyAddr[0] + pstImg->u16Stride[0] * pstImg->u16Height;
            pstImg->pu8VirAddr[1] = pstImg->pu8VirAddr[0] + pstImg->u16Stride[0] * pstImg->u16Height;

        }
        break;
        case IVE_IMAGE_TYPE_YUV422SP:
        {
            u32Size = pstImg->u16Stride[0] * pstImg->u16Height * 2;
            s32Ret = HI_MPI_SYS_MmzAlloc(&pstImg->u32PhyAddr[0], (void**)&pstImg->pu8VirAddr[0], NULL, HI_NULL, u32Size);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("Mmz Alloc fail,Error(%#x)\n", s32Ret);
                return s32Ret;
            }
            pstImg->u16Stride[1] = pstImg->u16Stride[0];
            pstImg->u32PhyAddr[1] = pstImg->u32PhyAddr[0] + pstImg->u16Stride[0] * pstImg->u16Height;
            pstImg->pu8VirAddr[1] = pstImg->pu8VirAddr[0] + pstImg->u16Stride[0] * pstImg->u16Height;

        }
        break;
        case IVE_IMAGE_TYPE_YUV420P:
            break;
        case IVE_IMAGE_TYPE_YUV422P:
            break;
        case IVE_IMAGE_TYPE_S8C2_PACKAGE:
            break;
        case IVE_IMAGE_TYPE_S8C2_PLANAR:
            break;
        case IVE_IMAGE_TYPE_S16C1:
        case IVE_IMAGE_TYPE_U16C1:
        {

            u32Size = pstImg->u16Stride[0] * pstImg->u16Height * sizeof(HI_U16);
            s32Ret = HI_MPI_SYS_MmzAlloc(&pstImg->u32PhyAddr[0], (void**)&pstImg->pu8VirAddr[0], NULL, HI_NULL, u32Size);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("Mmz Alloc fail,Error(%#x)\n", s32Ret);
                return s32Ret;
            }
        }
        break;
        case IVE_IMAGE_TYPE_U8C3_PACKAGE:
        {
            u32Size = pstImg->u16Stride[0] * pstImg->u16Height * 3;
            s32Ret = HI_MPI_SYS_MmzAlloc(&pstImg->u32PhyAddr[0], (void**)&pstImg->pu8VirAddr[0], NULL, HI_NULL, u32Size);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("Mmz Alloc fail,Error(%#x)\n", s32Ret);
                return s32Ret;
            }
            pstImg->pu8VirAddr[1] = pstImg->pu8VirAddr[0] + 1;
            pstImg->pu8VirAddr[2] = pstImg->pu8VirAddr[1] + 1;
            pstImg->u32PhyAddr[1] = pstImg->u32PhyAddr[0] + 1;
            pstImg->u32PhyAddr[2] = pstImg->u32PhyAddr[1] + 1;
            pstImg->u16Stride[1] = pstImg->u16Stride[0];
            pstImg->u16Stride[2] = pstImg->u16Stride[0];
        }
        break;
        case IVE_IMAGE_TYPE_U8C3_PLANAR:
            break;
        case IVE_IMAGE_TYPE_S32C1:
        case IVE_IMAGE_TYPE_U32C1:
        {
            u32Size = pstImg->u16Stride[0] * pstImg->u16Height * sizeof(HI_U32);
            s32Ret = HI_MPI_SYS_MmzAlloc(&pstImg->u32PhyAddr[0], (void**)&pstImg->pu8VirAddr[0], NULL, HI_NULL, u32Size);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("Mmz Alloc fail,Error(%#x)\n", s32Ret);
                return s32Ret;
            }
        }
        break;
        case IVE_IMAGE_TYPE_S64C1:
        case IVE_IMAGE_TYPE_U64C1:
        {

            u32Size = pstImg->u16Stride[0] * pstImg->u16Height * sizeof(HI_U64);
            s32Ret = HI_MPI_SYS_MmzAlloc(&pstImg->u32PhyAddr[0], (void**)&pstImg->pu8VirAddr[0], NULL, HI_NULL, u32Size);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("Mmz Alloc fail,Error(%#x)\n", s32Ret);
                return s32Ret;
            }
        }
        break;
        default:
            break;

    }

    return HI_SUCCESS;
}
/******************************************************************************
* function : Create memory info
******************************************************************************/
HI_S32 SAMPLE_COMM_IVE_CreateMemInfo(IVE_MEM_INFO_S* pstMemInfo, HI_U32 u32Size)
{
    HI_S32 s32Ret;

    if (NULL == pstMemInfo)
    {
        SAMPLE_PRT("pstMemInfo is null\n");
        return HI_FAILURE;
    }
    pstMemInfo->u32Size = u32Size;
    s32Ret = HI_MPI_SYS_MmzAlloc(&pstMemInfo->u32PhyAddr, (void**)&pstMemInfo->pu8VirAddr, NULL, HI_NULL, u32Size);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Mmz Alloc fail,Error(%#x)\n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
/******************************************************************************
* function : Create ive image by cached
******************************************************************************/
HI_S32 SAMPLE_COMM_IVE_CreateImageByCached(IVE_IMAGE_S* pstImg,
        IVE_IMAGE_TYPE_E enType, HI_U16 u16Width, HI_U16 u16Height)
{
    HI_U32 u32Size = 0;
    HI_S32 s32Ret;
    if (NULL == pstImg)
    {
        SAMPLE_PRT("pstImg is null\n");
        return HI_FAILURE;
    }

    pstImg->enType = enType;
    pstImg->u16Width = u16Width;
    pstImg->u16Height = u16Height;
    pstImg->u16Stride[0] = SAMPLE_COMM_IVE_CalcStride(pstImg->u16Width, IVE_ALIGN);

    switch (enType)
    {
        case IVE_IMAGE_TYPE_U8C1:
        case IVE_IMAGE_TYPE_S8C1:
        {
            u32Size = pstImg->u16Stride[0] * pstImg->u16Height;
            s32Ret = HI_MPI_SYS_MmzAlloc_Cached(&pstImg->u32PhyAddr[0], (void**)&pstImg->pu8VirAddr[0], NULL, HI_NULL, u32Size);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("Mmz Alloc fail,Error(%#x)\n", s32Ret);
                return s32Ret;
            }
        }
        break;
        case IVE_IMAGE_TYPE_YUV420SP:
            break;
        case IVE_IMAGE_TYPE_YUV422SP:
            break;
        case IVE_IMAGE_TYPE_YUV420P:
            break;
        case IVE_IMAGE_TYPE_YUV422P:
            break;
        case IVE_IMAGE_TYPE_S8C2_PACKAGE:
            break;
        case IVE_IMAGE_TYPE_S8C2_PLANAR:
            break;
        case IVE_IMAGE_TYPE_S16C1:
        case IVE_IMAGE_TYPE_U16C1:
        {

            u32Size = pstImg->u16Stride[0] * pstImg->u16Height * sizeof(HI_U16);
            s32Ret = HI_MPI_SYS_MmzAlloc_Cached(&pstImg->u32PhyAddr[0], (void**)&pstImg->pu8VirAddr[0], NULL, HI_NULL, u32Size);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("Mmz Alloc fail,Error(%#x)\n", s32Ret);
                return s32Ret;
            }
        }
        break;
        case IVE_IMAGE_TYPE_U8C3_PACKAGE:
            break;
        case IVE_IMAGE_TYPE_U8C3_PLANAR:
            break;
        case IVE_IMAGE_TYPE_S32C1:
        case IVE_IMAGE_TYPE_U32C1:
        {
            u32Size = pstImg->u16Stride[0] * pstImg->u16Height * sizeof(HI_U32);
            s32Ret = HI_MPI_SYS_MmzAlloc_Cached(&pstImg->u32PhyAddr[0], (void**)&pstImg->pu8VirAddr[0], NULL, HI_NULL, u32Size);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("Mmz Alloc fail,Error(%#x)\n", s32Ret);
                return s32Ret;
            }
        }
        break;
        case IVE_IMAGE_TYPE_S64C1:
        case IVE_IMAGE_TYPE_U64C1:
        {

            u32Size = pstImg->u16Stride[0] * pstImg->u16Height * sizeof(HI_U64);
            s32Ret = HI_MPI_SYS_MmzAlloc_Cached(&pstImg->u32PhyAddr[0], (void**)&pstImg->pu8VirAddr[0], NULL, HI_NULL, u32Size);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("Mmz Alloc fail,Error(%#x)\n", s32Ret);
                return s32Ret;
            }
        }
        break;
        default:
            break;

    }

    return HI_SUCCESS;

}
HI_S32 SAMPLE_COMM_IVE_CreateData(IVE_DATA_S* pstData,HI_U16 u16Width, HI_U16 u16Height)
{
	HI_S32 s32Ret;
	HI_U32 u32Size;
	
	if (NULL == pstData)
	{
		SAMPLE_PRT("pstData is null\n");
		return HI_FAILURE;
	}
	pstData->u16Width = u16Width;
	pstData->u16Height = u16Height;
	pstData->u16Stride = SAMPLE_COMM_IVE_CalcStride(pstData->u16Width, IVE_ALIGN);
	u32Size = pstData->u16Stride * pstData->u16Height;
	s32Ret = HI_MPI_SYS_MmzAlloc(&pstData->u32PhyAddr, (void**)&pstData->pu8VirAddr, NULL, HI_NULL, u32Size);
	if (s32Ret != HI_SUCCESS)
	{
		SAMPLE_PRT("Mmz Alloc fail,Error(%#x)\n", s32Ret);
		return HI_FAILURE;
	}

	return HI_SUCCESS;
}
/******************************************************************************
* function : Init Vb
******************************************************************************/
HI_S32 SAMPLE_COMM_IVE_VbInit(PIC_SIZE_E *paenSize,SIZE_S *pastSize,HI_U32 u32VpssChnNum)
{
	HI_S32 s32Ret;	
	HI_U32 i;
	HI_U32 u32BlkSize;
	VIDEO_NORM_E enNorm = VIDEO_ENCODING_MODE_PAL;
	VB_CONF_S stVbConf;

	memset(&stVbConf, 0, sizeof(VB_CONF_S));
	stVbConf.u32MaxPoolCnt = 128;

	for (i = 0; i < u32VpssChnNum; i++)
	{
		s32Ret = SAMPLE_COMM_SYS_GetPicSize(enNorm, paenSize[i], &pastSize[i]);		
		SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, VB_FAIL_0, 
			"SAMPLE_COMM_SYS_GetPicSize failed,Error(%#x)!\n",s32Ret);
		
		u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize2(&pastSize[i], 
						SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);		
		/* comm video buffer */
		stVbConf.astCommPool[i].u32BlkSize = u32BlkSize;
		stVbConf.astCommPool[i].u32BlkCnt  = 10;
	}  

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);	
	SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, VB_FAIL_1, 
		"SAMPLE_COMM_SYS_Init failed,Error(%#x)!\n", s32Ret);	

	return s32Ret;
VB_FAIL_1:
	SAMPLE_COMM_SYS_Exit();
VB_FAIL_0:
	
	return s32Ret;			

}

/******************************************************************************
* function : Start Vpss
******************************************************************************/
HI_S32 SAMPLE_COMM_IVE_StartVpss(SIZE_S *pastSize,HI_U32 u32VpssChnNum)
{
	HI_S32 s32Ret;
	HI_U32 i;
	HI_U32 u32Depth = 3;
	VPSS_GRP VpssGrp = 0;
    VPSS_CHN as32VpssChn[] = {VPSS_CHN0, VPSS_CHN3};
    VPSS_GRP_ATTR_S stVpssGrpAttr;
    //VPSS_CHN_ATTR_S stVpssChnAttr;
    VPSS_CHN_MODE_S stVpssChnMode;
	
	VpssGrp = 0;
	stVpssGrpAttr.u32MaxW	= pastSize[0].u32Width;
	stVpssGrpAttr.u32MaxH	= pastSize[0].u32Height;
	stVpssGrpAttr.bIeEn 	= HI_FALSE;
	stVpssGrpAttr.bNrEn 	= HI_FALSE;
	//stVpssGrpAttr.stNrAttr.u32RefFrameNum = 1;
	stVpssGrpAttr.bHistEn	= HI_FALSE;
	stVpssGrpAttr.bDciEn	= HI_FALSE;
	stVpssGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
	stVpssGrpAttr.enPixFmt	= SAMPLE_PIXEL_FORMAT;	
    stVpssGrpAttr.bStitchBlendEn   = HI_FALSE;
	s32Ret = SAMPLE_COMM_VPSS_StartGroup(VpssGrp, &stVpssGrpAttr);	
	SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, VPSS_FAIL_0, 
		"SAMPLE_COMM_VPSS_StartGroup failed,Error(%#x)!\n",s32Ret);	

	for(i = 0; i < u32VpssChnNum; i++)
	{
		stVpssChnMode.enChnMode = VPSS_CHN_MODE_USER;
		stVpssChnMode.u32Width	= pastSize[i].u32Width;
		stVpssChnMode.u32Height = pastSize[i].u32Height;
		stVpssChnMode.enPixelFormat = SAMPLE_PIXEL_FORMAT;
		stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;
		
		s32Ret = HI_MPI_VPSS_SetChnMode(VpssGrp, as32VpssChn[i], &stVpssChnMode);
		SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, VPSS_FAIL_1, 
			"HI_MPI_VPSS_SetChnMode failed, Error(%#x),VpssGrp(%#x),VpssChn(%#x)!\n",
			s32Ret,VpssGrp,as32VpssChn[i]); 

		s32Ret = HI_MPI_VPSS_EnableChn(VpssGrp, as32VpssChn[i]);
		SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, VPSS_FAIL_1, 
			"HI_MPI_VPSS_EnableChn failed, Error(%#x),VpssGrp(%#x),VpssChn(%#x)!\n", 
			s32Ret,VpssGrp,as32VpssChn[i]); 


		s32Ret = HI_MPI_VPSS_SetDepth(VpssGrp, as32VpssChn[i], u32Depth);
		SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, VPSS_FAIL_1, 
			"HI_MPI_VPSS_SetDepth failed, Error(%#x),VpssGrp(%#x),VpssChn(%#x)!\n",
			s32Ret,VpssGrp,as32VpssChn[i]); 
		
	}

	return s32Ret;
	
VPSS_FAIL_1:
	for(i = 0; i < u32VpssChnNum; i++)
    {
        s32Ret = HI_MPI_VPSS_DisableChn(VpssGrp, as32VpssChn[i]);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI_MPI_VPSS_DisableChn failed,Error(%#x)!\n", s32Ret);            
        }
    }
	(HI_VOID)SAMPLE_COMM_VPSS_StopGroup(VpssGrp);
VPSS_FAIL_0:
	
	return s32Ret;	

}
/******************************************************************************
* function : Stop Vpss
******************************************************************************/
HI_VOID SAMPLE_COMM_IVE_StopVpss(HI_U32 u32VpssChnNum)
{
	VPSS_GRP VpssGrp = 0;
    VPSS_CHN as32VpssChn[] = {VPSS_CHN0, VPSS_CHN3};
	HI_U32 i;
	for (i = 0 ; i < u32VpssChnNum; i++)
	{
		(HI_VOID)SAMPLE_COMM_VPSS_DisableChn(VpssGrp, as32VpssChn[i]);
	}
    (HI_VOID)SAMPLE_COMM_VPSS_StopGroup(VpssGrp);
}

/******************************************************************************
* function : Start Vo
******************************************************************************/
HI_S32 SAMPLE_COMM_IVE_StartVo(HI_VOID)
{
	HI_S32 s32Ret;
	VO_DEV VoDev = SAMPLE_VO_DEV_DSD0;	
	VO_LAYER VoLayer = 0;
	VO_PUB_ATTR_S stVoPubAttr;
	VO_VIDEO_LAYER_ATTR_S stLayerAttr;
	SAMPLE_VO_MODE_E enVoMode = VO_MODE_1MUX;

	stVoPubAttr.enIntfSync = VO_OUTPUT_1080P30;//VO_OUTPUT_PAL;//VO_OUTPUT_1080P30;
	stVoPubAttr.enIntfType = VO_INTF_BT1120;//VO_INTF_CVBS; //VO_INTF_HDMI | VO_INTF_VGA;
	stVoPubAttr.u32BgColor = 0x0000FF;
    s32Ret = SAMPLE_COMM_VO_StartDev(VoDev, &stVoPubAttr);	
	SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, VO_FAIL_0, 
		"SAMPLE_COMM_VO_StartDev failed,Error(%#x)!\n",s32Ret);	 
	
	stLayerAttr.bClusterMode = HI_FALSE;
    stLayerAttr.bDoubleFrame = HI_FALSE;
    stLayerAttr.enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    stLayerAttr.stDispRect.s32X = 0;
    stLayerAttr.stDispRect.s32Y = 0;
    s32Ret = SAMPLE_COMM_VO_GetWH(stVoPubAttr.enIntfSync,
                                  &stLayerAttr.stDispRect.u32Width, &stLayerAttr.stDispRect.u32Height,
                                  &stLayerAttr.u32DispFrmRt);
	SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, VO_FAIL_1, 
		"SAMPLE_COMM_VO_GetWH failed,Error(%#x)!\n",s32Ret);	 

    stLayerAttr.stImageSize.u32Width  = stLayerAttr.stDispRect.u32Width;
    stLayerAttr.stImageSize.u32Height = stLayerAttr.stDispRect.u32Height;

    s32Ret = SAMPLE_COMM_VO_StartLayer(VoLayer, &stLayerAttr,HI_FALSE /*HI_TRUE*/);
	SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, VO_FAIL_1, 
		"SAMPLE_COMM_VO_StartLayer failed,Error(%#x)!\n",s32Ret);	 

    s32Ret = SAMPLE_COMM_VO_StartChn(VoLayer, enVoMode);	
	SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, VO_FAIL_2, 
		"SAMPLE_COMM_VO_StartChn failed,Error(%#x)!\n",s32Ret);	 
	
	return s32Ret;
VO_FAIL_2:
	 SAMPLE_COMM_VO_StopLayer(VoLayer);
VO_FAIL_1:	
	 SAMPLE_COMM_VO_StopDev(VoDev);
VO_FAIL_0:
	return s32Ret;
}
/******************************************************************************
* function : Stop Vo
******************************************************************************/
HI_VOID SAMPLE_COMM_IVE_StopVo(HI_VOID)
{
    VO_DEV VoDev = SAMPLE_VO_DEV_DSD0;
    //VO_CHN VoChn = 0;
    VO_LAYER VoLayer = 0;
    SAMPLE_VO_MODE_E enVoMode = VO_MODE_1MUX;
    
    (HI_VOID)SAMPLE_COMM_VO_StopChn(VoDev, enVoMode);
    (HI_VOID)SAMPLE_COMM_VO_StopLayer(VoLayer);
    (HI_VOID)SAMPLE_COMM_VO_StopDev(VoDev);
}
/******************************************************************************
* function : Dma frame info to  ive image
******************************************************************************/
HI_S32 SAMPLE_COMM_IVE_DmaImage(VIDEO_FRAME_INFO_S *pstFrameInfo,IVE_DST_IMAGE_S *pstDst,HI_BOOL bInstant)
{
	HI_S32 s32Ret;
	IVE_HANDLE hIveHandle;
	IVE_SRC_DATA_S stSrcData;
	IVE_DST_DATA_S stDstData;
	IVE_DMA_CTRL_S stCtrl = {IVE_DMA_MODE_DIRECT_COPY,0};
	HI_BOOL bFinish = HI_FALSE;
	HI_BOOL bBlock = HI_TRUE;

	//fill src
	stSrcData.pu8VirAddr = (HI_U8*)pstFrameInfo->stVFrame.pVirAddr[0];
	stSrcData.u32PhyAddr = pstFrameInfo->stVFrame.u32PhyAddr[0];
	stSrcData.u16Width   = (HI_U16)pstFrameInfo->stVFrame.u32Width;
	stSrcData.u16Height  = (HI_U16)pstFrameInfo->stVFrame.u32Height;
	stSrcData.u16Stride  = (HI_U16)pstFrameInfo->stVFrame.u32Stride[0];

	//fill dst
	stDstData.pu8VirAddr = pstDst->pu8VirAddr[0];
	stDstData.u32PhyAddr = pstDst->u32PhyAddr[0];
	stDstData.u16Width   = pstDst->u16Width;
	stDstData.u16Height  = pstDst->u16Height;
	stDstData.u16Stride  = pstDst->u16Stride[0];

	s32Ret = HI_MPI_IVE_DMA(&hIveHandle,&stSrcData,&stDstData,&stCtrl,bInstant);	
	if (HI_SUCCESS != s32Ret)
	{
        SAMPLE_PRT("HI_MPI_IVE_DMA fail,Error(%#x)\n",s32Ret);
       return s32Ret;
    }
	
	if (HI_TRUE == bInstant)
	{
		s32Ret = HI_MPI_IVE_Query(hIveHandle,&bFinish,bBlock);			
		while(HI_ERR_IVE_QUERY_TIMEOUT == s32Ret)
		{
			usleep(100);					
			s32Ret = HI_MPI_IVE_Query(hIveHandle,&bFinish,bBlock);	
		}
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("HI_MPI_IVE_Query fail,Error(%#x)\n",s32Ret);
		   return s32Ret;
		}
	}

	return HI_SUCCESS;
}


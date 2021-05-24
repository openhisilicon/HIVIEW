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
#include "hi_comm_vgs.h"
#include "hi_comm_vi.h"
#include "hi_comm_vo.h"
#include "mpi_vb.h"
#include "mpi_sys.h"
#include "mpi_vi.h"
#include "mpi_vo.h"
#include "mpi_vgs.h"

#include "sample_comm_ive.h"

static HI_BOOL bMpiInit = HI_FALSE;

HI_U16 SAMPLE_COMM_IVE_CalcStride(HI_U32 u32Width, HI_U8 u8Align)
{
    return (u32Width + (u8Align - u32Width % u8Align) % u8Align);
}

static HI_S32 SAMPLE_IVE_MPI_Init(HI_VOID)
{
    HI_S32 s32Ret;

    HI_MPI_SYS_Exit();

    s32Ret = HI_MPI_SYS_Init();
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_SYS_Init fail,Error(%#x)\n", s32Ret);
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

    return HI_SUCCESS;
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
    HI_S32 s32Ret;

    (HI_VOID)fgetc(pFp);
    if (feof(pFp))
    {
        SAMPLE_PRT("end of file!\n");
        s32Ret = fseek(pFp, 0 , SEEK_SET );
        if (0 != s32Ret)
        {
            SAMPLE_PRT("fseek failed!\n");
            return s32Ret;
        }

    }
    else
    {
        s32Ret = fseek(pFp, -1 , SEEK_CUR );
        if (0 != s32Ret)
        {
            SAMPLE_PRT("fseek failed!\n");
            return s32Ret;
        }
    }

    //if (feof(pFp))
    //{
    //    SAMPLE_PRT("end of file!\n");
    //    fseek(pFp, 0 , SEEK_SET);
    //}

    height = pstImg->u32Height;
    width = pstImg->u32Width;

    switch (pstImg->enType)
    {
        case  IVE_IMAGE_TYPE_U8C1:
        {
            pU8 = (HI_U8 *)(HI_UL)pstImg->au64VirAddr[0];
            for (y = 0; y < height; y++)
            {
                if ( 1 != fread(pU8, width, 1, pFp))
                {
                    SAMPLE_PRT("Read file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->au32Stride[0];
            }
        }
        break;
        case  IVE_IMAGE_TYPE_YUV420SP:
        {
            pU8 = (HI_U8 *)(HI_UL)pstImg->au64VirAddr[0];
            for (y = 0; y < height; y++)
            {
                if ( 1 != fread(pU8, width, 1, pFp))
                {
                    SAMPLE_PRT("Read file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->au32Stride[0];
            }

            pU8 = (HI_U8 *)(HI_UL)pstImg->au64VirAddr[1];
            for (y = 0; y < height / 2; y++)
            {
                if ( 1 != fread(pU8, width, 1, pFp))
                {
                    SAMPLE_PRT("Read file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->au32Stride[1];
            }
        }
        break;
        case IVE_IMAGE_TYPE_YUV422SP:
        {
            pU8 = (HI_U8 *)(HI_UL)pstImg->au64VirAddr[0];
            for (y = 0; y < height; y++)
            {
                if ( 1 != fread(pU8, width, 1, pFp))
                {
                    SAMPLE_PRT("Read file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->au32Stride[0];
            }

            pU8 = (HI_U8 *)(HI_UL)pstImg->au64VirAddr[1];
            for (y = 0; y < height; y++)
            {
                if ( 1 != fread(pU8, width, 1, pFp))
                {
                    SAMPLE_PRT("Read file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->au32Stride[1];
            }
        }
        break;
        case IVE_IMAGE_TYPE_U8C3_PACKAGE:
        {
            pU8 = (HI_U8 *)(HI_UL)pstImg->au64VirAddr[0];
            for (y = 0; y < height; y++)
            {
                if ( 1 != fread(pU8, width * 3, 1, pFp))
                {
                    SAMPLE_PRT("Read file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->au32Stride[0] * 3;
            }

        }
        break;
        case IVE_IMAGE_TYPE_U8C3_PLANAR:
        {
            for (loop = 0; loop < 3; loop++)
            {
                pU8 = (HI_U8 *)(HI_UL)pstImg->au64VirAddr[loop];
                for (y = 0; y < height; y++)
                {
                    if ( 1 != fread(pU8, width, 1, pFp))
                    {
                        SAMPLE_PRT("Read file fail\n");
                        return HI_FAILURE;
                    }

                    pU8 += pstImg->au32Stride[loop];
                }
            }

        }
        break;
        case IVE_IMAGE_TYPE_S16C1:
        case IVE_IMAGE_TYPE_U16C1:
        {
            pU8 = (HI_U8 *)(HI_UL)pstImg->au64VirAddr[0];
            for ( y = 0; y < height; y++ )
            {
                if ( sizeof(HI_U16) != fread(pU8, width, sizeof(HI_U16), pFp) )
                {
                    SAMPLE_PRT("Read file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->au32Stride[0] * 2;
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

    height = pstImg->u32Height;
    width = pstImg->u32Width;

    switch (pstImg->enType)
    {
        case  IVE_IMAGE_TYPE_U8C1:
        case  IVE_IMAGE_TYPE_S8C1:
        {
            pU8 = (HI_U8 *)(HI_UL)pstImg->au64VirAddr[0];
            for (y = 0; y < height; y++)
            {
                if ( 1 != fwrite(pU8, width, 1, pFp))
                {
                    SAMPLE_PRT("Write file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->au32Stride[0];
            }
        }
        break;
        case  IVE_IMAGE_TYPE_YUV420SP:
        {
            pU8 = (HI_U8 *)(HI_UL)pstImg->au64VirAddr[0];
            for (y = 0; y < height; y++)
            {
                if ( width != fwrite(pU8, 1, width, pFp))
                {
                    SAMPLE_PRT("Write file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->au32Stride[0];
            }

            pU8 = (HI_U8 *)(HI_UL)pstImg->au64VirAddr[1];
            for (y = 0; y < height / 2; y++)
            {
                if ( width != fwrite(pU8, 1, width, pFp))
                {
                    SAMPLE_PRT("Write file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->au32Stride[1];
            }
        }
        break;
        case IVE_IMAGE_TYPE_YUV422SP:
        {
            pU8 = (HI_U8 *)(HI_UL)pstImg->au64VirAddr[0];
            for (y = 0; y < height; y++)
            {
                if ( width != fwrite(pU8, 1, width, pFp))
                {
                    SAMPLE_PRT("Write file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->au32Stride[0];
            }

            pU8 = (HI_U8 *)(HI_UL)pstImg->au64VirAddr[1];
            for (y = 0; y < height; y++)
            {
                if ( width != fwrite(pU8, 1, width, pFp))
                {
                    SAMPLE_PRT("Write file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->au32Stride[1];
            }
        }
        break;
        case IVE_IMAGE_TYPE_S16C1:
        case  IVE_IMAGE_TYPE_U16C1:
        {
            pU8 = (HI_U8 *)(HI_UL)pstImg->au64VirAddr[0];
            for ( y = 0; y < height; y++ )
            {
                if ( sizeof(HI_U16) != fwrite(pU8, width, sizeof(HI_U16), pFp) )
                {
                    SAMPLE_PRT("Write file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->au32Stride[0] * 2;
            }
        }
        break;
        case IVE_IMAGE_TYPE_U32C1:
        {

            pU8 = (HI_U8 *)(HI_UL)pstImg->au64VirAddr[0];
            for ( y = 0; y < height; y++ )
            {
                if ( width != fwrite(pU8, sizeof(HI_U32), width, pFp) )
                {
                    SAMPLE_PRT("Write file fail\n");
                    return HI_FAILURE;
                }

                pU8 += pstImg->au32Stride[0] * 4;
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
                                            HI_U32 u32SrcWidth, HI_U32 u32SrcHeight,
                                            HI_U32 u32DstWidth,HI_U32 u32DstHeight)
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
            pstRect->astRect[u16Num].astPoint[0].s32X = (HI_U32)((HI_FLOAT)pstBlob->astRegion[i].u16Left / (HI_FLOAT)u32SrcWidth * (HI_FLOAT)u32DstWidth) & (~1) ;
            pstRect->astRect[u16Num].astPoint[0].s32Y = (HI_U32)((HI_FLOAT)pstBlob->astRegion[i].u16Top / (HI_FLOAT)u32SrcHeight * (HI_FLOAT)u32DstHeight) & (~1);

            pstRect->astRect[u16Num].astPoint[1].s32X = (HI_U32)((HI_FLOAT)pstBlob->astRegion[i].u16Right/ (HI_FLOAT)u32SrcWidth * (HI_FLOAT)u32DstWidth) & (~1);
            pstRect->astRect[u16Num].astPoint[1].s32Y = (HI_U32)((HI_FLOAT)pstBlob->astRegion[i].u16Top / (HI_FLOAT)u32SrcHeight * (HI_FLOAT)u32DstHeight) & (~1);

            pstRect->astRect[u16Num].astPoint[2].s32X = (HI_U32)((HI_FLOAT)pstBlob->astRegion[i].u16Right / (HI_FLOAT)u32SrcWidth * (HI_FLOAT)u32DstWidth) & (~1);
            pstRect->astRect[u16Num].astPoint[2].s32Y = (HI_U32)((HI_FLOAT)pstBlob->astRegion[i].u16Bottom / (HI_FLOAT)u32SrcHeight * (HI_FLOAT)u32DstHeight) & (~1);

            pstRect->astRect[u16Num].astPoint[3].s32X = (HI_U32)((HI_FLOAT)pstBlob->astRegion[i].u16Left / (HI_FLOAT)u32SrcWidth * (HI_FLOAT)u32DstWidth) & (~1);
            pstRect->astRect[u16Num].astPoint[3].s32Y = (HI_U32)((HI_FLOAT)pstBlob->astRegion[i].u16Bottom / (HI_FLOAT)u32SrcHeight * (HI_FLOAT)u32DstHeight) & (~1);

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
HI_S32 SAMPLE_COMM_IVE_CreateImage(IVE_IMAGE_S* pstImg, IVE_IMAGE_TYPE_E enType, HI_U32 u32Width, HI_U32 u32Height)
{
    HI_U32 u32Size = 0;
    HI_S32 s32Ret;
    if (NULL == pstImg)
    {
        SAMPLE_PRT("pstImg is null\n");
        return HI_FAILURE;
    }

    pstImg->enType = enType;
    pstImg->u32Width = u32Width;
    pstImg->u32Height = u32Height;
    pstImg->au32Stride[0] = SAMPLE_COMM_IVE_CalcStride(pstImg->u32Width, IVE_ALIGN);

    switch (enType)
    {
        case IVE_IMAGE_TYPE_U8C1:
        case IVE_IMAGE_TYPE_S8C1:
        {
            u32Size = pstImg->au32Stride[0] * pstImg->u32Height;
            s32Ret = HI_MPI_SYS_MmzAlloc(&pstImg->au64PhyAddr[0], (HI_VOID**)&pstImg->au64VirAddr[0], NULL, HI_NULL, u32Size);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("Mmz Alloc fail,Error(%#x)\n", s32Ret);
                return s32Ret;
            }
        }
        break;
        case IVE_IMAGE_TYPE_YUV420SP:
        {
            u32Size = pstImg->au32Stride[0] * pstImg->u32Height * 3 / 2;
            s32Ret = HI_MPI_SYS_MmzAlloc(&pstImg->au64PhyAddr[0], (HI_VOID**)&pstImg->au64VirAddr[0], NULL, HI_NULL, u32Size);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("Mmz Alloc fail,Error(%#x)\n", s32Ret);
                return s32Ret;
            }
            pstImg->au32Stride[1] = pstImg->au32Stride[0];
            pstImg->au64PhyAddr[1] = pstImg->au64PhyAddr[0] + pstImg->au32Stride[0] * pstImg->u32Height;
            pstImg->au64VirAddr[1] = pstImg->au64VirAddr[0] + pstImg->au32Stride[0] * pstImg->u32Height;

        }
        break;
        case IVE_IMAGE_TYPE_YUV422SP:
        {
            u32Size = pstImg->au32Stride[0] * pstImg->u32Height * 2;
            s32Ret = HI_MPI_SYS_MmzAlloc(&pstImg->au64PhyAddr[0], (HI_VOID**)&pstImg->au64VirAddr[0], NULL, HI_NULL, u32Size);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("Mmz Alloc fail,Error(%#x)\n", s32Ret);
                return s32Ret;
            }
            pstImg->au32Stride[1] = pstImg->au32Stride[0];
            pstImg->au64PhyAddr[1] = pstImg->au64PhyAddr[0] + pstImg->au32Stride[0] * pstImg->u32Height;
            pstImg->au64VirAddr[1] = pstImg->au64VirAddr[0] + pstImg->au32Stride[0] * pstImg->u32Height;

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

            u32Size = pstImg->au32Stride[0] * pstImg->u32Height * sizeof(HI_U16);
            s32Ret = HI_MPI_SYS_MmzAlloc(&pstImg->au64PhyAddr[0], (HI_VOID**)&pstImg->au64VirAddr[0], NULL, HI_NULL, u32Size);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("Mmz Alloc fail,Error(%#x)\n", s32Ret);
                return s32Ret;
            }
        }
        break;
        case IVE_IMAGE_TYPE_U8C3_PACKAGE:
        {
            u32Size = pstImg->au32Stride[0] * pstImg->u32Height * 3;
            s32Ret = HI_MPI_SYS_MmzAlloc(&pstImg->au64PhyAddr[0], (HI_VOID**)&pstImg->au64VirAddr[0], NULL, HI_NULL, u32Size);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("Mmz Alloc fail,Error(%#x)\n", s32Ret);
                return s32Ret;
            }
            pstImg->au64VirAddr[1] = pstImg->au64VirAddr[0] + 1;
            pstImg->au64VirAddr[2] = pstImg->au64VirAddr[1] + 1;
            pstImg->au64PhyAddr[1] = pstImg->au64PhyAddr[0] + 1;
            pstImg->au64PhyAddr[2] = pstImg->au64PhyAddr[1] + 1;
            pstImg->au32Stride[1] = pstImg->au32Stride[0];
            pstImg->au32Stride[2] = pstImg->au32Stride[0];
        }
        break;
        case IVE_IMAGE_TYPE_U8C3_PLANAR:
            break;
        case IVE_IMAGE_TYPE_S32C1:
        case IVE_IMAGE_TYPE_U32C1:
        {
            u32Size = pstImg->au32Stride[0] * pstImg->u32Height * sizeof(HI_U32);
            s32Ret = HI_MPI_SYS_MmzAlloc(&pstImg->au64PhyAddr[0], (HI_VOID**)&pstImg->au64VirAddr[0], NULL, HI_NULL, u32Size);
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

            u32Size = pstImg->au32Stride[0] * pstImg->u32Height * sizeof(HI_U64);
            s32Ret = HI_MPI_SYS_MmzAlloc(&pstImg->au64PhyAddr[0], (HI_VOID**)&pstImg->au64VirAddr[0], NULL, HI_NULL, u32Size);
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
    s32Ret = HI_MPI_SYS_MmzAlloc(&pstMemInfo->u64PhyAddr, (HI_VOID**)&pstMemInfo->u64VirAddr, NULL, HI_NULL, u32Size);
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
        IVE_IMAGE_TYPE_E enType, HI_U32 u32Width, HI_U32 u32Height)
{
    HI_U32 u32Size = 0;
    HI_S32 s32Ret;
    if (NULL == pstImg)
    {
        SAMPLE_PRT("pstImg is null\n");
        return HI_FAILURE;
    }

    pstImg->enType = enType;
    pstImg->u32Width = u32Width;
    pstImg->u32Height = u32Height;
    pstImg->au32Stride[0] = SAMPLE_COMM_IVE_CalcStride(pstImg->u32Width, IVE_ALIGN);

    switch (enType)
    {
        case IVE_IMAGE_TYPE_U8C1:
        case IVE_IMAGE_TYPE_S8C1:
        {
            u32Size = pstImg->au32Stride[0] * pstImg->u32Height;
            s32Ret = HI_MPI_SYS_MmzAlloc_Cached(&pstImg->au64PhyAddr[0], (HI_VOID**)&pstImg->au64VirAddr[0], NULL, HI_NULL, u32Size);
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

            u32Size = pstImg->au32Stride[0] * pstImg->u32Height * sizeof(HI_U16);
            s32Ret = HI_MPI_SYS_MmzAlloc_Cached(&pstImg->au64PhyAddr[0], (HI_VOID**)&pstImg->au64VirAddr[0], NULL, HI_NULL, u32Size);
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
            u32Size = pstImg->au32Stride[0] * pstImg->u32Height * sizeof(HI_U32);
            s32Ret = HI_MPI_SYS_MmzAlloc_Cached(&pstImg->au64PhyAddr[0], (HI_VOID**)&pstImg->au64VirAddr[0], NULL, HI_NULL, u32Size);
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

            u32Size = pstImg->au32Stride[0] * pstImg->u32Height * sizeof(HI_U64);
            s32Ret = HI_MPI_SYS_MmzAlloc_Cached(&pstImg->au64PhyAddr[0], (HI_VOID**)&pstImg->au64VirAddr[0], NULL, HI_NULL, u32Size);
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

HI_S32 SAMPLE_COMM_IVE_CreateData(IVE_DATA_S* pstData,HI_U32 u32Width, HI_U32 u32Height)
{
    HI_S32 s32Ret;
    HI_U32 u32Size;

    if (NULL == pstData)
    {
        SAMPLE_PRT("pstData is null\n");
        return HI_FAILURE;
    }
    pstData->u32Width = u32Width;
    pstData->u32Height = u32Height;
    pstData->u32Stride = SAMPLE_COMM_IVE_CalcStride(pstData->u32Width, IVE_ALIGN);
    u32Size = pstData->u32Stride * pstData->u32Height;
    s32Ret = HI_MPI_SYS_MmzAlloc(&pstData->u64PhyAddr, (HI_VOID**)&pstData->u64VirAddr, NULL, HI_NULL, u32Size);
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
    HI_U64 u64BlkSize;
    VB_CONFIG_S stVbConf;

    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
    stVbConf.u32MaxPoolCnt = 128;

    for (i = 0; i < u32VpssChnNum; i++)
    {
        s32Ret = SAMPLE_COMM_SYS_GetPicSize(paenSize[i], &pastSize[i]);
        SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, VB_FAIL_0,
            "SAMPLE_COMM_SYS_GetPicSize failed,Error(%#x)!\n",s32Ret);

        u64BlkSize = COMMON_GetPicBufferSize(pastSize[i].u32Width, pastSize[i].u32Height,
                            SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
        /* comm video buffer */
        stVbConf.astCommPool[i].u64BlkSize = u64BlkSize;
        stVbConf.astCommPool[i].u32BlkCnt  = 16;
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
    //stSrcData.u64VirAddr = pstFrameInfo->stVFrame.u64VirAddr[0];
    stSrcData.u64PhyAddr = pstFrameInfo->stVFrame.u64PhyAddr[0];
    stSrcData.u32Width   = pstFrameInfo->stVFrame.u32Width;
    stSrcData.u32Height  = pstFrameInfo->stVFrame.u32Height;
    stSrcData.u32Stride  = pstFrameInfo->stVFrame.u32Stride[0];

    //fill dst
    //stDstData.u64VirAddr = pstDst->au64VirAddr[0];
    stDstData.u64PhyAddr = pstDst->au64PhyAddr[0];
    stDstData.u32Width   = pstDst->u32Width;
    stDstData.u32Height  = pstDst->u32Height;
    stDstData.u32Stride  = pstDst->au32Stride[0];

    s32Ret = HI_MPI_IVE_DMA(&hIveHandle,&stSrcData,&stDstData,&stCtrl,bInstant);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_DMA failed!\n",s32Ret);

    if (HI_TRUE == bInstant)
    {
        s32Ret = HI_MPI_IVE_Query(hIveHandle,&bFinish,bBlock);
        while(HI_ERR_IVE_QUERY_TIMEOUT == s32Ret)
        {
            usleep(100);
            s32Ret = HI_MPI_IVE_Query(hIveHandle,&bFinish,bBlock);
        }
        SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_Query failed!\n",s32Ret);
    }

    return HI_SUCCESS;
}

/******************************************************************************
* function : Start Vpss
******************************************************************************/
HI_S32 SAMPLE_COMM_IVE_StartVpss(SIZE_S *pastSize,HI_U32 u32VpssChnNum)
{
    HI_S32 i;
    VPSS_CHN_ATTR_S astVpssChnAttr[VPSS_MAX_CHN_NUM];
    VPSS_GRP_ATTR_S stVpssGrpAttr;
    HI_BOOL abChnEnable[VPSS_MAX_CHN_NUM] = {HI_FALSE, HI_FALSE, HI_FALSE, HI_FALSE};
    VPSS_GRP VpssGrp = 0;

    stVpssGrpAttr.u32MaxW = 1920;
    stVpssGrpAttr.u32MaxH = 1080;
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;
    stVpssGrpAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stVpssGrpAttr.enPixelFormat  = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    stVpssGrpAttr.bNrEn = HI_FALSE;

    for (i = 0; i < u32VpssChnNum; i++)
    {
        abChnEnable[i] = HI_TRUE;
    }

    for(i = 0; i < VPSS_MAX_CHN_NUM; i++)
    {
        astVpssChnAttr[i].u32Width                    = pastSize[i].u32Width;
        astVpssChnAttr[i].u32Height                   = pastSize[i].u32Height;
        astVpssChnAttr[i].enChnMode                   = VPSS_CHN_MODE_USER;
        astVpssChnAttr[i].enCompressMode              = COMPRESS_MODE_NONE;
        astVpssChnAttr[i].enDynamicRange              = DYNAMIC_RANGE_SDR8;
        astVpssChnAttr[i].enVideoFormat               = VIDEO_FORMAT_LINEAR;
        astVpssChnAttr[i].enPixelFormat               = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
        astVpssChnAttr[i].stFrameRate.s32SrcFrameRate = -1;
        astVpssChnAttr[i].stFrameRate.s32DstFrameRate = -1;
        astVpssChnAttr[i].u32Depth                    = 1;
        astVpssChnAttr[i].bMirror                     = HI_FALSE;
        astVpssChnAttr[i].bFlip                       = HI_FALSE;
        astVpssChnAttr[i].stAspectRatio.enMode        = ASPECT_RATIO_NONE;
    }

    return SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, &astVpssChnAttr[0]);

}
/******************************************************************************
* function : Stop Vpss
******************************************************************************/
HI_VOID SAMPLE_COMM_IVE_StopVpss(HI_U32 u32VpssChnNum)
{
    VPSS_GRP VpssGrp = 0;
    HI_BOOL abChnEnable[VPSS_MAX_CHN_NUM] = {HI_FALSE, HI_FALSE, HI_FALSE, HI_FALSE};
    HI_S32 i = 0;

    for (i = 0; i < u32VpssChnNum; i++)
    {
        abChnEnable[i] = HI_TRUE;
    }

    (HI_VOID)SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);

    return;
}

/******************************************************************************
* function : Start Vo
******************************************************************************/
HI_S32 SAMPLE_COMM_IVE_StartVo(HI_VOID)
{
    HI_S32 s32Ret;
    VO_DEV VoDev = SAMPLE_VO_DEV_DHD0;
    VO_LAYER VoLayer = 0;
    VO_PUB_ATTR_S stVoPubAttr;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    SAMPLE_VO_MODE_E enVoMode = VO_MODE_1MUX;
    HI_U32 u32DisBufLen = 3;

    stVoPubAttr.enIntfSync = VO_OUTPUT_1080P30;
    stVoPubAttr.enIntfType = VO_INTF_HDMI;
    stVoPubAttr.u32BgColor = COLOR_RGB_BLUE;
    s32Ret = SAMPLE_COMM_VO_StartDev(VoDev, &stVoPubAttr);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, VO_FAIL_0,
        "SAMPLE_COMM_VO_StartDev failed,Error(%#x)!\n",s32Ret);

    s32Ret = SAMPLE_COMM_VO_HdmiStart(stVoPubAttr.enIntfSync);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, VO_FAIL_1,
        "SAMPLE_COMM_VO_HdmiStart failed,Error(%#x)!\n",s32Ret);

    s32Ret = SAMPLE_COMM_VO_GetWH(stVoPubAttr.enIntfSync,&stLayerAttr.stDispRect.u32Width,
                                    &stLayerAttr.stDispRect.u32Height, &stLayerAttr.u32DispFrmRt);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, VO_FAIL_2,
        "SAMPLE_COMM_VO_GetWH failed,Error(%#x)!\n",s32Ret);

    s32Ret = HI_MPI_VO_SetDisplayBufLen(VoLayer, u32DisBufLen);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, VO_FAIL_2,
        "HI_MPI_VO_SetDisplayBufLen failed,Error(%#x)!\n",s32Ret);

    stLayerAttr.stDispRect.s32X        = 0;
    stLayerAttr.stDispRect.s32Y        = 0;
    stLayerAttr.stImageSize.u32Width = stLayerAttr.stDispRect.u32Width;
    stLayerAttr.stImageSize.u32Height = stLayerAttr.stDispRect.u32Height;
    stLayerAttr.bDoubleFrame        = HI_FALSE;
    stLayerAttr.bClusterMode        = HI_FALSE;
    stLayerAttr.enPixFormat            = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    stLayerAttr.enDstDynamicRange     = DYNAMIC_RANGE_SDR8;

    s32Ret = SAMPLE_COMM_VO_StartLayer(VoLayer, &stLayerAttr);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, VO_FAIL_2,
        "SAMPLE_COMM_VO_StartLayer failed,Error(%#x)!\n",s32Ret);

    s32Ret = SAMPLE_COMM_VO_StartChn(VoLayer, enVoMode);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, VO_FAIL_3,
        "SAMPLE_COMM_VO_StartChn failed,Error(%#x)!\n",s32Ret);

    return s32Ret;
VO_FAIL_3:
     SAMPLE_COMM_VO_StopLayer(VoLayer);
VO_FAIL_2:
     SAMPLE_COMM_VO_HdmiStop();
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
    VO_DEV VoDev = SAMPLE_VO_DEV_DHD0;
    VO_LAYER VoLayer = 0;
    SAMPLE_VO_MODE_E enVoMode = VO_MODE_1MUX;

    (HI_VOID)SAMPLE_COMM_VO_StopChn(VoDev, enVoMode);
    (HI_VOID)SAMPLE_COMM_VO_StopLayer(VoLayer);
    SAMPLE_COMM_VO_HdmiStop();
    (HI_VOID)SAMPLE_COMM_VO_StopDev(VoDev);
}
/******************************************************************************
* function : Start Vi/Vpss/Venc/Vo
******************************************************************************/
HI_S32 SAMPLE_COMM_IVE_StartViVpssVencVo(SAMPLE_VI_CONFIG_S *pstViConfig,
    SAMPLE_IVE_SWITCH_S *pstSwitch,PIC_SIZE_E *penExtPicSize)
{
    SIZE_S astSize[VPSS_CHN_NUM];
    PIC_SIZE_E aenSize[VPSS_CHN_NUM];
    VI_CHN_ATTR_S stViChnAttr;
    SAMPLE_RC_E enRcMode = SAMPLE_RC_CBR;
    PAYLOAD_TYPE_E enStreamType = PT_H264;
    HI_BOOL bRcnRefShareBuf=HI_FALSE;
    VENC_GOP_ATTR_S stGopAttr;
    VI_DEV ViDev0 = 0;
    VI_PIPE ViPipe0 = 0;
    VI_CHN ViChn = 0;
    HI_S32 s32ViCnt = 1;
    HI_S32 s32WorkSnsId  = 0;
    VPSS_GRP VpssGrp = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    VENC_CHN VeH264Chn = 0;
    WDR_MODE_E enWDRMode = WDR_MODE_NONE;
    DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
    PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
    COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;
    VI_VPSS_MODE_E enMastPipeMode = VI_ONLINE_VPSS_OFFLINE;

    memset(pstViConfig,0,sizeof(*pstViConfig));

    SAMPLE_COMM_VI_GetSensorInfo(pstViConfig);
    pstViConfig->s32WorkingViNum                           = s32ViCnt;

    pstViConfig->as32WorkingViId[0]                        = 0;
    pstViConfig->astViInfo[0].stSnsInfo.MipiDev            = SAMPLE_COMM_VI_GetComboDevBySensor(pstViConfig->astViInfo[0].stSnsInfo.enSnsType, 0);
    pstViConfig->astViInfo[0].stSnsInfo.s32BusId           = 0;

    pstViConfig->astViInfo[0].stDevInfo.ViDev              = ViDev0;
    pstViConfig->astViInfo[0].stDevInfo.enWDRMode          = enWDRMode;

    pstViConfig->astViInfo[0].stPipeInfo.enMastPipeMode    = enMastPipeMode;
    pstViConfig->astViInfo[0].stPipeInfo.aPipe[0]          = ViPipe0;
    pstViConfig->astViInfo[0].stPipeInfo.aPipe[1]          = -1;
    pstViConfig->astViInfo[0].stPipeInfo.aPipe[2]          = -1;
    pstViConfig->astViInfo[0].stPipeInfo.aPipe[3]          = -1;

    pstViConfig->astViInfo[0].stChnInfo.ViChn              = ViChn;
    pstViConfig->astViInfo[0].stChnInfo.enPixFormat        = enPixFormat;
    pstViConfig->astViInfo[0].stChnInfo.enDynamicRange     = enDynamicRange;
    pstViConfig->astViInfo[0].stChnInfo.enVideoFormat      = enVideoFormat;
    pstViConfig->astViInfo[0].stChnInfo.enCompressMode     = enCompressMode;

    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(pstViConfig->astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &aenSize[0]);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_INIT_0,
        "Error(%#x),SAMPLE_COMM_VI_GetSizeBySensor failed!\n",s32Ret);
    aenSize[1] = *penExtPicSize;

    /******************************************
     step  1: Init vb
    ******************************************/
    s32Ret = SAMPLE_COMM_IVE_VbInit(aenSize,astSize,VPSS_CHN_NUM);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_INIT_0,
        "Error(%#x),SAMPLE_COMM_IVE_VbInit failed!\n",s32Ret);
    /******************************************
     step 2: Start vi
    ******************************************/
    s32Ret = SAMPLE_COMM_VI_SetParam(pstViConfig);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_INIT_1,
        "Error(%#x),SAMPLE_COMM_VI_SetParam failed!\n",s32Ret);

    s32Ret = SAMPLE_COMM_VI_StartVi(pstViConfig);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_INIT_1,
        "Error(%#x),SAMPLE_COMM_VI_StartVi failed!\n",s32Ret);
    /******************************************
     step 3: Start vpss
    ******************************************/
    s32Ret = SAMPLE_COMM_IVE_StartVpss(astSize,VPSS_CHN_NUM);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_INIT_2,
        "Error(%#x),SAMPLE_IVS_StartVpss failed!\n",s32Ret);
    /******************************************
      step 4: Bind vpss to vi
     ******************************************/
    s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe0, ViChn, VpssGrp);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_INIT_3,
        "Error(%#x),SAMPLE_COMM_VI_BindVpss failed!\n",s32Ret);
    //Set vi frame
    s32Ret = HI_MPI_VI_GetChnAttr(ViPipe0, ViChn,&stViChnAttr);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_INIT_4,
        "Error(%#x),HI_MPI_VI_GetChnAttr failed!\n",s32Ret);

    s32Ret = HI_MPI_VI_SetChnAttr(ViPipe0, ViChn,&stViChnAttr);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_INIT_4,
        "Error(%#x),HI_MPI_VI_SetChnAttr failed!\n",s32Ret);
    /******************************************
     step 5: Start Vo
     ******************************************/
    if (HI_TRUE == pstSwitch->bVo)
    {
        s32Ret = SAMPLE_COMM_IVE_StartVo();
        SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_INIT_4,
            "Error(%#x),SAMPLE_COMM_IVE_StartVo failed!\n", s32Ret);
    }
    /******************************************
     step 6: Start Venc
    ******************************************/
    if (HI_TRUE == pstSwitch->bVenc)
    {
        s32Ret = SAMPLE_COMM_VENC_GetGopAttr(VENC_GOPMODE_NORMALP,&stGopAttr);
        SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_INIT_5,
            "Error(%#x),SAMPLE_COMM_VENC_GetGopAttr failed!\n",s32Ret);
        s32Ret = SAMPLE_COMM_VENC_Start(VeH264Chn, enStreamType,aenSize[0],enRcMode,0,bRcnRefShareBuf,&stGopAttr);
        SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_INIT_5,
            "Error(%#x),SAMPLE_COMM_VENC_Start failed!\n",s32Ret);
        s32Ret = SAMPLE_COMM_VENC_StartGetStream(&VeH264Chn, 1);
        SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_INIT_6,
            "Error(%#x),SAMPLE_COMM_VENC_StartGetStream failed!\n",s32Ret);
    }

    return HI_SUCCESS;

END_INIT_6:
    if (HI_TRUE == pstSwitch->bVenc)
    {
        SAMPLE_COMM_VENC_Stop(VeH264Chn);
    }
END_INIT_5:
    if (HI_TRUE == pstSwitch->bVo)
    {
        SAMPLE_COMM_IVE_StopVo();
    }
END_INIT_4:
    SAMPLE_COMM_VI_UnBind_VPSS(ViPipe0, ViChn, VpssGrp);
END_INIT_3:
    SAMPLE_COMM_IVE_StopVpss(VPSS_CHN_NUM);
END_INIT_2:
    SAMPLE_COMM_VI_StopVi(pstViConfig);
END_INIT_1:    //system exit
    SAMPLE_COMM_SYS_Exit();
    memset(pstViConfig,0,sizeof(*pstViConfig));
END_INIT_0:

    return s32Ret;
}
/******************************************************************************
* function : Stop Vi/Vpss/Venc/Vo
******************************************************************************/
HI_VOID SAMPLE_COMM_IVE_StopViVpssVencVo(SAMPLE_VI_CONFIG_S *pstViConfig,SAMPLE_IVE_SWITCH_S *pstSwitch)
{
    if (HI_TRUE == pstSwitch->bVenc)
    {
        SAMPLE_COMM_VENC_StopGetStream();
        SAMPLE_COMM_VENC_Stop(0);
    }
    if (HI_TRUE == pstSwitch->bVo)
    {
        SAMPLE_COMM_IVE_StopVo();
    }

    SAMPLE_COMM_VI_UnBind_VPSS(pstViConfig->astViInfo[0].stPipeInfo.aPipe[0],
        pstViConfig->astViInfo[0].stChnInfo.ViChn, 0);
    SAMPLE_COMM_IVE_StopVpss(VPSS_CHN_NUM);
    SAMPLE_COMM_VI_StopVi(pstViConfig);
    SAMPLE_COMM_SYS_Exit();

    memset(pstViConfig,0,sizeof(*pstViConfig));
}



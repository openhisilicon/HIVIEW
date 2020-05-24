#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <errno.h>
#include <pthread.h>
#include "hi_common.h"
#include "sample_comm.h"
#include "loadbmp.h"

#define OverlayMinHandle    0
#define OverlayExMinHandle 20
#define CoverMinHandle     40
#define CoverExMinHandle   60
#define MosaicMinHandle    80
HI_CHAR* Path_BMP = HI_NULL;

HI_S32 REGION_MST_LoadBmp(const char *filename, BITMAP_S *pstBitmap, HI_BOOL bFil, HI_U32 u16FilColor,PIXEL_FORMAT_E enPixelFormat)
{
    OSD_SURFACE_S Surface;
    OSD_BITMAPFILEHEADER bmpFileHeader;
    OSD_BITMAPINFO bmpInfo;
    HI_S32 s32BytesPerPix = 2;
    HI_U8* pu8Data;
    HI_S32 R_Value;
    HI_S32 G_Value;
    HI_S32 B_Value;
    HI_S32 Gr_Value;
    HI_U8  Value_tmp;
    HI_U8  Value;
    HI_S32 s32Width;

    if(GetBmpInfo(filename,&bmpFileHeader,&bmpInfo) < 0)
    {
        printf("GetBmpInfo err!\n");
        return HI_FAILURE;
    }
     if(enPixelFormat == PIXEL_FORMAT_ARGB_4444)
    {
        Surface.enColorFmt =OSD_COLOR_FMT_RGB4444;
    }
    else if(enPixelFormat == PIXEL_FORMAT_ARGB_1555 || enPixelFormat == PIXEL_FORMAT_ARGB_2BPP)
    {
        Surface.enColorFmt =OSD_COLOR_FMT_RGB1555;
    }
    else if(enPixelFormat == PIXEL_FORMAT_ARGB_8888)
    {
        Surface.enColorFmt =OSD_COLOR_FMT_RGB8888;
        s32BytesPerPix = 4;
    }
    else
    {
        printf("enPixelFormat err %d \n",enPixelFormat);
        return HI_FAILURE;
    }

    pstBitmap->pData = malloc(s32BytesPerPix * (bmpInfo.bmiHeader.biWidth) * (bmpInfo.bmiHeader.biHeight));

    if(NULL == pstBitmap->pData)
    {
        printf("malloc osd memroy err!\n");
        return HI_FAILURE;
    }
    CreateSurfaceByBitMap(filename,&Surface,(HI_U8*)(pstBitmap->pData));

    pstBitmap->u32Width = Surface.u16Width;
    pstBitmap->u32Height = Surface.u16Height;
    pstBitmap->enPixelFormat = enPixelFormat;

    int i,j, k;
    HI_U8*  pu8Temp;

    if (PIXEL_FORMAT_ARGB_2BPP == enPixelFormat)
    {
        s32Width = DIV_UP (bmpInfo.bmiHeader.biWidth,4);
        pu8Data = malloc((s32Width)* (bmpInfo.bmiHeader.biHeight));
        if (NULL == pu8Data)
        {
            printf("malloc osd memroy err!\n");
            return HI_FAILURE;
        }
    }


    if (PIXEL_FORMAT_ARGB_2BPP != enPixelFormat)
    {
        HI_U16 *pu16Temp;
        pu16Temp = (HI_U16*)pstBitmap->pData;
        if (bFil)
        {
            for (i = 0; i < pstBitmap->u32Height; i++)
            {
                for (j = 0; j < pstBitmap->u32Width; j++)
                {
                    if (u16FilColor == *pu16Temp)
                    {
                        *pu16Temp &= 0x7FFF;
                    }

                    pu16Temp++;
                }
            }

        }
    }
    else
    {
        HI_U16 *pu16Temp;

        pu16Temp = (HI_U16*)pstBitmap->pData;
        pu8Temp = (HI_U8*)pu8Data;

        for (i = 0; i < pstBitmap->u32Height; i++)
        {
            for (j = 0; j < pstBitmap->u32Width/4; j++)
            {
                Value = 0;

                for (k = j; k < j + 4; k++)
                {
                    B_Value = *pu16Temp & 0x001F;
                    G_Value = *pu16Temp >> 5 & 0x001F;
                    R_Value = *pu16Temp >> 10 & 0x001F;
                    pu16Temp++;

                    Gr_Value = (R_Value * 299 + G_Value * 587 + B_Value * 144 + 500) / 1000;
                    if (Gr_Value > 16)
                    {
                        Value_tmp = 0x01;
                    }
                    else
                    {
                        Value_tmp = 0x00;
                    }
                    Value = (Value << 2) + Value_tmp;
                }
                //printf("%s,%d,*pu16Temp:%d,Value:%d\n",__FUNCTION__,__LINE__,*pu16Temp,Value);

                *pu8Temp = Value;
                pu8Temp++;
            }
        }
        free(pstBitmap->pData);
        pstBitmap->pData = pu8Data;
    }


    return HI_SUCCESS;
}

HI_S32 REGION_MST_UpdateCanvas(const char* filename, BITMAP_S* pstBitmap, HI_BOOL bFil,
                               HI_U32 u16FilColor, SIZE_S* pstSize, HI_U32 u32Stride, PIXEL_FORMAT_E enPixelFmt)
{
    OSD_SURFACE_S Surface;
    OSD_BITMAPFILEHEADER bmpFileHeader;
    OSD_BITMAPINFO bmpInfo;

    if (GetBmpInfo(filename, &bmpFileHeader, &bmpInfo) < 0)
    {
        printf("GetBmpInfo err!\n");
        return HI_FAILURE;
    }

    if (PIXEL_FORMAT_ARGB_1555 == enPixelFmt)
    {
        Surface.enColorFmt = OSD_COLOR_FMT_RGB1555;
    }
    else if (PIXEL_FORMAT_ARGB_4444 == enPixelFmt)
    {
        Surface.enColorFmt = OSD_COLOR_FMT_RGB4444;
    }
    else if (PIXEL_FORMAT_ARGB_8888 == enPixelFmt)
    {
        Surface.enColorFmt = OSD_COLOR_FMT_RGB8888;
    }
    else
    {
        printf("Pixel format is not support!\n");
        return HI_FAILURE;
    }

    if (NULL == pstBitmap->pData)
    {
        printf("malloc osd memroy err!\n");
        return HI_FAILURE;
    }

    CreateSurfaceByCanvas(filename, &Surface, (HI_U8*)(pstBitmap->pData), pstSize->u32Width, pstSize->u32Height, u32Stride);

    pstBitmap->u32Width = Surface.u16Width;
    pstBitmap->u32Height = Surface.u16Height;

    if (PIXEL_FORMAT_ARGB_1555 == enPixelFmt)
    {
        pstBitmap->enPixelFormat = PIXEL_FORMAT_ARGB_1555;
    }
    else if (PIXEL_FORMAT_ARGB_4444 == enPixelFmt)
    {
        pstBitmap->enPixelFormat = PIXEL_FORMAT_ARGB_4444;
    }
    else if (PIXEL_FORMAT_ARGB_8888 == enPixelFmt)
    {
        pstBitmap->enPixelFormat = PIXEL_FORMAT_ARGB_8888;
    }

    int i, j;
    HI_U16* pu16Temp;
    pu16Temp = (HI_U16*)pstBitmap->pData;

    if (bFil)
    {
        for (i = 0; i < pstBitmap->u32Height; i++)
        {
            for (j = 0; j < pstBitmap->u32Width; j++)
            {
                if (u16FilColor == *pu16Temp)
                {
                    *pu16Temp &= 0x7FFF;
                }

                pu16Temp++;
            }
        }

    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_REGION_GetMinHandle(RGN_TYPE_E enType)
{
     HI_S32 MinHandle;
     switch(enType)
     {
        case OVERLAY_RGN:
            MinHandle = OverlayMinHandle;
            break;
        case OVERLAYEX_RGN:
            MinHandle = OverlayExMinHandle;
            break;
        case COVER_RGN:
            MinHandle = CoverMinHandle;
            break;
        case COVEREX_RGN:
            MinHandle = CoverExMinHandle;
            break;
        case MOSAIC_RGN:
            MinHandle = MosaicMinHandle;
            break;
        default:
            MinHandle = -1;
            break;
     }
     return MinHandle;
}

HI_S32 SAMPLE_REGION_CreateOverLay(HI_S32 HandleNum)
{
    HI_S32 s32Ret;
    HI_S32 i;
    RGN_ATTR_S stRegion;

    stRegion.enType = OVERLAY_RGN;
    stRegion.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_ARGB_1555;
    stRegion.unAttr.stOverlay.stSize.u32Height = 200;
    stRegion.unAttr.stOverlay.stSize.u32Width  = 200;
    stRegion.unAttr.stOverlay.u32BgColor = 0x00ff00ff;
    stRegion.unAttr.stOverlay.u32CanvasNum = 5;
    for(i = OverlayMinHandle;i < HandleNum;i++)
    {
        s32Ret = HI_MPI_RGN_Create(i, &stRegion);
        if(HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_RGN_Create failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }

    return s32Ret;
}

HI_S32 SAMPLE_REGION_CreateOverLayEx(HI_S32 HandleNum)
{
    HI_S32 s32Ret;
    HI_S32 i;
    RGN_ATTR_S stRegion;

    stRegion.enType = OVERLAYEX_RGN;
    stRegion.unAttr.stOverlayEx.enPixelFmt = PIXEL_FORMAT_ARGB_1555;
    stRegion.unAttr.stOverlayEx.stSize.u32Height = 200;
    stRegion.unAttr.stOverlayEx.stSize.u32Width  = 200;
    stRegion.unAttr.stOverlayEx.u32BgColor = 0x00ff00ff;
    stRegion.unAttr.stOverlayEx.u32CanvasNum =2;
    for(i = OverlayExMinHandle;i < OverlayExMinHandle + HandleNum;i++)
    {
        s32Ret = HI_MPI_RGN_Create(i, &stRegion);
        if(HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_RGN_Create failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }

    return s32Ret;
}

HI_S32 SAMPLE_REGION_CreateCover(HI_S32 HandleNum)
{
    HI_S32 s32Ret;
    HI_S32 i;
    RGN_ATTR_S stRegion;

    stRegion.enType = COVER_RGN;

    for(i = CoverMinHandle;i < CoverMinHandle + HandleNum;i++)
    {
        s32Ret = HI_MPI_RGN_Create(i, &stRegion);
        if(HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_RGN_Create failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }

    return s32Ret;
}

HI_S32 SAMPLE_REGION_CreateCoverEx(HI_S32 HandleNum)
{
    HI_S32 s32Ret;
    HI_S32 i;
    RGN_ATTR_S stRegion;

    stRegion.enType = COVEREX_RGN;

    for(i = CoverExMinHandle;i < CoverExMinHandle + HandleNum;i++)
    {
        s32Ret = HI_MPI_RGN_Create(i, &stRegion);
        if(HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_RGN_Create failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }

    return s32Ret;
}

HI_S32 SAMPLE_REGION_CreateMosaic(HI_S32 HandleNum)
{
    HI_S32 s32Ret;
    HI_S32 i;
    RGN_ATTR_S stRegion;

    stRegion.enType = MOSAIC_RGN;

    for(i = MosaicMinHandle;i < MosaicMinHandle + HandleNum;i++)
    {
        s32Ret = HI_MPI_RGN_Create(i, &stRegion);
        if(HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_RGN_Create failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }

    return s32Ret;
}

HI_S32 SAMPLE_REGION_Destroy(RGN_HANDLE Handle)
{
    HI_S32 s32Ret;
    s32Ret = HI_MPI_RGN_Destroy(Handle);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_RGN_Destroy failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

HI_S32 SAMPLE_REGION_SetAttr(RGN_HANDLE Handle,RGN_ATTR_S *pstRegion)
{
    HI_S32 s32Ret;
    s32Ret = HI_MPI_RGN_SetAttr(Handle,pstRegion);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_RGN_SetAttr failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

HI_S32 SAMPLE_REGION_GetAttr(RGN_HANDLE Handle,RGN_ATTR_S *pstRegion)
{
    HI_S32 s32Ret;
    s32Ret = HI_MPI_RGN_Create(Handle,pstRegion);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_RGN_Create failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

HI_S32 SAMPLE_REGION_AttachToChn(RGN_HANDLE Handle,MPP_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr)
{
    HI_S32 s32Ret;
    s32Ret = HI_MPI_RGN_AttachToChn(Handle, pstChn,pstChnAttr);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_RGN_AttachToChn failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

HI_S32 SAMPLE_REGION_DetachFromChn(RGN_HANDLE Handle,MPP_CHN_S *pstChn)
{
    HI_S32 s32Ret;
    s32Ret = HI_MPI_RGN_DetachFromChn(Handle, pstChn);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_RGN_DetachFromChn failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

HI_S32 SAMPLE_REGION_SetDisplayAttr(RGN_HANDLE Handle,MPP_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr)
{
    HI_S32 s32Ret;
    s32Ret = HI_MPI_RGN_SetDisplayAttr(Handle,pstChn, pstChnAttr);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_RGN_SetDisplayAttr failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

HI_S32 SAMPLE_REGION_GetDisplayAttr(RGN_HANDLE Handle,MPP_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr)
{
    HI_S32 s32Ret;
    s32Ret = HI_MPI_RGN_GetDisplayAttr(Handle,pstChn,pstChnAttr);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_RGN_GetDisplayAttr failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

HI_S32 SAMPLE_REGION_SetBitMap(RGN_HANDLE Handle, BITMAP_S *pstBitmap)
{
    HI_S32 s32Ret;
    s32Ret = HI_MPI_RGN_SetBitMap(Handle,pstBitmap);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_RGN_SetBitMap failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

HI_S32 SAMPLE_REGION_GetUpCanvasInfo(RGN_HANDLE Handle, RGN_CANVAS_INFO_S *pstCanvasInfo)
{
    HI_S32 s32Ret;
    s32Ret = HI_MPI_RGN_GetCanvasInfo(Handle,pstCanvasInfo);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_RGN_GetCanvasInfo failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }


    s32Ret = HI_MPI_RGN_UpdateCanvas(Handle);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_RGN_UpdateCanvas failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

HI_S32 SAMPLE_COMM_REGION_Create(HI_S32 HandleNum,RGN_TYPE_E enType)
{
    HI_S32 s32Ret;
    if(HandleNum <= 0 || HandleNum > 16)
    {
        SAMPLE_PRT("HandleNum is illegal %d!\n", HandleNum);
        return HI_FAILURE;
    }
    if(enType < 0 || enType > 4)
    {
        SAMPLE_PRT("enType is illegal %d!\n", enType);
        return HI_FAILURE;
    }
    switch(enType)
    {
        case OVERLAY_RGN:
             s32Ret = SAMPLE_REGION_CreateOverLay(HandleNum);
             break;
        case OVERLAYEX_RGN:
             s32Ret = SAMPLE_REGION_CreateOverLayEx(HandleNum);
             break;
        case COVER_RGN:
             s32Ret = SAMPLE_REGION_CreateCover(HandleNum);
             break;
        case COVEREX_RGN:
             s32Ret = SAMPLE_REGION_CreateCoverEx(HandleNum);
             break;
        case MOSAIC_RGN:
             s32Ret = SAMPLE_REGION_CreateMosaic(HandleNum);
             break;
            default:
                break;
    }
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_REGION_Create failed! HandleNum%d,entype:%d!\n",HandleNum, enType);
        return HI_FAILURE;
    }
    return s32Ret;
}

HI_S32 SAMPLE_COMM_REGION_Destroy(HI_S32 HandleNum,RGN_TYPE_E enType)
{
    HI_S32 i;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 MinHadle;

    if(HandleNum <= 0 || HandleNum > 16)
    {
        SAMPLE_PRT("HandleNum is illegal %d!\n", HandleNum);
        return HI_FAILURE;
    }
    if(enType < 0 || enType > 4)
    {
        SAMPLE_PRT("enType is illegal %d!\n", enType);
        return HI_FAILURE;
    }
    switch(enType)
    {
        case OVERLAY_RGN:
            MinHadle  = OverlayMinHandle;
            break;
        case OVERLAYEX_RGN:
            MinHadle  = OverlayExMinHandle;
            break;
        case COVER_RGN:
            MinHadle  = CoverMinHandle;
            break;
        case COVEREX_RGN:
            MinHadle  = CoverExMinHandle;
            break;
        case MOSAIC_RGN:
            MinHadle  = MosaicMinHandle;
            break;
        default:
            break;
    }
    for(i = MinHadle; i < MinHadle+HandleNum;i++)
    {
        s32Ret = SAMPLE_REGION_Destroy(i);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_REGION_Destroy failed!\n");
        }
    }
    return s32Ret;
}

HI_S32 SAMPLE_COMM_REGION_AttachToChn(HI_S32 HandleNum,RGN_TYPE_E enType, MPP_CHN_S *pstMppChn)
{
    HI_S32 i;
    HI_S32 s32Ret;
    HI_S32 MinHadle;
    RGN_CHN_ATTR_S stChnAttr;

    if(HandleNum <= 0 || HandleNum > 16)
    {
        SAMPLE_PRT("HandleNum is illegal %d!\n", HandleNum);
        return HI_FAILURE;
    }
    if(enType < 0 || enType > 4)
    {
        SAMPLE_PRT("enType is illegal %d!\n", enType);
        return HI_FAILURE;
    }
    if(HI_NULL == pstMppChn)
    {
        SAMPLE_PRT("pstMppChn is NULL !\n");
        return HI_FAILURE;
    }
    /*set the chn config*/
    stChnAttr.bShow = HI_TRUE;
    switch(enType)
        {
        case OVERLAY_RGN:
            MinHadle = OverlayMinHandle;

            stChnAttr.bShow = HI_TRUE;
            stChnAttr.enType = OVERLAY_RGN;

            stChnAttr.unChnAttr.stOverlayChn.u32BgAlpha = 128;
            stChnAttr.unChnAttr.stOverlayChn.u32FgAlpha = 128;

            stChnAttr.unChnAttr.stOverlayChn.stQpInfo.bQpDisable = HI_FALSE;
            stChnAttr.unChnAttr.stOverlayChn.stQpInfo.bAbsQp = HI_TRUE;
            stChnAttr.unChnAttr.stOverlayChn.stQpInfo.s32Qp  = 30;

            stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Height = 16;
            stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Width = 16;
            stChnAttr.unChnAttr.stOverlayChn.stInvertColor.u32LumThresh = 128;
            stChnAttr.unChnAttr.stOverlayChn.stInvertColor.enChgMod = LESSTHAN_LUM_THRESH;
            stChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn = HI_FALSE;
            stChnAttr.unChnAttr.stOverlayChn.u16ColorLUT[0] = 0x2abc;
            stChnAttr.unChnAttr.stOverlayChn.u16ColorLUT[1] = 0x7FF0;
            stChnAttr.unChnAttr.stOverlayChn.enAttachDest = ATTACH_JPEG_MAIN;
            break;
        case OVERLAYEX_RGN:
            MinHadle = OverlayExMinHandle;
            stChnAttr.bShow = HI_TRUE;
            stChnAttr.enType = OVERLAYEX_RGN;

            stChnAttr.unChnAttr.stOverlayExChn.u32BgAlpha = 128;
            stChnAttr.unChnAttr.stOverlayExChn.u32FgAlpha = 128;
            break;
        case COVER_RGN:
            MinHadle = CoverMinHandle;

            stChnAttr.bShow = HI_TRUE;
            stChnAttr.enType = COVER_RGN;
            stChnAttr.unChnAttr.stCoverChn.enCoverType = AREA_RECT;

            stChnAttr.unChnAttr.stCoverChn.stRect.u32Height = 200;
            stChnAttr.unChnAttr.stCoverChn.stRect.u32Width  = 200;

            stChnAttr.unChnAttr.stCoverChn.u32Color      = 0x0000ffff;

            stChnAttr.unChnAttr.stCoverChn.enCoordinate = RGN_ABS_COOR;
            break;
        case COVEREX_RGN:
            MinHadle = CoverExMinHandle;

            stChnAttr.bShow = HI_TRUE;
            stChnAttr.enType = COVEREX_RGN;
            stChnAttr.unChnAttr.stCoverExChn.enCoverType = AREA_RECT;

            stChnAttr.unChnAttr.stCoverExChn.stRect.u32Height = 200;
            stChnAttr.unChnAttr.stCoverExChn.stRect.u32Width  = 200;

            stChnAttr.unChnAttr.stCoverExChn.u32Color      = 0x0000ffff;
            break;
        case MOSAIC_RGN:
            MinHadle = MosaicMinHandle;
            stChnAttr.enType = MOSAIC_RGN;
            stChnAttr.unChnAttr.stMosaicChn.enBlkSize = MOSAIC_BLK_SIZE_32;
            stChnAttr.unChnAttr.stMosaicChn.stRect.u32Height = 200;
            stChnAttr.unChnAttr.stMosaicChn.stRect.u32Width  = 200;
            break;
        default:
                break;
        }
    /*attach to Chn*/
    for(i =MinHadle; i <MinHadle+HandleNum;i++)
    {
        if(OVERLAY_RGN == enType)
        {
            stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = 20 + 200*(i-OverlayMinHandle);
            stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = 20 + 200*(i-OverlayMinHandle);
            stChnAttr.unChnAttr.stOverlayChn.u32Layer = i - OverlayMinHandle;
        }
        if(OVERLAYEX_RGN == enType)
        {
            stChnAttr.unChnAttr.stOverlayExChn.stPoint.s32X = 20 + 200*(i - OverlayExMinHandle);
            stChnAttr.unChnAttr.stOverlayExChn.stPoint.s32Y = 20 + 200*(i - OverlayExMinHandle);
            stChnAttr.unChnAttr.stOverlayExChn.u32Layer = i - OverlayExMinHandle;
        }
        if(COVER_RGN == enType)
        {
            stChnAttr.unChnAttr.stCoverChn.stRect.s32X = 20 + 200*(i - CoverMinHandle) ;
            stChnAttr.unChnAttr.stCoverChn.stRect.s32Y = 20 + 200*(i - CoverMinHandle);
            stChnAttr.unChnAttr.stCoverChn.u32Layer = i - CoverMinHandle;
        }
        if(COVEREX_RGN == enType)
        {
            stChnAttr.unChnAttr.stCoverExChn.stRect.s32X = 20 + 200*(i - CoverExMinHandle);
            stChnAttr.unChnAttr.stCoverExChn.stRect.s32Y = 20 + 200*(i - CoverExMinHandle);
            stChnAttr.unChnAttr.stCoverExChn.u32Layer = i - CoverExMinHandle;
        }
        if(MOSAIC_RGN == enType)
        {
            stChnAttr.unChnAttr.stMosaicChn.stRect.s32X = 20 + 200*(i-MosaicMinHandle);
            stChnAttr.unChnAttr.stMosaicChn.stRect.s32Y = 20 + 200*(i-MosaicMinHandle);
            stChnAttr.unChnAttr.stMosaicChn.u32Layer = i-MosaicMinHandle;
        }
        s32Ret = SAMPLE_REGION_AttachToChn(i,pstMppChn,&stChnAttr);
        if(HI_SUCCESS !=s32Ret)
        {
            SAMPLE_PRT("SAMPLE_REGION_AttachToChn failed!\n");
            break;
        }
    }
    /*detach region from chn */
    if(HI_SUCCESS != s32Ret && i>0)
    {
        i--;
        for(;i>=MinHadle;i--)
        {
            s32Ret = SAMPLE_REGION_DetachFromChn(i,pstMppChn);
        }

    }
    return s32Ret;
}

HI_S32 SAMPLE_COMM_REGION_DetachFrmChn(HI_S32 HandleNum,RGN_TYPE_E enType, MPP_CHN_S *pstMppChn)
{
    HI_S32 i;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 MinHadle;

    if(HandleNum <= 0 || HandleNum > 16)
    {
        SAMPLE_PRT("HandleNum is illegal %d!\n", HandleNum);
        return HI_FAILURE;
    }
    if(enType < 0 || enType > 4)
    {
        SAMPLE_PRT("enType is illegal %d!\n", enType);
        return HI_FAILURE;
    }
    if(HI_NULL == pstMppChn)
    {
        SAMPLE_PRT("pstMppChn is NULL !\n");
        return HI_FAILURE;
    }
    switch(enType)
    {
        case OVERLAY_RGN:
            MinHadle  = OverlayMinHandle;
            break;
        case OVERLAYEX_RGN:
            MinHadle  = OverlayExMinHandle;
            break;
        case COVER_RGN:
            MinHadle  = CoverMinHandle;
            break;
        case COVEREX_RGN:
            MinHadle  = CoverExMinHandle;
            break;
        case MOSAIC_RGN:
            MinHadle  = MosaicMinHandle;
            break;
        default:
            break;
    }
    for(i = MinHadle;i< MinHadle + HandleNum;i++)
    {
        s32Ret = SAMPLE_REGION_DetachFromChn(i, pstMppChn);
        if(HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_REGION_DetachFromChn failed! Handle:%d\n",i);
        }
    }
    return s32Ret;
}

HI_S32 SAMPLE_COMM_REGION_SetBitMap(RGN_HANDLE Handle,PIXEL_FORMAT_E enPixelFmt)
{
   HI_S32 s32Ret;
   BITMAP_S stBitmap;
   REGION_MST_LoadBmp(Path_BMP, &stBitmap, HI_FALSE, 0,enPixelFmt);
   s32Ret = SAMPLE_REGION_SetBitMap(Handle, &stBitmap);
   if (s32Ret != HI_SUCCESS)
   {
        SAMPLE_PRT("SAMPLE_REGION_SetBitMap failed!Handle:%d\n",Handle);
   }
   free(stBitmap.pData);
   return s32Ret;
}
HI_S32 SAMPLE_COMM_REGION_GetUpCanvas(RGN_HANDLE Handle)
{
    HI_S32 s32Ret;
    SIZE_S stSize;
    BITMAP_S stBitmap;
    RGN_CANVAS_INFO_S stCanvasInfo;

    s32Ret = HI_MPI_RGN_GetCanvasInfo(Handle,&stCanvasInfo);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_RGN_GetCanvasInfo failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    stBitmap.pData   = (HI_VOID*)(HI_UL)stCanvasInfo.u64VirtAddr;
    stSize.u32Width  = stCanvasInfo.stSize.u32Width;
    stSize.u32Height = stCanvasInfo.stSize.u32Height;
    REGION_MST_UpdateCanvas(Path_BMP, &stBitmap, HI_FALSE, 0, &stSize, stCanvasInfo.u32Stride, PIXEL_FORMAT_ARGB_1555);

    s32Ret = HI_MPI_RGN_UpdateCanvas(Handle);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_RGN_UpdateCanvas failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


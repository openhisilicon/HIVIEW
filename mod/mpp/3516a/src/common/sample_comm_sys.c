/******************************************************************************
  Some simple Hisilicon Hi3531 system functions.

  Copyright (C), 2010-2011, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2011-2 Created
******************************************************************************/
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
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>

#include "sample_comm.h"

/******************************************************************************
* function : get picture size(w*h), according Norm and enPicSize
******************************************************************************/
HI_S32 SAMPLE_COMM_SYS_GetPicSize(VIDEO_NORM_E enNorm, PIC_SIZE_E enPicSize, SIZE_S* pstSize)
{
    switch (enPicSize)
    {
        case PIC_QCIF:
            pstSize->u32Width  = 176;
            pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 144 : 120;
            break;
        case PIC_CIF:
            pstSize->u32Width  = 352;
            pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 288 : 240;
            break;
        case PIC_D1:
            pstSize->u32Width  = 720;
            pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 576 : 480;
            break;
        case PIC_960H:
            pstSize->u32Width  = 960;
            pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 576 : 480;
            break;
        case PIC_2CIF:
            pstSize->u32Width  = 360;
            pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 576 : 480;
            break;
        case PIC_QVGA:    /* 320 * 240 */
            pstSize->u32Width  = 320;
            pstSize->u32Height = 240;
            break;
        case PIC_VGA:     /* 640 * 480 */
            pstSize->u32Width  = 640;
            pstSize->u32Height = 480;
            break;
        case PIC_XGA:     /* 1024 * 768 */
            pstSize->u32Width  = 1024;
            pstSize->u32Height = 768;
            break;
        case PIC_SXGA:    /* 1400 * 1050 */
            pstSize->u32Width  = 1400;
            pstSize->u32Height = 1050;
            break;
        case PIC_UXGA:    /* 1600 * 1200 */
            pstSize->u32Width  = 1600;
            pstSize->u32Height = 1200;
            break;
        case PIC_QXGA:    /* 2048 * 1536 */
            pstSize->u32Width  = 2048;
            pstSize->u32Height = 1536;
            break;
        case PIC_WVGA:    /* 854 * 480 */
            pstSize->u32Width  = 854;
            pstSize->u32Height = 480;
            break;
        case PIC_WSXGA:   /* 1680 * 1050 */
            pstSize->u32Width = 1680;
            pstSize->u32Height = 1050;
            break;
        case PIC_WUXGA:   /* 1920 * 1200 */
            pstSize->u32Width  = 1920;
            pstSize->u32Height = 1200;
            break;
        case PIC_WQXGA:   /* 2560 * 1600 */
            pstSize->u32Width  = 2560;
            pstSize->u32Height = 1600;
            break;
        case PIC_HD720:   /* 1280 * 720 */
            pstSize->u32Width  = 1280;
            pstSize->u32Height = 720;
            break;
        case PIC_HD1080:  /* 1920 * 1080 */
            pstSize->u32Width  = 1920;
            pstSize->u32Height = 1080;
            break;
        case PIC_2304x1296:  /* 2304 * 1296 */
            pstSize->u32Width  = 2304;
            pstSize->u32Height = 1296;
            break;
        case PIC_2592x1520:  /* 2592 * 1520 */
            pstSize->u32Width  = 2592;
            pstSize->u32Height = 1520;
            break;
        case PIC_5M:      /* 2592 * 1944 */
            pstSize->u32Width  = 2592;
            pstSize->u32Height = 1944;
            break;

        default:
            return HI_FAILURE;
    }
    return HI_SUCCESS;
}

/******************************************************************************
* function : calculate VB Block size of Histogram.
******************************************************************************/
HI_U32 SAMPLE_COMM_SYS_CalcHistVbBlkSize(VIDEO_NORM_E enNorm, PIC_SIZE_E enPicSize, SIZE_S* pstHistBlkSize, HI_U32 u32AlignWidth)
{
    HI_S32 s32Ret;
    SIZE_S stPicSize;

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enNorm, enPicSize, &stPicSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size[%d] failed!\n", enPicSize);
        return HI_FAILURE;
    }

    SAMPLE_PRT("stPicSize.u32Width%d,pstHistBlkSize->u32Width%d\n,stPicSize.u32Height%d,pstHistBlkSize->u32Height%d\n",
               stPicSize.u32Width, pstHistBlkSize->u32Width,
               stPicSize.u32Height, pstHistBlkSize->u32Height );
    return (CEILING_2_POWER(44, u32AlignWidth) * CEILING_2_POWER(44, u32AlignWidth) * 16 * 4);

    return HI_SUCCESS;
}

/******************************************************************************
* function : calculate VB Block size of picture.
******************************************************************************/
HI_U32 SAMPLE_COMM_SYS_CalcPicVbBlkSize(VIDEO_NORM_E enNorm, PIC_SIZE_E enPicSize, PIXEL_FORMAT_E enPixFmt, HI_U32 u32AlignWidth)
{
    HI_S32 s32Ret = HI_FAILURE;
    SIZE_S stSize;
    HI_U32 u32VbSize;
    HI_U32 u32HeaderSize;

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enNorm, enPicSize, &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size[%d] failed!\n", enPicSize);
        return HI_FAILURE;
    }

    if (PIXEL_FORMAT_YUV_SEMIPLANAR_422 != enPixFmt && PIXEL_FORMAT_YUV_SEMIPLANAR_420 != enPixFmt)
    {
        SAMPLE_PRT("pixel format[%d] input failed!\n", enPixFmt);
        return HI_FAILURE;
    }

    if (16 != u32AlignWidth && 32 != u32AlignWidth && 64 != u32AlignWidth)
    {
        SAMPLE_PRT("system align width[%d] input failed!\n", \
                   u32AlignWidth);
        return HI_FAILURE;
    }
    //SAMPLE_PRT("w:%d, u32AlignWidth:%d\n", CEILING_2_POWER(stSize.u32Width,u32AlignWidth), u32AlignWidth);
    u32VbSize = (CEILING_2_POWER(stSize.u32Width, u32AlignWidth) * \
                 CEILING_2_POWER(stSize.u32Height, u32AlignWidth) * \
                 ((PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixFmt) ? 2 : 1.5));

    VB_PIC_HEADER_SIZE(stSize.u32Width, stSize.u32Height, enPixFmt, u32HeaderSize);
    u32VbSize += u32HeaderSize;

    return u32VbSize;
}

/******************************************************************************
* function : calculate VB Block size of picture.
******************************************************************************/
HI_U32 VI_COMM_SYS_CalcPicVbBlkSize(VIDEO_NORM_E enNorm, HI_U32  u32Width , HI_U32  u32Height, PIXEL_FORMAT_E enPixFmt, HI_U32 u32AlignWidth)
{
    SIZE_S stSize;

    stSize.u32Width = u32Width;
    stSize.u32Height = u32Height;

    if (PIXEL_FORMAT_YUV_SEMIPLANAR_422 != enPixFmt && PIXEL_FORMAT_YUV_SEMIPLANAR_420 != enPixFmt)
    {
        SAMPLE_PRT("pixel format[%d] input failed!\n", enPixFmt);
        return HI_FAILURE;
    }

    if (16 != u32AlignWidth && 32 != u32AlignWidth && 64 != u32AlignWidth)
    {
        SAMPLE_PRT("system align width[%d] input failed!\n", \
                   u32AlignWidth);
        return HI_FAILURE;
    }
    //SAMPLE_PRT("w:%d, u32AlignWidth:%d\n", CEILING_2_POWER(stSize.u32Width,u32AlignWidth), u32AlignWidth);
    return (CEILING_2_POWER(stSize.u32Width, u32AlignWidth) * \
            CEILING_2_POWER(stSize.u32Height, u32AlignWidth) * \
            ((PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixFmt) ? 2 : 1.5));
}



/******************************************************************************
* function : Set system memory location
******************************************************************************/
HI_S32 SAMPLE_COMM_SYS_MemConfig(HI_VOID)
{
    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    HI_CHAR* pcMmzName;
    MPP_CHN_S stMppChnVI;
    MPP_CHN_S stMppChnVO;
    MPP_CHN_S stMppChnVPSS;
    MPP_CHN_S stMppChnGRP;
    MPP_CHN_S stMppChnVENC;
    MPP_CHN_S stMppChnVDEC;

    /*VI,VDEC最大通道数为32*/
    for (i = 0; i < 32; i++)
    {
        stMppChnVI.enModId = HI_ID_VIU;
        stMppChnVI.s32DevId = 0;
        stMppChnVI.s32ChnId = i;

        stMppChnVDEC.enModId = HI_ID_VDEC;
        stMppChnVDEC.s32DevId = 0;
        stMppChnVDEC.s32ChnId = i;

        if (0 == (i % 2))
        {
            pcMmzName = NULL;
        }
        else
        {
            pcMmzName = "ddr1";
        }

        /*vi*/
        s32Ret = HI_MPI_SYS_SetMemConf(&stMppChnVI, pcMmzName);
        if (s32Ret)
        {
            SAMPLE_PRT("HI_MPI_SYS_SetMemConf ERR !\n");
            return HI_FAILURE;
        }

        /*vdec*/
        s32Ret = HI_MPI_SYS_SetMemConf(&stMppChnVDEC, pcMmzName);
        if (s32Ret)
        {
            SAMPLE_PRT("HI_MPI_SYS_SetMemConf ERR !\n");
            return HI_FAILURE;
        }

    }

    /*the max chn number of vpss,grp,venc is 64*/
    for (i = 0; i < 64; i++)
    {
        stMppChnVPSS.enModId  = HI_ID_VPSS;
        stMppChnVPSS.s32DevId = i;
        stMppChnVPSS.s32ChnId = 0;

        stMppChnGRP.enModId  = HI_ID_GROUP;
        stMppChnGRP.s32DevId = i;
        stMppChnGRP.s32ChnId = 0;

        stMppChnVENC.enModId = HI_ID_VENC;
        stMppChnVENC.s32DevId = 0;
        stMppChnVENC.s32ChnId = i;

        if (0 == (i % 2))
        {
            pcMmzName = NULL;
        }
        else
        {
            pcMmzName = "ddr1";
        }

        /*vpss*/
        s32Ret = HI_MPI_SYS_SetMemConf(&stMppChnVPSS, pcMmzName);
        if (s32Ret)
        {
            SAMPLE_PRT("HI_MPI_SYS_SetMemConf ERR !\n");
            return HI_FAILURE;
        }

        /*grp*/
        s32Ret = HI_MPI_SYS_SetMemConf(&stMppChnGRP, pcMmzName);
        if (s32Ret)
        {
            SAMPLE_PRT("HI_MPI_SYS_SetMemConf ERR !\n");
            return HI_FAILURE;
        }

        /*venc*/
        s32Ret = HI_MPI_SYS_SetMemConf(&stMppChnVENC, pcMmzName);
        if (s32Ret)
        {
            SAMPLE_PRT("HI_MPI_SYS_SetMemConf ERR !\n");
            return HI_FAILURE;
        }

    }

    /*config memory for vo*/
    stMppChnVO.enModId  = HI_ID_VOU;
    stMppChnVO.s32DevId = 0;
    stMppChnVO.s32ChnId = 0;
    s32Ret = HI_MPI_SYS_SetMemConf(&stMppChnVO, "ddr1");
    if (s32Ret)
    {
        SAMPLE_PRT("HI_MPI_SYS_SetMemConf ERR !\n");
        return HI_FAILURE;
    }

    return s32Ret;
}

/******************************************************************************
* function : vb init & MPI system init
******************************************************************************/
HI_S32 SAMPLE_COMM_SYS_Init(VB_CONF_S* pstVbConf)
{
    MPP_SYS_CONF_S stSysConf = {0};
    HI_S32 s32Ret = HI_FAILURE;

    HI_MPI_SYS_Exit();
    HI_MPI_VB_Exit();

    if (NULL == pstVbConf)
    {
        SAMPLE_PRT("input parameter is null, it is invaild!\n");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VB_SetConf(pstVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VB_SetConf failed!\n");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VB_Init();
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VB_Init failed!\n");
        return HI_FAILURE;
    }

    stSysConf.u32AlignWidth = SAMPLE_SYS_ALIGN_WIDTH;
    s32Ret = HI_MPI_SYS_SetConf(&stSysConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_SYS_SetConf failed\n");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_SYS_Init();
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_SYS_Init failed!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/******************************************************************************
* function : vb init & MPI system init
******************************************************************************/
HI_S32 SAMPLE_COMM_SYS_Payload2FilePostfix(PAYLOAD_TYPE_E enPayload, HI_CHAR* szFilePostfix)
{
    if (PT_H264 == enPayload)
    {
        strcpy(szFilePostfix, ".h264");
    }
    else if (PT_JPEG == enPayload)
    {
        strcpy(szFilePostfix, ".jpg");
    }
    else if (PT_MJPEG == enPayload)
    {
        strcpy(szFilePostfix, ".mjp");
    }
    else
    {
        SAMPLE_PRT("payload type err!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

/******************************************************************************
* function : vb exit & MPI system exit
******************************************************************************/
HI_VOID SAMPLE_COMM_SYS_Exit(void)
{
    HI_MPI_SYS_Exit();
    HI_MPI_VB_Exit();
    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

/******************************************************************************
  Some simple Hisilicon Hi35xx vda functions.

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

typedef struct hiVDA_OD_PARAM_S
{
    HI_BOOL bThreadStart;
    VDA_CHN VdaChn;
} VDA_OD_PARAM_S;
typedef struct hiVDA_MD_PARAM_S
{
    HI_BOOL bThreadStart;
    VDA_CHN VdaChn;
} VDA_MD_PARAM_S;

#define SAMPLE_VDA_MD_CHN 0
#define SAMPLE_VDA_OD_CHN 1

static pthread_t gs_VdaPid[2];
static VDA_MD_PARAM_S gs_stMdParam;
static VDA_OD_PARAM_S gs_stOdParam;

/******************************************************************************
* funciton : vda MD mode print -- Md OBJ
******************************************************************************/
HI_S32 SAMPLE_COMM_VDA_MdPrtObj(FILE* fp, VDA_DATA_S* pstVdaData)
{
    VDA_OBJ_S* pstVdaObj;
    HI_S32 i;

    fprintf(fp, "===== %s =====\n", __FUNCTION__);

    if (HI_TRUE != pstVdaData->unData.stMdData.bObjValid)
    {
        fprintf(fp, "bMbObjValid = FALSE.\n");
        return HI_SUCCESS;
    }

    fprintf(fp, "ObjNum=%d, IndexOfMaxObj=%d, SizeOfMaxObj=%d, SizeOfTotalObj=%d\n", \
            pstVdaData->unData.stMdData.stObjData.u32ObjNum, \
            pstVdaData->unData.stMdData.stObjData.u32IndexOfMaxObj, \
            pstVdaData->unData.stMdData.stObjData.u32SizeOfMaxObj, \
            pstVdaData->unData.stMdData.stObjData.u32SizeOfTotalObj);
    for (i = 0; i < pstVdaData->unData.stMdData.stObjData.u32ObjNum; i++)
    {
        pstVdaObj = pstVdaData->unData.stMdData.stObjData.pstAddr + i;
        fprintf(fp, "[%d]\t left=%d, top=%d, right=%d, bottom=%d\n", i, \
                pstVdaObj->u16Left, pstVdaObj->u16Top, \
                pstVdaObj->u16Right, pstVdaObj->u16Bottom);
    }
    fflush(fp);
    return HI_SUCCESS;
}
/******************************************************************************
* funciton : vda MD mode print -- Alarm Pixel Count
******************************************************************************/
HI_S32 SAMPLE_COMM_VDA_MdPrtAp(FILE* fp, VDA_DATA_S* pstVdaData)
{
    fprintf(fp, "===== %s =====\n", __FUNCTION__);

    if (HI_TRUE != pstVdaData->unData.stMdData.bPelsNumValid)
    {
        fprintf(fp, "bMbObjValid = FALSE.\n");
        return HI_SUCCESS;
    }

    fprintf(fp, "AlarmPixelCount=%d\n", pstVdaData->unData.stMdData.u32AlarmPixCnt);
    fflush(fp);
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : vda MD mode print -- SAD
******************************************************************************/
HI_S32 SAMPLE_COMM_VDA_MdPrtSad(FILE* fp, VDA_DATA_S* pstVdaData)
{
    HI_S32 i, j;
    HI_VOID* pAddr;

    fprintf(fp, "===== %s =====\n", __FUNCTION__);
    if (HI_TRUE != pstVdaData->unData.stMdData.bMbSadValid)
    {
        fprintf(fp, "bMbSadValid = FALSE.\n");
        return HI_SUCCESS;
    }

    for (i = 0; i < pstVdaData->u32MbHeight; i++)
    {
        pAddr = (HI_VOID*)((HI_U32)pstVdaData->unData.stMdData.stMbSadData.pAddr
                           + i * pstVdaData->unData.stMdData.stMbSadData.u32Stride);

        for (j = 0; j < pstVdaData->u32MbWidth; j++)
        {
            HI_U8*  pu8Addr;
            HI_U16* pu16Addr;

            if (VDA_MB_SAD_8BIT == pstVdaData->unData.stMdData.stMbSadData.enMbSadBits)
            {
                pu8Addr = (HI_U8*)pAddr + j;

                fprintf(fp, "%-2d ", *pu8Addr);

            }
            else
            {
                pu16Addr = (HI_U16*)pAddr + j;

                fprintf(fp, "%-4d ", *pu16Addr);
            }
        }

        printf("\n");
    }

    fflush(fp);
    return HI_SUCCESS;
}
/******************************************************************************
* funciton : vda MD mode thread process
******************************************************************************/
HI_VOID* SAMPLE_COMM_VDA_MdGetResult(HI_VOID* pdata)
{
    HI_S32 s32Ret;
    VDA_CHN VdaChn;
    VDA_DATA_S stVdaData;
    VDA_MD_PARAM_S* pgs_stMdParam;
    HI_S32 maxfd = 0;
    FILE* fp = stdout;
    HI_S32 VdaFd;
    fd_set read_fds;
    struct timeval TimeoutVal;

    pgs_stMdParam = (VDA_MD_PARAM_S*)pdata;

    VdaChn   = pgs_stMdParam->VdaChn;

    /* decide the stream file name, and open file to save stream */
    /* Set Venc Fd. */
    VdaFd = HI_MPI_VDA_GetFd(VdaChn);
    if (VdaFd < 0)
    {
        SAMPLE_PRT("HI_MPI_VDA_GetFd failed with %#x!\n",
                   VdaFd);
        return NULL;
    }
    if (maxfd <= VdaFd)
    {
        maxfd = VdaFd;
    }
    system("clear");
    while (HI_TRUE == pgs_stMdParam->bThreadStart)
    {
        FD_ZERO(&read_fds);
        FD_SET(VdaFd, &read_fds);

        TimeoutVal.tv_sec  = 2;
        TimeoutVal.tv_usec = 0;
        s32Ret = select(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("select failed!\n");
            break;
        }
        else if (s32Ret == 0)
        {
            SAMPLE_PRT("get vda result time out, exit thread\n");
            break;
        }
        else
        {
            if (FD_ISSET(VdaFd, &read_fds))
            {
                /*******************************************************
                   step 2.3 : call mpi to get one-frame stream
                   *******************************************************/
                s32Ret = HI_MPI_VDA_GetData(VdaChn, &stVdaData, -1);
                if (s32Ret != HI_SUCCESS)
                {
                    SAMPLE_PRT("HI_MPI_VDA_GetData failed with %#x!\n", s32Ret);
                    return NULL;
                }
                /*******************************************************
                   *step 2.4 : save frame to file
                   *******************************************************/
                printf("\033[0;0H");/*move cursor*/
                SAMPLE_COMM_VDA_MdPrtSad(fp, &stVdaData);
                SAMPLE_COMM_VDA_MdPrtObj(fp, &stVdaData);
                SAMPLE_COMM_VDA_MdPrtAp(fp, &stVdaData);
                /*******************************************************
                   *step 2.5 : release stream
                   *******************************************************/
                s32Ret = HI_MPI_VDA_ReleaseData(VdaChn, &stVdaData);
                if (s32Ret != HI_SUCCESS)
                {
                    SAMPLE_PRT("HI_MPI_VDA_ReleaseData failed with %#x!\n", s32Ret);
                    return NULL;
                }
            }
        }
    }

    return HI_NULL;
}

/******************************************************************************
* funciton : vda OD mode thread process
******************************************************************************/
HI_S32 SAMPLE_COMM_VDA_OdPrt(FILE* fp, VDA_DATA_S* pstVdaData)
{
    HI_S32 i;

    fprintf(fp, "===== %s =====\n", __FUNCTION__);
    fprintf(fp, "OD region total count =%d\n", pstVdaData->unData.stOdData.u32RgnNum);
    for (i = 0; i < pstVdaData->unData.stOdData.u32RgnNum; i++)
    {
        fprintf(fp, "OD region[%d]: %d\n", i, pstVdaData->unData.stOdData.abRgnAlarm[i]);
    }
    fflush(fp);
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : vda OD mode thread process
******************************************************************************/
HI_VOID* SAMPLE_COMM_VDA_OdGetResult(HI_VOID* pdata)
{
    HI_S32 i;
    HI_S32 s32Ret;
    VDA_CHN VdaChn;
    VDA_DATA_S stVdaData;
    HI_U32 u32RgnNum;
    VDA_OD_PARAM_S* pgs_stOdParam;
    FILE* fp = stdout;

    pgs_stOdParam = (VDA_OD_PARAM_S*)pdata;

    VdaChn    = pgs_stOdParam->VdaChn;


    while (HI_TRUE == pgs_stOdParam->bThreadStart)
    {
        s32Ret = HI_MPI_VDA_GetData(VdaChn, &stVdaData, -1);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI_MPI_VDA_GetData failed with %#x!\n", s32Ret);
            return NULL;
        }

        SAMPLE_COMM_VDA_OdPrt(fp, &stVdaData);

        u32RgnNum = stVdaData.unData.stOdData.u32RgnNum;

        for (i = 0; i < u32RgnNum; i++)
        {
            if (HI_TRUE == stVdaData.unData.stOdData.abRgnAlarm[i])
            {
                printf("################VdaChn--%d,Rgn--%d,Occ!\n", VdaChn, i);
                s32Ret = HI_MPI_VDA_ResetOdRegion(VdaChn, i);
                if (s32Ret != HI_SUCCESS)
                {
                    SAMPLE_PRT("HI_MPI_VDA_ResetOdRegion failed with %#x!\n", s32Ret);
                    return NULL;
                }
            }
        }

        s32Ret = HI_MPI_VDA_ReleaseData(VdaChn, &stVdaData);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI_MPI_VDA_ReleaseData failed with %#x!\n", s32Ret);
            return NULL;
        }

        usleep(200 * 1000);
    }

    return HI_NULL;
}

/******************************************************************************
* funciton : start vda MD mode
******************************************************************************/
HI_S32 SAMPLE_COMM_VDA_MdStart(VDA_CHN VdaChn, HI_U32 u32Chn, SIZE_S* pstSize)
{
    HI_S32 s32Ret = HI_SUCCESS;
    VDA_CHN_ATTR_S stVdaChnAttr;
    MPP_CHN_S stSrcChn, stDestChn;

    if (VDA_MAX_WIDTH < pstSize->u32Width || VDA_MAX_HEIGHT < pstSize->u32Height)
    {
        SAMPLE_PRT("Picture size invaild!\n");
        return HI_FAILURE;
    }

    /* step 1 create vda channel */
    stVdaChnAttr.enWorkMode = VDA_WORK_MODE_MD;
    stVdaChnAttr.u32Width   = pstSize->u32Width;
    stVdaChnAttr.u32Height  = pstSize->u32Height;

    stVdaChnAttr.unAttr.stMdAttr.enVdaAlg      = VDA_ALG_REF;
    stVdaChnAttr.unAttr.stMdAttr.enMbSize      = VDA_MB_16PIXEL;
    stVdaChnAttr.unAttr.stMdAttr.enMbSadBits   = VDA_MB_SAD_8BIT;
    stVdaChnAttr.unAttr.stMdAttr.enRefMode     = VDA_REF_MODE_DYNAMIC;
    stVdaChnAttr.unAttr.stMdAttr.u32MdBufNum   = 8;
    stVdaChnAttr.unAttr.stMdAttr.u32VdaIntvl   = 4;
    stVdaChnAttr.unAttr.stMdAttr.u32BgUpSrcWgt = 128;
    stVdaChnAttr.unAttr.stMdAttr.u32SadTh      = 100;
    stVdaChnAttr.unAttr.stMdAttr.u32ObjNumMax  = 128;

    s32Ret = HI_MPI_VDA_CreateChn(VdaChn, &stVdaChnAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("err!\n");
        return s32Ret;
    }

    /* step 2: vda chn bind vi chn */
    stSrcChn.enModId  = HI_ID_VPSS;
    stSrcChn.s32ChnId = u32Chn;
    stSrcChn.s32DevId = 0;

    stDestChn.enModId  = HI_ID_VDA;
    stDestChn.s32ChnId = VdaChn;
    stDestChn.s32DevId = 0;

    s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("err, s32Ret: 0x%x!\n", s32Ret);
        return s32Ret;
    }

    /* step 3: vda chn start recv picture */
    s32Ret = HI_MPI_VDA_StartRecvPic(VdaChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("err!\n");
        return s32Ret;
    }
    /* step 4: create thread to get result */
    gs_stMdParam.bThreadStart = HI_TRUE;
    gs_stMdParam.VdaChn   = VdaChn;

    pthread_create(&gs_VdaPid[SAMPLE_VDA_MD_CHN], 0, SAMPLE_COMM_VDA_MdGetResult, (HI_VOID*)&gs_stMdParam);

    return HI_SUCCESS;
}
/******************************************************************************
* funciton : start vda OD mode
******************************************************************************/
HI_S32 SAMPLE_COMM_VDA_OdStart(VDA_CHN VdaChn, HI_U32 u32Chn, SIZE_S* pstSize)
{
    VDA_CHN_ATTR_S stVdaChnAttr;
    MPP_CHN_S stSrcChn, stDestChn;
    HI_S32 s32Ret = HI_SUCCESS;

    if (VDA_MAX_WIDTH < pstSize->u32Width || VDA_MAX_HEIGHT < pstSize->u32Height)
    {
        SAMPLE_PRT("Picture size invaild!\n");
        return HI_FAILURE;
    }

    /********************************************
     step 1 : create vda channel
    ********************************************/
    stVdaChnAttr.enWorkMode = VDA_WORK_MODE_OD;
    stVdaChnAttr.u32Width   = pstSize->u32Width;
    stVdaChnAttr.u32Height  = pstSize->u32Height;

    stVdaChnAttr.unAttr.stOdAttr.enVdaAlg      = VDA_ALG_REF;
    stVdaChnAttr.unAttr.stOdAttr.enMbSize      = VDA_MB_8PIXEL;
    stVdaChnAttr.unAttr.stOdAttr.enMbSadBits   = VDA_MB_SAD_8BIT;
    stVdaChnAttr.unAttr.stOdAttr.enRefMode     = VDA_REF_MODE_DYNAMIC;
    stVdaChnAttr.unAttr.stOdAttr.u32VdaIntvl   = 4;
    stVdaChnAttr.unAttr.stOdAttr.u32BgUpSrcWgt = 128;

    stVdaChnAttr.unAttr.stOdAttr.u32RgnNum = 1;

    stVdaChnAttr.unAttr.stOdAttr.astOdRgnAttr[0].stRect.s32X = 0;
    stVdaChnAttr.unAttr.stOdAttr.astOdRgnAttr[0].stRect.s32Y = 0;
    stVdaChnAttr.unAttr.stOdAttr.astOdRgnAttr[0].stRect.u32Width  = pstSize->u32Width;
    stVdaChnAttr.unAttr.stOdAttr.astOdRgnAttr[0].stRect.u32Height = pstSize->u32Height;

    stVdaChnAttr.unAttr.stOdAttr.astOdRgnAttr[0].u32SadTh      = 100;
    stVdaChnAttr.unAttr.stOdAttr.astOdRgnAttr[0].u32AreaTh     = 60;
    stVdaChnAttr.unAttr.stOdAttr.astOdRgnAttr[0].u32OccCntTh   = 6;
    stVdaChnAttr.unAttr.stOdAttr.astOdRgnAttr[0].u32UnOccCntTh = 2;

    s32Ret = HI_MPI_VDA_CreateChn(VdaChn, &stVdaChnAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("err!\n");
        return (s32Ret);
    }

    /********************************************
     step 2 : bind vda channel to vpss channel
    ********************************************/
    stSrcChn.enModId  = HI_ID_VPSS;
    stSrcChn.s32ChnId = u32Chn;
    stSrcChn.s32DevId = 0;

    stDestChn.enModId  = HI_ID_VDA;
    stDestChn.s32ChnId = VdaChn;
    stDestChn.s32DevId = 0;

    s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("err!\n");
        return s32Ret;
    }

    /* vda start rcv picture */
    s32Ret = HI_MPI_VDA_StartRecvPic(VdaChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("err!\n");
        return (s32Ret);
    }

    /*........*/
    gs_stOdParam.bThreadStart = HI_TRUE;
    gs_stOdParam.VdaChn   = VdaChn;

    pthread_create(&gs_VdaPid[SAMPLE_VDA_OD_CHN], 0, SAMPLE_COMM_VDA_OdGetResult, (HI_VOID*)&gs_stOdParam);

    return HI_SUCCESS;
}
/******************************************************************************
* funciton : stop vda, and stop vda thread -- MD
******************************************************************************/
HI_VOID SAMPLE_COMM_VDA_MdStop(VDA_CHN VdaChn, HI_U32 u32Chn)
{
    HI_S32 s32Ret = HI_SUCCESS;

    MPP_CHN_S stSrcChn, stDestChn;

    /* join thread */
    if (HI_TRUE == gs_stMdParam.bThreadStart)
    {
        gs_stMdParam.bThreadStart = HI_FALSE;
        pthread_join(gs_VdaPid[SAMPLE_VDA_MD_CHN], 0);
    }

    /* vda stop recv picture */
    s32Ret = HI_MPI_VDA_StopRecvPic(VdaChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("err(0x%x)!!!!\n", s32Ret);
    }

    /* unbind vda chn & vi chn */

    stSrcChn.enModId = HI_ID_VPSS;
    stSrcChn.s32ChnId = u32Chn;
    stSrcChn.s32DevId = 0;
    stDestChn.enModId = HI_ID_VDA;
    stDestChn.s32ChnId = VdaChn;
    stDestChn.s32DevId = 0;

    s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("err(0x%x)!!!!\n", s32Ret);
    }

    /* destroy vda chn */
    s32Ret = HI_MPI_VDA_DestroyChn(VdaChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("err(0x%x)!!!!\n", s32Ret);
    }

    return;
}

/******************************************************************************
* funciton : stop vda, and stop vda thread -- OD
******************************************************************************/
HI_VOID SAMPLE_COMM_VDA_OdStop(VDA_CHN VdaChn, HI_U32 u32Chn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn, stDestChn;

    /* join thread */
    if (HI_TRUE == gs_stOdParam.bThreadStart)
    {
        gs_stOdParam.bThreadStart = HI_FALSE;
        pthread_join(gs_VdaPid[SAMPLE_VDA_OD_CHN], 0);
    }

    /* vda stop recv picture */
    s32Ret = HI_MPI_VDA_StopRecvPic(VdaChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("err(0x%x)!!!!\n", s32Ret);
    }

    /* unbind vda chn & vi chn */
    stSrcChn.enModId = HI_ID_VPSS;
    stSrcChn.s32ChnId = u32Chn;
    stSrcChn.s32DevId = 0;
    stDestChn.enModId = HI_ID_VDA;
    stDestChn.s32ChnId = VdaChn;
    stDestChn.s32DevId = 0;
    s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("err(0x%x)!!!!\n", s32Ret);
    }

    /* destroy vda chn */
    s32Ret = HI_MPI_VDA_DestroyChn(VdaChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("err(0x%x)!!!!\n", s32Ret);
    }
    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


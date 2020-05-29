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
#include <semaphore.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <math.h>

#include "sample_comm_ive.h"

typedef struct hiSAMPLE_IVE_OD_S
{
    IVE_SRC_IMAGE_S stSrc;
    IVE_DST_IMAGE_S stInteg;
    IVE_INTEG_CTRL_S stIntegCtrl;
    HI_U32 u32W;
    HI_U32 u32H;
} SAMPLE_IVE_OD_S;

static HI_BOOL s_bStopSignal = HI_FALSE;
static pthread_t s_hIveThread = 0;
static SAMPLE_IVE_OD_S s_stOd;
static SAMPLE_IVE_SWITCH_S s_stOdSwitch = {HI_FALSE,HI_TRUE};
static SAMPLE_VI_CONFIG_S s_stViConfig = {0};

static HI_VOID SAMPLE_IVE_Od_Uninit(SAMPLE_IVE_OD_S* pstOd)
{
    IVE_MMZ_FREE(pstOd->stSrc.au64PhyAddr[0], pstOd->stSrc.au64VirAddr[0]);
    IVE_MMZ_FREE(pstOd->stInteg.au64PhyAddr[0], pstOd->stInteg.au64VirAddr[0]);
}

static HI_S32 SAMPLE_IVE_Od_Init(SAMPLE_IVE_OD_S* pstOd, HI_U32 u32Width, HI_U32 u32Height)
{
    HI_S32 s32Ret = HI_SUCCESS;

    memset(pstOd, 0, sizeof(SAMPLE_IVE_OD_S));

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&pstOd->stSrc, IVE_IMAGE_TYPE_U8C1, u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, OD_INIT_FAIL,
        "Error(%#x),Create src image failed!\n", s32Ret);

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&pstOd->stInteg, IVE_IMAGE_TYPE_U64C1, u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, OD_INIT_FAIL,
        "Error(%#x),Create integ image failed!\n", s32Ret);

    pstOd->stIntegCtrl.enOutCtrl = IVE_INTEG_OUT_CTRL_COMBINE;

    pstOd->u32W = u32Width / IVE_CHAR_CALW;
    pstOd->u32H = u32Height / IVE_CHAR_CALH;


OD_INIT_FAIL:

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_IVE_Od_Uninit(pstOd);
    }
    return s32Ret;

}

static HI_S32 SAMPLE_IVE_Linear2DClassifer(POINT_S* pstChar, HI_S32 s32CharNum,
        POINT_S* pstLinearPoint, HI_S32 s32Linearnum )
{
    HI_S32 s32ResultNum;
    HI_S32 i, j;
    HI_BOOL bTestFlag;
    POINT_S* pstNextLinearPoint;

    s32ResultNum = 0;
    pstNextLinearPoint = &pstLinearPoint[1];
    for (i = 0; i < s32CharNum; i++)
    {
        bTestFlag = HI_TRUE;
        for (j = 0; j < (s32Linearnum - 1); j++)
        {
            if ( ( (pstChar[i].s32Y - pstLinearPoint[j].s32Y) * (pstNextLinearPoint[j].s32X - pstLinearPoint[j].s32X) >
                   (pstChar[i].s32X - pstLinearPoint[j].s32X) * (pstNextLinearPoint[j].s32Y - pstLinearPoint[j].s32Y)
                   && (pstNextLinearPoint[j].s32X != pstLinearPoint[j].s32X))
                 || ( (pstChar[i].s32X > pstLinearPoint[j].s32X) && (pstNextLinearPoint[j].s32X == pstLinearPoint[j].s32X) ))
            {
                bTestFlag = HI_FALSE;
                break;
            }

        }
        if (HI_TRUE == bTestFlag)
        {
            s32ResultNum++;
        }
    }
    return s32ResultNum;
}


static HI_VOID* SAMPLE_IVE_OdProc(HI_VOID* pArgs)
{
    HI_S32 s32Ret;
    HI_U32 i, j;
    SAMPLE_IVE_OD_S* pstOd;
    VIDEO_FRAME_INFO_S stBaseFrmInfo;
    VIDEO_FRAME_INFO_S stExtFrmInfo;
    HI_S32 s32VpssGrp = 0;
    HI_S32 as32VpssChn[] = {VPSS_CHN0, VPSS_CHN1};
    HI_S32 s32MilliSec = 20000;
    IVE_DATA_S stSrc;
    IVE_DATA_S stDst;
    IVE_HANDLE IveHandle;
    HI_BOOL bFinish = HI_FALSE;
    HI_BOOL bBlock = HI_TRUE;
    HI_BOOL bInstant = HI_TRUE;

    POINT_S stChar[IVE_CHAR_NUM];
    POINT_S astPoints[10] = {{0, 0}};
    IVE_LINEAR_DATA_S stIveLinerData;
    HI_U64* pu64VirData = HI_NULL;
    IVE_DMA_CTRL_S stDmaCtrl = {IVE_DMA_MODE_DIRECT_COPY, 0, 0, 0, 0};

    HI_U64 u64TopLeft, u64TopRight, u64BtmLeft, u64BtmRight;
    HI_U64* pu64TopRow, *pu64BtmRow;
    HI_U64 u64BlockSum, u64BlockSq;
    HI_FLOAT fSqVar;
    HI_S32 s32VoLayer = 0;
    HI_S32 s32VoChn = 0;

    pstOd = (SAMPLE_IVE_OD_S*)(pArgs);
    pu64VirData = SAMPLE_COMM_IVE_CONVERT_64BIT_ADDR(HI_U64,pstOd->stInteg.au64VirAddr[0]);

    stIveLinerData.pstLinearPoint = &astPoints[0];
    stIveLinerData.s32LinearNum = 2;
    stIveLinerData.s32ThreshNum = IVE_CHAR_NUM / 2;
    stIveLinerData.pstLinearPoint[0].s32X = 80;
    stIveLinerData.pstLinearPoint[0].s32Y = 0;
    stIveLinerData.pstLinearPoint[1].s32X = 80;
    stIveLinerData.pstLinearPoint[1].s32Y = 20;

    while (HI_FALSE == s_bStopSignal)
    {
        s32Ret = HI_MPI_VPSS_GetChnFrame(s32VpssGrp, as32VpssChn[1], &stExtFrmInfo, s32MilliSec);
        if(HI_SUCCESS != s32Ret)
        {
          SAMPLE_PRT("Error(%#x),HI_MPI_VPSS_GetChnFrame failed, VPSS_GRP(%d), VPSS_CHN(%d)!\n",
              s32Ret,s32VpssGrp, as32VpssChn[1]);
          continue;
        }

        s32Ret = HI_MPI_VPSS_GetChnFrame(s32VpssGrp, as32VpssChn[0], &stBaseFrmInfo, s32MilliSec);
        SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS!=s32Ret, EXT_RELEASE,
          "Error(%#x),HI_MPI_VPSS_GetChnFrame failed, VPSS_GRP(%d), VPSS_CHN(%d)!\n",
          s32Ret,s32VpssGrp, as32VpssChn[0]);

        stSrc.u64VirAddr = stExtFrmInfo.stVFrame.u64VirAddr[0];
        stSrc.u64PhyAddr = stExtFrmInfo.stVFrame.u64PhyAddr[0];
        stSrc.u32Stride = stExtFrmInfo.stVFrame.u32Stride[0];
        stSrc.u32Width = stExtFrmInfo.stVFrame.u32Width;
        stSrc.u32Height = stExtFrmInfo.stVFrame.u32Height;

        stDst.u64VirAddr = pstOd->stSrc.au64VirAddr[0];
        stDst.u64PhyAddr = pstOd->stSrc.au64PhyAddr[0];
        stDst.u32Stride = stExtFrmInfo.stVFrame.u32Stride[0];
        stDst.u32Width =  stExtFrmInfo.stVFrame.u32Width;
        stDst.u32Height = stExtFrmInfo.stVFrame.u32Height;

        bInstant = HI_FALSE;
        s32Ret = HI_MPI_IVE_DMA(&IveHandle, &stSrc, &stDst, &stDmaCtrl, bInstant);
        SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS!=s32Ret, BASE_RELEASE,
            "Error(%#x),HI_MPI_IVE_DMA failed!\n", s32Ret);

        bInstant = HI_TRUE;
        s32Ret = HI_MPI_IVE_Integ(&IveHandle, &pstOd->stSrc, &pstOd->stInteg, &pstOd->stIntegCtrl, bInstant);
        SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS!=s32Ret, BASE_RELEASE,
            "Error(%#x),HI_MPI_IVE_Integ failed!\n", s32Ret);

        s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, bBlock);
        while (HI_ERR_IVE_QUERY_TIMEOUT == s32Ret)
        {
            usleep(100);
            s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, bBlock);
        }

        SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS!=s32Ret, BASE_RELEASE,
            "Error(%#x),HI_MPI_IVE_Query failed!\n", s32Ret);

        for (j = 0; j < IVE_CHAR_CALH; j++)
        {
            pu64TopRow = (0 == j) ? (pu64VirData) : ( pu64VirData + (j * pstOd->u32H - 1) * pstOd->stInteg.au32Stride[0]);
            pu64BtmRow = pu64VirData + ((j + 1) * pstOd->u32H - 1) * pstOd->stInteg.au32Stride[0];

            for (i = 0; i < IVE_CHAR_CALW; i++)
            {
                u64TopLeft  = (0 == j) ? (0) : ((0 == i) ? (0) : (pu64TopRow[i * pstOd->u32W - 1]));
                u64TopRight = (0 == j) ? (0) : (pu64TopRow[(i + 1) * pstOd->u32W - 1]);
                u64BtmLeft  = (0 == i) ? (0) : (pu64BtmRow[i * pstOd->u32W - 1]);
                u64BtmRight = pu64BtmRow[(i + 1) * pstOd->u32W - 1];

                u64BlockSum = (u64TopLeft & 0xfffffffLL) + (u64BtmRight & 0xfffffffLL)
                              - (u64BtmLeft & 0xfffffffLL) - (u64TopRight & 0xfffffffLL);

                u64BlockSq  = (u64TopLeft >> 28) + (u64BtmRight >> 28)
                              - (u64BtmLeft >> 28) - (u64TopRight >> 28);

                // mean
                stChar[j * IVE_CHAR_CALW + i].s32X = u64BlockSum / (pstOd->u32W * pstOd->u32H);
                // sigma=sqrt(1/(w*h)*sum((x(i,j)-mean)^2)= sqrt(sum(x(i,j)^2)/(w*h)-mean^2)
                fSqVar = u64BlockSq / (pstOd->u32W * pstOd->u32H) - stChar[j * IVE_CHAR_CALW + i].s32X * stChar[j * IVE_CHAR_CALW + i].s32X;
                stChar[j * IVE_CHAR_CALW + i].s32Y = (HI_U32)sqrt(fSqVar);
            }
        }

        s32Ret = SAMPLE_IVE_Linear2DClassifer(&stChar[0], IVE_CHAR_NUM,
                                              stIveLinerData.pstLinearPoint, stIveLinerData.s32LinearNum);
        if (s32Ret > stIveLinerData.s32ThreshNum)
        {
            SAMPLE_PRT("\033[0;31m Occlusion detected!\033[0;39m\n");
        }
        else
        {

        }
        s32Ret = HI_MPI_VO_SendFrame(s32VoLayer, s32VoChn, &stBaseFrmInfo,s32MilliSec);
        SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS!=s32Ret, BASE_RELEASE,
            "Error(%#x),SAMPLE_VO_SendFrame failed!\n", s32Ret);

       BASE_RELEASE:
            s32Ret = HI_MPI_VPSS_ReleaseChnFrame(s32VpssGrp,as32VpssChn[0], &stBaseFrmInfo);
            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("Error(%#x),HI_MPI_VPSS_ReleaseChnFrame failed,Grp(%d) chn(%d)!\n",
                    s32Ret,s32VpssGrp,as32VpssChn[0]);
            }

        EXT_RELEASE:
            s32Ret = HI_MPI_VPSS_ReleaseChnFrame(s32VpssGrp,as32VpssChn[1], &stExtFrmInfo);
            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("Error(%#x),HI_MPI_VPSS_ReleaseChnFrame failed,Grp(%d) chn(%d)!\n",
                    s32Ret,s32VpssGrp,as32VpssChn[1]);
            }

    }
    return HI_NULL;
}

HI_VOID SAMPLE_IVE_Od(HI_VOID)
{

    SIZE_S  stSize;
    PIC_SIZE_E enSize = PIC_1080P;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR acThreadName[16] = {0};

    memset(&s_stOd,0,sizeof(s_stOd));
    /******************************************
     step 1: start vi vpss venc vo
     ******************************************/
    s32Ret = SAMPLE_COMM_IVE_StartViVpssVencVo(&s_stViConfig,&s_stOdSwitch,&enSize);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_OD_0,
        "Error(%#x),SAMPLE_COMM_IVE_StartViVpssVencVo failed!\n", s32Ret);

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSize, &stSize);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_OD_0,
        "Error(%#x),SAMPLE_COMM_SYS_GetPicSize failed!\n", s32Ret);
    /******************************************
     step 2: Init OD
     ******************************************/
    s32Ret = SAMPLE_IVE_Od_Init(&s_stOd, stSize.u32Width, stSize.u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_OD_0,
        "SAMPLE_IVE_Od_Init failed, Error(%#x)!\n", s32Ret);
    s_bStopSignal = HI_FALSE;
    /******************************************
     step 3: Create work thread
     ******************************************/
    snprintf(acThreadName, 16, "IVE_OdProc");
    prctl(PR_SET_NAME, (unsigned long)acThreadName, 0,0,0);
    pthread_create(&s_hIveThread, 0, SAMPLE_IVE_OdProc, (HI_VOID*)&s_stOd);

    SAMPLE_PAUSE();

    s_bStopSignal = HI_TRUE;
    pthread_join(s_hIveThread, HI_NULL);
    s_hIveThread = 0;
    SAMPLE_IVE_Od_Uninit(&(s_stOd));
    memset(&s_stOd,0,sizeof(s_stOd));

END_OD_0:
    SAMPLE_COMM_IVE_StopViVpssVencVo(&s_stViConfig,&s_stOdSwitch);
    return ;
}

/******************************************************************************
* function : Od sample signal handle
******************************************************************************/
HI_VOID SAMPLE_IVE_Od_HandleSig(HI_VOID)
{
    s_bStopSignal = HI_TRUE;
    if (0 != s_hIveThread)
    {
        pthread_join(s_hIveThread, HI_NULL);
        s_hIveThread = 0;
    }
    SAMPLE_IVE_Od_Uninit(&(s_stOd));
    memset(&s_stOd,0,sizeof(s_stOd));
    SAMPLE_COMM_IVE_StopViVpssVencVo(&s_stViConfig,&s_stOdSwitch);
}



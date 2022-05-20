
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>

#include "sample_comm.h"
#include "acodec.h"
#include "audio_aac_adp.h"
#include "audio_dl_adp.h"
#include "audio_mp3_adp.h"

#include "hi_resampler_api.h"

#if defined(HI_VQE_USE_STATIC_MODULE_REGISTER)
#include "hi_vqe_register_api.h"
#endif

#ifndef __Mp3decSupport__
    static PAYLOAD_TYPE_E gs_enPayloadType = PT_AAC;
#else
    static PAYLOAD_TYPE_E gs_enPayloadType = PT_MP3;
#endif

HI_BOOL gs_bAioReSample  = HI_FALSE;
static HI_BOOL gs_bUserGetMode  = HI_FALSE;
static HI_BOOL gs_bAoVolumeCtrl = HI_FALSE;
AUDIO_SAMPLE_RATE_E enInSampleRate  = AUDIO_SAMPLE_RATE_BUTT;
static AUDIO_SAMPLE_RATE_E enOutSampleRate = AUDIO_SAMPLE_RATE_BUTT;

/* 0: close, 1: record*/
static HI_U32 u32AiVqeType = 1;

#define SAMPLE_DBG(s32Ret)\
    do{\
        printf("s32Ret=%#x,fuc:%s,line:%d\n", s32Ret, __FUNCTION__, __LINE__);\
    }while(0)

#define SAMPLE_RES_CHECK_NULL_PTR(ptr)\
    do{\
        if(NULL == (HI_U8*)(ptr) )\
        {\
            printf("ptr is NULL,fuc:%s,line:%d\n", __FUNCTION__, __LINE__);\
            return NULL;\
        }\
    }while(0)

#define RES_LIB_NAME "libhive_RES.so"

/******************************************************************************
* function : PT Number to String
******************************************************************************/
static char* SAMPLE_AUDIO_Pt2Str(PAYLOAD_TYPE_E enType)
{
    if (PT_G711A == enType)
    {
        return "g711a";
    }
    else if (PT_G711U == enType)
    {
        return "g711u";
    }
    else if (PT_ADPCMA == enType)
    {
        return "adpcm";
    }
    else if (PT_G726 == enType)
    {
        return "g726";
    }
    else if (PT_LPCM == enType)
    {
        return "pcm";
    }
    else if (PT_AAC == enType)
    {
        return "aac";
    }
    else if (PT_MP3 == enType)
    {
        return "mp3";
    }
    else
    {
        return "data";
    }
}

typedef struct tagSAMPLE_AI2EXTRES_S
{
    HI_BOOL bStart;
    HI_S32  AiDev;
    HI_S32  AiChn;
    AUDIO_SAMPLE_RATE_E enInSample;
    AUDIO_SAMPLE_RATE_E enOutSample;
    HI_U32 u32PerFrame;
    FILE* pfd;
    pthread_t stAiPid;
} SAMPLE_AI2EXTRES_S;

typedef HI_VOID* (*pHI_Resampler_Create_Callback)(HI_S32 s32Inrate, HI_S32 s32Outrate, HI_S32 s32Chans);
typedef HI_S32 (*pHI_Resampler_Process_Callback)(HI_VOID* inst, HI_S16* s16Inbuf, HI_S32 s32Insamps, HI_S16* s16Outbuf);
typedef HI_VOID (*pHI_Resampler_Destroy_Callback)(HI_VOID* inst);
typedef HI_S32 (*pHI_Resampler_GetMaxOutputNum_Callback)(HI_VOID* inst, HI_S32 s32Insamps);

typedef struct
{
    HI_VOID *pLibHandle;

    pHI_Resampler_Create_Callback pHI_Resampler_Create;
    pHI_Resampler_Process_Callback pHI_Resampler_Process;
    pHI_Resampler_Destroy_Callback pHI_Resampler_Destroy;
    pHI_Resampler_GetMaxOutputNum_Callback pHI_Resampler_GetMaxOutputNum;
} SAMPLE_RES_FUN_S;

static SAMPLE_AI2EXTRES_S   gs_stSampleAiExtRes[AI_DEV_MAX_NUM* AI_MAX_CHN_NUM];
static SAMPLE_RES_FUN_S     gs_stSampleResFun = {0};

/******************************************************************************
* function : get frame from Ai, send it  to Resampler
******************************************************************************/
void* SAMPLE_COMM_AUDIO_AiExtResProc(void* parg)
{
    HI_S32 s32Ret;
    HI_S32 AiFd;
    SAMPLE_AI2EXTRES_S* pstAiCtl = (SAMPLE_AI2EXTRES_S*)parg;
    AUDIO_FRAME_S stFrame;
    AEC_FRAME_S   stAecFrm;
    fd_set read_fds;
    struct timeval TimeoutVal;
    AI_CHN_PARAM_S stAiChnPara;
    HI_U32 u32CacheCount = 0;
    HI_S16 s16Cache[16]; /*Max 64/8 * 2.*/
    HI_VOID* hRes;
    HI_S16* ps16OutBuf = HI_NULL;
    HI_U32 u32ProcFrame = 0;
    HI_U32 u32OutSample = 0;

    SAMPLE_RES_CHECK_NULL_PTR(gs_stSampleResFun.pHI_Resampler_Create);
    SAMPLE_RES_CHECK_NULL_PTR(gs_stSampleResFun.pHI_Resampler_Process);
    SAMPLE_RES_CHECK_NULL_PTR(gs_stSampleResFun.pHI_Resampler_Destroy);
    SAMPLE_RES_CHECK_NULL_PTR(gs_stSampleResFun.pHI_Resampler_GetMaxOutputNum);

    s32Ret = HI_MPI_AI_GetChnParam(pstAiCtl->AiDev, pstAiCtl->AiChn, &stAiChnPara);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: Get ai chn param failed\n", __FUNCTION__);
        return NULL;
    }

    stAiChnPara.u32UsrFrmDepth = 30;

    s32Ret = HI_MPI_AI_SetChnParam(pstAiCtl->AiDev, pstAiCtl->AiChn, &stAiChnPara);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: set ai chn param failed\n", __FUNCTION__);
        return NULL;
    }

    /*Create Resample.*/
    /* only support mono channel. */
    hRes = gs_stSampleResFun.pHI_Resampler_Create(pstAiCtl->enInSample, pstAiCtl->enOutSample, 1);

    ps16OutBuf = malloc(gs_stSampleResFun.pHI_Resampler_GetMaxOutputNum(hRes, pstAiCtl->u32PerFrame) * sizeof(HI_S16) + 2);

#if 1
    HI_S32 s32Mulit = 1;
    if (pstAiCtl->enInSample % pstAiCtl->enOutSample == 0)
    {
        s32Mulit = pstAiCtl->enInSample / pstAiCtl->enOutSample;
        if (pstAiCtl->u32PerFrame % s32Mulit == 0)
        {
            s32Mulit = 1;
        }
    }
#endif

    FD_ZERO(&read_fds);
    AiFd = HI_MPI_AI_GetFd(pstAiCtl->AiDev, pstAiCtl->AiChn);
    FD_SET(AiFd, &read_fds);

    while (pstAiCtl->bStart)
    {
        TimeoutVal.tv_sec = 1;
        TimeoutVal.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(AiFd, &read_fds);

        s32Ret = select(AiFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            break;
        }
        else if (0 == s32Ret)
        {
            printf("%s: get ai frame select time out\n", __FUNCTION__);
            break;
        }

        if (FD_ISSET(AiFd, &read_fds))
        {
            /* get frame from ai chn */
            memset(&stAecFrm, 0, sizeof(AEC_FRAME_S));
            s32Ret = HI_MPI_AI_GetFrame(pstAiCtl->AiDev, pstAiCtl->AiChn, &stFrame, &stAecFrm, HI_FALSE);
            if (HI_SUCCESS != s32Ret )
            {
#if 0
                printf("%s: HI_MPI_AI_GetFrame(%d, %d), failed with %#x!\n", \
                       __FUNCTION__, pstAiCtl->AiDev, pstAiCtl->AiChn, s32Ret);
                pstAiCtl->bStart = HI_FALSE;
                return NULL;
#else
                continue;
#endif
            }

            /* send frame to encoder */
            if (1 != s32Mulit)
            {
                if (0 != u32CacheCount)
                {
                    memcpy(s16Cache + u32CacheCount*sizeof(HI_S16), stFrame.u64VirAddr[0], (s32Mulit - u32CacheCount)*sizeof(HI_S16));
                    u32OutSample = gs_stSampleResFun.pHI_Resampler_Process(hRes, s16Cache, s32Mulit, ps16OutBuf);
                    (HI_VOID)fwrite(ps16OutBuf, 2, u32OutSample, pstAiCtl->pfd);
                }
                u32ProcFrame = pstAiCtl->u32PerFrame - (s32Mulit - u32CacheCount);
                u32ProcFrame = u32ProcFrame - u32ProcFrame % s32Mulit;
                u32OutSample = gs_stSampleResFun.pHI_Resampler_Process(hRes, (HI_S16*)(stFrame.u64VirAddr[0] + (s32Mulit - u32CacheCount)*sizeof(HI_S16)), u32ProcFrame, ps16OutBuf);
                (HI_VOID)fwrite(ps16OutBuf, 2, u32OutSample, pstAiCtl->pfd);

                if ((pstAiCtl->u32PerFrame - u32ProcFrame - (s32Mulit - u32CacheCount)) != 0)
                {
                    memcpy(s16Cache, stFrame.u64VirAddr[0] + (s32Mulit - u32CacheCount + u32ProcFrame)*sizeof(HI_S16), \
                        (pstAiCtl->u32PerFrame - u32ProcFrame - (s32Mulit - u32CacheCount))*sizeof(HI_S16));
                }
                u32CacheCount = pstAiCtl->u32PerFrame - u32ProcFrame - (s32Mulit - u32CacheCount);
            }
            else
            {
                u32OutSample = gs_stSampleResFun.pHI_Resampler_Process(hRes, (HI_S16*)stFrame.u64VirAddr[0], pstAiCtl->u32PerFrame, ps16OutBuf);
                (HI_VOID)fwrite(ps16OutBuf, 2, u32OutSample, pstAiCtl->pfd);
            }

            fflush(pstAiCtl->pfd);

            /* finally you must release the stream */
            s32Ret = HI_MPI_AI_ReleaseFrame(pstAiCtl->AiDev, pstAiCtl->AiChn, &stFrame, &stAecFrm);
            if (HI_SUCCESS != s32Ret )
            {
                printf("%s: HI_MPI_AI_ReleaseFrame(%d, %d), failed with %#x!\n", \
                       __FUNCTION__, pstAiCtl->AiDev, pstAiCtl->AiChn, s32Ret);
                pstAiCtl->bStart = HI_FALSE;
                free(ps16OutBuf);
                gs_stSampleResFun.pHI_Resampler_Destroy(hRes);
                return NULL;
            }
        }
    }

    pstAiCtl->bStart = HI_FALSE;
    free(ps16OutBuf);
    gs_stSampleResFun.pHI_Resampler_Destroy(hRes);

    return NULL;
}

/******************************************************************************
* function : Create the thread to get frame from ai and send to aenc
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_CreatTrdAiExtRes(AIO_ATTR_S* pstAioAttr, AUDIO_DEV AiDev, AI_CHN AiChn,  AUDIO_SAMPLE_RATE_E enOutSampleRate, FILE* pResFd)
{
    SAMPLE_AI2EXTRES_S* pstAi2ExtRes = NULL;

    pstAi2ExtRes = &gs_stSampleAiExtRes[AiDev * AI_MAX_CHN_NUM + AiChn];
    pstAi2ExtRes->AiDev = AiDev;
    pstAi2ExtRes->AiChn = AiChn;
    pstAi2ExtRes->enInSample = pstAioAttr->enSamplerate;
    pstAi2ExtRes->enOutSample = enOutSampleRate;
    pstAi2ExtRes->u32PerFrame = pstAioAttr->u32PtNumPerFrm;
    pstAi2ExtRes->pfd = pResFd;
    pstAi2ExtRes->bStart = HI_TRUE;
    pthread_create(&pstAi2ExtRes->stAiPid, 0, SAMPLE_COMM_AUDIO_AiExtResProc, pstAi2ExtRes);

    return HI_SUCCESS;
}

/******************************************************************************
* function : Destory the thread to get frame from ai and send to extern resamler
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_DestoryTrdAiExtRes(AUDIO_DEV AiDev, AI_CHN AiChn)
{
    SAMPLE_AI2EXTRES_S* pstAi = NULL;

    pstAi = &gs_stSampleAiExtRes[AiDev * AI_MAX_CHN_NUM + AiChn];
    if (pstAi->bStart)
    {
        pstAi->bStart = HI_FALSE;
        pthread_join(pstAi->stAiPid, 0);
    }

    if (pstAi->pfd != HI_NULL) {
        fclose(pstAi->pfd);
        pstAi->pfd = HI_NULL;
    }

    return HI_SUCCESS;
}

/******************************************************************************
* function : Add dynamic load path
******************************************************************************/
static HI_VOID SAMPLE_AUDIO_AddLibPath(HI_VOID)
{
    HI_S32 s32Ret;
    HI_CHAR aszLibPath[FILE_NAME_LEN] = {0};
#ifdef __HuaweiLite__
    snprintf(aszLibPath, FILE_NAME_LEN, "/sharefs/");
#else
#endif
    s32Ret = Audio_Dlpath(aszLibPath);
    if(HI_SUCCESS != s32Ret)
    {
       printf("%s: add lib path %s failed\n", __FUNCTION__, aszLibPath);
    }
    return;
}

/******************************************************************************
* function : DeInit resamle functions
******************************************************************************/
static HI_S32 SAMPLE_AUDIO_DeInitExtResFun(HI_VOID)
{
#if !defined(HI_VQE_USE_STATIC_MODULE_REGISTER)
    if(HI_NULL != gs_stSampleResFun.pLibHandle)
    {
        Audio_Dlclose(gs_stSampleResFun.pLibHandle);
        memset(&gs_stSampleResFun, 0, sizeof(SAMPLE_RES_FUN_S));
    }
#endif

    return HI_SUCCESS;
}

/******************************************************************************
* function : Init resamle functions
******************************************************************************/
static HI_S32 SAMPLE_AUDIO_InitExtResFun(HI_VOID)
{
    HI_S32 s32Ret;
    SAMPLE_RES_FUN_S stSampleResFun;

    s32Ret = SAMPLE_AUDIO_DeInitExtResFun();
    if(HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n",
            __FUNCTION__, __LINE__, "Unload resample lib fail!\n");
        return HI_FAILURE;
    }

    memset(&stSampleResFun, 0, sizeof(SAMPLE_RES_FUN_S));

#if defined(HI_VQE_USE_STATIC_MODULE_REGISTER)
    stSampleResFun.pHI_Resampler_Create = HI_Resampler_Create;
    stSampleResFun.pHI_Resampler_Process = HI_Resampler_Process;
    stSampleResFun.pHI_Resampler_Destroy = HI_Resampler_Destroy;
    stSampleResFun.pHI_Resampler_GetMaxOutputNum = HI_Resampler_GetMaxOutputNum;
#else
    s32Ret = Audio_Dlopen(&(stSampleResFun.pLibHandle), RES_LIB_NAME);
    if(HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n",
            __FUNCTION__, __LINE__, "load resample lib fail!\n");
        return HI_FAILURE;
    }

    s32Ret = Audio_Dlsym((HI_VOID** )&(stSampleResFun.pHI_Resampler_Create), stSampleResFun.pLibHandle, "HI_Resampler_Create");
    if(HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n",
            __FUNCTION__, __LINE__, "find symbol error!\n");
        return HI_FAILURE;
    }

    s32Ret = Audio_Dlsym((HI_VOID** )&(stSampleResFun.pHI_Resampler_Process), stSampleResFun.pLibHandle, "HI_Resampler_Process");
    if(HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n",
            __FUNCTION__, __LINE__, "find symbol error!\n");
        return HI_FAILURE;
    }

    s32Ret = Audio_Dlsym((HI_VOID** )&(stSampleResFun.pHI_Resampler_Destroy), stSampleResFun.pLibHandle, "HI_Resampler_Destroy");
    if(HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n",
            __FUNCTION__, __LINE__, "find symbol error!\n");
        return HI_FAILURE;
    }

    s32Ret = Audio_Dlsym((HI_VOID** )&(stSampleResFun.pHI_Resampler_GetMaxOutputNum), stSampleResFun.pLibHandle, "HI_Resampler_GetMaxOutputNum");
    if(HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n",
            __FUNCTION__, __LINE__, "find symbol error!\n");
        return HI_FAILURE;
    }
#endif

    memcpy(&gs_stSampleResFun, &stSampleResFun, sizeof(SAMPLE_RES_FUN_S));

    return HI_SUCCESS;
}

/******************************************************************************
* function : Open Aenc File
******************************************************************************/
static FILE* SAMPLE_AUDIO_OpenAencFile(AENC_CHN AeChn, PAYLOAD_TYPE_E enType)
{
    FILE* pfd;
    HI_CHAR aszFileName[FILE_NAME_LEN] = {0};

    /* create file for save stream*/
#ifdef __HuaweiLite__
    snprintf(aszFileName, FILE_NAME_LEN, "/sharefs/audio_chn%d.%s", AeChn, SAMPLE_AUDIO_Pt2Str(enType));
#else
    snprintf(aszFileName, FILE_NAME_LEN, "audio_chn%d.%s", AeChn, SAMPLE_AUDIO_Pt2Str(enType));
#endif
    pfd = fopen(aszFileName, "w+");
    if (NULL == pfd)
    {
        printf("%s: open file %s failed\n", __FUNCTION__, aszFileName);
        return NULL;
    }
    printf("open stream file:\"%s\" for aenc ok\n", aszFileName);
    return pfd;
}

/******************************************************************************
* function : Open ExtResample File
******************************************************************************/
static FILE* SAMPLE_AUDIO_OpenResFile(AI_CHN AiChn)
{
    FILE* pfd;
    HI_CHAR aszFileName[FILE_NAME_LEN] = {0};

    /* create file for save stream*/
#ifdef __HuaweiLite__
    snprintf(aszFileName, FILE_NAME_LEN, "/sharefs/Res_chn%d.pcm", AiChn);
#else
    snprintf(aszFileName, FILE_NAME_LEN, "Res_chn%d.pcm", AiChn);
#endif
    pfd = fopen(aszFileName, "w+");
    if (NULL == pfd)
    {
        printf("%s: open file %s failed\n", __FUNCTION__, aszFileName);
        return NULL;
    }
    printf("open stream file:\"%s\" for Resample ok\n", aszFileName);
    return pfd;
}

/******************************************************************************
* function : Open Adec File
******************************************************************************/
static FILE* SAMPLE_AUDIO_OpenAdecFile(ADEC_CHN AdChn, PAYLOAD_TYPE_E enType)
{
    FILE* pfd;
    HI_CHAR aszFileName[FILE_NAME_LEN] = {0};

    /* create file for save stream*/
#ifdef __HuaweiLite__
    snprintf(aszFileName, FILE_NAME_LEN, "/sharefs/audio_chn%d.%s", AdChn, SAMPLE_AUDIO_Pt2Str(enType));
#else
    snprintf(aszFileName, FILE_NAME_LEN, "audio_chn%d.%s", AdChn, SAMPLE_AUDIO_Pt2Str(enType));
#endif

    pfd = fopen(aszFileName, "rb");
    if (NULL == pfd)
    {
        printf("%s: open file %s failed\n", __FUNCTION__, aszFileName);
        return NULL;
    }
    printf("open stream file:\"%s\" for adec ok\n", aszFileName);
    return pfd;
}


/******************************************************************************
* function : file -> Adec -> Ao
******************************************************************************/
HI_S32 SAMPLE_AUDIO_AdecAo(gsf_mpp_frm_attr_t *attr)
{
    HI_S32      s32Ret;
    AO_CHN      AoChn = 0;
    ADEC_CHN    AdChn = 0;
    HI_S32      s32AoChnCnt;
    FILE*        pfd = NULL;
    AIO_ATTR_S stAioAttr;

#ifdef HI_ACODEC_TYPE_TLV320AIC31
    AUDIO_DEV   AoDev = SAMPLE_AUDIO_EXTERN_AO_DEV;
    stAioAttr.enSamplerate   = AUDIO_SAMPLE_RATE_48000;
    stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
    stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode    = AUDIO_SOUND_MODE_STEREO;
    stAioAttr.u32EXFlag      = 0;
    stAioAttr.u32FrmNum      = 30;
    stAioAttr.u32PtNumPerFrm = AACLC_SAMPLES_PER_FRAME;
    stAioAttr.u32ChnCnt      = 2;
    stAioAttr.u32ClkSel      = 1;
    stAioAttr.enI2sType      = AIO_I2STYPE_EXTERN;
#else
    AUDIO_DEV   AoDev = SAMPLE_AUDIO_INNER_HDMI_AO_DEV;//SAMPLE_AUDIO_INNER_AO_DEV;
    stAioAttr.enSamplerate   = AUDIO_SAMPLE_RATE_48000;
    stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
    stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode    = AUDIO_SOUND_MODE_STEREO;
    stAioAttr.u32EXFlag      = 0;
    stAioAttr.u32FrmNum      = 30;
    stAioAttr.u32PtNumPerFrm = AACLC_SAMPLES_PER_FRAME;
    stAioAttr.u32ChnCnt      = 2;
    stAioAttr.u32ClkSel      = 0;
    stAioAttr.enI2sType      = (AoDev == SAMPLE_AUDIO_INNER_AO_DEV)?AIO_I2STYPE_INNERCODEC:AIO_I2STYPE_INNERHDMI;
#endif

    gs_bAioReSample = HI_FALSE;
    enInSampleRate  = AUDIO_SAMPLE_RATE_BUTT;
    enOutSampleRate = AUDIO_SAMPLE_RATE_BUTT;

    s32Ret = SAMPLE_COMM_AUDIO_StartAdec(AdChn, gs_enPayloadType);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        goto ADECAO_ERR3;
    }

    s32AoChnCnt = stAioAttr.u32ChnCnt;
    s32Ret = SAMPLE_COMM_AUDIO_StartAo(AoDev, s32AoChnCnt, &stAioAttr, enInSampleRate, gs_bAioReSample);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        goto ADECAO_ERR2;
    }

    s32Ret = SAMPLE_COMM_AUDIO_CfgAcodec(&stAioAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        goto ADECAO_ERR1;
    }

    s32Ret = SAMPLE_COMM_AUDIO_AoBindAdec(AoDev, AoChn, AdChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        goto ADECAO_ERR1;
    }

    //maohw
    if(attr)
      return s32Ret;


    pfd = SAMPLE_AUDIO_OpenAdecFile(AdChn, gs_enPayloadType);
    if (!pfd)
    {
        SAMPLE_DBG(HI_FAILURE);
        goto ADECAO_ERR0;
    }
    s32Ret = SAMPLE_COMM_AUDIO_CreatTrdFileAdec(AdChn, pfd);
    if (s32Ret != HI_SUCCESS)
    {
        fclose(pfd);
        pfd = HI_NULL;
        SAMPLE_DBG(s32Ret);
        goto ADECAO_ERR0;
    }

    printf("bind adec:%d to ao(%d,%d) ok \n", AdChn, AoDev, AoChn);

    printf("\nplease press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    s32Ret = SAMPLE_COMM_AUDIO_DestoryTrdFileAdec(AdChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        return HI_FAILURE;
    }

ADECAO_ERR0:
    s32Ret = SAMPLE_COMM_AUDIO_AoUnbindAdec(AoDev, AoChn, AdChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
    }

ADECAO_ERR1:
    s32Ret |= SAMPLE_COMM_AUDIO_StopAo(AoDev, s32AoChnCnt, gs_bAioReSample);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
    }
ADECAO_ERR2:
    s32Ret |= SAMPLE_COMM_AUDIO_StopAdec(AdChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
    }

ADECAO_ERR3:
    return s32Ret;
}

/******************************************************************************
* function : Ai -> Aenc -> file
*                       -> Adec -> Ao
******************************************************************************/
HI_S32 SAMPLE_AUDIO_AiAenc(gsf_mpp_aenc_t *aenc)
{
    HI_S32 i, j, s32Ret;
    AI_CHN      AiChn = 0;
    AO_CHN      AoChn = 0;
    ADEC_CHN    AdChn = 0;
    HI_S32      s32AiChnCnt;
    HI_S32      s32AoChnCnt;
    HI_S32      s32AencChnCnt;
    AENC_CHN    AeChn = 0;
    HI_BOOL     bSendAdec = (aenc)?HI_FALSE:HI_TRUE;//maohw HI_TRUE;
    FILE*        pfd = NULL;
    AIO_ATTR_S stAioAttr;

#ifdef HI_ACODEC_TYPE_TLV320AIC31
    AUDIO_DEV   AiDev = SAMPLE_AUDIO_EXTERN_AI_DEV;
    AUDIO_DEV   AoDev = SAMPLE_AUDIO_EXTERN_AO_DEV;
    stAioAttr.enSamplerate   = AUDIO_SAMPLE_RATE_48000;
    stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
    stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode    = AUDIO_SOUND_MODE_STEREO;
    stAioAttr.u32EXFlag      = 0;
    stAioAttr.u32FrmNum      = 30;
    stAioAttr.u32PtNumPerFrm = AACLC_SAMPLES_PER_FRAME;
    stAioAttr.u32ChnCnt      = 2;
    stAioAttr.u32ClkSel      = 1;
    stAioAttr.enI2sType      = AIO_I2STYPE_EXTERN;
#else
    AUDIO_DEV   AiDev = SAMPLE_AUDIO_INNER_AI_DEV;
    AUDIO_DEV   AoDev = SAMPLE_AUDIO_INNER_AO_DEV;
    stAioAttr.enSamplerate   = aenc->sp;//AUDIO_SAMPLE_RATE_48000;
    stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
    stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode    = aenc->stereo;//AUDIO_SOUND_MODE_STEREO;
    stAioAttr.u32EXFlag      = 0;
    stAioAttr.u32FrmNum      = 30;
    stAioAttr.u32PtNumPerFrm = (aenc->enPayLoad==PT_AAC)?AACLC_SAMPLES_PER_FRAME:80*6;//AACLC_SAMPLES_PER_FRAME;
    stAioAttr.u32ChnCnt      = 1<<aenc->stereo;
    stAioAttr.u32ClkSel      = 0;
    stAioAttr.enI2sType      = AIO_I2STYPE_INNERCODEC;
#endif
    gs_bAioReSample = HI_FALSE;
    enInSampleRate  = AUDIO_SAMPLE_RATE_BUTT;
    enOutSampleRate = AUDIO_SAMPLE_RATE_BUTT;


    // for RECORDVQE;
    AI_RECORDVQE_CONFIG_S stAiVqeRecordAttr;
    HI_VOID     *pAiVqeAttr = NULL;
    
    //maohw;
    gs_enPayloadType = aenc->enPayLoad;
    u32AiVqeType = stAioAttr.enSoundmode;
    
    if (1 == u32AiVqeType)
    {
        memset(&stAiVqeRecordAttr, 0, sizeof(AI_RECORDVQE_CONFIG_S));
        stAiVqeRecordAttr.s32WorkSampleRate    = AUDIO_SAMPLE_RATE_48000;
        stAiVqeRecordAttr.s32FrameSample       = AACLC_SAMPLES_PER_FRAME;
        stAiVqeRecordAttr.enWorkstate          = VQE_WORKSTATE_COMMON; //VQE_WORKSTATE_MUSIC;//
        stAiVqeRecordAttr.s32InChNum           = 2;
        stAiVqeRecordAttr.s32OutChNum          = 2;
        stAiVqeRecordAttr.enRecordType         = VQE_RECORD_NORMAL;
        
        stAiVqeRecordAttr.stHpfCfg.bUsrMode    = HI_TRUE;
        stAiVqeRecordAttr.stHpfCfg.enHpfFreq   = AUDIO_HPF_FREQ_80;
        stAiVqeRecordAttr.stDrcCfg.bUsrMode    = HI_FALSE;
        stAiVqeRecordAttr.stRnrCfg.bUsrMode    = HI_FALSE;
        stAiVqeRecordAttr.stHdrCfg.bUsrMode    = HI_FALSE;
        
        
        stAiVqeRecordAttr.stAgcCfg.bUsrMode          = HI_TRUE; /* mode 0: auto, mode 1: manual. */
        stAiVqeRecordAttr.stAgcCfg.s8TargetLevel     = -2;      /* target voltage level, range: [-40, -1]dB */
        stAiVqeRecordAttr.stAgcCfg.s8NoiseFloor      = -40;     /* noise floor, range: TalkVqe/AoVqe[-65, -20]dB, RecordVqe[-50, -20]dB */
        stAiVqeRecordAttr.stAgcCfg.s8MaxGain         = 8;      /* max gain, range: [0, 30]dB */
        stAiVqeRecordAttr.stAgcCfg.s8AdjustSpeed     = 2;      /* adjustable speed, range: [0, 10]dB/s */
        stAiVqeRecordAttr.stAgcCfg.s8ImproveSNR      = 0;       /* switch for improving SNR, range: [0:close, 1:upper limit 3dB, 2:upper limit 6dB] */
        stAiVqeRecordAttr.stAgcCfg.s8UseHighPassFilt = 0;    /* switch for using high pass filt, range: [0:close, 1:80Hz, 2:120Hz, 3:150:Hz, 4:300Hz: 5:500Hz] */
        stAiVqeRecordAttr.stAgcCfg.s8OutputMode      = 0;    /* output mode, mute when lower than noise floor, range: [0:close, 1:open] */
        stAiVqeRecordAttr.stAgcCfg.s16NoiseSupSwitch = 0;    /* switch for noise suppression, range: [0:close, 1:open] */
       

        //stAiVqeRecordAttr.u32OpenMask = AI_RECORDVQE_MASK_DRC | AI_RECORDVQE_MASK_HDR | AI_RECORDVQE_MASK_HPF | AI_RECORDVQE_MASK_RNR;
        stAiVqeRecordAttr.u32OpenMask = AI_RECORDVQE_MASK_AGC | AI_RECORDVQE_MASK_HPF;
        pAiVqeAttr = (HI_VOID *)&stAiVqeRecordAttr;
    }
    else
    {
        pAiVqeAttr = HI_NULL;
    }

    /********************************************
      step 1: start Ai
    ********************************************/
    s32AiChnCnt = stAioAttr.u32ChnCnt;
    s32Ret = SAMPLE_COMM_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr, enOutSampleRate, gs_bAioReSample, pAiVqeAttr, u32AiVqeType);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        goto AIAENC_ERR6;
    }

    if (1 == u32AiVqeType)
    {
      // Hi3559AV100ES/Hi3559AV100/Hi3519AV100/Hi3516CV500/Hi3516EV200 不支持此接口
      // 建议用户使用内置Audio Codec和AO的音量调节功能。
      HI_S32 s32VolumeDb = 6; //[-20,10]
      HI_MPI_AI_SetVqeVolume(AiDev, AiChn, s32VolumeDb);
    }

    /********************************************
      step 2: config audio codec
    ********************************************/
    s32Ret = SAMPLE_COMM_AUDIO_CfgAcodec(&stAioAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        goto AIAENC_ERR5;
    }

    /********************************************
      step 3: start Aenc
    ********************************************/
    s32AencChnCnt = stAioAttr.u32ChnCnt >> stAioAttr.enSoundmode;
    s32Ret = SAMPLE_COMM_AUDIO_StartAenc(s32AencChnCnt, &stAioAttr, gs_enPayloadType);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        goto AIAENC_ERR5;
    }

    /********************************************
      step 4: Aenc bind Ai Chn
    ********************************************/
    for (i = 0; i < s32AencChnCnt; i++)
    {
        AeChn = i;
        AiChn = i;

        if (HI_TRUE == gs_bUserGetMode)
        {
            s32Ret = SAMPLE_COMM_AUDIO_CreatTrdAiAenc(AiDev, AiChn, AeChn);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_DBG(s32Ret);
                for (j=0; j<i; j++)
                {
                    SAMPLE_COMM_AUDIO_DestoryTrdAi(AiDev, j);
                }
                goto AIAENC_ERR4;
            }
        }
        else
        {
            s32Ret = SAMPLE_COMM_AUDIO_AencBindAi(AiDev, AiChn, AeChn);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_DBG(s32Ret);
                for (j=0; j<i; j++)
                {
                    SAMPLE_COMM_AUDIO_AencUnbindAi(AiDev, j, j);
                }
                goto AIAENC_ERR4;
            }
        }
        printf("Ai(%d,%d) bind to AencChn:%d ok!\n", AiDev , AiChn, AeChn);
    }

    /********************************************
      step 5: start Adec & Ao. ( if you want )
    ********************************************/
    if (HI_TRUE == bSendAdec)
    {
        s32Ret = SAMPLE_COMM_AUDIO_StartAdec(AdChn, gs_enPayloadType);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_DBG(s32Ret);
            goto AIAENC_ERR3;
        }

        s32AoChnCnt = stAioAttr.u32ChnCnt;
        s32Ret = SAMPLE_COMM_AUDIO_StartAo(AoDev, s32AoChnCnt, &stAioAttr, enInSampleRate, gs_bAioReSample);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_DBG(s32Ret);
            goto AIAENC_ERR2;
        }

        pfd = SAMPLE_AUDIO_OpenAencFile(AdChn, gs_enPayloadType);
        if (!pfd)
        {
            SAMPLE_DBG(HI_FAILURE);
            goto AIAENC_ERR1;
        }
        s32Ret = SAMPLE_COMM_AUDIO_CreatTrdAencAdec(AeChn, AdChn, pfd);
        if (s32Ret != HI_SUCCESS)
        {
            fclose(pfd);
            pfd = HI_NULL;
            SAMPLE_DBG(s32Ret);
            goto AIAENC_ERR1;
        }

        s32Ret = SAMPLE_COMM_AUDIO_AoBindAdec(AoDev, AoChn, AdChn);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_DBG(s32Ret);
            goto AIAENC_ERR0;
        }

        printf("bind adec:%d to ao(%d,%d) ok \n", AdChn, AoDev, AoChn);
    }
    else
    {
        s32Ret = SAMPLE_COMM_AUDIO_CreatTrdAencAdecCb(AeChn, aenc->cb, aenc->uargs);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_DBG(s32Ret);
            goto AIAENC_ERR3;
        }
        
        printf(" SAMPLE_COMM_AUDIO_CreatTrdAencAdecCb AeChn:%d, ok \n", AeChn);
        return s32Ret;
    }


    /********************************************
      step 6: exit the process
    ********************************************/
    if (HI_TRUE == bSendAdec)
    {
        printf("\nplease press twice ENTER to exit this sample\n");
        getchar();
        getchar();
      
        s32Ret = SAMPLE_COMM_AUDIO_AoUnbindAdec(AoDev, AoChn, AdChn);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_DBG(s32Ret);
            return HI_FAILURE;
        }

AIAENC_ERR0:
        s32Ret = SAMPLE_COMM_AUDIO_DestoryTrdAencAdec(AdChn);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_DBG(s32Ret);
        }

AIAENC_ERR1:
        s32Ret |= SAMPLE_COMM_AUDIO_StopAo(AoDev, s32AoChnCnt, gs_bAioReSample);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_DBG(s32Ret);
        }

AIAENC_ERR2:
        s32Ret |= SAMPLE_COMM_AUDIO_StopAdec(AdChn);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_DBG(s32Ret);
        }

    }

AIAENC_ERR3:
    for (i = 0; i < s32AencChnCnt; i++)
    {
        AeChn = i;
        AiChn = i;

        if (HI_TRUE == gs_bUserGetMode)
        {
            s32Ret |= SAMPLE_COMM_AUDIO_DestoryTrdAi(AiDev, AiChn);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_DBG(s32Ret);
            }
        }
        else
        {
            s32Ret |= SAMPLE_COMM_AUDIO_AencUnbindAi(AiDev, AiChn, AeChn);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_DBG(s32Ret);
            }
        }
    }

AIAENC_ERR4:
    s32Ret |= SAMPLE_COMM_AUDIO_StopAenc(s32AencChnCnt);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
    }

AIAENC_ERR5:
    s32Ret |= SAMPLE_COMM_AUDIO_StopAi(AiDev, s32AiChnCnt, gs_bAioReSample, HI_FALSE);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
    }

AIAENC_ERR6:

    return s32Ret;
}

HI_S32 SAMPLE_AUDIO_AiAencStop(gsf_mpp_aenc_t *aenc)
{
    HI_S32 i, j, s32Ret;
    AI_CHN      AiChn;
    AENC_CHN    AeChn = 0;
    HI_S32      s32AiChnCnt;
    HI_S32      s32AencChnCnt;
    AIO_ATTR_S stAioAttr;
    
#ifdef HI_ACODEC_TYPE_TLV320AIC31
    AUDIO_DEV   AiDev = SAMPLE_AUDIO_EXTERN_AI_DEV;
    AUDIO_DEV   AoDev = SAMPLE_AUDIO_EXTERN_AO_DEV;
    stAioAttr.enSamplerate   = AUDIO_SAMPLE_RATE_48000;
    stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
    stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode    = AUDIO_SOUND_MODE_STEREO;
    stAioAttr.u32EXFlag      = 0;
    stAioAttr.u32FrmNum      = 30;
    stAioAttr.u32PtNumPerFrm = AACLC_SAMPLES_PER_FRAME;
    stAioAttr.u32ChnCnt      = 2;
    stAioAttr.u32ClkSel      = 1;
    stAioAttr.enI2sType      = AIO_I2STYPE_EXTERN;
#else
    AUDIO_DEV   AiDev = SAMPLE_AUDIO_INNER_AI_DEV;
    AUDIO_DEV   AoDev = SAMPLE_AUDIO_INNER_AO_DEV;
    stAioAttr.enSamplerate   = aenc->sp;//AUDIO_SAMPLE_RATE_48000;
    stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
    stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode    = aenc->stereo;//AUDIO_SOUND_MODE_STEREO;
    stAioAttr.u32EXFlag      = 0;
    stAioAttr.u32FrmNum      = 30;
    stAioAttr.u32PtNumPerFrm = (aenc->enPayLoad==PT_AAC)?AACLC_SAMPLES_PER_FRAME:80*6;//AACLC_SAMPLES_PER_FRAME;
    stAioAttr.u32ChnCnt      = 1<<aenc->stereo;
    stAioAttr.u32ClkSel      = 0;
    stAioAttr.enI2sType      = AIO_I2STYPE_INNERCODEC;
#endif
    gs_bAioReSample = HI_FALSE;
    enInSampleRate  = AUDIO_SAMPLE_RATE_BUTT;
    enOutSampleRate = AUDIO_SAMPLE_RATE_BUTT;
    
    s32AiChnCnt = stAioAttr.u32ChnCnt;
    s32AencChnCnt = stAioAttr.u32ChnCnt >> stAioAttr.enSoundmode;
    
    for (i = 0; i < s32AencChnCnt; i++)
    {
        AeChn = i;
        AiChn = i;

        if (HI_TRUE == gs_bUserGetMode)
        {
            s32Ret |= SAMPLE_COMM_AUDIO_DestoryTrdAi(AiDev, AiChn);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_DBG(s32Ret);
            }
        }
        else
        {
            s32Ret |= SAMPLE_COMM_AUDIO_AencUnbindAi(AiDev, AiChn, AeChn);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_DBG(s32Ret);
            }
        }
    }

AIAENC_ERR4:
    s32Ret |= SAMPLE_COMM_AUDIO_StopAenc(s32AencChnCnt);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
    }

AIAENC_ERR5:
    s32Ret |= SAMPLE_COMM_AUDIO_StopAi(AiDev, s32AiChnCnt, gs_bAioReSample, u32AiVqeType==1);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
    }

AIAENC_ERR6:
  return s32Ret;
}

/******************************************************************************
* function : Ai -> ExtResample -> file
******************************************************************************/
HI_S32 SAMPLE_AUDIO_AiToExtResample(HI_VOID)
{
    HI_S32 i, j, s32Ret;
    AI_CHN      AiChn;
    HI_S32      s32AiChnCnt;
    FILE*        pfd = NULL;
    AIO_ATTR_S stAioAttr;

#ifdef HI_ACODEC_TYPE_TLV320AIC31
    AUDIO_DEV   AiDev = SAMPLE_AUDIO_EXTERN_AI_DEV;
    stAioAttr.enSamplerate   = AUDIO_SAMPLE_RATE_48000;
    stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
    stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode    = AUDIO_SOUND_MODE_MONO;
    stAioAttr.u32EXFlag      = 0;
    stAioAttr.u32FrmNum      = 30;
    stAioAttr.u32PtNumPerFrm = AACLC_SAMPLES_PER_FRAME;
    stAioAttr.u32ChnCnt      = 1;
    stAioAttr.u32ClkSel      = 1;
    stAioAttr.enI2sType      = AIO_I2STYPE_EXTERN;
#else
    AUDIO_DEV   AiDev = SAMPLE_AUDIO_INNER_AI_DEV;
    stAioAttr.enSamplerate   = AUDIO_SAMPLE_RATE_48000;
    stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
    stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode    = AUDIO_SOUND_MODE_MONO;
    stAioAttr.u32EXFlag      = 0;
    stAioAttr.u32FrmNum      = 30;
    stAioAttr.u32PtNumPerFrm = AACLC_SAMPLES_PER_FRAME;
    stAioAttr.u32ChnCnt      = 1;
    stAioAttr.u32ClkSel      = 0;
    stAioAttr.enI2sType      = AIO_I2STYPE_INNERCODEC;
#endif
    gs_bAioReSample = HI_FALSE;
    enOutSampleRate = AUDIO_SAMPLE_RATE_24000;

    /********************************************
      step 1: start Ai, disable inner resample.
    ********************************************/
    s32AiChnCnt = stAioAttr.u32ChnCnt;
    s32Ret = SAMPLE_COMM_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr, enOutSampleRate, gs_bAioReSample, NULL, 0);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        goto AIRES_ERR3;
    }

    /********************************************
      step 2: config audio codec
    ********************************************/
    s32Ret = SAMPLE_COMM_AUDIO_CfgAcodec(&stAioAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        goto AIRES_ERR2;
    }

    /********************************************
      step 3: enable extern resample.
    ********************************************/
    s32Ret = SAMPLE_AUDIO_InitExtResFun();
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        goto AIRES_ERR2;
    }

    /********************************************
      step 4: Create thread to resample.
    ********************************************/
    for (i = 0; i < s32AiChnCnt; i++)
    {
        AiChn = i;

        pfd = SAMPLE_AUDIO_OpenResFile(AiChn);
        if (!pfd)
        {
            SAMPLE_DBG(HI_FAILURE);
            goto AIRES_ERR1;
        }

        s32Ret = SAMPLE_COMM_AUDIO_CreatTrdAiExtRes(&stAioAttr, AiDev, AiChn, enOutSampleRate, pfd);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_DBG(s32Ret);
            fclose(pfd);
            pfd = HI_NULL;
            for (j=0; j<i; j++)
            {
                SAMPLE_COMM_AUDIO_DestoryTrdAi(AiDev, j);
            }
            goto AIRES_ERR1;
        }

        printf("Ai(%d,%d) extern resample ok!\n", AiDev , AiChn);
    }

    printf("\nplease press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    /********************************************
      step 5: exit the process
    ********************************************/
    for (i = 0; i < s32AiChnCnt; i++)
    {
        AiChn = i;

        s32Ret |= SAMPLE_COMM_AUDIO_DestoryTrdAiExtRes(AiDev, AiChn);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_DBG(s32Ret);
        }
    }

AIRES_ERR1:
    s32Ret |= SAMPLE_AUDIO_DeInitExtResFun();
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
    }

AIRES_ERR2:
    s32Ret |= SAMPLE_COMM_AUDIO_StopAi(AiDev, s32AiChnCnt, gs_bAioReSample, HI_FALSE);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
    }

AIRES_ERR3:

    return s32Ret;
}


/******************************************************************************
* function : Ai -> Ao(with fade in/out and volume adjust)
******************************************************************************/
HI_S32 SAMPLE_AUDIO_AiAo(HI_VOID)
{
    HI_S32 s32Ret;
    HI_S32 s32AiChnCnt;
    HI_S32 s32AoChnCnt;
    AI_CHN      AiChn = 0;
    AO_CHN      AoChn = 0;
    AIO_ATTR_S stAioAttr;

#ifdef HI_ACODEC_TYPE_TLV320AIC31
    AUDIO_DEV   AiDev = SAMPLE_AUDIO_EXTERN_AI_DEV;
    AUDIO_DEV   AoDev = SAMPLE_AUDIO_EXTERN_AO_DEV;
    stAioAttr.enSamplerate   = AUDIO_SAMPLE_RATE_48000;
    stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
    stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode    = AUDIO_SOUND_MODE_MONO;
    stAioAttr.u32EXFlag      = 0;
    stAioAttr.u32FrmNum      = 30;
    stAioAttr.u32PtNumPerFrm = AACLC_SAMPLES_PER_FRAME;
    stAioAttr.u32ChnCnt      = 1;
    stAioAttr.u32ClkSel      = 1;
    stAioAttr.enI2sType      = AIO_I2STYPE_EXTERN;
#else //  inner acodec
    AUDIO_DEV   AiDev = SAMPLE_AUDIO_INNER_AI_DEV;
    AUDIO_DEV   AoDev = SAMPLE_AUDIO_INNER_AO_DEV;
    stAioAttr.enSamplerate   = AUDIO_SAMPLE_RATE_48000;
    stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
    stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode    = AUDIO_SOUND_MODE_STEREO;
    stAioAttr.u32EXFlag      = 0;
    stAioAttr.u32FrmNum      = 30;
    stAioAttr.u32PtNumPerFrm = AACLC_SAMPLES_PER_FRAME;
    stAioAttr.u32ChnCnt      = 2;
    stAioAttr.u32ClkSel      = 0;
    stAioAttr.enI2sType      = AIO_I2STYPE_INNERCODEC;
#endif

    gs_bAioReSample = HI_FALSE;
    /* config ao resample attr if needed */
    if (HI_TRUE == gs_bAioReSample)
    {
        /* ai 48k -> 32k */
        enOutSampleRate = AUDIO_SAMPLE_RATE_32000;

        /* ao 32k -> 48k */
        enInSampleRate  = AUDIO_SAMPLE_RATE_32000;
    }
    else
    {
        enInSampleRate  = AUDIO_SAMPLE_RATE_BUTT;
        enOutSampleRate = AUDIO_SAMPLE_RATE_BUTT;
    }

    /* resample and anr should be user get mode */
    gs_bUserGetMode = (HI_TRUE == gs_bAioReSample) ? HI_TRUE : HI_FALSE;

    /* enable AI channle */
    s32AiChnCnt = stAioAttr.u32ChnCnt;
    s32Ret = SAMPLE_COMM_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr, enOutSampleRate, gs_bAioReSample, NULL, 0);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        goto AIAO_ERR3;
    }

    /* enable AO channle */
    s32AoChnCnt = stAioAttr.u32ChnCnt;
    s32Ret = SAMPLE_COMM_AUDIO_StartAo(AoDev, s32AoChnCnt, &stAioAttr, enInSampleRate, gs_bAioReSample);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
         goto AIAO_ERR2;
    }

    /* config internal audio codec */
    s32Ret = SAMPLE_COMM_AUDIO_CfgAcodec(&stAioAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        goto AIAO_ERR1;
    }

    /* bind AI to AO channle */
    if (HI_TRUE == gs_bUserGetMode)
    {
        s32Ret = SAMPLE_COMM_AUDIO_CreatTrdAiAo(AiDev, AiChn, AoDev, AoChn);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_DBG(s32Ret);
             goto AIAO_ERR1;
        }
    }
    else
    {
        s32Ret = SAMPLE_COMM_AUDIO_AoBindAi(AiDev, AiChn, AoDev, AoChn);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_DBG(s32Ret);
            goto AIAO_ERR1;
        }
    }
    printf("ai(%d,%d) bind to ao(%d,%d) ok\n", AiDev, AiChn, AoDev, AoChn);

    if (HI_TRUE == gs_bAoVolumeCtrl)
    {
        s32Ret = SAMPLE_COMM_AUDIO_CreatTrdAoVolCtrl(AoDev);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_DBG(s32Ret);
            goto AIAO_ERR0;
        }
    }
#if 0 //maohw
    printf("\nplease press twice ENTER to exit this sample\n");
    getchar();
    getchar();
#else
    while(1) sleep(1);
#endif
    if (HI_TRUE == gs_bAoVolumeCtrl)
    {
        s32Ret = SAMPLE_COMM_AUDIO_DestoryTrdAoVolCtrl(AoDev);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_DBG(s32Ret);
            return HI_FAILURE;
        }
    }

AIAO_ERR0:

    if (HI_TRUE == gs_bUserGetMode)
    {
        s32Ret = SAMPLE_COMM_AUDIO_DestoryTrdAi(AiDev, AiChn);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_DBG(s32Ret);
        }
    }
    else
    {
        s32Ret = SAMPLE_COMM_AUDIO_AoUnbindAi(AiDev, AiChn, AoDev, AoChn);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_DBG(s32Ret);
        }
    }

AIAO_ERR1:

    s32Ret |= SAMPLE_COMM_AUDIO_StopAo(AoDev, s32AoChnCnt, gs_bAioReSample);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
    }

AIAO_ERR2:
    s32Ret |= SAMPLE_COMM_AUDIO_StopAi(AiDev, s32AiChnCnt, gs_bAioReSample, HI_FALSE);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
    }

AIAO_ERR3:

    return s32Ret;
}

/******************************************************************************
* function : Ai -> Ao(with fade in/out and volume adjust)
******************************************************************************/
HI_S32 SAMPLE_AUDIO_AiToAoSysChn(HI_VOID)
{
    HI_S32 s32Ret;
    HI_S32 s32AiChnCnt;
    HI_S32 s32AoChnCnt;
    AI_CHN      AiChn = 0;
    AIO_ATTR_S stAioAttr;

#ifdef HI_ACODEC_TYPE_TLV320AIC31
    AUDIO_DEV   AiDev = SAMPLE_AUDIO_EXTERN_AI_DEV;
    AUDIO_DEV   AoDev = SAMPLE_AUDIO_EXTERN_AO_DEV;
    stAioAttr.enSamplerate   = AUDIO_SAMPLE_RATE_48000;
    stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
    stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode    = AUDIO_SOUND_MODE_MONO;
    stAioAttr.u32EXFlag      = 0;
    stAioAttr.u32FrmNum      = 30;
    stAioAttr.u32PtNumPerFrm = AACLC_SAMPLES_PER_FRAME;
    stAioAttr.u32ChnCnt      = 1;
    stAioAttr.u32ClkSel      = 1;
    stAioAttr.enI2sType      = AIO_I2STYPE_EXTERN;
#else //  inner acodec
    AUDIO_DEV   AiDev = SAMPLE_AUDIO_INNER_AI_DEV;
    AUDIO_DEV   AoDev = SAMPLE_AUDIO_INNER_AO_DEV;
    stAioAttr.enSamplerate   = AUDIO_SAMPLE_RATE_48000;
    stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
    stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode    = AUDIO_SOUND_MODE_STEREO;
    stAioAttr.u32EXFlag      = 0;
    stAioAttr.u32FrmNum      = 30;
    stAioAttr.u32PtNumPerFrm = AACLC_SAMPLES_PER_FRAME;
    stAioAttr.u32ChnCnt      = 2;
    stAioAttr.u32ClkSel      = 0;
    stAioAttr.enI2sType      = AIO_I2STYPE_INNERCODEC;
#endif
    gs_bAioReSample = HI_FALSE;
    /* config ao resample attr if needed */
    if (HI_TRUE == gs_bAioReSample)
    {
        /* ai 48k -> 32k */
        enOutSampleRate = AUDIO_SAMPLE_RATE_32000;

        /* ao 32k -> 48k */
        enInSampleRate  = AUDIO_SAMPLE_RATE_32000;
    }
    else
    {
        enInSampleRate  = AUDIO_SAMPLE_RATE_BUTT;
        enOutSampleRate = AUDIO_SAMPLE_RATE_BUTT;
    }

    /* resample and anr should be user get mode */
    gs_bUserGetMode = (HI_TRUE == gs_bAioReSample) ? HI_TRUE : HI_FALSE;

    /* enable AI channle */
    s32AiChnCnt = stAioAttr.u32ChnCnt;
    s32Ret = SAMPLE_COMM_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr, enOutSampleRate, gs_bAioReSample, NULL, 0);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        goto AIAO_ERR3;
    }

    /* enable AO channle */
    s32AoChnCnt = stAioAttr.u32ChnCnt;
    s32Ret = SAMPLE_COMM_AUDIO_StartAo(AoDev, s32AoChnCnt, &stAioAttr, enInSampleRate, gs_bAioReSample);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
         goto AIAO_ERR2;
    }

    /* config internal audio codec */
    s32Ret = SAMPLE_COMM_AUDIO_CfgAcodec(&stAioAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        goto AIAO_ERR1;
    }

    /* bind AI to AO channle */
    s32Ret = SAMPLE_COMM_AUDIO_CreatTrdAiAo(AiDev, AiChn, AoDev, AO_SYSCHN_CHNID);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
         goto AIAO_ERR1;
    }
    printf("ai(%d,%d) bind to ao(%d,%d) ok\n", AiDev, AiChn, AoDev, AO_SYSCHN_CHNID);


    printf("\nplease press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    s32Ret = SAMPLE_COMM_AUDIO_DestoryTrdAi(AiDev, AiChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
    }

AIAO_ERR1:

    s32Ret |= SAMPLE_COMM_AUDIO_StopAo(AoDev, s32AoChnCnt, gs_bAioReSample);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
    }

AIAO_ERR2:
    s32Ret |= SAMPLE_COMM_AUDIO_StopAi(AiDev, s32AiChnCnt, gs_bAioReSample, HI_FALSE);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
    }

AIAO_ERR3:

    return s32Ret;
}



/******************************************************************************
* function : Ai -> Ao
******************************************************************************/
HI_S32 SAMPLE_AUDIO_AiVqeProcessAo(HI_VOID)
{
    HI_S32 i, j, s32Ret;
    AUDIO_DEV   AiDev = SAMPLE_AUDIO_INNER_AI_DEV;
    AUDIO_DEV   AoDev = SAMPLE_AUDIO_INNER_AO_DEV;
    HI_S32      s32AiChnCnt;
    HI_S32      s32AoChnCnt;
    AIO_ATTR_S  stAioAttr;
    AI_RECORDVQE_CONFIG_S stAiVqeRecordAttr;
    HI_VOID     *pAiVqeAttr = NULL;

#ifdef HI_ACODEC_TYPE_TLV320AIC31
    stAioAttr.enSamplerate   = AUDIO_SAMPLE_RATE_48000;
    stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
    stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode    = AUDIO_SOUND_MODE_STEREO;
    stAioAttr.u32EXFlag      = 0;
    stAioAttr.u32FrmNum      = 30;
    stAioAttr.u32PtNumPerFrm = SAMPLE_AUDIO_PTNUMPERFRM;
    stAioAttr.u32ChnCnt      = 2;
    stAioAttr.u32ClkSel      = 1;
    stAioAttr.enI2sType      = AIO_I2STYPE_EXTERN;
#else
    stAioAttr.enSamplerate   = AUDIO_SAMPLE_RATE_48000;
    stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
    stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode    = AUDIO_SOUND_MODE_STEREO;
    stAioAttr.u32EXFlag      = 0;
    stAioAttr.u32FrmNum      = 30;
    stAioAttr.u32PtNumPerFrm = SAMPLE_AUDIO_PTNUMPERFRM;
    stAioAttr.u32ChnCnt      = 2;
    stAioAttr.u32ClkSel      = 0;
    stAioAttr.enI2sType      = AIO_I2STYPE_INNERCODEC;
#endif
    gs_bAioReSample = HI_FALSE;
    enInSampleRate  = AUDIO_SAMPLE_RATE_BUTT;
    enOutSampleRate = AUDIO_SAMPLE_RATE_BUTT;
    
    if (1 == u32AiVqeType)
    {
        memset(&stAiVqeRecordAttr, 0, sizeof(AI_RECORDVQE_CONFIG_S));
        stAiVqeRecordAttr.s32WorkSampleRate    = AUDIO_SAMPLE_RATE_48000;
        stAiVqeRecordAttr.s32FrameSample       = SAMPLE_AUDIO_PTNUMPERFRM;
        stAiVqeRecordAttr.enWorkstate          = VQE_WORKSTATE_COMMON;
        stAiVqeRecordAttr.s32InChNum           = 2;
        stAiVqeRecordAttr.s32OutChNum          = 2;
        stAiVqeRecordAttr.enRecordType         = VQE_RECORD_NORMAL;
        stAiVqeRecordAttr.stDrcCfg.bUsrMode    = HI_FALSE;
        stAiVqeRecordAttr.stRnrCfg.bUsrMode    = HI_FALSE;
        stAiVqeRecordAttr.stHdrCfg.bUsrMode    = HI_FALSE;
        stAiVqeRecordAttr.stHpfCfg.bUsrMode    = HI_TRUE;
        stAiVqeRecordAttr.stHpfCfg.enHpfFreq   = AUDIO_HPF_FREQ_80;

        stAiVqeRecordAttr.u32OpenMask = AI_RECORDVQE_MASK_DRC | AI_RECORDVQE_MASK_HDR | AI_RECORDVQE_MASK_HPF | AI_RECORDVQE_MASK_RNR;

        pAiVqeAttr = (HI_VOID *)&stAiVqeRecordAttr;
    }
    else
    {
        pAiVqeAttr = HI_NULL;
    }

    /********************************************
      step 1: start Ai
    ********************************************/
    s32AiChnCnt = stAioAttr.u32ChnCnt;
    s32Ret = SAMPLE_COMM_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr, enOutSampleRate, gs_bAioReSample, pAiVqeAttr, u32AiVqeType);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        goto VQE_ERR2;
    }

    /********************************************
      step 2: start Ao
    ********************************************/
    s32AoChnCnt = stAioAttr.u32ChnCnt;
    s32Ret = SAMPLE_COMM_AUDIO_StartAo(AoDev, s32AoChnCnt, &stAioAttr, enInSampleRate, gs_bAioReSample);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        goto VQE_ERR1;
    }

    /********************************************
      step 3: config audio codec
    ********************************************/
    s32Ret = SAMPLE_COMM_AUDIO_CfgAcodec(&stAioAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        goto VQE_ERR0;
    }

    /********************************************
      step 4: Ao bind Ai Chn
    ********************************************/
    for (i = 0; i < s32AiChnCnt>>stAioAttr.enSoundmode; i++)
    {
        s32Ret = SAMPLE_COMM_AUDIO_CreatTrdAiAo(AiDev, i, AoDev, i);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_DBG(s32Ret);
            for (j=0; j<i; j++)
            {
                SAMPLE_COMM_AUDIO_DestoryTrdAi(AiDev, j);
            }
            goto VQE_ERR0;
        }

        printf("bind ai(%d,%d) to ao(%d,%d) ok \n", AiDev, i, AoDev, i);
    }

    printf("\nplease press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    /********************************************
      step 6: exit the process
    ********************************************/
    for (i = 0; i < s32AiChnCnt; i++)
    {
        s32Ret = SAMPLE_COMM_AUDIO_DestoryTrdAi(AiDev, i);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_DBG(s32Ret);
        }
    }

VQE_ERR0:
    s32Ret = SAMPLE_COMM_AUDIO_StopAo(AoDev, s32AoChnCnt, gs_bAioReSample);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
    }

VQE_ERR1:
    s32Ret |= SAMPLE_COMM_AUDIO_StopAi(AiDev, s32AiChnCnt, gs_bAioReSample, HI_TRUE);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
    }

VQE_ERR2:

    return s32Ret;
}

/******************************************************************************
* function : Ai -> Ao(Hdmi)
******************************************************************************/
HI_S32 SAMPLE_AUDIO_AiHdmiAo(HI_VOID)
{
    HI_S32 s32Ret, s32AiChnCnt;
    AUDIO_DEV   AiDev = SAMPLE_AUDIO_INNER_AI_DEV;
    AI_CHN      AiChn = 0;
    AUDIO_DEV   AoDev = SAMPLE_AUDIO_INNER_HDMI_AO_DEV;
    AO_CHN      AoChn = 0;

    AIO_ATTR_S stAioAttr;
    AIO_ATTR_S stHdmiAoAttr;

#ifdef HI_ACODEC_TYPE_TLV320AIC31
    AiDev = SAMPLE_AUDIO_INNER_AI_DEV;
    stAioAttr.enI2sType = AIO_I2STYPE_EXTERN;
#else
    AiDev = SAMPLE_AUDIO_EXTERN_AI_DEV;
    stAioAttr.enI2sType = AIO_I2STYPE_INNERCODEC;
#endif

    stAioAttr.enSamplerate   = AUDIO_SAMPLE_RATE_48000;
    stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
    stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode    = AUDIO_SOUND_MODE_STEREO;
    stAioAttr.u32EXFlag      = 1;
    stAioAttr.u32FrmNum      = 30;
    stAioAttr.u32PtNumPerFrm = SAMPLE_AUDIO_PTNUMPERFRM;
    stAioAttr.u32ChnCnt      = 2;
    stAioAttr.u32ClkSel      = 0;

    stHdmiAoAttr.enSamplerate   = AUDIO_SAMPLE_RATE_48000;
    stHdmiAoAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
    stHdmiAoAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    stHdmiAoAttr.enSoundmode    = AUDIO_SOUND_MODE_STEREO;
    stHdmiAoAttr.u32EXFlag      = 1;
    stHdmiAoAttr.u32FrmNum      = 30;
    stHdmiAoAttr.u32PtNumPerFrm = SAMPLE_AUDIO_PTNUMPERFRM;
    stHdmiAoAttr.u32ChnCnt      = 2;
    stHdmiAoAttr.u32ClkSel      = 0;
    stHdmiAoAttr.enI2sType      = AIO_I2STYPE_INNERHDMI;

    gs_bAioReSample = HI_FALSE;
    /* resample should be user get mode */
    gs_bUserGetMode = (HI_TRUE == gs_bAioReSample) ? HI_TRUE : HI_FALSE;

    /* enable AI channle */
    s32AiChnCnt = stAioAttr.u32ChnCnt;
    s32Ret = SAMPLE_COMM_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, HI_FALSE, NULL, 0);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        return HI_FAILURE;
    }

    /* enable AO channle */
    s32Ret = SAMPLE_COMM_AUDIO_StartAo(AoDev, stHdmiAoAttr.u32ChnCnt, &stHdmiAoAttr, stHdmiAoAttr.enSamplerate, HI_FALSE);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        return HI_FAILURE;
    }

    /* config audio codec */
    s32Ret = SAMPLE_COMM_AUDIO_CfgAcodec(&stAioAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        return HI_FAILURE;
    }

    /* AI to AO channel */
    s32Ret = SAMPLE_COMM_AUDIO_CreatTrdAiAo(AiDev, AiChn, AoDev, AoChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        return HI_FAILURE;
    }

    return 0;
    printf("\nplease press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    s32Ret = SAMPLE_COMM_AUDIO_DestoryTrdAi(AiDev, AiChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_AUDIO_StopAi(AiDev, s32AiChnCnt, HI_FALSE, HI_FALSE);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_AUDIO_StopAo(AoDev, stHdmiAoAttr.u32ChnCnt, HI_FALSE);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_DBG(s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_VOID SAMPLE_AUDIO_Usage(HI_VOID)
{
    printf("\n\n/Usage:./sample_audio <index>/\n");
    printf("\tindex and its function list below\n");
    printf("\t0:  start AI to AO loop\n");
    printf("\t1:  send audio frame to AENC channel from AI, save them\n");
    printf("\t2:  read audio stream from file, decode and send AO\n");
    printf("\t3:  start AI(VQE process), then send to AO\n");
    printf("\t4:  start AI to AO(Hdmi) loop\n");
    printf("\t5:  start AI to AO(Syschn) loop\n");
    printf("\t6:  start AI to Extern Resampler\n");
}

/******************************************************************************
* function : to process abnormal case
******************************************************************************/
void SAMPLE_AUDIO_HandleSig(HI_S32 signo)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    if (SIGINT == signo || SIGTERM == signo)
    {
        hi_u32 dev_id, chn_id;
        for (dev_id = 0; dev_id < AI_DEV_MAX_NUM; dev_id ++) {
            for (chn_id = 0; chn_id < AI_MAX_CHN_NUM; chn_id ++) {
                if (SAMPLE_COMM_AUDIO_DestoryTrdAiExtRes(dev_id, chn_id) != HI_SUCCESS) {
                    printf("%s: SAMPLE_COMM_AUDIO_DestoryTrdAiExtRes(%d,%d) failed!\n", __FUNCTION__,
                        dev_id, chn_id);
                }
            }
        }

        SAMPLE_COMM_AUDIO_DestoryAllTrd();
        SAMPLE_COMM_SYS_Exit();
        printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
    }

    exit(0);
}

#if defined(HI_VQE_USE_STATIC_MODULE_REGISTER)
/******************************************************************************
* function : to register vqe module
******************************************************************************/
HI_S32 SAMPLE_AUDIO_RegisterVQEModule(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    AUDIO_VQE_REGISTER_S stVqeRegCfg = {0};

    /* Resample */
    stVqeRegCfg.stResModCfg.pHandle = HI_VQE_RESAMPLE_GetHandle();

    /* RecordVQE */
    stVqeRegCfg.stRecordModCfg.pHandle = HI_VQE_RECORD_GetHandle();

    /* TalkVQE */
    stVqeRegCfg.stHpfModCfg.pHandle = HI_VQE_HPF_GetHandle();
    stVqeRegCfg.stAecModCfg.pHandle = HI_VQE_AEC_GetHandle();
    stVqeRegCfg.stAgcModCfg.pHandle = HI_VQE_AGC_GetHandle();
    stVqeRegCfg.stAnrModCfg.pHandle = HI_VQE_ANR_GetHandle();
    stVqeRegCfg.stEqModCfg.pHandle = HI_VQE_EQ_GetHandle();

    s32Ret = HI_MPI_AUDIO_RegisterVQEModule(&stVqeRegCfg);
    if (s32Ret != HI_SUCCESS) {
        printf("%s: register vqe module fail with s32Ret = 0x%x!\n", __FUNCTION__, s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
#endif

/******************************************************************************
* function : main
******************************************************************************/
#ifdef __HuaweiLite__
HI_S32 app_main(int argc, char* argv[])
#else
HI_S32 sample_audio_main(int argc, char* argv[])
#endif
{
    HI_S32 s32Ret = HI_SUCCESS;
    VB_CONFIG_S stVbConf;
    HI_U32 u32Index = 0;

    if (2 != argc)
    {
        SAMPLE_AUDIO_Usage();
        return HI_FAILURE;
    }

    s32Ret = strcmp(argv[1], "-h");
    if (HI_SUCCESS == s32Ret)
    {
        SAMPLE_AUDIO_Usage();
        return HI_FAILURE;
    }

    u32Index = atoi(argv[1]);

    if (u32Index > 6)
    {
        SAMPLE_AUDIO_Usage();
        return HI_FAILURE;
    }
#if 0 //maohw
#ifndef __HuaweiLite__
    signal(SIGINT, SAMPLE_AUDIO_HandleSig);
    signal(SIGTERM, SAMPLE_AUDIO_HandleSig);
#endif

#if defined(HI_VQE_USE_STATIC_MODULE_REGISTER)
    SAMPLE_AUDIO_RegisterVQEModule();
#endif

    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: system init failed with %d!\n", __FUNCTION__, s32Ret);
        return HI_FAILURE;
    }
#endif

    SAMPLE_AUDIO_AddLibPath();

    HI_MPI_AENC_AacInit();
    HI_MPI_ADEC_AacInit();

#ifdef __Mp3decSupport__
    HI_MPI_ADEC_Mp3Init();
#endif

    switch (u32Index)
    {
        case 0:
        {
            SAMPLE_AUDIO_AiAo();
            break;
        }
        case 1:
        {
            SAMPLE_AUDIO_AiAenc(NULL);
            break;
        }
        case 2:
        {
            SAMPLE_AUDIO_AdecAo(NULL);
            break;
        }
        case 3:
        {
            SAMPLE_AUDIO_AiVqeProcessAo();
            break;
        }
        case 4:
        {
            SAMPLE_AUDIO_AiHdmiAo();
            break;
        }
        case 5:
        {
            SAMPLE_AUDIO_AiToAoSysChn();
            break;
        }
        case 6:
        {
            SAMPLE_AUDIO_AiToExtResample();
            break;
        }
        default:
        {
            break;
        }
    }

    HI_MPI_AENC_AacDeInit();
    HI_MPI_ADEC_AacDeInit();
#if 0 //maohw
    SAMPLE_COMM_SYS_Exit();
#endif
    return s32Ret;
}



HI_S32 sample_audio_init()
{
#if defined(HI_VQE_USE_STATIC_MODULE_REGISTER)
    SAMPLE_AUDIO_RegisterVQEModule();
#endif
  SAMPLE_AUDIO_AddLibPath();

  HI_MPI_AENC_AacInit();
  HI_MPI_ADEC_AacInit();

#ifdef __Mp3decSupport__
  HI_MPI_ADEC_Mp3Init();
#endif
  return 0;
}


HI_S32 sample_audio_deinit()
{
  HI_MPI_AENC_AacDeInit();
  HI_MPI_ADEC_AacDeInit();
  return 0;
}
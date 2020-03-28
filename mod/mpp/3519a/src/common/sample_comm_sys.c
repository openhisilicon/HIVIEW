

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
#include <sys/mman.h>

#include "sample_comm.h"





static HI_S32 s_s32SampleMemDev = -1;
#define SAMPLE_MEM_DEV_OPEN if (s_s32SampleMemDev <= 0)\
{\
    s_s32SampleMemDev = open("/dev/mem", O_RDWR|O_SYNC);\
    if (s_s32SampleMemDev < 0)\
    {\
        perror("Open dev/mem error");\
        return NULL;\
    }\
}\


HI_VOID * SAMPLE_SYS_IOMmap(HI_U64 u64PhyAddr, HI_U32 u32Size)
{
    HI_U32 u32Diff;
    HI_U64 u64PagePhy;
    HI_U8 * pPageAddr;
    HI_UL    ulPageSize;

    SAMPLE_MEM_DEV_OPEN;

    /**********************************************************
    PageSize will be 0 when u32size is 0 and u32Diff is 0,
    and then mmap will be error (error: Invalid argument)
    ***********************************************************/
    if (!u32Size)
    {
        printf("Func: %s u32Size can't be 0.\n", __FUNCTION__);
        return NULL;
    }

    /* The mmap address should align with page */
    u64PagePhy = u64PhyAddr & 0xfffffffffffff000ULL;
    u32Diff    = u64PhyAddr - u64PagePhy;

    /* The mmap size shuld be mutliples of 1024 */
    ulPageSize = ((u32Size + u32Diff - 1) & 0xfffff000UL) + 0x1000;

    pPageAddr    = mmap ((void *)0, ulPageSize, PROT_READ|PROT_WRITE,
                                    MAP_SHARED, s_s32SampleMemDev, u64PagePhy);
    if (MAP_FAILED == pPageAddr )
    {
        perror("mmap error");
        return NULL;
    }
    return (HI_VOID *) (pPageAddr + u32Diff);
}


HI_S32 SAMPLE_SYS_Munmap(HI_VOID* pVirAddr, HI_U32 u32Size)
{
    HI_U64 u64PageAddr;
    HI_U32 u32PageSize;
    HI_U32 u32Diff;

    u64PageAddr = (((HI_UL)pVirAddr) & 0xfffffffffffff000ULL);
    u32Diff     = (HI_UL)pVirAddr - u64PageAddr;
    u32PageSize = ((u32Size + u32Diff - 1) & 0xfffff000UL) + 0x1000;

    return munmap((HI_VOID*)(HI_UL)u64PageAddr, u32PageSize);
}



HI_S32 SAMPLE_SYS_SetReg(HI_U64 u64Addr, HI_U32 u32Value)
{
    HI_U32 *pu32RegAddr = NULL;
    HI_U32 u32MapLen = sizeof(u32Value);

    pu32RegAddr = (HI_U32 *)SAMPLE_SYS_IOMmap(u64Addr, u32MapLen);
    if(NULL == pu32RegAddr)
    {
        return HI_FAILURE;
    }

    *pu32RegAddr = u32Value;

    return SAMPLE_SYS_Munmap(pu32RegAddr, u32MapLen);
}

HI_S32 SAMPLE_SYS_GetReg(HI_U64 u64Addr, HI_U32 *pu32Value)
{
    HI_U32 *pu32RegAddr = NULL;
    HI_U32 u32MapLen;

    if (NULL == pu32Value)
    {
        return HI_ERR_SYS_NULL_PTR;
    }

    u32MapLen = sizeof(*pu32Value);
    pu32RegAddr = (HI_U32 *)SAMPLE_SYS_IOMmap(u64Addr, u32MapLen);
    if(NULL == pu32RegAddr)
    {
        return HI_FAILURE;
    }

    *pu32Value = *pu32RegAddr;

    return SAMPLE_SYS_Munmap(pu32RegAddr, u32MapLen);
}



/******************************************************************************
* function : get picture size(w*h), according enPicSize
******************************************************************************/
HI_S32 SAMPLE_COMM_SYS_GetPicSize(PIC_SIZE_E enPicSize, SIZE_S* pstSize)
{
    switch (enPicSize)
    {
        case PIC_CIF:   /* 352 * 288 */
            pstSize->u32Width  = 352;
            pstSize->u32Height = 288;
            break;

        case PIC_D1_PAL:   /* 720 * 576 */
            pstSize->u32Width  = 720;
            pstSize->u32Height = 576;
            break;

        case PIC_D1_NTSC:   /* 720 * 480 */
            pstSize->u32Width  = 720;
            pstSize->u32Height = 480;
            break;

        case PIC_720P:   /* 1280 * 720 */
            pstSize->u32Width  = 1280;
            pstSize->u32Height = 720;
            break;

        case PIC_1080P:  /* 1920 * 1080 */
            pstSize->u32Width  = 1920;
            pstSize->u32Height = 1080;
            break;

        case PIC_2592x1520:
            pstSize->u32Width  = 2592;
            pstSize->u32Height = 1520;
            break;

        case PIC_2592x1944:
            pstSize->u32Width  = 2592;
            pstSize->u32Height = 1944;
            break;

        case PIC_3840x2160:
            pstSize->u32Width  = 3840;
            pstSize->u32Height = 2160;
            break;

        case PIC_3000x3000:
            pstSize->u32Width  = 3000;
            pstSize->u32Height = 3000;
            break;

        case PIC_4000x3000:
            pstSize->u32Width  = 4000;
            pstSize->u32Height = 3000;
            break;

        case PIC_4096x2160:
            pstSize->u32Width  = 4096;
            pstSize->u32Height = 2160;
            break;

        case PIC_7680x4320:
            pstSize->u32Width  = 7680;
            pstSize->u32Height = 4320;
            break;
        case PIC_3840x8640:
            pstSize->u32Width = 3840;
            pstSize->u32Height = 8640;
            break;
        default:
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}




/******************************************************************************
* function : Set system memory location
******************************************************************************/
HI_S32 SAMPLE_COMM_SYS_MemConfig(HI_VOID)
{
    HI_S32 i, j;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR* pcMmzName = NULL;
    MPP_CHN_S stMppChn;

    /*config memory for vi*/
    for (i = 0; i < VI_MAX_PIPE_NUM; i++)
    {
        for (j = 0; j < VI_MAX_CHN_NUM; j++)
        {
            stMppChn.enModId  = HI_ID_VI;
            stMppChn.s32DevId = i;
            stMppChn.s32ChnId = j;
            s32Ret = HI_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

            if (s32Ret)
            {
                SAMPLE_PRT("HI_MPI_SYS_SetMemConfig ERR !\n");
                return HI_FAILURE;
            }
        }
    }

    /*config memory for avs */
    for (i = 0; i < AVS_MAX_GRP_NUM; i++)
    {
        stMppChn.enModId  = HI_ID_AVS;
        stMppChn.s32DevId = i;
        stMppChn.s32ChnId = 0;
        s32Ret = HI_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

        if (s32Ret)
        {
            SAMPLE_PRT("HI_MPI_SYS_SetMemConfig ERR !\n");
            return HI_FAILURE;
        }
    }

    /*config memory for vpss */
    for (i = 0; i < VPSS_MAX_GRP_NUM; i++)
    {
        stMppChn.enModId  = HI_ID_VPSS;
        stMppChn.s32DevId = i;
        stMppChn.s32ChnId = 0;
        s32Ret = HI_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

        if (s32Ret)
        {
            SAMPLE_PRT("HI_MPI_SYS_SetMemConfig ERR !\n");
            return HI_FAILURE;
        }
    }

    /*config memory for venc */
    for (i = 0; i < VENC_MAX_CHN_NUM; i++)
    {

        stMppChn.enModId  = HI_ID_VENC;
        stMppChn.s32DevId = 0;
        stMppChn.s32ChnId = i;
        s32Ret = HI_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

        if (s32Ret)
        {
            SAMPLE_PRT("HI_MPI_SYS_SetMemConf ERR !\n");
            return HI_FAILURE;
        }
    }

    /*config memory for vo*/
    for (i = 0; i < VO_MAX_LAYER_NUM; i++)
    {
        for (j = 0; j < VO_MAX_CHN_NUM; j++)
        {
            stMppChn.enModId    = HI_ID_VO;
            stMppChn.s32DevId = i;
            stMppChn.s32ChnId = j;
            s32Ret = HI_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

            if (s32Ret)
            {
                SAMPLE_PRT("HI_MPI_SYS_SetMemConfig ERR !\n");
                return HI_FAILURE;
            }
        }
    }

    /*config memory for vdec */
    for (i = 0; i < VDEC_MAX_CHN_NUM; i++)
    {

        stMppChn.enModId  = HI_ID_VDEC;
        stMppChn.s32DevId = 0;
        stMppChn.s32ChnId = i;
        s32Ret = HI_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

        if (s32Ret)
        {
            SAMPLE_PRT("HI_MPI_SYS_SetMemConf ERR !\n");
            return HI_FAILURE;
        }
    }


    return s32Ret;
}

/******************************************************************************
* function : vb init & MPI system init
******************************************************************************/
HI_S32 SAMPLE_COMM_SYS_Init(VB_CONFIG_S* pstVbConfig)
{
    HI_S32 s32Ret = HI_FAILURE;

    HI_MPI_SYS_Exit();
    HI_MPI_VB_Exit();

    if (NULL == pstVbConfig)
    {
        SAMPLE_PRT("input parameter is null, it is invaild!\n");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VB_SetConfig(pstVbConfig);

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

    s32Ret = HI_MPI_SYS_Init();

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_SYS_Init failed!\n");
        HI_MPI_VB_Exit();
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/******************************************************************************
* function : vb init with VbSupplement & MPI system init
******************************************************************************/
HI_S32 SAMPLE_COMM_SYS_InitWithVbSupplement(VB_CONFIG_S* pstVbConf, HI_U32 u32SupplementConfig)
{
    VB_SUPPLEMENT_CONFIG_S stSupplementConf = {0};
    HI_S32 s32Ret = HI_FAILURE;

    HI_MPI_SYS_Exit();
    HI_MPI_VB_Exit();

    if (NULL == pstVbConf)
    {
        SAMPLE_PRT("input parameter is null, it is invaild!\n");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VB_SetConfig(pstVbConf);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VB_SetConf failed!\n");
        return HI_FAILURE;
    }

    stSupplementConf.u32SupplementConfig = u32SupplementConfig;

    s32Ret = HI_MPI_VB_SetSupplementConfig(&stSupplementConf);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VB_SetSupplementConf failed!\n");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VB_Init();

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VB_Init failed!\n");
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
* function : vb exit & MPI system exit
******************************************************************************/
HI_VOID SAMPLE_COMM_SYS_Exit(void)
{
    HI_MPI_SYS_Exit();
    HI_MPI_VB_ExitModCommPool(VB_UID_VDEC);
    HI_MPI_VB_Exit();
    return;
}
HI_S32 SAMPLE_COMM_VI_Bind_VO(VI_PIPE ViPipe, VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = HI_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(HI_MPI_SYS_Bind(&stSrcChn, &stDestChn), "HI_MPI_SYS_Bind(VI-VO)");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_UnBind_VO(VI_PIPE ViPipe, VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = HI_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "HI_MPI_SYS_UnBind(VI-VO)");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_Bind_VPSS(VI_PIPE ViPipe, VI_CHN ViChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = HI_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(HI_MPI_SYS_Bind(&stSrcChn, &stDestChn), "HI_MPI_SYS_Bind(VI-VPSS)");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_UnBind_VPSS(VI_PIPE ViPipe, VI_CHN ViChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = HI_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "HI_MPI_SYS_UnBind(VI-VPSS)");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_Bind_VENC(VI_PIPE ViPipe, VI_CHN ViChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = HI_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(HI_MPI_SYS_Bind(&stSrcChn, &stDestChn), "HI_MPI_SYS_Bind(VI-VENC)");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_UnBind_VENC(VI_PIPE ViPipe, VI_CHN ViChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = HI_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "HI_MPI_SYS_UnBind(VI-VENC)");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VPSS_Bind_AVS(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP AvsGrp, AVS_PIPE AvsPipe)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = HI_ID_AVS;
    stDestChn.s32DevId = AvsGrp;
    stDestChn.s32ChnId = AvsPipe;

    CHECK_RET(HI_MPI_SYS_Bind(&stSrcChn, &stDestChn), "HI_MPI_SYS_Bind(VPSS-AVS)");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VPSS_UnBind_AVS(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP AvsGrp, AVS_PIPE AvsPipe)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = HI_ID_AVS;
    stDestChn.s32DevId = AvsGrp;
    stDestChn.s32ChnId = AvsPipe;

    CHECK_RET(HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "HI_MPI_SYS_UnBind(VPSS-AVS)");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VPSS_Bind_VO(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = HI_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(HI_MPI_SYS_Bind(&stSrcChn, &stDestChn), "HI_MPI_SYS_Bind(VPSS-VO)");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VPSS_UnBind_VO(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = HI_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "HI_MPI_SYS_UnBind(VPSS-VO)");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VPSS_Bind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = HI_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(HI_MPI_SYS_Bind(&stSrcChn, &stDestChn), "HI_MPI_SYS_Bind(VPSS-VENC)");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VPSS_UnBind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = HI_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "HI_MPI_SYS_UnBind(VPSS-VENC)");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_AVS_Bind_AVS(AVS_GRP AvsSrcGrp, AVS_CHN AvsSrcChn, AVS_GRP AvsDestGrp, AVS_CHN AvsDestChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_AVS;
    stSrcChn.s32DevId  = AvsSrcGrp;
    stSrcChn.s32ChnId  = AvsSrcChn;

    stDestChn.enModId  = HI_ID_AVS;
    stDestChn.s32DevId = AvsDestGrp;
    stDestChn.s32ChnId = AvsDestChn;

    CHECK_RET(HI_MPI_SYS_Bind(&stSrcChn, &stDestChn), "HI_MPI_SYS_Bind(AVS-AVS)");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_AVS_UnBind_AVS(AVS_GRP AvsSrcGrp, AVS_CHN AvsSrcChn, AVS_GRP AvsDestGrp, AVS_CHN AvsDestChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_AVS;
    stSrcChn.s32DevId  = AvsSrcGrp;
    stSrcChn.s32ChnId  = AvsSrcChn;

    stDestChn.enModId  = HI_ID_AVS;
    stDestChn.s32DevId = AvsDestGrp;
    stDestChn.s32ChnId = AvsDestChn;

    CHECK_RET(HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "HI_MPI_SYS_UnBind(AVS-AVS)");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_AVS_Bind_VPSS(AVS_GRP AvsGrp, AVS_CHN AvsChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_AVS;
    stSrcChn.s32DevId  = AvsGrp;
    stSrcChn.s32ChnId  = AvsChn;

    stDestChn.enModId  = HI_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(HI_MPI_SYS_Bind(&stSrcChn, &stDestChn), "HI_MPI_SYS_Bind(AVS-VPSS)");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_AVS_UnBind_VPSS(AVS_GRP AvsGrp, AVS_CHN AvsChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_AVS;
    stSrcChn.s32DevId  = AvsGrp;
    stSrcChn.s32ChnId  = AvsChn;

    stDestChn.enModId  = HI_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "HI_MPI_SYS_UnBind(AVS-VPSS)");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_AVS_Bind_VENC(AVS_GRP AvsGrp, AVS_CHN AvsChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_AVS;
    stSrcChn.s32DevId  = AvsGrp;
    stSrcChn.s32ChnId  = AvsChn;

    stDestChn.enModId  = HI_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(HI_MPI_SYS_Bind(&stSrcChn, &stDestChn), "HI_MPI_SYS_Bind(AVS-VENC)");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_AVS_UnBind_VENC(AVS_GRP AvsGrp, AVS_CHN AvsChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_AVS;
    stSrcChn.s32DevId  = AvsGrp;
    stSrcChn.s32ChnId  = AvsChn;

    stDestChn.enModId  = HI_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "HI_MPI_SYS_UnBind(AVS-VENC)");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_AVS_Bind_VO(AVS_GRP AvsGrp, AVS_CHN AvsChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_AVS;
    stSrcChn.s32DevId  = AvsGrp;
    stSrcChn.s32ChnId  = AvsChn;

    stDestChn.enModId  = HI_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(HI_MPI_SYS_Bind(&stSrcChn, &stDestChn), "HI_MPI_SYS_Bind(AVS-VO)");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_AVS_UnBind_VO(AVS_GRP AvsGrp, AVS_CHN AvsChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_AVS;
    stSrcChn.s32DevId  = AvsGrp;
    stSrcChn.s32ChnId  = AvsChn;

    stDestChn.enModId  = HI_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "HI_MPI_SYS_UnBind(AVS-VO)");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VDEC_Bind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = HI_ID_VDEC;
    stSrcChn.s32DevId  = 0;
    stSrcChn.s32ChnId  = VdecChn;

    stDestChn.enModId  = HI_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(HI_MPI_SYS_Bind(&stSrcChn, &stDestChn), "HI_MPI_SYS_Bind(VDEC-VPSS)");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VDEC_UnBind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId  = HI_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VdecChn;

    stDestChn.enModId  = HI_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "HI_MPI_SYS_UnBind(VDEC-VPSS)");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VO_Bind_VO(VO_LAYER  SrcVoLayer, VO_CHN SrcVoChn, VO_LAYER DstVoLayer, VO_CHN DstVoChn)
{
    MPP_CHN_S stSrcChn, stDestChn;
    stSrcChn.enModId    = HI_ID_VO;
    stSrcChn.s32DevId   = SrcVoLayer;
    stSrcChn.s32ChnId   = SrcVoChn;

    stDestChn.enModId   = HI_ID_VO;
    stDestChn.s32DevId  = DstVoLayer;
    stDestChn.s32ChnId  = DstVoChn;

    return HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

HI_S32 SAMPLE_COMM_VO_UnBind_VO(VO_LAYER DstVoLayer, VO_CHN DstVoChn)
{
    MPP_CHN_S stDestChn;
    stDestChn.enModId   = HI_ID_VO;
    stDestChn.s32DevId  = DstVoLayer;
    stDestChn.s32ChnId  = DstVoChn;

    return HI_MPI_SYS_UnBind(NULL, &stDestChn);
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

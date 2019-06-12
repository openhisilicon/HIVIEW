#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <sys/prctl.h>
#include "hi_sceneauto_comm.h"
#include "hi_sceneauto_comm_ext.h"
#include "hi_common.h"
#include "hi_comm_isp.h"
#include "hi_comm_vpss.h"
#include "hi_comm_video.h"
#include "hi_comm_sys.h"
#include "hi_comm_vo.h"
#include "hi_comm_rc.h"
#include "hi_comm_venc.h"
#include "hi_ive.h"
#include "hi_comm_ive.h"
#include "hi_comm_vgs.h"

#include "mpi_isp.h"
#include "mpi_venc.h"
#include "mpi_ae.h"
#include "mpi_vi.h"
#include "mpi_awb.h"
#include "mpi_vpss.h"
#include "mpi_vb.h"
#include "mpi_sys.h"
#include "mpi_vi.h"
#include "mpi_vo.h"
#include "mpi_ive.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define IVE2_ALIGN 16
#define calcStride(width,align)		((width+align-1)&(~(align-1)))
#define CCIMGWIDTH	160
#define CCIMGHEIGHT 128
HI_U16 u16IThreshTable[12] = {20, 25, 30, 35, 40, 50, 60, 70, 75, 80, 85, 90};
HI_U16 u16PThreshTable[12] = {20, 25, 30, 35, 40, 50, 60, 70, 75, 80, 85, 90};


#define CHECK_RET(name,express)\
    do{\
        if (HI_SUCCESS != express)\
        {\
            printf("%s failed at %s: LINE: %d ! errno:%x \n",\
                   name, __FUNCTION__, __LINE__, express);\
        }\
    }while(0)

#define IVE_MMZ_FREE(phy,vir)\
    do{\
        if ((0 != (phy)) && (NULL != (vir)))\
        {\
            HI_MPI_SYS_MmzFree((phy),(vir));\
            (phy) = 0;\
            (vir) = NULL;\
        }\
    }while(0)

static __inline int iClip2(int x, int b)       {{ if (x < 0) x = 0; };{ if (x > b) x = b; }; return x; }
static __inline int iClip3(int x, int a,int b) {{ if (x < a) x = a; };{ if (x > b) x = b; }; return x; } 
static __inline int iMin2(int a, int b) {{ if (a > b) a = b; }; return a; }
static __inline int iMax2(int a, int b) {{ if (a > b) b = a; }; return b; }
static __inline int iMin3(int a, int b, int c) {{ if (a > b) a = b; }; { if (a > c) a = c; }; return a; }

typedef enum HI_MBRCMODE_E
{
    E_MBINTEG = 0,
    E_MBSTEP = 1,
    E_MBDEFAULT = 2
} MBRCMODE_E;

typedef struct IVE_MD
{
    pthread_t hIveMBThread;
    IVE_IMAGE_S  stImg1;	// for mb bakup
    IVE_IMAGE_S  stImg3; // for intergal image
    IVE_IMAGE_S  stImg4; //for cal madi tmp
    IVE_IMAGE_S  stImg5; //for mb madi

    VIDEO_FRAME_INFO_S stCCScaleTmp;	// for scale picture buffer
    VIDEO_FRAME_INFO_S stCCScaleOut;	// for scale picture out
    IVE_IMAGE_S	  stCCIntegtmp;			// for intergal image buffer
    IVE_IMAGE_S	  stCCInteg;			// for cal interg image
    IVE_IMAGE_S   stCCCalBuf;			// for cc cal


    IVE_IMAGE_S stHistInfo;
    HI_U16 u16VideoWidth ;
    HI_U16 u16VideoHeight;
    HI_U16 u16OriWidth;
    HI_U16 u16OriHeight;
    HI_BOOL bStopSignal;
    HI_BOOL bPause;
    MBRCMODE_E eRCMode;
    HI_U32 ViExtChn;
    HI_S32 s32VpssGrp;
    HI_S32 s32VpssChn;
} IVE_MD;

static IVE_MD stIveMd;

typedef struct hiVENC_FACE_CFG_S
{
    HI_BOOL  bIntra4Ang[2][35];
    HI_BOOL  bIntra8Ang[2][35];
    HI_BOOL  bIntra16Ang[2][35];
    HI_BOOL  bIntra32Ang[2][35];
    HI_BOOL  bImproveEn[2];

    HI_U32   u32Norm32MaxNum[2];
    HI_U32   u32Norm32ProtectNum[2];
    HI_U32   u32Norm16MaxNum[2];
    HI_U32   u32Norm16ProtectNum[2];

    HI_U32   u32Skin32MaxNum[2];
    HI_U32   u32Skin32ProtectNum[2];
    HI_U32   u32Skin16MaxNum[2];
    HI_U32   u32Skin16ProtectNum[2];

    HI_U32   u32Still32MaxNum[2];
    HI_U32   u32Still32ProtectNum[2];
    HI_U32   u32Still16MaxNum[2];
    HI_U32   u32Still16ProtectNum[2];

    HI_U32   u32Hedge32MaxNum[2];
    HI_U32   u32Hedge32ProtectNum[2];
    HI_U32   u32Hedge16MaxNum[2];
    HI_U32   u32Hedge16ProtectNum[2];

    HI_U8    u8NormIntra32RdCost[2];
    HI_U8    u8NormIntra16RdCost[2];
    HI_U8    u8NormIntra8RdCost[2];
    HI_U8    u8NormIntra4RdCost[2];

    HI_U8    u8NormMerg64RdCost[2];
    HI_U8    u8NormMerg32RdCost[2];
    HI_U8    u8NormMerg16RdCost[2];
    HI_U8    u8NormMerg8RdCost[2];

    HI_U8    u8NormFme64RdCost[2];
    HI_U8    u8NormFme32RdCost[2];
    HI_U8    u8NormFme16RdCost[2];
    HI_U8    u8NormFme8RdCost[2];

    HI_U8    u8SkinIntra32RdCost[2];
    HI_U8    u8SkinIntra16RdCost[2];
    HI_U8    u8SkinIntra8RdCost[2];
    HI_U8    u8SkinIntra4RdCost[2];

    HI_U8    u8SkinMerg64RdCost[2];
    HI_U8    u8SkinMerg32RdCost[2];
    HI_U8    u8SkinMerg16RdCost[2];
    HI_U8    u8SkinMerg8RdCost[2];

    HI_U8    u8SkinFme64RdCost[2];
    HI_U8    u8SkinFme32RdCost[2];
    HI_U8    u8SkinFme16RdCost[2];
    HI_U8    u8SkinFme8RdCost[2];

    HI_U8    u8HedgeIntra32RdCost[2];
    HI_U8    u8HedgeIntra16RdCost[2];
    HI_U8    u8HedgeIntra8RdCost[2];
    HI_U8    u8HedgeIntra4RdCost[2];

    HI_U8    u8HedgeMerg64RdCost[2];
    HI_U8    u8HedgeMerg32RdCost[2];
    HI_U8    u8HedgeMerg16RdCost[2];
    HI_U8    u8HedgeMerg8RdCost[2];

    HI_U8    u8HedgeFme64RdCost[2];
    HI_U8    u8HedgeFme32RdCost[2];
    HI_U8    u8HedgeFme16RdCost[2];
    HI_U8    u8HedgeFme8RdCost[2];

    HI_BOOL  bStroEdgeEn[2];
    HI_U32   u32StroEdgeQpDelta[2];

    HI_BOOL  bSkinEn[2];
    HI_U32   u32SkinQpDelta[2];

    HI_U8    u8SkinUMax[2];
    HI_U8    u8SkinUMin[2];
    HI_U8    u8SkinVMax[2];
    HI_U8    u8SkinVMin[2];

    HI_U32   u32SkinNum[2];
    HI_U8    u8HedgeCnt[2];
    HI_U8    u8HedgeThr[2];

    HI_U32   u32StillThr[2];

} VENC_FACE_CFG_S;


extern HI_S32 HI_MPI_VPSS_SetNRV2Param(VPSS_GRP VpssGrp, const VPSS_GRP_VPPNRXEX_S* pstVpssGrpVppNrEx);
extern HI_S32 HI_MPI_VPSS_GetNRV2Param(VPSS_GRP VpssGrp, VPSS_GRP_VPPNRXEX_S* pstVpssGrpVppNrEx);
extern HI_S32 HI_MPI_VENC_SetFaceCfg(VENC_CHN VeChn, const  VENC_FACE_CFG_S* pstFaceCfg);
extern HI_S32 HI_MPI_VENC_GetFaceCfg(VENC_CHN VeChn,  VENC_FACE_CFG_S* pstFaceCfg);
extern HI_S32 HI_MPI_VPSS_SetNRV3Param(VPSS_GRP VpssGrp, VPSS_GRP_VPPNRBEX_S *pstVpssNrParam);
extern HI_S32 HI_MPI_VPSS_GetNRV3Param(VPSS_GRP VpssGrp, VPSS_GRP_VPPNRBEX_S *pstVpssNrParam);

int IVE_DetectDestory(IVE_MD* pstIveMd)
{
    if (pstIveMd->stHistInfo.u32PhyAddr[0])
    {
        IVE_MMZ_FREE(pstIveMd->stHistInfo.u32PhyAddr[0], pstIveMd->stHistInfo.pu8VirAddr[0]);
        pstIveMd->stHistInfo.u32PhyAddr[0] = 0;
    }

    if (pstIveMd->stImg3.u32PhyAddr[0])
    {
        IVE_MMZ_FREE(pstIveMd->stImg3.u32PhyAddr[0], pstIveMd->stImg3.pu8VirAddr[0]);
        pstIveMd->stImg3.u32PhyAddr[0] = 0;
    }

    if (pstIveMd->stImg5.u32PhyAddr[0])
    {
        IVE_MMZ_FREE(pstIveMd->stImg5.u32PhyAddr[0], pstIveMd->stImg5.pu8VirAddr[0]);
        pstIveMd->stImg5.u32PhyAddr[0] = 0;
    }

    if (pstIveMd->stCCScaleTmp.stVFrame.u32PhyAddr[0])
    {
        IVE_MMZ_FREE(pstIveMd->stCCScaleTmp.stVFrame.u32PhyAddr[0], pstIveMd->stCCScaleTmp.stVFrame.pVirAddr[0]);
        pstIveMd->stCCScaleTmp.stVFrame.u32PhyAddr[0] = 0;
    }

    if (pstIveMd->stCCScaleOut.stVFrame.u32PhyAddr[0])
    {
        IVE_MMZ_FREE(pstIveMd->stCCScaleOut.stVFrame.u32PhyAddr[0], pstIveMd->stCCScaleOut.stVFrame.pVirAddr[0]);
        pstIveMd->stCCScaleOut.stVFrame.u32PhyAddr[0] = 0;
    }

    if (pstIveMd->stCCIntegtmp.u32PhyAddr[0])
    {
        IVE_MMZ_FREE(pstIveMd->stCCIntegtmp.u32PhyAddr[0], pstIveMd->stCCIntegtmp.pu8VirAddr[0]);
        pstIveMd->stCCIntegtmp.u32PhyAddr[0] = 0;
    }

    if (pstIveMd->stCCCalBuf.pu8VirAddr[0])
    {
        free(pstIveMd->stCCCalBuf.pu8VirAddr[0]);
        pstIveMd->stCCCalBuf.pu8VirAddr[0] = 0;
    }

    return HI_SUCCESS;
}


int IVE_DetectInit(IVE_MD* pstIveMd)
{

    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32Size = 0;
    int i;

    VPSS_GRP stVpssGrp = pstIveMd->s32VpssGrp;
    VPSS_CHN stVpssChn = pstIveMd->s32VpssChn;
    HI_U32 u32Depth = 1;
    s32Ret = HI_MPI_VPSS_SetDepth(stVpssGrp, stVpssChn, u32Depth);
    if (s32Ret != HI_SUCCESS)
    {
        CHECK_RET("HI_MPI_VPSS_SetDepth fail\n", s32Ret);
        goto FAIL;
    }
    pstIveMd->stImg3.enType = IVE_IMAGE_TYPE_U64C1;
    pstIveMd->stImg3.u16Width = ((pstIveMd->u16OriWidth >> 3) & (~0xf)) + 2;
    pstIveMd->stImg3.u16Height = ((pstIveMd->u16OriHeight >> 3) & (~0xf)) + 1;
    pstIveMd->stImg3.u16Stride[0] = calcStride( (pstIveMd->stImg3.u16Width), IVE2_ALIGN) << 3;
    u32Size = pstIveMd->stImg3.u16Stride[0] * pstIveMd->stImg3.u16Height;
    s32Ret = HI_MPI_SYS_MmzAlloc(&pstIveMd->stImg3.u32PhyAddr[0], (void**)&pstIveMd->stImg3.pu8VirAddr[0], NULL, HI_NULL, u32Size);
    if (s32Ret != HI_SUCCESS)
    {
        CHECK_RET("pstIveMd->stImage 3 malloc fail\n", s32Ret);
        goto FAIL;
    }

    memset(pstIveMd->stImg3.pu8VirAddr[0], 0, 16 + pstIveMd->stImg3.u16Stride[0]);
    for (i = 1; i < pstIveMd->stImg3.u16Height; i++)
    {
        memset(pstIveMd->stImg3.pu8VirAddr[0] + i * pstIveMd->stImg3.u16Stride[0], 0, 16);
    }
    pstIveMd->stImg4.enType = IVE_IMAGE_TYPE_U64C1;
    pstIveMd->stImg4.u16Width = ((pstIveMd->u16OriWidth >> 3) & (~0xf));
    pstIveMd->stImg4.u16Height = ((pstIveMd->u16OriHeight >> 3) & (~0xf));
    pstIveMd->stImg4.u16Stride[0] = (pstIveMd->stImg3.u16Stride[0] >> 3);
    pstIveMd->stImg4.u32PhyAddr[0] = pstIveMd->stImg3.u32PhyAddr[0] + 16 + pstIveMd->stImg3.u16Stride[0];
    pstIveMd->stImg4.pu8VirAddr[0] = pstIveMd->stImg3.pu8VirAddr[0] + 16 + pstIveMd->stImg3.u16Stride[0];


    pstIveMd->stImg5.enType = IVE_IMAGE_TYPE_U8C1;
    pstIveMd->stImg5.u16Width = (pstIveMd->u16OriWidth >> 4) & (~0x3);
    pstIveMd->stImg5.u16Height = (pstIveMd->u16OriHeight >> 4) & (~0x3);
    pstIveMd->stImg5.u16Stride[0] = pstIveMd->stImg5.u16Width;
    u32Size = pstIveMd->stImg5.u16Stride[0] * pstIveMd->stImg5.u16Height;
    s32Ret = HI_MPI_SYS_MmzAlloc(&pstIveMd->stImg5.u32PhyAddr[0], (void**)&pstIveMd->stImg5.pu8VirAddr[0], NULL, HI_NULL, u32Size);
    if (s32Ret != HI_SUCCESS)
    {
        CHECK_RET("pstIveMd->stImage 5 fail \n", s32Ret);
        printf("alloc size: %d w %d h %d s %d\n", u32Size, pstIveMd->stImg5.u16Width, pstIveMd->stImg5.u16Height, pstIveMd->stImg5.u16Stride[0]);
        goto FAIL;
    }

    memcpy(&pstIveMd->stImg1, &pstIveMd->stImg5, sizeof(pstIveMd->stImg5));

    if ((pstIveMd->stImg1.u16Stride[0] & (IVE2_ALIGN - 1)) != 0)
    {
        for (i = 4; i < 255; i++)
        {
            if (u32Size == (u32Size / (i * IVE2_ALIGN)) * (i * IVE2_ALIGN))
            { break; }
        }
        if (i == 255)
        {
            printf("can't find stride,can't use ive hist\n");
            goto FAIL;
        }
        pstIveMd->stImg1.u16Stride[0] = i * IVE2_ALIGN;
        pstIveMd->stImg1.u16Width = pstIveMd->stImg1.u16Stride[0];
        pstIveMd->stImg1.u16Height = u32Size / (i * IVE2_ALIGN);
    }

    u32Size = 1024 * 4;
    s32Ret = HI_MPI_SYS_MmzAlloc(&pstIveMd->stHistInfo.u32PhyAddr[0], (void**)&pstIveMd->stHistInfo.pu8VirAddr[0], NULL, HI_NULL, u32Size);
    if (s32Ret != HI_SUCCESS)
    {
        CHECK_RET("pstIveMd->stHistInfo fail\n", s32Ret);
        goto FAIL;
    }



    pstIveMd->stCCScaleTmp.stVFrame.u32Width = calcStride(MAX2((pstIveMd->u16OriWidth >> 4), CCIMGWIDTH), IVE2_ALIGN);
    pstIveMd->stCCScaleTmp.stVFrame.u32Height = MAX2((pstIveMd->u16OriWidth >> 4), CCIMGHEIGHT);
    pstIveMd->stCCScaleTmp.stVFrame.u32Stride[0] = pstIveMd->stCCScaleTmp.stVFrame.u32Width;
    u32Size = pstIveMd->stCCScaleTmp.stVFrame.u32Width * pstIveMd->stCCScaleTmp.stVFrame.u32Height * 3 >> 1;
    s32Ret = HI_MPI_SYS_MmzAlloc(&pstIveMd->stCCScaleTmp.stVFrame.u32PhyAddr[0], (void**)&pstIveMd->stCCScaleTmp.stVFrame.pVirAddr[0], NULL, HI_NULL, u32Size);
    pstIveMd->stCCScaleTmp.stVFrame.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    pstIveMd->stCCScaleTmp.stVFrame.enCompressMode = COMPRESS_MODE_NONE;
    pstIveMd->stCCScaleTmp.stVFrame.u32PhyAddr[1] = pstIveMd->stCCScaleTmp.stVFrame.u32PhyAddr[0] + pstIveMd->stCCScaleTmp.stVFrame.u32Stride[0] * pstIveMd->stCCScaleTmp.stVFrame.u32Height;
    pstIveMd->stCCScaleTmp.stVFrame.pVirAddr[1] =	(HI_VOID*)((HI_U8*)pstIveMd->stCCScaleTmp.stVFrame.pVirAddr[0] + pstIveMd->stCCScaleTmp.stVFrame.u32Stride[0] * pstIveMd->stCCScaleTmp.stVFrame.u32Height);
    pstIveMd->stCCScaleTmp.stVFrame.u32PhyAddr[2] = pstIveMd->stCCScaleTmp.stVFrame.u32PhyAddr[1] + (pstIveMd->stCCScaleTmp.stVFrame.u32Stride[1] * pstIveMd->stCCScaleTmp.stVFrame.u32Height >> 1) + 1;
    pstIveMd->stCCScaleTmp.stVFrame.pVirAddr[2] = (HI_VOID*)((HI_U8*)pstIveMd->stCCScaleTmp.stVFrame.pVirAddr[1] + (pstIveMd->stCCScaleTmp.stVFrame.u32Stride[1] * pstIveMd->stCCScaleTmp.stVFrame.u32Height >> 1) + 1);
    pstIveMd->stCCScaleTmp.stVFrame.u32Stride[1] = pstIveMd->stCCScaleTmp.stVFrame.u32Stride[0];
    pstIveMd->stCCScaleTmp.stVFrame.u32Stride[2] = pstIveMd->stCCScaleTmp.stVFrame.u32Stride[0];
    if (s32Ret != HI_SUCCESS)
    {
        CHECK_RET("pstIveMd->stCCScaleTmp fail\n", s32Ret);
        goto FAIL;
    }
    if ( (pstIveMd->stCCScaleTmp.stVFrame.u32Width == CCIMGWIDTH) && (pstIveMd->stCCScaleTmp.stVFrame.u32Height == CCIMGHEIGHT) )
    {
        memset(&pstIveMd->stCCScaleOut, 0, sizeof(pstIveMd->stCCScaleOut));
    }
    else
    {
        pstIveMd->stCCScaleOut.stVFrame.u32Width = CCIMGWIDTH;
        pstIveMd->stCCScaleOut.stVFrame.u32Height = CCIMGHEIGHT;
        pstIveMd->stCCScaleOut.stVFrame.u32Stride[0] = pstIveMd->stCCScaleOut.stVFrame.u32Width;
        u32Size = pstIveMd->stCCScaleOut.stVFrame.u32Width * pstIveMd->stCCScaleOut.stVFrame.u32Height * 3 >> 1;
        s32Ret = HI_MPI_SYS_MmzAlloc(&pstIveMd->stCCScaleOut.stVFrame.u32PhyAddr[0], (void**)&pstIveMd->stCCScaleOut.stVFrame.pVirAddr[0], NULL, HI_NULL, u32Size);
        if (s32Ret != HI_SUCCESS)
        {
            CHECK_RET("pstIveMd->stCCScaleOut fail\n", s32Ret);
            goto FAIL;
        }
        pstIveMd->stCCScaleOut.stVFrame.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
        pstIveMd->stCCScaleOut.stVFrame.enCompressMode = COMPRESS_MODE_NONE;
        pstIveMd->stCCScaleOut.stVFrame.u32PhyAddr[1] = pstIveMd->stCCScaleOut.stVFrame.u32PhyAddr[0] + pstIveMd->stCCScaleOut.stVFrame.u32Stride[0] * pstIveMd->stCCScaleOut.stVFrame.u32Height;
        pstIveMd->stCCScaleOut.stVFrame.pVirAddr[1] =	(HI_VOID*)((HI_U8*)pstIveMd->stCCScaleTmp.stVFrame.pVirAddr[0] + pstIveMd->stCCScaleOut.stVFrame.u32Stride[0] * pstIveMd->stCCScaleOut.stVFrame.u32Height);
        pstIveMd->stCCScaleOut.stVFrame.u32PhyAddr[2] = pstIveMd->stCCScaleOut.stVFrame.u32PhyAddr[1] + (pstIveMd->stCCScaleOut.stVFrame.u32Stride[1] * pstIveMd->stCCScaleOut.stVFrame.u32Height >> 1) + 1;
        pstIveMd->stCCScaleOut.stVFrame.pVirAddr[2] = (HI_VOID*)((HI_U8*)pstIveMd->stCCScaleOut.stVFrame.pVirAddr[1] + (pstIveMd->stCCScaleOut.stVFrame.u32Stride[1] * pstIveMd->stCCScaleOut.stVFrame.u32Height >> 1) + 1);
        pstIveMd->stCCScaleOut.stVFrame.u32Stride[1] = pstIveMd->stCCScaleOut.stVFrame.u32Stride[0];
        pstIveMd->stCCScaleOut.stVFrame.u32Stride[2] = pstIveMd->stCCScaleOut.stVFrame.u32Stride[0];

    }

    pstIveMd->stCCIntegtmp.u16Width = CCIMGWIDTH;
    pstIveMd->stCCIntegtmp.u16Height = CCIMGHEIGHT + 1;
    pstIveMd->stCCIntegtmp.u16Stride[0] = (calcStride(pstIveMd->stCCIntegtmp.u16Width + 4, IVE2_ALIGN)) << 2;
    u32Size = pstIveMd->stCCIntegtmp.u16Stride[0] * pstIveMd->stCCIntegtmp.u16Height;
    s32Ret = HI_MPI_SYS_MmzAlloc(&pstIveMd->stCCIntegtmp.u32PhyAddr[0], (void**)&pstIveMd->stCCIntegtmp.pu8VirAddr[0], NULL, HI_NULL, u32Size);
    if (s32Ret != HI_SUCCESS)
    {
        CHECK_RET("pstIveMd->stCCInteg fail\n", s32Ret);
        goto FAIL;
    }
    memset(pstIveMd->stCCIntegtmp.pu8VirAddr[0], 0, (pstIveMd->stCCIntegtmp.u16Stride[0]));
    for (i = 0; i < pstIveMd->stCCIntegtmp.u16Height; i++)
    {
        memset(pstIveMd->stCCIntegtmp.pu8VirAddr[0] + i * pstIveMd->stCCIntegtmp.u16Stride[0], 0, 16);
    }
    memcpy(&pstIveMd->stCCInteg, &pstIveMd->stCCIntegtmp, sizeof(IVE_IMAGE_S));
    pstIveMd->stCCInteg.u16Stride[0] = pstIveMd->stCCIntegtmp.u16Stride[0] >> 2;
    pstIveMd->stCCInteg.u32PhyAddr[0] = pstIveMd->stCCIntegtmp.u32PhyAddr[0] + (pstIveMd->stCCIntegtmp.u16Stride[0] + 16);
    pstIveMd->stCCInteg.pu8VirAddr[0] = pstIveMd->stCCIntegtmp.pu8VirAddr[0] + (pstIveMd->stCCIntegtmp.u16Stride[0] + 16);
    pstIveMd->stCCInteg.u16Height = pstIveMd->stCCIntegtmp.u16Height - 1;
    pstIveMd->stCCInteg.enType = IVE_IMAGE_TYPE_U32C1;

    pstIveMd->stCCCalBuf.u16Width = (CCIMGWIDTH << 4) + (CCIMGHEIGHT << 4);
    pstIveMd->stCCCalBuf.u16Height = 1;
    pstIveMd->stCCCalBuf.u16Stride[0] = pstIveMd->stCCCalBuf.u16Width;
    pstIveMd->stCCCalBuf.pu8VirAddr[0] = (HI_U8*)malloc(pstIveMd->stCCCalBuf.u16Width);
    if (pstIveMd->stCCCalBuf.pu8VirAddr[0] == HI_NULL)
    {
        CHECK_RET("pstIveMd->stCCInteg fail\n", s32Ret);
        goto FAIL;
    }




FAIL:
    if (HI_SUCCESS != s32Ret)
    {
        IVE_DetectDestory(pstIveMd);
    }
    return s32Ret;
}

int HI_MPI_IVE_GetFrame(int ViExtChn, VIDEO_FRAME_INFO_S* pstFrameInfo, int s32GetFrameMilliSec)
{
    int s32Ret;
    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;

    VpssGrp = stIveMd.s32VpssGrp;
    VpssChn = stIveMd.s32VpssChn;
    //s32Ret = HI_MPI_VI_GetFrame(ViExtChn, pstFrameInfo, s32GetFrameMilliSec);
    s32Ret = HI_MPI_VPSS_GetChnFrame( VpssGrp, VpssChn, pstFrameInfo, s32GetFrameMilliSec);
    if (s32Ret != HI_SUCCESS)
    {
        CHECK_RET("get frame failed\n", s32Ret);
        return s32Ret;
    }

    return s32Ret;
}

int HI_MPI_IVE_ReleaseFrame(int ViExtChn, VIDEO_FRAME_INFO_S* pstFrameInfo)
{
    int s32Ret;
    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VpssGrp = stIveMd.s32VpssGrp;
    VpssChn = stIveMd.s32VpssChn;

    //s32Ret = HI_MPI_VI_ReleaseFrame(ViExtChn, pstFrameInfo);
    s32Ret = HI_MPI_VPSS_ReleaseChnFrame( VpssGrp, VpssChn, pstFrameInfo);
    if (s32Ret != HI_SUCCESS)
    {
        CHECK_RET("release frame failed\n", s32Ret);
        return s32Ret;
    }

    return s32Ret;
}

int EncMadiCal(IVE_IMAGE_S* pstDstImage, IVE_IMAGE_S* pstSrcImage, int offsetxy)
{
    int s32EncOffset;
    HI_U32 u32ValAL, u32ValBL, u32ValCL, u32ValDL;
    HI_U32 u32ValAH, u32ValBH, u32ValCH, u32ValDH;
    HI_U8* pu8Img;
    HI_U32 mean, sqsum, *pu32Src;
    HI_U64 u64ValA, u64ValB, u64ValC, u64ValD;
    int i, j;
    s32EncOffset = (pstSrcImage->u16Width * pstSrcImage->u16Height >> 8) * offsetxy;
    pu8Img = pstDstImage->pu8VirAddr[0] + s32EncOffset;
    pu32Src = (HI_U32*)pstSrcImage->pu8VirAddr[0];
    for (i = 0; i < (pstSrcImage->u16Height >> 4); i++)
    {
        u32ValAL = pu32Src[-2];
        u32ValAH = pu32Src[-1];
        u32ValCL = pu32Src[15 * (pstSrcImage->u16Stride[0] * 2) - 2];
        u32ValCH = pu32Src[15 * (pstSrcImage->u16Stride[0] * 2) - 1];
        for (j = 0; j < (pstSrcImage->u16Width >> 4); j++)
        {
            u32ValBL = pu32Src[(j * 16 + 15) * 2 - (pstSrcImage->u16Stride[0] * 2)];
            u32ValBH = pu32Src[(j * 16 + 15) * 2 + 1 - (pstSrcImage->u16Stride[0] * 2)];
            u32ValDL = pu32Src[j * 32 + 30 * (pstSrcImage->u16Stride[0]) + 30];
            u32ValDH = pu32Src[j * 32 + 30 * (pstSrcImage->u16Stride[0]) + 30 + 1];
            mean = ((u32ValAL & 0xfffffff) + (u32ValDL & 0xfffffff) - (u32ValBL & 0xfffffff) - (u32ValCL & 0xfffffff));
            u64ValA = (u32ValAH << 4);
            u64ValA = (u32ValAL >> 28) | u64ValA;
            u64ValB = (u32ValBH << 4);
            u64ValB = (u32ValBL >> 28) | u64ValB;
            u64ValC = (u32ValCH << 4);
            u64ValC = (u32ValCL >> 28) | u64ValC;
            u64ValD = (u32ValDH << 4);
            u64ValD = (u32ValDL >> 28) | u64ValD;
            sqsum = (u64ValD + u64ValA - u64ValB - u64ValC);
            *pu8Img++ = (HI_U8)(MAX2((sqrt((HI_S32)(sqsum - (mean * mean >> 8) + 64) >> 8)), 0));
            u32ValAL = u32ValBL;
            u32ValAH = u32ValBH;
            u32ValCL = u32ValDL;
            u32ValCH = u32ValDH;

        }
        pu32Src += ((pstSrcImage->u16Stride[0]) << 5);
    }
    return 0;
}

HI_S32 EncSetMbQp(int EncChn, int* ps32Hist, int width, int height, int u32HistSize)
{
    int i, j;
    float f32factor;
    HI_U16 u16Hist[256];
    HI_U32 pu32IThresh[12];
    HI_U32 pu32PThresh[12];
    VENC_RC_PARAM_S stRcParam;
    int s32Ret;
    j = 0;
    f32factor = 1.0 / u32HistSize;
    u16Hist[0] = ps32Hist[0];
    for (i = 1; i < 256; i++)
    {
        u16Hist[i] = u16Hist[i - 1] + ps32Hist[i];
        u16Hist[i - 1] = (HI_U16)(u16Hist[i - 1] * f32factor * 100);
    }
    u16Hist[255] = 100;
    for (i = 0; i < 12; i++)
    {
        pu32IThresh[i] = 255;
        pu32PThresh[i] = 255;
    }
    i = 0;
    for (j = 0; j < 12; j++)
    {
        while ((u16Hist[i] < u16IThreshTable[j]) && (i < 254))
        {
            i++;
        }
        pu32IThresh[j] = MIN2(i + 1, 255);
    }
    i = 0;
    for (j = 0; j < 12; j++)
    {
        while ((u16Hist[i] < u16PThreshTable[j]) && (i < 254))
        {
            i++;
        }
        pu32PThresh[j] = MIN2(i + 1, 255);
    }

    s32Ret = HI_MPI_VENC_GetRcParam(EncChn, &stRcParam);
    if (HI_SUCCESS != s32Ret)
    {
        CHECK_RET("HI_MPI_VENC_GetRcParam failed\n", s32Ret);
    }
    memcpy(stRcParam.u32ThrdI, pu32IThresh, 12 * 4);
    memcpy(stRcParam.u32ThrdP, pu32PThresh, 12 * 4);
    s32Ret = HI_MPI_VENC_SetRcParam(EncChn, &stRcParam);
    if (HI_SUCCESS != s32Ret)
    {
        CHECK_RET("HI_MPI_VENC_SetRcParam failed\n", s32Ret);
    }
    return s32Ret;
}

#define HI3516A_RC
HI_S32 EncSetMbQpSec(int EncChn, int* ps32Hist, int width, int height, int u32HistSize)
{
    int i, StaticMadi[25] = {0}, percentmadi[25], totalmadi[25], totalpercentmadi[25], tage8 = 25, tage2 = 2, tage5 = 7, dp = 0;
    int mbnum, lastmbnum;
    HI_U32 pu32IThresh[12];
    HI_U32 pu32PThresh[12];
    VENC_RC_PARAM_S stRcParam;
    int s32Ret;

    mbnum = u32HistSize;
    lastmbnum = 0;
    for (i = 0; i < 24; i++)
    {
        StaticMadi[i] = ps32Hist[2 * i] + ps32Hist[2 * i + 1];
        lastmbnum += StaticMadi[i];
    }
    StaticMadi[24] = mbnum - lastmbnum;

    for (i = 0; i < 25; i++)
    {
        percentmadi[i] = StaticMadi[i] * 100 / mbnum;
    }
    totalmadi[0] = StaticMadi[0];
    totalpercentmadi[0] = percentmadi[0];
    for (i = 1; i < 25; i++)
    {
        totalmadi[i] = totalmadi[i - 1] + StaticMadi[i];
        totalpercentmadi[i] =  totalmadi[i] * 100 / mbnum;
    }
    for (i = 1; i < 25; i++)
    {
        if (totalpercentmadi[i] >= 80 && totalpercentmadi[i - 1] < 80)
        {
            tage8 = i * 2;
        }
    }
    for (i = 1; i < 25; i++)
    {
        if (totalpercentmadi[i] >= 50 && totalpercentmadi[i - 1] < 50)
        {
            tage5 = i * 2;
        }
    }
    for (i = 1; i < 25; i++)
    {
        if (totalpercentmadi[i] >= 20 && totalpercentmadi[i - 1] < 20)
        {
            tage2 = i * 2 + 1;
        }
    }
#ifdef HI3516A_RC
    {
        if (totalpercentmadi[0] < 1 && totalpercentmadi[1] < 5)
        {
            pu32IThresh[0] = tage2;
            pu32IThresh[1] = tage2;
            pu32IThresh[2] = tage2;
            pu32IThresh[3] = tage2;
        }
        else
        {
            pu32IThresh[0] = tage2;
            pu32IThresh[1] = tage2;
            pu32IThresh[2] = tage2 + 2;
            pu32IThresh[3] = tage2 + 2;
        }

    }
#else
    {
        if (totalpercentmadi[0] < 1 && totalpercentmadi[1] < 5)
        {
            pu32IThresh[0] = 0;
            pu32IThresh[1] = 0;
            pu32IThresh[2] = 2;
            pu32IThresh[3] = 2;
        }
        else
        {
            if (totalpercentmadi[1] < 10 && percentmadi[0] == 0)
            {
                pu32IThresh[0] = 0;
                pu32IThresh[1] = 0;
                pu32IThresh[2] = 3;
                pu32IThresh[3] = 3;
            }
            else if (totalpercentmadi[1] > 30 && totalpercentmadi[2] > 50)
            {
                pu32IThresh[0] = 2;
                pu32IThresh[1] = 2;
                pu32IThresh[2] = 4;
                pu32IThresh[3] = 4;
            }
            else
            {
                pu32IThresh[0] = 3;
                pu32IThresh[1] = 3;
                pu32IThresh[2] = 5;
                pu32IThresh[3] = 5;

            }
        }
    }
#endif
    tage8 = MAX2(tage8, 15);
    tage5 = MAX2(tage5, 7);
    dp = (tage8 - tage5) / 3;
    pu32PThresh[0] = pu32IThresh[0];
    pu32PThresh[1] = pu32IThresh[1];
    pu32PThresh[2] = pu32IThresh[2];
    pu32PThresh[3] = pu32IThresh[3];
    for ( i = 0; i < 5; i++)
    {
        pu32PThresh[i] = tage5;
    }
    for ( i = 5; i < 9; i++)
    {
        pu32PThresh[i] = pu32PThresh[4] + dp;
    }
    for ( i = 9; i < 12; i++)
    {
        pu32PThresh[i] = pu32PThresh[8] + dp + 1;
    }
    dp = (tage8 - pu32IThresh[3] + 2) / 4;
    for ( i = 4; i < 11; i++)
    {
        pu32IThresh[i] = pu32IThresh[i - 1] + dp;
        pu32IThresh[i + 1] = pu32IThresh[i];
        i++;
    }
#if 0
    for ( i = 0; i < 12; i++)
    {
        printf("%d\t", pu32IThresh[i]);
    }
    printf("\n");
#endif
    s32Ret = HI_MPI_VENC_GetRcParam(EncChn, &stRcParam);
    if (HI_SUCCESS != s32Ret)
    {
        CHECK_RET("HI_MPI_VENC_GetRcParam failed\n", s32Ret);
    }
    memcpy(stRcParam.u32ThrdI, pu32IThresh, 12 * 4);
    memcpy(stRcParam.u32ThrdP, pu32PThresh, 12 * 4);
    s32Ret = HI_MPI_VENC_SetRcParam(EncChn, &stRcParam);
    if (HI_SUCCESS != s32Ret)
    {
        CHECK_RET("HI_MPI_VENC_SetRcParam failed\n", s32Ret);
    }
    return s32Ret;

}

HI_VOID* ImageAnalyProc(HI_VOID* pArgs)
{
    IVE_MD* pstIveMd;
    HI_S32 s32Ret;
    //HI_S32 s32GetFrameMilliSec = 100;
    IVE_IMAGE_S stSrcImage;
    IVE_HANDLE IveHandle;
    HI_U32 u32Offset;
    IVE_INTEG_CTRL_S eIveIntegCtrl;
    //IVE_DMA_CTRL_S stIveDmaCtrl;
    HI_BOOL bFinish, bBlock;
    IVE_MEM_INFO_S stHistMem;
    int i, j;
    VIDEO_FRAME_INFO_S* pstFrameInfo;
    prctl(PR_SET_NAME, (unsigned long)"ImageAnaly", 0, 0, 0);

    pstIveMd = (IVE_MD*)pArgs;
    bBlock = HI_TRUE;
    pstFrameInfo = (VIDEO_FRAME_INFO_S*)malloc(sizeof(VIDEO_FRAME_INFO_S) * 2);
    if (pstFrameInfo == NULL)
    {
        printf("can't alloc frame info\n");
        return NULL;
    }
    eIveIntegCtrl.enOutCtrl = IVE_INTEG_OUT_CTRL_COMBINE;
    stSrcImage.enType = IVE_IMAGE_TYPE_U8C1;

    stSrcImage.u16Height = (pstIveMd->u16OriHeight >> 3) & (~0xf);
    stSrcImage.u16Width = (pstIveMd->u16OriWidth >> 3) & (~0xf);

    //stIveDmaCtrl.enMode=IVE_DMA_MODE_DIRECT_COPY;

    while (pstIveMd->bStopSignal == HI_FALSE)
    {
        s32Ret = HI_MPI_IVE_GetFrame(0, pstFrameInfo, 200);
        if (HI_SUCCESS != s32Ret)
        {
            usleep(200000);
            continue;
        }
        for (i = 0; i < 8; i++)
        {
            for (j = 0; j < 8; j++)
            {
                u32Offset = (j * stSrcImage.u16Width) + (i * (stSrcImage.u16Height) * pstFrameInfo->stVFrame.u32Stride[0]);
                stSrcImage.pu8VirAddr[0] = ((HI_U8*)pstFrameInfo->stVFrame.pVirAddr[0] + u32Offset);
                stSrcImage.u32PhyAddr[0] = pstFrameInfo->stVFrame.u32PhyAddr[0] + u32Offset;
                stSrcImage.u16Stride[0] = pstFrameInfo->stVFrame.u32Stride[0];

                s32Ret = HI_MPI_IVE_Integ(&IveHandle, &stSrcImage, &pstIveMd->stImg4, &eIveIntegCtrl, HI_FALSE);
                if (HI_SUCCESS != s32Ret)
                {
                    CHECK_RET("HI_MPI_IVE_Integ failed\n", s32Ret);
                    (HI_VOID)HI_MPI_IVE_ReleaseFrame(0, pstFrameInfo);
                    goto ANALYFAIL;
                }
                s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, bBlock);
                if (HI_SUCCESS != s32Ret || bFinish != HI_TRUE)
                {
                    CHECK_RET("HI_MPI_IVE_Query failed\n", s32Ret);
                    (HI_VOID)HI_MPI_IVE_ReleaseFrame(0, pstFrameInfo);
                    goto ANALYFAIL;
                }
                EncMadiCal(&pstIveMd->stImg5, &pstIveMd->stImg4, i * 8 + j);
                s32Ret = HI_MPI_SYS_MmzFlushCache(pstIveMd->stImg4.u32PhyAddr[0], pstIveMd->stImg4.pu8VirAddr[0],
                                                  (pstIveMd->stImg4.u16Stride[0] * pstIveMd->stImg4.u16Height << 3));
                CHECK_RET("HI_MPI_SYS_MmzFlushCache failed\n", s32Ret);
#if 0
                while (1)
                {
                    usleep(10000);
                }
#endif
            }
        }
        stHistMem.pu8VirAddr = pstIveMd->stHistInfo.pu8VirAddr[0];
        stHistMem.u32PhyAddr = pstIveMd->stHistInfo.u32PhyAddr[0];
        stHistMem.u32Size = 4 * 1024;

        s32Ret = HI_MPI_SYS_MmzFlushCache(pstIveMd->stImg5.u32PhyAddr[0], pstIveMd->stImg5.pu8VirAddr[0],
                                          pstIveMd->stImg5.u16Stride[0] * pstIveMd->stImg5.u16Height);
        CHECK_RET("HI_MPI_SYS_MmzFlushCache failed\n", s32Ret);


        s32Ret = HI_MPI_IVE_Hist(&IveHandle, &pstIveMd->stImg1, &stHistMem, HI_FALSE);

        if (HI_SUCCESS != s32Ret)
        {
            CHECK_RET("HI_MPI_IVE_Hist failed\n", s32Ret);
            (HI_VOID)HI_MPI_IVE_ReleaseFrame(0, pstFrameInfo);
            goto ANALYFAIL;
        }
        s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, bBlock);
        if (HI_SUCCESS != s32Ret)
        {
            CHECK_RET("HI_MPI_IVE_Query failed\n", s32Ret);
            (HI_VOID)HI_MPI_IVE_ReleaseFrame(0, pstFrameInfo);
            goto ANALYFAIL;
        }
        s32Ret = HI_MPI_IVE_ReleaseFrame(0, pstFrameInfo);
        if (HI_SUCCESS != s32Ret)
        {
            CHECK_RET("HI_MPI_IVE_ReleaseFrame failed\n", s32Ret);
            goto ANALYFAIL;
        }
        if (pstIveMd->eRCMode == E_MBINTEG)
        {
            s32Ret = EncSetMbQp(0, (int*)(pstIveMd->stHistInfo.pu8VirAddr[0]), pstIveMd->u16OriWidth, pstIveMd->u16OriHeight, pstIveMd->stImg5.u16Width * pstIveMd->stImg5.u16Height);
        }
        else
        {
            s32Ret = EncSetMbQpSec(0, (int*)(pstIveMd->stHistInfo.pu8VirAddr[0]), pstIveMd->u16OriWidth, pstIveMd->u16OriHeight, pstIveMd->stImg5.u16Width * pstIveMd->stImg5.u16Height);
        }
        if (HI_SUCCESS != s32Ret)
        {
            CHECK_RET("EncSetMbQp failed\n", s32Ret);
            (HI_VOID)HI_MPI_IVE_ReleaseFrame(0, pstFrameInfo);
            goto ANALYFAIL;
        }
        usleep(2000000);
    }
ANALYFAIL:
    free(pstFrameInfo);

    return HI_NULL;
}


HI_S32 CommSceneautoIVEStart(HI_S32 s32VpssGrp, HI_S32 s32VpssChn)
{
    HI_S32 s32Ret = 0;
    VPSS_GRP VpssGrp = s32VpssGrp;
    VPSS_CHN VpssChn = s32VpssChn;
    VPSS_CHN_MODE_S stVpssChnMode;
    int ViExtChn = VIU_EXT_CHN_START;

    memset(&stIveMd, 0, sizeof(IVE_MD));
    stIveMd.s32VpssGrp = s32VpssGrp;
    stIveMd.s32VpssChn = s32VpssChn;
    s32Ret = HI_MPI_VPSS_GetChnMode(VpssGrp, VpssChn, &stVpssChnMode);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VPSS_GetChnMode failed\n");
        return HI_FAILURE;
    }


    stIveMd.u16OriWidth = stVpssChnMode.u32Width;
    stIveMd.u16OriHeight = stVpssChnMode.u32Height;

    stIveMd.bPause = HI_TRUE;
    stIveMd.bStopSignal = HI_FALSE;

    IVE_DetectInit(&stIveMd);

    stIveMd.ViExtChn = ViExtChn;

    s32Ret = pthread_create(&stIveMd.hIveMBThread, 0, ImageAnalyProc, (HI_VOID*)&stIveMd);
    if (HI_SUCCESS != s32Ret)
    {
        printf("create ImageAnalyProc thread failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoIVEStop()
{
    stIveMd.bStopSignal = HI_TRUE;

    (void)pthread_join(stIveMd.hIveMBThread, NULL);
    IVE_DetectDestory(&stIveMd);

    return HI_SUCCESS;
}

HI_S32 CommSceneautoSetRegister(HI_S32 s32IspDev, HI_U32 u32Addr, HI_U32 u32Vaule)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = s32IspDev;

    s32Ret = HI_MPI_ISP_SetRegister(IspDev, u32Addr, u32Vaule);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetRegister failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetAcmAttr(HI_S32 s32IspDev, ADPT_SCENEAUTO_ACM_ATTR_S* pstAdptAcmAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = s32IspDev;
    ISP_ACM_ATTR_S stAcmAttr;

    s32Ret = HI_MPI_ISP_GetAcmAttr(IspDev, &stAcmAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetAcmAttr failed\n");
        return HI_FAILURE;
    }

    pstAdptAcmAttr->bEnable = stAcmAttr.bEnable;
    pstAdptAcmAttr->u32CbcrThr = stAcmAttr.u32CbcrThr;
    pstAdptAcmAttr->u32GainHue = stAcmAttr.u32GainHue;
    pstAdptAcmAttr->u32GainLuma = stAcmAttr.u32GainLuma;
    pstAdptAcmAttr->u32GainSat = stAcmAttr.u32GainSat;

    return HI_SUCCESS;
}

HI_S32 CommSceneautoSetAcmAttr(HI_S32 s32IspDev, const ADPT_SCENEAUTO_ACM_ATTR_S* pstAdptAcmAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = s32IspDev;
    ISP_ACM_ATTR_S stAcmAttr;

    s32Ret = HI_MPI_ISP_GetAcmAttr(IspDev, &stAcmAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetAcmAttr failed\n");
        return HI_FAILURE;
    }


    stAcmAttr.bEnable = pstAdptAcmAttr->bEnable;
    stAcmAttr.u32CbcrThr = pstAdptAcmAttr->u32CbcrThr;
    stAcmAttr.u32GainHue = pstAdptAcmAttr->u32GainHue;
    stAcmAttr.u32GainLuma = pstAdptAcmAttr->u32GainLuma;
    stAcmAttr.u32GainSat = pstAdptAcmAttr->u32GainSat;

    s32Ret = HI_MPI_ISP_SetAcmAttr(IspDev, &stAcmAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetAcmAttr failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetCcmAttr(HI_S32 s32IspDev, ADPT_SCENEAUTO_CCM_ATTR_S* pstAdptCcmAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    ISP_DEV IspDev = s32IspDev;
    ISP_COLORMATRIX_ATTR_S stCcmAttr;

    s32Ret = HI_MPI_ISP_GetCCMAttr(IspDev, &stCcmAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetCCMAttr failed\n");
        return HI_FAILURE;
    }

    pstAdptCcmAttr->u16HighColorTemp = stCcmAttr.stAuto.u16HighColorTemp;
    pstAdptCcmAttr->u16MidColorTemp = stCcmAttr.stAuto.u16MidColorTemp;
    pstAdptCcmAttr->u16LowColorTemp = stCcmAttr.stAuto.u16LowColorTemp;

    for (i = 0; i < 9; i++)
    {

        pstAdptCcmAttr->au16HighCCM[i] = stCcmAttr.stAuto.au16HighCCM[i];
        pstAdptCcmAttr->au16MidCCM[i] = stCcmAttr.stAuto.au16MidCCM[i];
        pstAdptCcmAttr->au16LowCCM[i] = stCcmAttr.stAuto.au16LowCCM[i];
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoSetCcmAttr(HI_S32 s32IspDev, const ADPT_SCENEAUTO_CCM_ATTR_S* pstAdptCcmAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    ISP_DEV IspDev = s32IspDev;
    ISP_COLORMATRIX_ATTR_S stCcmAttr;

    s32Ret = HI_MPI_ISP_GetCCMAttr(IspDev, &stCcmAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetCCMAttr failed\n");
        return HI_FAILURE;
    }

    stCcmAttr.stAuto.u16HighColorTemp = pstAdptCcmAttr->u16HighColorTemp;
    stCcmAttr.stAuto.u16MidColorTemp = pstAdptCcmAttr->u16MidColorTemp;
    stCcmAttr.stAuto.u16LowColorTemp = pstAdptCcmAttr->u16LowColorTemp;

    for (i = 0; i < 9; i++)
    {

        stCcmAttr.stAuto.au16HighCCM[i] = pstAdptCcmAttr->au16HighCCM[i];
        stCcmAttr.stAuto.au16MidCCM[i] = pstAdptCcmAttr->au16MidCCM[i];
        stCcmAttr.stAuto.au16LowCCM[i] = pstAdptCcmAttr->au16LowCCM[i];
    }

    stCcmAttr.enOpType = OP_TYPE_AUTO;
    s32Ret = HI_MPI_ISP_SetCCMAttr(IspDev, &stCcmAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetCCMAttr failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetDemosaic(HI_S32 s32IspDev, ADPT_SCENEAUTO_DEMOSAIC_S* pstAdptDemosaic)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    ISP_DEV IspDev = s32IspDev;
    ISP_DEMOSAIC_ATTR_S stDemosaicAttr;

    s32Ret = HI_MPI_ISP_GetDemosaicAttr(IspDev, &stDemosaicAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDemosaicAttr failed\n");
        return HI_FAILURE;
    }

    pstAdptDemosaic->u8AaSlope = stDemosaicAttr.u8AaSlope;
    pstAdptDemosaic->u8UuSlope = stDemosaicAttr.u8UuSlope;
    pstAdptDemosaic->u8VaSlope = stDemosaicAttr.u8VaSlope;
    pstAdptDemosaic->u8VhSlope = stDemosaicAttr.u8VhSlope;
    for (i = 0; i < 16; i++)
    {
        pstAdptDemosaic->au8LumThresh[i] = stDemosaicAttr.au8LumThresh[i];
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoSetDemosaic(HI_S32 s32IspDev, const ADPT_SCENEAUTO_DEMOSAIC_S* pstAdptDemosaic)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    ISP_DEV IspDev = s32IspDev;
    ISP_DEMOSAIC_ATTR_S stDemosaicAttr;

    s32Ret = HI_MPI_ISP_GetDemosaicAttr(IspDev, &stDemosaicAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDemosaicAttr failed\n");
        return HI_FAILURE;
    }

    stDemosaicAttr.u8AaSlope = pstAdptDemosaic->u8AaSlope;
    stDemosaicAttr.u8UuSlope = pstAdptDemosaic->u8UuSlope;
    stDemosaicAttr.u8VaSlope = pstAdptDemosaic->u8VaSlope;
    stDemosaicAttr.u8VhSlope = pstAdptDemosaic->u8VhSlope;
    for (i = 0; i < 16; i++)
    {
        stDemosaicAttr.au8LumThresh[i] = pstAdptDemosaic->au8LumThresh[i];
    }
    s32Ret = HI_MPI_ISP_SetDemosaicAttr(IspDev, &stDemosaicAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetDemosaicAttr failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetSharpen(HI_S32 s32IspDev, ADPT_SCENEAUTO_SHARPEN_S* pstAdptSharpen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    ISP_DEV IspDev = s32IspDev;
    ISP_SHARPEN_ATTR_S stSharpenAttr;

    s32Ret = HI_MPI_ISP_GetSharpenAttr(IspDev, &stSharpenAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetSharpenAttr failed\n");
        return HI_FAILURE;
    }

    for (i = 0; i < 16; i++)
    {
        pstAdptSharpen->au8SharpenD[i] = stSharpenAttr.stAuto.au8SharpenD[i];
        pstAdptSharpen->au8SharpenRGB[i] = stSharpenAttr.stAuto.au8SharpenRGB[i];
        pstAdptSharpen->au8SharpenUd[i] = stSharpenAttr.stAuto.au8SharpenUd[i];
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoSetSharpen(HI_S32 s32IspDev, const ADPT_SCENEAUTO_SHARPEN_S* pstAdptSharpen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    ISP_DEV IspDev = s32IspDev;
    ISP_SHARPEN_ATTR_S stSharpenAttr;

    s32Ret = HI_MPI_ISP_GetSharpenAttr(IspDev, &stSharpenAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetSharpenAttr failed\n");
        return HI_FAILURE;
    }

    for (i = 0; i < 16; i++)
    {
        stSharpenAttr.stAuto.au8SharpenD[i] = pstAdptSharpen->au8SharpenD[i];
        stSharpenAttr.stAuto.au8SharpenRGB[i] = pstAdptSharpen->au8SharpenRGB[i];
        stSharpenAttr.stAuto.au8SharpenUd[i] = pstAdptSharpen->au8SharpenUd[i];
    }

    s32Ret = HI_MPI_ISP_SetSharpenAttr(IspDev, &stSharpenAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetSharpenAttr failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetDP(HI_S32 s32IspDev, ADPT_SCENEAUTO_DP_S* pstAdptDP)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = s32IspDev;
    ISP_DP_ATTR_S stDPAttr;

    s32Ret = HI_MPI_ISP_GetDPAttr(IspDev, &stDPAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDPAttr failed\n");
        return HI_FAILURE;
    }
    pstAdptDP->u16Slope = stDPAttr.stDynamicAttr.u16Slope;

    return HI_SUCCESS;
}

HI_S32 CommSceneautoSetDP(HI_S32 s32IspDev, const ADPT_SCENEAUTO_DP_S* pstAdptDP)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = s32IspDev;
    ISP_DP_ATTR_S stDPAttr;

    s32Ret = HI_MPI_ISP_GetDPAttr(IspDev, &stDPAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDPAttr failed\n");
        return HI_FAILURE;
    }

    stDPAttr.stDynamicAttr.u16Slope = pstAdptDP->u16Slope;
    s32Ret = HI_MPI_ISP_SetDPAttr(IspDev, &stDPAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetDPAttr failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetPubAttr(HI_S32 s32IspDev, ADPT_SCENEAUTO_PUBATTR_S* pstAdptPubAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = s32IspDev;
    ISP_PUB_ATTR_S stPubAttr;

    s32Ret = HI_MPI_ISP_GetPubAttr(IspDev, &stPubAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetPubAttr failed\n");
        return HI_FAILURE;
    }

    pstAdptPubAttr->u32Frame = (HI_U32)stPubAttr.f32FrameRate;

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetGamma(HI_S32 s32IspDev, ADPT_SCENEAUTO_GAMMA_S* pstAdptGamma)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    ISP_DEV IspDev = s32IspDev;
    ISP_GAMMA_ATTR_S stGammaAttr;

    s32Ret = HI_MPI_ISP_GetGammaAttr(IspDev, &stGammaAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetGammaAttr failed\n");
        return HI_FAILURE;
    }

    pstAdptGamma->u8CurveType = stGammaAttr.enCurveType;
    for (i = 0; i < 257; i++)
    {
        pstAdptGamma->au16GammaTable[i] = stGammaAttr.u16Table[i];
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoSetGamma(HI_S32 s32IspDev, const ADPT_SCENEAUTO_GAMMA_S* pstAdptGamma)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    ISP_DEV IspDev = s32IspDev;
    ISP_GAMMA_ATTR_S stGammaAttr;

    s32Ret = HI_MPI_ISP_GetGammaAttr(IspDev, &stGammaAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetGammaAttr failed\n");
        return HI_FAILURE;
    }

    switch (pstAdptGamma->u8CurveType)
    {
        case 0:
            stGammaAttr.enCurveType = ISP_GAMMA_CURVE_DEFAULT;
            break;
        case 1:
            stGammaAttr.enCurveType = ISP_GAMMA_CURVE_SRGB;
            break;
        case 2:
            stGammaAttr.enCurveType = ISP_GAMMA_CURVE_USER_DEFINE;
            break;
        default:
            break;
    }
    for (i = 0; i < 257; i++)
    {
        stGammaAttr.u16Table[i] = pstAdptGamma->au16GammaTable[i];
    }

    s32Ret = HI_MPI_ISP_SetGammaAttr(IspDev, &stGammaAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetGammaAttr failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetH264Trans(HI_S32 s32VencChn, ADPT_SCENEAUTO_H264TRANS_S* pstAdptH264Trans)
{
    HI_S32 s32Ret = HI_SUCCESS;
    VENC_CHN VencChn = s32VencChn;
    VENC_PARAM_H264_TRANS_S stH264Trans;

    s32Ret = HI_MPI_VENC_GetH264Trans(VencChn, &stH264Trans);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_GetH264Trans failed\n");
        return HI_FAILURE;
    }

    pstAdptH264Trans->chroma_qp_index_offset = stH264Trans.chroma_qp_index_offset;

    return HI_SUCCESS;
}

HI_S32 CommSceneautoSetH264Trans(HI_S32 s32VencChn, const ADPT_SCENEAUTO_H264TRANS_S* pstAdptH264Trans)
{
    HI_S32 s32Ret = HI_SUCCESS;
    VENC_CHN VencChn = s32VencChn;
    VENC_PARAM_H264_TRANS_S stH264Trans;

    s32Ret = HI_MPI_VENC_GetH264Trans(VencChn, &stH264Trans);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_GetH264Trans failed\n");
        return HI_FAILURE;
    }

    stH264Trans.chroma_qp_index_offset = pstAdptH264Trans->chroma_qp_index_offset;

    s32Ret = HI_MPI_VENC_SetH264Trans(VencChn, &stH264Trans);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_SetH264Trans failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetH24Deblock(HI_S32 s32VencChn, ADPT_SCENEAUTO_H264_DEBLOCK_S* pstAdptH264Deblock)
{
    HI_S32 s32Ret = HI_SUCCESS;
    VENC_CHN VencChn = s32VencChn;
    VENC_PARAM_H264_DBLK_S stH264Dblk;

    s32Ret = HI_MPI_VENC_GetH264Dblk(VencChn, &stH264Dblk);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_GetH264Dblk failed\n");
        return HI_FAILURE;
    }

    pstAdptH264Deblock->disable_deblocking_filter_idc = stH264Dblk.disable_deblocking_filter_idc;
    pstAdptH264Deblock->slice_alpha_c0_offset_div2 = stH264Dblk.slice_alpha_c0_offset_div2;
    pstAdptH264Deblock->slice_beta_offset_div2 = stH264Dblk.slice_beta_offset_div2;

    return HI_SUCCESS;
}

HI_S32 CommSceneautoSetH24Deblock(HI_S32 s32VencChn, const ADPT_SCENEAUTO_H264_DEBLOCK_S* pstAdptH264Deblock)
{
    HI_S32 s32Ret = HI_SUCCESS;
    VENC_CHN VencChn = s32VencChn;
    VENC_PARAM_H264_DBLK_S stH264Dblk;

    s32Ret = HI_MPI_VENC_GetH264Dblk(VencChn, &stH264Dblk);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_GetH264Dblk failed\n");
        return HI_FAILURE;
    }

    stH264Dblk.disable_deblocking_filter_idc = pstAdptH264Deblock->disable_deblocking_filter_idc;
    stH264Dblk.slice_alpha_c0_offset_div2 = pstAdptH264Deblock->slice_alpha_c0_offset_div2;
    stH264Dblk.slice_beta_offset_div2 = pstAdptH264Deblock->slice_beta_offset_div2;

    s32Ret = HI_MPI_VENC_SetH264Dblk(VencChn, &stH264Dblk);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_GetH264Dblk failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetH264RcParam(HI_S32 s32VencChn, ADPT_SCENEAUTO_H264_RCPARAM_S* pstAdptH264RCParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    VENC_CHN VencChn = s32VencChn;
    VENC_RC_PARAM_S stRCParam;
    VENC_CHN_ATTR_S stChnAttr;

    s32Ret = HI_MPI_VENC_GetChnAttr(VencChn, &stChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_GetChnAttr failed\n");
        return HI_FAILURE;
    }
    if (stChnAttr.stRcAttr.enRcMode != VENC_RC_MODE_H264CBR && stChnAttr.stRcAttr.enRcMode != VENC_RC_MODE_H264VBR)
    {
        return HI_SUCCESS;
    }
    s32Ret = HI_MPI_VENC_GetRcParam(VencChn, &stRCParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_GetRcParam failed\n");
        return HI_FAILURE;
    }

    pstAdptH264RCParam->u32QpDelta = stRCParam.u32RowQpDelta;
    for (i = 0; i < 12; i++)
    {
        pstAdptH264RCParam->u32ThrdI[i] = stRCParam.u32ThrdI[i];
        pstAdptH264RCParam->u32ThrdP[i] = stRCParam.u32ThrdP[i];
    }
    switch (stChnAttr.stRcAttr.enRcMode)
    {
        case VENC_RC_MODE_H264CBR:
            pstAdptH264RCParam->s32IPQPDelta = stRCParam.stParamH264Cbr.s32IPQPDelta;
            break;
        case VENC_RC_MODE_H264VBR:
            pstAdptH264RCParam->s32IPQPDelta = stRCParam.stParamH264VBR.s32IPQPDelta;
            break;
        default:
            break;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoSetH264RcParam(HI_S32 s32VencChn, const ADPT_SCENEAUTO_H264_RCPARAM_S* pstAdptH264RCParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    VENC_CHN VencChn = s32VencChn;
    VENC_RC_PARAM_S stRCParam;
    VENC_CHN_ATTR_S stChnAttr;
    s32Ret = HI_MPI_VENC_GetChnAttr(VencChn, &stChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_GetChnAttr failed\n");
        return HI_FAILURE;
    }
    if (stChnAttr.stRcAttr.enRcMode != VENC_RC_MODE_H264CBR && stChnAttr.stRcAttr.enRcMode != VENC_RC_MODE_H264VBR)
    {
        return HI_SUCCESS;
    }

    s32Ret = HI_MPI_VENC_GetRcParam(VencChn, &stRCParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_GetRcParam failed\n");
        return HI_FAILURE;
    }

    stRCParam.u32RowQpDelta = pstAdptH264RCParam->u32QpDelta;
    for (i = 0; i < 12; i++)
    {
        stRCParam.u32ThrdI[i] = pstAdptH264RCParam->u32ThrdI[i];
        stRCParam.u32ThrdP[i] = pstAdptH264RCParam->u32ThrdP[i];
    }
    switch (stChnAttr.stRcAttr.enRcMode)
    {
        case VENC_RC_MODE_H264CBR:
            stRCParam.stParamH264Cbr.s32IPQPDelta = pstAdptH264RCParam->s32IPQPDelta;
            break;
        case VENC_RC_MODE_H264VBR:
            stRCParam.stParamH264VBR.s32IPQPDelta = pstAdptH264RCParam->s32IPQPDelta;
            break;
        default:
            break;
    }

    s32Ret = HI_MPI_VENC_SetRcParam(VencChn, &stRCParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_SetRcParam failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetAERoute(HI_S32 s32IspDev, ADPT_SCENEAUTO_AEROUTE_S* pstAdptAERoute)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    ISP_DEV IspDev = s32IspDev;
    ISP_AE_ROUTE_S stAERoute;

    s32Ret = HI_MPI_ISP_GetAERouteAttr(IspDev, &stAERoute);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetAERouteAttr failed\n");
        return HI_FAILURE;
    }

    pstAdptAERoute->u32TotalNum = stAERoute.u32TotalNum;
    for (i = 0; i < 8; i++)
    {
        pstAdptAERoute->astRouteNode[i].u32IntTime = stAERoute.astRouteNode[i].u32IntTime;
        pstAdptAERoute->astRouteNode[i].u32SysGain = stAERoute.astRouteNode[i].u32SysGain;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoSetAERoute(HI_S32 s32IspDev, const ADPT_SCENEAUTO_AEROUTE_S* pstAdptAERoute)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    ISP_DEV IspDev = s32IspDev;
    ISP_AE_ROUTE_S stAERoute;

    s32Ret = HI_MPI_ISP_GetAERouteAttr(IspDev, &stAERoute);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetAERouteAttr failed\n");
        return HI_FAILURE;
    }

    stAERoute.u32TotalNum = pstAdptAERoute->u32TotalNum;
    for (i = 0; i < 8; i++)
    {
        stAERoute.astRouteNode[i].u32IntTime = pstAdptAERoute->astRouteNode[i].u32IntTime;
        stAERoute.astRouteNode[i].u32SysGain = pstAdptAERoute->astRouteNode[i].u32SysGain;
    }

    s32Ret = HI_MPI_ISP_SetAERouteAttr(IspDev, &stAERoute);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetAERouteAttr failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetDCIParam(HI_S32 s32ViDev, ADPT_SCENEAUTO_DCIPARAM_S* pstAdptDCIPara)
{
    HI_S32 s32Ret = HI_SUCCESS;
    VI_DCI_PARAM_S stDCIParam;

    s32Ret = HI_MPI_VI_GetDCIParam(s32ViDev,  &stDCIParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_GetDCIParam failed\n");
        return HI_FAILURE;
    }
    pstAdptDCIPara->bEnable = stDCIParam.bEnable;
    pstAdptDCIPara->u32BlackGain = stDCIParam.u32BlackGain;
    pstAdptDCIPara->u32ContrastGain = stDCIParam.u32ContrastGain;
    pstAdptDCIPara->u32LightGain = stDCIParam.u32LightGain;

    return HI_SUCCESS;
}


HI_S32 CommSceneautoSetDCIParam(HI_S32 s32ViDev, const ADPT_SCENEAUTO_DCIPARAM_S* pstAdptDCIPara)
{
    HI_S32 s32Ret = HI_SUCCESS;
    VI_DCI_PARAM_S stDCIParam;

    s32Ret = HI_MPI_VI_GetDCIParam(s32ViDev,  &stDCIParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_GetDCIParam failed\n");
        return HI_FAILURE;
    }

    stDCIParam.bEnable = pstAdptDCIPara->bEnable;
    stDCIParam.u32BlackGain = pstAdptDCIPara->u32BlackGain;
    stDCIParam.u32ContrastGain = pstAdptDCIPara->u32ContrastGain;
    stDCIParam.u32LightGain = pstAdptDCIPara->u32LightGain;

    s32Ret = HI_MPI_VI_SetDCIParam(s32ViDev,  &stDCIParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_SetDCIParam failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetDRCAttr(HI_S32 s32IspDev, ADPT_SCENEAUTO_DRCATTR_S* pstAdptDRCAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DRC_ATTR_S stDRCAttr;

    s32Ret = HI_MPI_ISP_GetDRCAttr(s32IspDev, &stDRCAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDRCAttr failed\n");
        return HI_FAILURE;
    }

    pstAdptDRCAttr->bEnable = stDRCAttr.bEnable;
    if (stDRCAttr.enOpType == OP_TYPE_MANUAL)
    {
        pstAdptDRCAttr->bManulEnable = HI_TRUE;
    }
    else
    {
        pstAdptDRCAttr->bManulEnable = HI_FALSE;
    }
    pstAdptDRCAttr->u32Strength = stDRCAttr.stManual.u32Strength;
    pstAdptDRCAttr->u32SlopeMax = stDRCAttr.u32SlopeMax;
    pstAdptDRCAttr->u32SlopeMin = stDRCAttr.u32SlopeMin;
    pstAdptDRCAttr->u32VarianceSpace = stDRCAttr.u32VarianceSpace;
    pstAdptDRCAttr->u32VarianceIntensity = stDRCAttr.u32VarianceIntensity;
    pstAdptDRCAttr->u32WhiteLevel = stDRCAttr.u32WhiteLevel;
    pstAdptDRCAttr->u32BlackLevel = stDRCAttr.u32BlackLevel;

    return HI_SUCCESS;
}

HI_S32 CommSceneautoSetDRCAttr(HI_S32 s32IspDev, const ADPT_SCENEAUTO_DRCATTR_S* pstAdptDRCAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DRC_ATTR_S stDRCAttr;

    s32Ret = HI_MPI_ISP_GetDRCAttr(s32IspDev, &stDRCAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDRCAttr failed\n");
        return HI_FAILURE;
    }

    stDRCAttr.bEnable = pstAdptDRCAttr->bEnable;
    if (pstAdptDRCAttr->bManulEnable == HI_TRUE)
    {
        stDRCAttr.enOpType = OP_TYPE_MANUAL;
    }
    else
    {
        stDRCAttr.enOpType = OP_TYPE_AUTO;
    }
    stDRCAttr.stManual.u32Strength = pstAdptDRCAttr->u32Strength;
    stDRCAttr.u32SlopeMax = pstAdptDRCAttr->u32SlopeMax;
    stDRCAttr.u32SlopeMin = pstAdptDRCAttr->u32SlopeMin;
    stDRCAttr.u32VarianceSpace = pstAdptDRCAttr->u32VarianceSpace;
    stDRCAttr.u32VarianceIntensity = pstAdptDRCAttr->u32VarianceIntensity;
    stDRCAttr.u32WhiteLevel = pstAdptDRCAttr->u32WhiteLevel;
    stDRCAttr.u32BlackLevel = pstAdptDRCAttr->u32BlackLevel;

    s32Ret = HI_MPI_ISP_SetDRCAttr(s32IspDev, &stDRCAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetDRCAttr failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetSaturation(HI_S32 s32IspDev, ADPT_SCENEAUTO_SATURATION_S* pstAdptSaturation)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    ISP_DEV IspDev = s32IspDev;
    ISP_SATURATION_ATTR_S stSaturation;

    s32Ret = HI_MPI_ISP_GetSaturationAttr(IspDev, &stSaturation);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetSaturationAttr failed\n");
        return HI_FAILURE;
    }

    pstAdptSaturation->u8OpType = stSaturation.enOpType;
    pstAdptSaturation->u8ManualSat = stSaturation.stManual.u8Saturation;
    for (i = 0; i < 16; i++)
    {
        pstAdptSaturation->au8AutoSat[i] = stSaturation.stAuto.au8Sat[i];
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoSetSaturation(HI_S32 s32IspDev, const ADPT_SCENEAUTO_SATURATION_S* pstAdptSaturation)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    ISP_DEV IspDev = s32IspDev;
    ISP_SATURATION_ATTR_S stSaturation;

    s32Ret = HI_MPI_ISP_GetSaturationAttr(IspDev, &stSaturation);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetSaturationAttr failed\n");
        return HI_FAILURE;
    }

    switch (pstAdptSaturation->u8OpType)
    {
        case 0:
            stSaturation.enOpType = OP_TYPE_AUTO;
            break;
        case 1:
            stSaturation.enOpType = OP_TYPE_MANUAL;
            break;
        default:
            break;
    }
    stSaturation.stManual.u8Saturation = pstAdptSaturation->u8ManualSat;
    for (i = 0; i < 16; i++)
    {
        stSaturation.stAuto.au8Sat[i] = pstAdptSaturation->au8AutoSat[i];
    }

    s32Ret = HI_MPI_ISP_SetSaturationAttr(IspDev, &stSaturation);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetSaturationAttr failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetDISAttr(HI_S32 s32IspDev, ADPT_SCENEAUTO_DIS_ATTR_S* pstAdptDisAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = s32IspDev;
    ISP_DIS_ATTR_S stDisAttr;

    pstAdptDisAttr->bEnable = HI_FALSE;
    s32Ret = HI_MPI_ISP_GetDISAttr(IspDev, &stDisAttr);
    if (HI_SUCCESS != s32Ret)
    {
        //online mode not support dis
        if (0xA0108008 == (HI_U32)s32Ret)
        {
            return HI_SUCCESS;
        }

        printf("HI_MPI_ISP_GetDISAttr failed\n");
        return HI_FAILURE;
    }

    pstAdptDisAttr->bEnable = stDisAttr.bEnable;

    return HI_SUCCESS;
}

HI_S32 CommSceneautoSetDISAttr(HI_S32 s32IspDev, const ADPT_SCENEAUTO_DIS_ATTR_S* pstAdptDisAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = s32IspDev;
    ISP_DIS_ATTR_S stDisAttr;

    s32Ret = HI_MPI_ISP_GetDISAttr(IspDev, &stDisAttr);
    if (HI_SUCCESS != s32Ret)
    {
        //online mode not support dis
        if (0xA0108008 == (HI_U32)s32Ret)
        {
            return HI_SUCCESS;
        }
        printf("HI_MPI_ISP_GetDISAttr failed\n");
        return HI_FAILURE;
    }

    stDisAttr.bEnable = pstAdptDisAttr->bEnable;

    s32Ret = HI_MPI_ISP_SetDISAttr(IspDev, &stDisAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetDISAttr failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetAEAttr(HI_S32 s32IspDev, ADPT_SCENEAUTO_AEATTR_S* pstAdptAEAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = s32IspDev;
    ISP_EXPOSURE_ATTR_S stExposureAttr;
    int i,j;

    s32Ret = HI_MPI_ISP_GetExposureAttr(IspDev, &stExposureAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetExposureAttr failed\n");
        return HI_FAILURE;
    }

    pstAdptAEAttr->u8ExpCompensation = stExposureAttr.stAuto.u8Compensation;
    pstAdptAEAttr->u16BlackDelayFrame = stExposureAttr.stAuto.stAEDelayAttr.u16BlackDelayFrame;
    pstAdptAEAttr->u16WhiteDelayFrame = stExposureAttr.stAuto.stAEDelayAttr.u16WhiteDelayFrame;
    switch (stExposureAttr.stAuto.enAEStrategyMode)
    {
        case AE_EXP_HIGHLIGHT_PRIOR:
            pstAdptAEAttr->u8AEStrategyMode = 0;
            break;
        case AE_EXP_LOWLIGHT_PRIOR:
            pstAdptAEAttr->u8AEStrategyMode = 1;
            break;
        default:
            pstAdptAEAttr->u8AEStrategyMode = 0;
    }
    pstAdptAEAttr->u8AEStrategyMode = stExposureAttr.stAuto.enAEStrategyMode;
    pstAdptAEAttr->u8MaxHistOffset = stExposureAttr.stAuto.u8MaxHistOffset;
    pstAdptAEAttr->u16HistRatioSlope = stExposureAttr.stAuto.u16HistRatioSlope;
    pstAdptAEAttr->u8Speed = stExposureAttr.stAuto.u8Speed;
    pstAdptAEAttr->u8Tolerance = stExposureAttr.stAuto.u8Tolerance;
    pstAdptAEAttr->u32SysGainMax = stExposureAttr.stAuto.stSysGainRange.u32Max;
    pstAdptAEAttr->u8AERunInterval =stExposureAttr.u8AERunInterval;

    for (i = 0; i < AE_ZONE_ROW; i++)
    {
        for (j = 0; j < AE_ZONE_COLUMN; j++)
        {
            pstAdptAEAttr->au8AeWeight[i][j] = stExposureAttr.stAuto.au8Weight[i][j];
        }
    }
    return HI_SUCCESS;
}

HI_S32 CommSceneautoSetAEAttr(HI_S32 s32IspDev, const ADPT_SCENEAUTO_AEATTR_S* pstAdptAEAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = s32IspDev;
    ISP_EXPOSURE_ATTR_S stExposureAttr;
    int i,j;

    s32Ret = HI_MPI_ISP_GetExposureAttr(IspDev, &stExposureAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetExposureAttr failed\n");
        return HI_FAILURE;
    }

    stExposureAttr.stAuto.enAEStrategyMode = AE_EXP_HIGHLIGHT_PRIOR;
    stExposureAttr.stAuto.u8Compensation = pstAdptAEAttr->u8ExpCompensation;
    stExposureAttr.stAuto.stAEDelayAttr.u16BlackDelayFrame = pstAdptAEAttr->u16BlackDelayFrame;
    stExposureAttr.stAuto.stAEDelayAttr.u16WhiteDelayFrame = pstAdptAEAttr->u16WhiteDelayFrame;
    switch (pstAdptAEAttr->u8AEStrategyMode)
    {
        case 0:
            stExposureAttr.stAuto.enAEStrategyMode = AE_EXP_HIGHLIGHT_PRIOR;
            break;
        case 1:
            stExposureAttr.stAuto.enAEStrategyMode = AE_EXP_LOWLIGHT_PRIOR;
            break;
        default:
            stExposureAttr.stAuto.enAEStrategyMode = AE_EXP_HIGHLIGHT_PRIOR;
            break;
    }
    stExposureAttr.stAuto.u8MaxHistOffset = pstAdptAEAttr->u8MaxHistOffset;
    stExposureAttr.stAuto.u16HistRatioSlope = pstAdptAEAttr->u16HistRatioSlope;
    stExposureAttr.stAuto.u8Speed = pstAdptAEAttr->u8Speed;
    stExposureAttr.stAuto.u8Tolerance = pstAdptAEAttr->u8Tolerance;
    stExposureAttr.stAuto.stSysGainRange.u32Max = pstAdptAEAttr->u32SysGainMax;
    stExposureAttr.u8AERunInterval = pstAdptAEAttr->u8AERunInterval;

    for (i = 0; i < AE_ZONE_ROW; i++)
    {
        for (j = 0; j < AE_ZONE_COLUMN; j++)
        {
            stExposureAttr.stAuto.au8Weight[i][j] = pstAdptAEAttr->au8AeWeight[i][j];
        }
    }    
    s32Ret = HI_MPI_ISP_SetExposureAttr(IspDev, &stExposureAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetExposureAttr failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetExposureInfo(HI_S32 s32IspDev, ADPT_SCENEAUTO_EXPOSUREINFO_S* pstAdptExposureInfo)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = s32IspDev;
    HI_S32 i;
    ISP_EXP_INFO_S stIspExpInfo;

    s32Ret = HI_MPI_ISP_QueryExposureInfo(IspDev, &stIspExpInfo);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_QueryExposureInfo failed\n");
        return HI_FAILURE;
    }
    pstAdptExposureInfo->u32AGain = stIspExpInfo.u32AGain;
    pstAdptExposureInfo->u32DGain = stIspExpInfo.u32DGain;
    pstAdptExposureInfo->u32ISPDGain = stIspExpInfo.u32ISPDGain;
    pstAdptExposureInfo->u32ExpTime = stIspExpInfo.u32ExpTime;
    pstAdptExposureInfo->u32Exposure = stIspExpInfo.u32Exposure;
    pstAdptExposureInfo->u8AveLum = stIspExpInfo.u8AveLum;

    for (i = 0; i < 256; i++)
    {
        pstAdptExposureInfo->u32Hist256Value[i] = stIspExpInfo.u32AE_Hist256Value[i];
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetWDRAttr(HI_S32 s32IspDev, ADPT_SCENEAUTO_WDRATTR_S* pstAdptWDRAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = s32IspDev;
    ISP_WDR_MODE_S stWDRMode;

    s32Ret = HI_MPI_ISP_GetWDRMode(IspDev, &stWDRMode);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetWDRMode failed\n");
        return HI_FAILURE;
    }
    pstAdptWDRAttr->u8WdrMode = stWDRMode.enWDRMode;

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetQueryInnerStateInfo(HI_S32 s32IspDev, ADPT_SCENEAUTO_STATEINFO_S* pstAdptStatInfo)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = s32IspDev;
    ISP_INNER_STATE_INFO_S stInnerStateInfo;

    s32Ret = HI_MPI_ISP_QueryInnerStateInfo(IspDev, &stInnerStateInfo);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDRCAttr failed\n");
        return HI_FAILURE;
    }

    pstAdptStatInfo->u32DRCStrengthActual = stInnerStateInfo.u32DRCStrengthActual;
    pstAdptStatInfo->u32DefogStrengthActual = stInnerStateInfo.u32DefogStrengthActual;

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetBitrate(HI_S32 s32VencChn, HI_U32* pu32Bitrate)
{
    HI_S32 s32Ret = HI_SUCCESS;
    VENC_CHN VencChn = s32VencChn;
    VENC_CHN_ATTR_S stVencChnAttr;

    s32Ret = HI_MPI_VENC_GetChnAttr(VencChn, &stVencChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_GetChnAttr failed\n");
        return HI_FAILURE;
    }
    *pu32Bitrate = stVencChnAttr.stRcAttr.stAttrH264Cbr.u32BitRate;

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetDefogAttr(HI_S32 s32IspDev, ADPT_SCENEAUTO_DEFOG_ATTR_S* pstAdptDefogAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = s32IspDev;
    ISP_DEFOG_ATTR_S stDefogAttr;

    s32Ret = HI_MPI_ISP_GetDeFogAttr(IspDev, &stDefogAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDeFogAttr failed\n");
        return HI_FAILURE;
    }

    pstAdptDefogAttr->bEnable = stDefogAttr.bEnable;

    return HI_SUCCESS;
}

HI_S32 CommSceneautoSetFPNAttr(HI_S32 s32IspDev, const ADPT_SCENEAUTO_FPN_ATTR_S* pstAdptFpnAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEV IspDev = s32IspDev;
    ISP_FPN_ATTR_S stFpnAttr;

    s32Ret = HI_MPI_ISP_GetFPNAttr(IspDev, &stFpnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetFPNAttr failed\n");
        return HI_FAILURE;
    }

    stFpnAttr.bEnable = pstAdptFpnAttr->bEnable;

    s32Ret = HI_MPI_ISP_SetFPNAttr(IspDev, &stFpnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetFPNAttr failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoSet3DNR(HI_S32 s32VpssGrp, ADPT_SCENEAUTO_3DNR *pstAdpt3dnr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    VPSS_GRP VpssGrp = s32VpssGrp;
    VPSS_GRP_VPPNRBEX_S stVppnrbex;

    s32Ret = HI_MPI_VPSS_GetNRV3Param(VpssGrp, &stVppnrbex);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VPSS_GetNRV3Param failed\n");
    }

    stVppnrbex.iNRb.SBSi = pstAdpt3dnr->u8SBS[0];
    stVppnrbex.iNRb.SBSj = pstAdpt3dnr->u8SBS[1]; 
    stVppnrbex.iNRb.SBSk = pstAdpt3dnr->u8SBS[2]; 
    stVppnrbex.iNRb.PSFS = pstAdpt3dnr->u16PSFS;
    stVppnrbex.iNRb.SBTi = pstAdpt3dnr->u8SBT[0];
    stVppnrbex.iNRb.SBTj = pstAdpt3dnr->u8SBT[1]; 
    stVppnrbex.iNRb.SBTk = pstAdpt3dnr->u8SBT[2];

    stVppnrbex.iNRb.SDSi = pstAdpt3dnr->u8SDS[0];
    stVppnrbex.iNRb.SDSj = pstAdpt3dnr->u8SDS[1]; 
    stVppnrbex.iNRb.SDSk = pstAdpt3dnr->u8SDS[2];
    stVppnrbex.iNRb.SDTi = pstAdpt3dnr->u8SDT[0];
    stVppnrbex.iNRb.SDTj = pstAdpt3dnr->u8SDT[1]; 
    stVppnrbex.iNRb.SDTk = pstAdpt3dnr->u8SDT[2];

    stVppnrbex.iNRb.SBFi = pstAdpt3dnr->u16SBF[0];
    stVppnrbex.iNRb.SBFj = pstAdpt3dnr->u16SBF[1]; 
    stVppnrbex.iNRb.SBFk = pstAdpt3dnr->u16SBF[2];
    stVppnrbex.iNRb.SFC = pstAdpt3dnr->u8SFC;
    stVppnrbex.iNRb.SHPi = pstAdpt3dnr->u8SHP[0];
    stVppnrbex.iNRb.SHPj = pstAdpt3dnr->u8SHP[1]; 
    stVppnrbex.iNRb.SHPk = pstAdpt3dnr->u8SHP[2];
    stVppnrbex.iNRb.TFC = pstAdpt3dnr->u8TFC;

    stVppnrbex.iNRb.TFSi = pstAdpt3dnr->u16TFS[0];
    stVppnrbex.iNRb.TFSj = pstAdpt3dnr->u16TFS[1];
    stVppnrbex.iNRb.TFSk = pstAdpt3dnr->u16TFS[2];
    stVppnrbex.iNRb.TFRi = pstAdpt3dnr->u16TFR[0];
    stVppnrbex.iNRb.TFRj = pstAdpt3dnr->u16TFR[1];
    stVppnrbex.iNRb.TFRk = pstAdpt3dnr->u16TFR[2];

    stVppnrbex.iNRb.MDZi = pstAdpt3dnr->u16MDZ[0]; 
    stVppnrbex.iNRb.MDZj = pstAdpt3dnr->u16MDZ[1]; 
    stVppnrbex.iNRb.MATH = pstAdpt3dnr->u16MDZ[2];
    stVppnrbex.iNRb.Post = pstAdpt3dnr->u16POST;

    stVppnrbex.MABW     = 1;//pstAdpt3dnr->u8MABW;
    stVppnrbex.PostROW  = 0;//pstAdpt3dnr->u8PostROW;
    stVppnrbex.MATW     = 1;//pstAdpt3dnr->u8MATW;
    stVppnrbex.MDAF     = 3;//pstAdpt3dnr->u8MDAF;
    stVppnrbex.TextThr  = 16;//pstAdpt3dnr->u8TextThr;
    stVppnrbex.MTFS     = 255;//pstAdpt3dnr->u8MTFS;
    stVppnrbex.ExTfThr  = 12;//pstAdpt3dnr->u8ExTfThr;

    s32Ret = HI_MPI_VPSS_SetNRV3Param(VpssGrp, &stVppnrbex);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VPSS_SetNRV3Param failed\n");
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetVencAttr(HI_S32 s32VencChn, ADPT_SCENEAUTO_VENC_ATTR_S* pstAdptVencAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    VENC_CHN VencChn = s32VencChn;
    VENC_CHN_ATTR_S stVencChnAttr;

    s32Ret = HI_MPI_VENC_GetChnAttr(VencChn, &stVencChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_GetChnAttr failed\n");
        return HI_FAILURE;
    }

    switch (stVencChnAttr.stRcAttr.enRcMode)
    {
        case VENC_RC_MODE_H264FIXQP:
        case VENC_RC_MODE_H264CBR:
            pstAdptVencAttr->eRcMode = ADPT_SCENEAUTO_RCMODE_H264;
            pstAdptVencAttr->u32BitRate = stVencChnAttr.stRcAttr.stAttrH264Cbr.u32BitRate;
            break;
        case VENC_RC_MODE_H265FIXQP:
        case VENC_RC_MODE_H265CBR:
            pstAdptVencAttr->eRcMode = ADPT_SCENEAUTO_RCMODE_H265;
            pstAdptVencAttr->u32BitRate = stVencChnAttr.stRcAttr.stAttrH265Cbr.u32BitRate;
            break;
        case VENC_RC_MODE_H264VBR:
            pstAdptVencAttr->eRcMode = ADPT_SCENEAUTO_RCMODE_H264;
            pstAdptVencAttr->u32BitRate = stVencChnAttr.stRcAttr.stAttrH264Vbr.u32MaxBitRate;
            break;
        case VENC_RC_MODE_H265VBR:
            pstAdptVencAttr->eRcMode = ADPT_SCENEAUTO_RCMODE_H265;
            pstAdptVencAttr->u32BitRate = stVencChnAttr.stRcAttr.stAttrH265Vbr.u32MaxBitRate;
            break;
        default:
            break;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetH265FaceCfg(HI_S32 s32VencChn, ADPT_SCENEAUTO_H265_FACE_CFG_S* pstAdptH265FaceCfg)
{
    HI_S32 s32Ret = HI_SUCCESS;
    VENC_CHN VencChn = s32VencChn;
    VENC_FACE_CFG_S stVencFaceCfg;

    s32Ret = HI_MPI_VENC_GetFaceCfg(VencChn, &stVencFaceCfg);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_GetChnAttr failed\n");
        return HI_FAILURE;
    }

    {
        pstAdptH265FaceCfg->u8NormIntra4RdCost_I = stVencFaceCfg.u8NormIntra4RdCost[0];
        pstAdptH265FaceCfg->u8NormIntra8RdCost_I = stVencFaceCfg.u8NormIntra8RdCost[0];
        pstAdptH265FaceCfg->u8NormIntra16RdCost_I = stVencFaceCfg.u8NormIntra16RdCost[0];
        pstAdptH265FaceCfg->u8NormIntra32RdCost_I = stVencFaceCfg.u8NormIntra32RdCost[0];
        pstAdptH265FaceCfg->u8SkinIntra4RdCost_I = stVencFaceCfg.u8SkinIntra4RdCost[0];
        pstAdptH265FaceCfg->u8SkinIntra8RdCost_I = stVencFaceCfg.u8SkinIntra8RdCost[0];
        pstAdptH265FaceCfg->u8SkinIntra16RdCost_I = stVencFaceCfg.u8SkinIntra16RdCost[0];
        pstAdptH265FaceCfg->u8SkinIntra32RdCost_I = stVencFaceCfg.u8SkinIntra32RdCost[0];
        pstAdptH265FaceCfg->u8HedgeIntra4RdCost_I = stVencFaceCfg.u8HedgeIntra4RdCost[0];
        pstAdptH265FaceCfg->u8HedgeIntra8RdCost_I = stVencFaceCfg.u8HedgeIntra8RdCost[0];
        pstAdptH265FaceCfg->u8HedgeIntra16RdCost_I = stVencFaceCfg.u8HedgeIntra16RdCost[0];
        pstAdptH265FaceCfg->u8HedgeIntra32RdCost_I = stVencFaceCfg.u8HedgeIntra32RdCost[0];

        pstAdptH265FaceCfg->u8NormIntra4RdCost_P = stVencFaceCfg.u8NormIntra4RdCost[1];
        pstAdptH265FaceCfg->u8NormIntra8RdCost_P = stVencFaceCfg.u8NormIntra8RdCost[1];
        pstAdptH265FaceCfg->u8NormIntra16RdCost_P = stVencFaceCfg.u8NormIntra16RdCost[1];
        pstAdptH265FaceCfg->u8NormIntra32RdCost_P = stVencFaceCfg.u8NormIntra32RdCost[1];
        pstAdptH265FaceCfg->u8SkinIntra4RdCost_P = stVencFaceCfg.u8SkinIntra4RdCost[1];
        pstAdptH265FaceCfg->u8SkinIntra8RdCost_P = stVencFaceCfg.u8SkinIntra8RdCost[1];
        pstAdptH265FaceCfg->u8SkinIntra16RdCost_P = stVencFaceCfg.u8SkinIntra16RdCost[1];
        pstAdptH265FaceCfg->u8SkinIntra32RdCost_P = stVencFaceCfg.u8SkinIntra32RdCost[1];
        pstAdptH265FaceCfg->u8HedgeIntra4RdCost_P = stVencFaceCfg.u8HedgeIntra4RdCost[1];
        pstAdptH265FaceCfg->u8HedgeIntra8RdCost_P = stVencFaceCfg.u8HedgeIntra8RdCost[1];
        pstAdptH265FaceCfg->u8HedgeIntra16RdCost_P = stVencFaceCfg.u8HedgeIntra16RdCost[1];
        pstAdptH265FaceCfg->u8HedgeIntra32RdCost_P = stVencFaceCfg.u8HedgeIntra32RdCost[1];

        pstAdptH265FaceCfg->u8NormFme8RdCost_P = stVencFaceCfg.u8NormFme8RdCost[1];
        pstAdptH265FaceCfg->u8NormFme16RdCost_P = stVencFaceCfg.u8NormFme16RdCost[1];
        pstAdptH265FaceCfg->u8NormFme32RdCost_P = stVencFaceCfg.u8NormFme32RdCost[1];
        pstAdptH265FaceCfg->u8NormFme64RdCost_P = stVencFaceCfg.u8NormFme64RdCost[1];
        pstAdptH265FaceCfg->u8SkinFme8RdCost_P = stVencFaceCfg.u8SkinFme8RdCost[1];
        pstAdptH265FaceCfg->u8SkinFme16RdCost_P = stVencFaceCfg.u8SkinFme16RdCost[1];
        pstAdptH265FaceCfg->u8SkinFme32RdCost_P = stVencFaceCfg.u8SkinFme32RdCost[1];
        pstAdptH265FaceCfg->u8SkinFme64RdCost_P = stVencFaceCfg.u8SkinFme64RdCost[1];
        pstAdptH265FaceCfg->u8HedgeFme8RdCost_P = stVencFaceCfg.u8HedgeFme8RdCost[1];
        pstAdptH265FaceCfg->u8HedgeFme16RdCost_P = stVencFaceCfg.u8HedgeFme16RdCost[1];
        pstAdptH265FaceCfg->u8HedgeFme32RdCost_P = stVencFaceCfg.u8HedgeFme32RdCost[1];
        pstAdptH265FaceCfg->u8HedgeFme64RdCost_P = stVencFaceCfg.u8HedgeFme64RdCost[1];

        pstAdptH265FaceCfg->u8NormMerg8RdCost_P = stVencFaceCfg.u8NormMerg8RdCost[1];
        pstAdptH265FaceCfg->u8NormMerg16RdCost_P = stVencFaceCfg.u8NormMerg16RdCost[1];
        pstAdptH265FaceCfg->u8NormMerg32RdCost_P = stVencFaceCfg.u8NormMerg32RdCost[1];
        pstAdptH265FaceCfg->u8NormMerg64RdCost_P = stVencFaceCfg.u8NormMerg64RdCost[1];
        pstAdptH265FaceCfg->u8SkinMerg8RdCost_P = stVencFaceCfg.u8SkinMerg8RdCost[1];
        pstAdptH265FaceCfg->u8SkinMerg16RdCost_P = stVencFaceCfg.u8SkinMerg16RdCost[1];
        pstAdptH265FaceCfg->u8SkinMerg32RdCost_P = stVencFaceCfg.u8SkinMerg32RdCost[1];
        pstAdptH265FaceCfg->u8SkinMerg64RdCost_P = stVencFaceCfg.u8SkinMerg64RdCost[1];
        pstAdptH265FaceCfg->u8HedgeMerg8RdCost_P = stVencFaceCfg.u8HedgeMerg8RdCost[1];
        pstAdptH265FaceCfg->u8HedgeMerg16RdCost_P = stVencFaceCfg.u8HedgeMerg16RdCost[1];
        pstAdptH265FaceCfg->u8HedgeMerg32RdCost_P = stVencFaceCfg.u8HedgeMerg32RdCost[1];
        pstAdptH265FaceCfg->u8HedgeMerg64RdCost_P = stVencFaceCfg.u8HedgeMerg64RdCost[1];

        pstAdptH265FaceCfg->bSkinEn_I = stVencFaceCfg.bSkinEn[0];
        pstAdptH265FaceCfg->u32SkinQpDelta_I = stVencFaceCfg.u32SkinQpDelta[0];
        pstAdptH265FaceCfg->u8SkinUMax_I = stVencFaceCfg.u8SkinUMax[0];
        pstAdptH265FaceCfg->u8SkinUMin_I = stVencFaceCfg.u8SkinUMin[0];
        pstAdptH265FaceCfg->u8SkinVMax_I = stVencFaceCfg.u8SkinVMax[0];
        pstAdptH265FaceCfg->u8SkinVMin_I = stVencFaceCfg.u8SkinVMin[0];
        pstAdptH265FaceCfg->u32SkinNum_I = stVencFaceCfg.u32SkinNum[0];

        pstAdptH265FaceCfg->bSkinEn_P = stVencFaceCfg.bSkinEn[1];;
        pstAdptH265FaceCfg->u32SkinQpDelta_P = stVencFaceCfg.u32SkinQpDelta[1];
        pstAdptH265FaceCfg->u8SkinUMax_P = stVencFaceCfg.u8SkinUMax[1];
        pstAdptH265FaceCfg->u8SkinUMin_P = stVencFaceCfg.u8SkinUMin[1];
        pstAdptH265FaceCfg->u8SkinVMax_P = stVencFaceCfg.u8SkinVMax[1];
        pstAdptH265FaceCfg->u8SkinVMin_P = stVencFaceCfg.u8SkinVMin[1];
        pstAdptH265FaceCfg->u32SkinNum_P = stVencFaceCfg.u32SkinNum[1];

        pstAdptH265FaceCfg->u8HedgeThr_I = stVencFaceCfg.u8HedgeThr[0];
        pstAdptH265FaceCfg->u8HedgeCnt_I = stVencFaceCfg.u8HedgeCnt[0];
        pstAdptH265FaceCfg->bStroEdgeEn_I = stVencFaceCfg.bStroEdgeEn[0];
        pstAdptH265FaceCfg->u32StroEdgeQpDelta_I = stVencFaceCfg.u32StroEdgeQpDelta[0];

        pstAdptH265FaceCfg->u8HedgeThr_P = stVencFaceCfg.u8HedgeThr[1];
        pstAdptH265FaceCfg->u8HedgeCnt_P = stVencFaceCfg.u8HedgeCnt[1];
        pstAdptH265FaceCfg->bStroEdgeEn_P = stVencFaceCfg.bStroEdgeEn[1];
        pstAdptH265FaceCfg->u32StroEdgeQpDelta_P = stVencFaceCfg.u32StroEdgeQpDelta[1];

        pstAdptH265FaceCfg->bImproveEn_I = stVencFaceCfg.bImproveEn[0];

        pstAdptH265FaceCfg->bImproveEn_P = stVencFaceCfg.bImproveEn[1];
        pstAdptH265FaceCfg->u32Norm32MaxNum_P = stVencFaceCfg.u32Norm32MaxNum[1];
        pstAdptH265FaceCfg->u32Norm16MaxNum_P = stVencFaceCfg.u32Norm16MaxNum[1];
        pstAdptH265FaceCfg->u32Norm32ProtectNum_P = stVencFaceCfg.u32Norm32ProtectNum[1];
        pstAdptH265FaceCfg->u32Norm16ProtectNum_P = stVencFaceCfg.u32Norm16ProtectNum[1];
        pstAdptH265FaceCfg->u32Skin32MaxNum_P = stVencFaceCfg.u32Skin32MaxNum[1];
        pstAdptH265FaceCfg->u32Skin16MaxNum_P = stVencFaceCfg.u32Skin16MaxNum[1];
        pstAdptH265FaceCfg->u32Skin32ProtectNum_P = stVencFaceCfg.u32Skin32ProtectNum[1];
        pstAdptH265FaceCfg->u32Skin16ProtectNum_P = stVencFaceCfg.u32Skin16ProtectNum[1];
        pstAdptH265FaceCfg->u32Still32MaxNum_P = stVencFaceCfg.u32Still32MaxNum[1];
        pstAdptH265FaceCfg->u32Still16MaxNum_P = stVencFaceCfg.u32Still16MaxNum[1];
        pstAdptH265FaceCfg->u32Still32ProtectNum_P = stVencFaceCfg.u32Still32ProtectNum[1];
        pstAdptH265FaceCfg->u32Still16ProtectNum_P = stVencFaceCfg.u32Still16ProtectNum[1];
        pstAdptH265FaceCfg->u32Hedge32MaxNum_P = stVencFaceCfg.u32Hedge32MaxNum[1];
        pstAdptH265FaceCfg->u32Hedge16MaxNum_P = stVencFaceCfg.u32Hedge16MaxNum[1];
        pstAdptH265FaceCfg->u32Hedge32ProtectNum_P = stVencFaceCfg.u32Hedge32ProtectNum[1];
        pstAdptH265FaceCfg->u32Hedge16ProtectNum_P = stVencFaceCfg.u32Hedge16ProtectNum[1];
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoSetH265FaceCfg(HI_S32 s32VencChn, const ADPT_SCENEAUTO_H265_FACE_CFG_S* pstAdptH265FaceCfg)
{
    HI_S32 s32Ret = HI_SUCCESS;
    VENC_CHN VencChn = s32VencChn;
    VENC_FACE_CFG_S stVencFaceCfg;

    s32Ret = HI_MPI_VENC_GetFaceCfg(VencChn, &stVencFaceCfg);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_GetChnAttr failed\n");
        return HI_FAILURE;
    }

    {
        stVencFaceCfg.u8NormIntra4RdCost[0] = pstAdptH265FaceCfg->u8NormIntra4RdCost_I;
        stVencFaceCfg.u8NormIntra8RdCost[0] = pstAdptH265FaceCfg->u8NormIntra8RdCost_I;
        stVencFaceCfg.u8NormIntra16RdCost[0] = pstAdptH265FaceCfg->u8NormIntra16RdCost_I;
        stVencFaceCfg.u8NormIntra32RdCost[0] = pstAdptH265FaceCfg->u8NormIntra32RdCost_I;
        stVencFaceCfg.u8SkinIntra4RdCost[0] = pstAdptH265FaceCfg->u8SkinIntra4RdCost_I;
        stVencFaceCfg.u8SkinIntra8RdCost[0] = pstAdptH265FaceCfg->u8SkinIntra8RdCost_I;
        stVencFaceCfg.u8SkinIntra16RdCost[0] = pstAdptH265FaceCfg->u8SkinIntra16RdCost_I;
        stVencFaceCfg.u8SkinIntra32RdCost[0] = pstAdptH265FaceCfg->u8SkinIntra32RdCost_I;
        stVencFaceCfg.u8HedgeIntra4RdCost[0] = pstAdptH265FaceCfg->u8HedgeIntra4RdCost_I;
        stVencFaceCfg.u8HedgeIntra8RdCost[0] = pstAdptH265FaceCfg->u8HedgeIntra8RdCost_I;
        stVencFaceCfg.u8HedgeIntra16RdCost[0] = pstAdptH265FaceCfg->u8HedgeIntra16RdCost_I;
        stVencFaceCfg.u8HedgeIntra32RdCost[0] = pstAdptH265FaceCfg->u8HedgeIntra32RdCost_I;

        stVencFaceCfg.u8NormIntra4RdCost[1] = pstAdptH265FaceCfg->u8NormIntra4RdCost_P;
        stVencFaceCfg.u8NormIntra8RdCost[1] = pstAdptH265FaceCfg->u8NormIntra8RdCost_P;
        stVencFaceCfg.u8NormIntra16RdCost[1] = pstAdptH265FaceCfg->u8NormIntra16RdCost_P;
        stVencFaceCfg.u8NormIntra32RdCost[1] = pstAdptH265FaceCfg->u8NormIntra32RdCost_P;
        stVencFaceCfg.u8SkinIntra4RdCost[1] = pstAdptH265FaceCfg->u8SkinIntra4RdCost_P;
        stVencFaceCfg.u8SkinIntra8RdCost[1] = pstAdptH265FaceCfg->u8SkinIntra8RdCost_P;
        stVencFaceCfg.u8SkinIntra16RdCost[1] = pstAdptH265FaceCfg->u8SkinIntra16RdCost_P;
        stVencFaceCfg.u8SkinIntra32RdCost[1] = pstAdptH265FaceCfg->u8SkinIntra32RdCost_P;
        stVencFaceCfg.u8HedgeIntra4RdCost[1] = pstAdptH265FaceCfg->u8HedgeIntra4RdCost_P;
        stVencFaceCfg.u8HedgeIntra8RdCost[1] = pstAdptH265FaceCfg->u8HedgeIntra8RdCost_P;
        stVencFaceCfg.u8HedgeIntra16RdCost[1] = pstAdptH265FaceCfg->u8HedgeIntra16RdCost_P;
        stVencFaceCfg.u8HedgeIntra32RdCost[1] = pstAdptH265FaceCfg->u8HedgeIntra32RdCost_P;

        stVencFaceCfg.u8NormFme8RdCost[1] = pstAdptH265FaceCfg->u8NormFme8RdCost_P;
        stVencFaceCfg.u8NormFme16RdCost[1] = pstAdptH265FaceCfg->u8NormFme16RdCost_P;
        stVencFaceCfg.u8NormFme32RdCost[1] = pstAdptH265FaceCfg->u8NormFme32RdCost_P;
        stVencFaceCfg.u8NormFme64RdCost[1] = pstAdptH265FaceCfg->u8NormFme64RdCost_P;
        stVencFaceCfg.u8SkinFme8RdCost[1] = pstAdptH265FaceCfg->u8SkinFme8RdCost_P;
        stVencFaceCfg.u8SkinFme16RdCost[1] = pstAdptH265FaceCfg->u8SkinFme16RdCost_P;
        stVencFaceCfg.u8SkinFme32RdCost[1] = pstAdptH265FaceCfg->u8SkinFme32RdCost_P;
        stVencFaceCfg.u8SkinFme64RdCost[1] = pstAdptH265FaceCfg->u8SkinFme64RdCost_P;
        stVencFaceCfg.u8HedgeFme8RdCost[1] = pstAdptH265FaceCfg->u8HedgeFme8RdCost_P;
        stVencFaceCfg.u8HedgeFme16RdCost[1] = pstAdptH265FaceCfg->u8HedgeFme16RdCost_P;
        stVencFaceCfg.u8HedgeFme32RdCost[1] = pstAdptH265FaceCfg->u8HedgeFme32RdCost_P;
        stVencFaceCfg.u8HedgeFme64RdCost[1] = pstAdptH265FaceCfg->u8HedgeFme64RdCost_P;

        stVencFaceCfg.u8NormMerg8RdCost[1] = pstAdptH265FaceCfg->u8NormMerg8RdCost_P;
        stVencFaceCfg.u8NormMerg16RdCost[1] = pstAdptH265FaceCfg->u8NormMerg16RdCost_P;
        stVencFaceCfg.u8NormMerg32RdCost[1] = pstAdptH265FaceCfg->u8NormMerg32RdCost_P;
        stVencFaceCfg.u8NormMerg64RdCost[1] = pstAdptH265FaceCfg->u8NormMerg64RdCost_P;
        stVencFaceCfg.u8SkinMerg8RdCost[1] = pstAdptH265FaceCfg->u8SkinMerg8RdCost_P;
        stVencFaceCfg.u8SkinMerg16RdCost[1] = pstAdptH265FaceCfg->u8SkinMerg16RdCost_P;
        stVencFaceCfg.u8SkinMerg32RdCost[1] = pstAdptH265FaceCfg->u8SkinMerg32RdCost_P;
        stVencFaceCfg.u8SkinMerg64RdCost[1] = pstAdptH265FaceCfg->u8SkinMerg64RdCost_P;
        stVencFaceCfg.u8HedgeMerg8RdCost[1] = pstAdptH265FaceCfg->u8HedgeMerg8RdCost_P;
        stVencFaceCfg.u8HedgeMerg16RdCost[1] = pstAdptH265FaceCfg->u8HedgeMerg16RdCost_P;
        stVencFaceCfg.u8HedgeMerg32RdCost[1] = pstAdptH265FaceCfg->u8HedgeMerg32RdCost_P;
        stVencFaceCfg.u8HedgeMerg64RdCost[1] = pstAdptH265FaceCfg->u8HedgeMerg64RdCost_P;

        stVencFaceCfg.bSkinEn[0] = pstAdptH265FaceCfg->bSkinEn_I;
        stVencFaceCfg.u32SkinQpDelta[0] = pstAdptH265FaceCfg->u32SkinQpDelta_I;
        stVencFaceCfg.u8SkinUMax[0] = pstAdptH265FaceCfg->u8SkinUMax_I;
        stVencFaceCfg.u8SkinUMin[0] = pstAdptH265FaceCfg->u8SkinUMin_I;
        stVencFaceCfg.u8SkinVMax[0] = pstAdptH265FaceCfg->u8SkinVMax_I;
        stVencFaceCfg.u8SkinVMin[0] = pstAdptH265FaceCfg->u8SkinVMin_I;
        stVencFaceCfg.u32SkinNum[0] = pstAdptH265FaceCfg->u32SkinNum_I;

        stVencFaceCfg.bSkinEn[1] = pstAdptH265FaceCfg->bSkinEn_P;
        stVencFaceCfg.u32SkinQpDelta[1] = pstAdptH265FaceCfg->u32SkinQpDelta_P;
        stVencFaceCfg.u8SkinUMax[1] = pstAdptH265FaceCfg->u8SkinUMax_P;
        stVencFaceCfg.u8SkinUMin[1] = pstAdptH265FaceCfg->u8SkinUMin_P;
        stVencFaceCfg.u8SkinVMax[1] = pstAdptH265FaceCfg->u8SkinVMax_P;
        stVencFaceCfg.u8SkinVMin[1] = pstAdptH265FaceCfg->u8SkinVMin_P;
        stVencFaceCfg.u32SkinNum[1] = pstAdptH265FaceCfg->u32SkinNum_P;

        stVencFaceCfg.u8HedgeThr[0] = pstAdptH265FaceCfg->u8HedgeThr_I;
        stVencFaceCfg.u8HedgeCnt[0] = pstAdptH265FaceCfg->u8HedgeCnt_I;
        stVencFaceCfg.bStroEdgeEn[0] = pstAdptH265FaceCfg->bStroEdgeEn_I;
        stVencFaceCfg.u32StroEdgeQpDelta[0] = pstAdptH265FaceCfg->u32StroEdgeQpDelta_I;

        stVencFaceCfg.u8HedgeThr[1] = pstAdptH265FaceCfg->u8HedgeThr_P;
        stVencFaceCfg.u8HedgeCnt[1] = pstAdptH265FaceCfg->u8HedgeCnt_P;
        stVencFaceCfg.bStroEdgeEn[1] = pstAdptH265FaceCfg->bStroEdgeEn_P;
        stVencFaceCfg.u32StroEdgeQpDelta[1] = pstAdptH265FaceCfg->u32StroEdgeQpDelta_P;

        stVencFaceCfg.bImproveEn[0] = pstAdptH265FaceCfg->bImproveEn_I;

        stVencFaceCfg.bImproveEn[1] = pstAdptH265FaceCfg->bImproveEn_P;
        stVencFaceCfg.u32Norm32MaxNum[1] = pstAdptH265FaceCfg->u32Norm32MaxNum_P;
        stVencFaceCfg.u32Norm16MaxNum[1] = pstAdptH265FaceCfg->u32Norm16MaxNum_P;
        stVencFaceCfg.u32Norm32ProtectNum[1] = pstAdptH265FaceCfg->u32Norm32ProtectNum_P;
        stVencFaceCfg.u32Norm16ProtectNum[1] = pstAdptH265FaceCfg->u32Norm16ProtectNum_P;
        stVencFaceCfg.u32Skin32MaxNum[1] = pstAdptH265FaceCfg->u32Skin32MaxNum_P;
        stVencFaceCfg.u32Skin16MaxNum[1] = pstAdptH265FaceCfg->u32Skin16MaxNum_P;
        stVencFaceCfg.u32Skin32ProtectNum[1] = pstAdptH265FaceCfg->u32Skin32ProtectNum_P;
        stVencFaceCfg.u32Skin16ProtectNum[1] = pstAdptH265FaceCfg->u32Skin16ProtectNum_P;
        stVencFaceCfg.u32Still32MaxNum[1] = pstAdptH265FaceCfg->u32Still32MaxNum_P;
        stVencFaceCfg.u32Still16MaxNum[1] = pstAdptH265FaceCfg->u32Still16MaxNum_P;
        stVencFaceCfg.u32Still32ProtectNum[1] = pstAdptH265FaceCfg->u32Still32ProtectNum_P;
        stVencFaceCfg.u32Still16ProtectNum[1] = pstAdptH265FaceCfg->u32Still16ProtectNum_P;
        stVencFaceCfg.u32Hedge32MaxNum[1] = pstAdptH265FaceCfg->u32Hedge32MaxNum_P;
        stVencFaceCfg.u32Hedge16MaxNum[1] = pstAdptH265FaceCfg->u32Hedge16MaxNum_P;
        stVencFaceCfg.u32Hedge32ProtectNum[1] = pstAdptH265FaceCfg->u32Hedge32ProtectNum_P;
        stVencFaceCfg.u32Hedge16ProtectNum[1] = pstAdptH265FaceCfg->u32Hedge16ProtectNum_P;
    }

    //s32Ret = HI_MPI_VENC_SetFaceCfg(VencChn, &stVencFaceCfg);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_GetChnAttr failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoGetH265RcParam(HI_S32 s32VencChn, ADPT_SCENEAUTO_H265_RCPARAM_S* pstAdptH265RCParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    VENC_CHN VencChn = s32VencChn;
    VENC_RC_PARAM_S stRCParam;
    VENC_CHN_ATTR_S stChnAttr;
    s32Ret = HI_MPI_VENC_GetChnAttr(VencChn, &stChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_GetChnAttr failed\n");
        return HI_FAILURE;
    }
    if (stChnAttr.stRcAttr.enRcMode != VENC_RC_MODE_H265CBR && stChnAttr.stRcAttr.enRcMode != VENC_RC_MODE_H265VBR)
    {
        return HI_SUCCESS;
    }

    s32Ret = HI_MPI_VENC_GetRcParam(VencChn, &stRCParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_GetRcParam failed\n");
        return HI_FAILURE;
    }

    pstAdptH265RCParam->u32QpDelta = stRCParam.u32RowQpDelta;

    for (i = 0; i < 12; i++)
    {
        pstAdptH265RCParam->u32ThrdI[i] = stRCParam.u32ThrdI[i];
        pstAdptH265RCParam->u32ThrdP[i] = stRCParam.u32ThrdP[i];
    }
    switch (stChnAttr.stRcAttr.enRcMode)
    {
        case VENC_RC_MODE_H265CBR:
            pstAdptH265RCParam->s32IPQPDelta = stRCParam.stParamH265Cbr.s32IPQPDelta;
            break;
        case VENC_RC_MODE_H265VBR:
            pstAdptH265RCParam->s32IPQPDelta = stRCParam.stParamH265Vbr.s32IPQPDelta;
            break;
        default:
            break;
    }

    return HI_SUCCESS;
}

HI_S32 CommSceneautoSetH265RcParam(HI_S32 s32VencChn, const ADPT_SCENEAUTO_H265_RCPARAM_S* pstAdptH265RCParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    VENC_CHN VencChn = s32VencChn;
    VENC_RC_PARAM_S stRCParam;
    VENC_CHN_ATTR_S stChnAttr;
    s32Ret = HI_MPI_VENC_GetChnAttr(VencChn, &stChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_GetChnAttr failed\n");
        return HI_FAILURE;
    }
    if (stChnAttr.stRcAttr.enRcMode != VENC_RC_MODE_H265CBR && stChnAttr.stRcAttr.enRcMode != VENC_RC_MODE_H265VBR)
    {
        return HI_SUCCESS;
    }

    s32Ret = HI_MPI_VENC_GetRcParam(VencChn, &stRCParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_GetRcParam failed\n");
        return HI_FAILURE;
    }

    stRCParam.u32RowQpDelta = pstAdptH265RCParam->u32QpDelta;
    switch (stChnAttr.stRcAttr.enRcMode)
    {
        case VENC_RC_MODE_H265CBR:
            stRCParam.stParamH265Cbr.s32IPQPDelta = pstAdptH265RCParam->s32IPQPDelta;
            break;
        case VENC_RC_MODE_H265VBR:
            stRCParam.stParamH265Vbr.s32IPQPDelta = pstAdptH265RCParam->s32IPQPDelta;
            break;
        default:
            break;
    }
    for (i = 0; i < 12; i++)
    {
        stRCParam.u32ThrdI[i] = pstAdptH265RCParam->u32ThrdI[i];
        stRCParam.u32ThrdP[i] = pstAdptH265RCParam->u32ThrdP[i];
    }

    s32Ret = HI_MPI_VENC_SetRcParam(VencChn, &stRCParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_SetRcParam failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

/******************************************************************************
  Some simple Hisilicon Hi35xx system functions.

  Copyright (C), 2010-2011, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2011-2 Created
******************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C"{
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


/* g_s32VBSource: 0 to module common vb, 1 to private vb, 2 to user vb 
   And don't forget to set the value of VBSource file "load35xx" */
HI_S32 g_s32VBSource = 0;
VB_POOL g_ahVbPool[VB_MAX_POOLS] = {[0 ... (VB_MAX_POOLS-1)] = VB_INVALID_POOLID};

#define PRINTF_VDEC_CHN_STATE(Chn, stChnStat) \
    do{\
            printf(" chn:%2d,  bStart:%2d,	DecodeFrames:%4d,  LeftPics:%3d,  LeftBytes:%10d,  LeftFrames:%4d,	RecvFrames:%6d\n",\
                Chn,\
                stChnStat.bStartRecvStream,\
                stChnStat.u32DecodeStreamFrames,\
                stChnStat.u32LeftPics,\
                stChnStat.u32LeftStreamBytes,\
                stChnStat.u32LeftStreamFrames,\
                stChnStat.u32RecvStreamFrames);\
        }while(0) 


HI_VOID	SAMPLE_COMM_VDEC_Sysconf(VB_CONF_S *pstVbConf, SIZE_S *pstSize)
{
    memset(pstVbConf, 0, sizeof(VB_CONF_S));	  
    pstVbConf->u32MaxPoolCnt = 1;	
    pstVbConf->astCommPool[0].u32BlkSize = (pstSize->u32Width * pstSize->u32Height * 4) >> 1;
    pstVbConf->astCommPool[0].u32BlkCnt	 = 3;
}


HI_VOID	SAMPLE_COMM_VDEC_ModCommPoolConf(VB_CONF_S *pstModVbConf, 
    PAYLOAD_TYPE_E enType, SIZE_S *pstSize, HI_S32 s32ChnNum, HI_BOOL bCompress)
{
    HI_S32 PicSize, PmvSize;
	
    memset(pstModVbConf, 0, sizeof(VB_CONF_S));
    pstModVbConf->u32MaxPoolCnt = 2;
	
    VB_PIC_BLK_SIZE(pstSize->u32Width, pstSize->u32Height, enType, PicSize);	
    /***********vdec compressed vb needs header****************/
    pstModVbConf->astCommPool[0].u32BlkSize = PicSize;
    pstModVbConf->astCommPool[0].u32BlkCnt  = 5*s32ChnNum;

    /* NOTICE: 			   
    1. if the VDEC channel is H264 channel and support to decode B frame, then you should allocate PmvBuffer 
    2. if the VDEC channel is MPEG4 channel, then you should allocate PmvBuffer.
    */
    if(PT_H265 == enType)
    {
        VB_PMV_BLK_SIZE(pstSize->u32Width, pstSize->u32Height, enType, PmvSize);
        pstModVbConf->astCommPool[1].u32BlkSize = PmvSize;
        pstModVbConf->astCommPool[1].u32BlkCnt  = 5*s32ChnNum;
    }
}

HI_VOID	SAMPLE_COMM_VDEC_ChnAttr(HI_S32 s32ChnNum, 
    VDEC_CHN_ATTR_S *pstVdecChnAttr, PAYLOAD_TYPE_E enType, SIZE_S *pstSize)
{
    HI_S32 i;

    for(i=0; i<s32ChnNum; i++)
    {
        pstVdecChnAttr[i].enType       = enType;
        pstVdecChnAttr[i].u32BufSize   = 3 * pstSize->u32Width * pstSize->u32Height;
        pstVdecChnAttr[i].u32Priority  = 5;
        pstVdecChnAttr[i].u32PicWidth  = pstSize->u32Width;
        pstVdecChnAttr[i].u32PicHeight = pstSize->u32Height;
        if (PT_H264 == enType || PT_MP4VIDEO == enType)
        {
            pstVdecChnAttr[i].stVdecVideoAttr.enMode=VIDEO_MODE_FRAME;
            pstVdecChnAttr[i].stVdecVideoAttr.u32RefFrameNum = 2;
            pstVdecChnAttr[i].stVdecVideoAttr.bTemporalMvpEnable = 0;
        }
        else if (PT_JPEG == enType || PT_MJPEG == enType)
        {
            pstVdecChnAttr[i].stVdecJpegAttr.enMode = VIDEO_MODE_FRAME;
            pstVdecChnAttr[i].stVdecJpegAttr.enJpegFormat = JPG_COLOR_FMT_YCBCR420;
        }
        else if(PT_H265 == enType)
        {
            pstVdecChnAttr[i].stVdecVideoAttr.enMode=VIDEO_MODE_STREAM;
            pstVdecChnAttr[i].stVdecVideoAttr.u32RefFrameNum = 2;
            pstVdecChnAttr[i].stVdecVideoAttr.bTemporalMvpEnable = 1;
        }    
    }
}

HI_VOID	SAMPLE_COMM_VDEC_VpssGrpAttr(HI_S32 s32ChnNum, VPSS_GRP_ATTR_S *pstVpssGrpAttr, SIZE_S *pstSize)
{
    HI_S32 i;

    for(i=0; i<s32ChnNum; i++)
    {
        pstVpssGrpAttr->enDieMode = VPSS_DIE_MODE_NODIE;
        pstVpssGrpAttr->bIeEn     = HI_FALSE;
        pstVpssGrpAttr->bDciEn    = HI_TRUE;
        pstVpssGrpAttr->bNrEn     = HI_FALSE;
        pstVpssGrpAttr->bHistEn   = HI_FALSE;
		pstVpssGrpAttr->bEsEn	  = HI_FALSE;
        pstVpssGrpAttr->enPixFmt  = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
        pstVpssGrpAttr->u32MaxW   = ALIGN_UP(pstSize->u32Width,  16);
        pstVpssGrpAttr->u32MaxH   = ALIGN_UP(pstSize->u32Height, 16);
    }
}



HI_VOID	SAMPLE_COMM_VDEC_VoAttr(HI_S32 s32ChnNum, VO_DEV VoDev ,VO_PUB_ATTR_S *pstVoPubAttr, VO_VIDEO_LAYER_ATTR_S *pstVoLayerAttr)
{
    HI_S32 u32Width, u32Height;

    /*********** set the pub attr of VO ****************/
    if (0 == VoDev)
    {
        pstVoPubAttr->enIntfSync = VO_OUTPUT_720P50;
        pstVoPubAttr->enIntfType = VO_INTF_BT1120 | VO_INTF_VGA;
    }
    else if (1 == VoDev)
    {
        pstVoPubAttr->enIntfSync = VO_OUTPUT_720P50;
        pstVoPubAttr->enIntfType = VO_INTF_VGA;
    }
    else if (VoDev>=2 && VoDev <=3)
    {
        pstVoPubAttr->enIntfSync = VO_OUTPUT_PAL;
        pstVoPubAttr->enIntfType = VO_INTF_CVBS;
    }
    pstVoPubAttr->u32BgColor = VO_BKGRD_BLUE;


    /***************** set the layer attr of VO  *******************/
    if(pstVoPubAttr->enIntfSync == VO_OUTPUT_720P50)
    {
        u32Width  = 1280;
        u32Height = 720;
    }
    else if (pstVoPubAttr->enIntfSync == VO_OUTPUT_PAL)
    {
        u32Width  = 720;
        u32Height = 576;
    }	
    pstVoLayerAttr->stDispRect.s32X		  = 0;
    pstVoLayerAttr->stDispRect.s32Y		  = 0;
    pstVoLayerAttr->stDispRect.u32Width	  = u32Width;
    pstVoLayerAttr->stDispRect.u32Height  = u32Height;
    pstVoLayerAttr->stImageSize.u32Width  = u32Width;
    pstVoLayerAttr->stImageSize.u32Height = u32Height;
    pstVoLayerAttr->bDoubleFrame		  = HI_FALSE;
    pstVoLayerAttr->bClusterMode          = HI_FALSE;
    pstVoLayerAttr->u32DispFrmRt		  = 30;
    pstVoLayerAttr->enPixFormat			  = PIXEL_FORMAT_YUV_SEMIPLANAR_420;		
}




HI_VOID SAMPLE_COMM_VDEC_ThreadParam(HI_S32 s32ChnNum, VdecThreadParam *pstVdecSend, 
	                                                       VDEC_CHN_ATTR_S *pstVdecChnAttr, char *pStreamFileName)
{
    int i;

    for(i=0; i<s32ChnNum; i++)
    {
        sprintf(pstVdecSend[i].cFileName, pStreamFileName, i);
        pstVdecSend[i].s32MilliSec     = 0;
        pstVdecSend[i].s32ChnId        = i;
        pstVdecSend[i].s32IntervalTime = 1;
        pstVdecSend[i].u64PtsInit      = 0;
        pstVdecSend[i].u64PtsIncrease  = 0;
        pstVdecSend[i].eCtrlSinal      = VDEC_CTRL_START;
        pstVdecSend[i].bLoopSend       = HI_TRUE;
        pstVdecSend[i].bManuSend       = HI_FALSE;
        pstVdecSend[i].enType          = pstVdecChnAttr[i].enType;
        pstVdecSend[i].s32MinBufSize   = (pstVdecChnAttr[i].u32PicWidth * pstVdecChnAttr[i].u32PicHeight * 3)>>1;
        if (PT_H264 == pstVdecChnAttr[i].enType  || PT_MP4VIDEO == pstVdecChnAttr[i].enType)
        {
            pstVdecSend[i].s32StreamMode  = pstVdecChnAttr[i].stVdecVideoAttr.enMode;
        }
        else
        {
            pstVdecSend[i].s32StreamMode = VIDEO_MODE_FRAME;
        }
    }
}

HI_VOID Sample_COMM_SetCrop(HI_S32 VpssGrp)
{
    VPSS_CROP_INFO_S stCropInfo;
    memset(&stCropInfo, 0, sizeof(VPSS_CROP_INFO_S));
    stCropInfo.bEnable = HI_TRUE;
    stCropInfo.enCropCoordinate = VPSS_CROP_ABS_COOR;
    stCropInfo.stCropRect.s32X = 160;
    stCropInfo.stCropRect.s32Y= 160;
    stCropInfo.stCropRect.u32Width = 480;
    stCropInfo.stCropRect.u32Height= 240;
    HI_MPI_VPSS_SetGrpCrop(VpssGrp, &stCropInfo);    
}


HI_S32 Sample_COMM_VPSS_StartCover(HI_S32 VpssGrp)
{
	HI_S32 s32ChnCnt = 1;
	HI_S32 j;
	RGN_HANDLE hRgn;
	RGN_ATTR_S stRgnAttr;
	MPP_CHN_S stMppChn;
	RGN_CHN_ATTR_S stRgnChnAttr;

    stRgnAttr.enType = COVEREX_RGN;
    hRgn = VpssGrp + 1;
    CHECK_RET(HI_MPI_RGN_Create(hRgn, &stRgnAttr), "HI_MPI_RGN_Create");


    stMppChn.enModId = HI_ID_VPSS;
    stMppChn.s32DevId = VpssGrp;

    stRgnChnAttr.bShow	= HI_TRUE;
    stRgnChnAttr.enType = stRgnAttr.enType;

    stRgnChnAttr.unChnAttr.stCoverExChn.u32Layer = 0;
    stRgnChnAttr.unChnAttr.stCoverExChn.stRect.s32X = 160;
    stRgnChnAttr.unChnAttr.stCoverExChn.stRect.s32Y = 160;
    stRgnChnAttr.unChnAttr.stCoverExChn.stRect.u32Width = 72;
    stRgnChnAttr.unChnAttr.stCoverExChn.stRect.u32Height = 60;
    stRgnChnAttr.unChnAttr.stCoverExChn.u32Color = 0x00ff;
    stRgnChnAttr.unChnAttr.stCoverExChn.enCoverType = AREA_RECT;

    for (j = 0; j < s32ChnCnt; j++)
    {
        if(j==1)/*chn12-- 3-cover*/        
		{
            continue;
        }
        stRgnChnAttr.unChnAttr.stCoverExChn.u32Layer = 1;
        //stRgnChnAttr.unChnAttr.stCoverExChn.u32Color = 0xadcde + j * 100000;
        stMppChn.s32ChnId = j;
        CHECK_RET(HI_MPI_RGN_AttachToChn(hRgn, &stMppChn, &stRgnChnAttr), "HI_MPI_RGN_AttachToChn");
    }
	
	return HI_SUCCESS;
}


HI_VOID Sample_MST_GetDefVoAttr(VO_DEV VoDev, 
        VO_PUB_ATTR_S *pstPubAttr, VO_VIDEO_LAYER_ATTR_S *pstLayerAttr,
        HI_S32 s32SquareSort, VO_CHN_ATTR_S *astChnAttr)
{
    HI_U32 u32Frmt, u32Width, u32Height, j;
   
    u32Width = 720; 
    u32Height = 576; 
    u32Frmt = 25; 
    HI_MPI_VO_GetPubAttr(VoDev, pstPubAttr);    
    if (NULL != pstPubAttr)
    {
        pstPubAttr->enIntfSync = VO_OUTPUT_PAL;
        pstPubAttr->enIntfType = VO_INTF_VGA;
    }
    
    HI_MPI_VO_GetVideoLayerAttr(VoDev, pstLayerAttr);  
    //HI_MPI_VO_GetVideoLayerPartitionMode(VoDev, VO_PART_MODE_MULTI);    
    if (NULL != pstLayerAttr)
    {
        pstLayerAttr->stDispRect.s32X       = 0;
        pstLayerAttr->stDispRect.s32Y       = 0;
        pstLayerAttr->stDispRect.u32Width   = u32Width;
        pstLayerAttr->stDispRect.u32Height  = u32Height;
        pstLayerAttr->stImageSize.u32Width  = u32Width;
        pstLayerAttr->stImageSize.u32Height = u32Height;
        pstLayerAttr->u32DispFrmRt          = u32Frmt;
        //pstLayerAttr->u32DispFrmRt          = 30;
        pstLayerAttr->enPixFormat           = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
        pstLayerAttr->bDoubleFrame          = HI_FALSE;
        pstLayerAttr->bClusterMode          = HI_FALSE;
    }

    if (NULL != astChnAttr)
    {
        for (j=0; j<(s32SquareSort * s32SquareSort); j++)
        {
            astChnAttr[j].stRect.s32X       = ALIGN_BACK((u32Width/s32SquareSort) * (j%s32SquareSort), 2);
            astChnAttr[j].stRect.s32Y       = ALIGN_BACK((u32Height/s32SquareSort) * (j/s32SquareSort), 2);
            astChnAttr[j].stRect.u32Width   = ALIGN_BACK(u32Width/s32SquareSort, 2);
            astChnAttr[j].stRect.u32Height  = ALIGN_BACK(u32Height/s32SquareSort, 2);
            astChnAttr[j].u32Priority       = 0;
            astChnAttr[j].bDeflicker        = HI_FALSE;
        }
    }
    
    return ;
}


HI_VOID Sample_MST_StartPIPVideoLayer(VO_DEV VoDev, VO_VIDEO_LAYER_ATTR_S *pstLayerAttr,
    VO_CHN_ATTR_S *astChnAttr, HI_S32 s32ChnNum)
{
    HI_U32 i;

    //enVoPartitionMode = VO_PART_MODE_MULTI;
    HI_MPI_VO_SetVideoLayerPartitionMode(VO_LAYER_PIP, VO_PART_MODE_MULTI);    
    if (VO_DEV_HD_END == VoDev)
    {
        HI_MPI_VO_UnBindVideoLayer(VO_LAYER_PIP, 0);
    }
    HI_MPI_VO_BindVideoLayer(VO_LAYER_PIP, VoDev);
    HI_MPI_VO_SetVideoLayerAttr(VO_LAYER_PIP, pstLayerAttr);
    HI_MPI_VO_EnableVideoLayer(VO_LAYER_PIP);   
    
    for (i = 0; i < s32ChnNum; i++)
    {   
        astChnAttr[i].stRect.u32Width = 1280;
		astChnAttr[i].stRect.u32Width = 720;
        HI_MPI_VO_SetChnAttr(VO_LAYER_PIP, i, &astChnAttr[i]);             
        HI_MPI_VO_EnableChn(VO_LAYER_PIP, i);
    }
}


HI_S32	SAMPLE_COMM_VDEC_InitModCommVb(VB_CONF_S *pstModVbConf)
{
    HI_S32 i;
    HI_S32 s32Ret;

    HI_MPI_VB_ExitModCommPool(VB_UID_VDEC);

    if(0 == g_s32VBSource)
    {
        CHECK_RET(HI_MPI_VB_SetModPoolConf(VB_UID_VDEC, pstModVbConf), "HI_MPI_VB_SetModPoolConf");
        CHECK_RET(HI_MPI_VB_InitModCommPool(VB_UID_VDEC), "HI_MPI_VB_InitModCommPool");
    }
    else if (2 == g_s32VBSource)
    {
        if (pstModVbConf->u32MaxPoolCnt > VB_MAX_POOLS)
        {
            printf("vb pool num(%d) is larger than VB_MAX_POOLS. \n", pstModVbConf->u32MaxPoolCnt);
            return HI_FAILURE;
        }
        for (i = 0; i < pstModVbConf->u32MaxPoolCnt; i++)
        {
            if (pstModVbConf->astCommPool[i].u32BlkSize && pstModVbConf->astCommPool[i].u32BlkCnt)
            {
                g_ahVbPool[i] = HI_MPI_VB_CreatePool(pstModVbConf->astCommPool[i].u32BlkSize, 
                    pstModVbConf->astCommPool[i].u32BlkCnt, NULL);
                if (VB_INVALID_POOLID == g_ahVbPool[i])
                    goto fail;
            }
        }
        return HI_SUCCESS;               

    fail:
        for (;i>=0;i--)
        {   
            if (VB_INVALID_POOLID != g_ahVbPool[i])
            {
                s32Ret = HI_MPI_VB_DestroyPool(g_ahVbPool[i]);
                HI_ASSERT(HI_SUCCESS == s32Ret);
                g_ahVbPool[i] = VB_INVALID_POOLID;
            }
        }
        return HI_FAILURE;
    }
    
    return HI_SUCCESS;
}


HI_VOID * SAMPLE_COMM_VDEC_SendStream(HI_VOID *pArgs)
{
    VdecThreadParam *pstVdecThreadParam =(VdecThreadParam *)pArgs;
    FILE *fpStrm=NULL;
    HI_U8 *pu8Buf = NULL;
    VDEC_STREAM_S stStream;
    HI_BOOL bFindStart, bFindEnd;
    HI_S32 s32Ret,  i,  start = 0;
    HI_S32 s32UsedBytes = 0, s32ReadLen = 0;
    HI_U64 u64pts = 0;
    HI_S32 len;
    HI_BOOL sHasReadStream = HI_FALSE; 

    prctl(PR_SET_NAME, "hi_SendStream", 0, 0, 0);

    if(pstVdecThreadParam->cFileName != 0)
    {
        fpStrm = fopen(pstVdecThreadParam->cFileName, "rb");
        if(fpStrm == NULL)
        {
            printf("SAMPLE_TEST:can't open file %s in send stream thread:%d\n",pstVdecThreadParam->cFileName, pstVdecThreadParam->s32ChnId);
            return (HI_VOID *)(HI_FAILURE);
        }
    }
    //printf("SAMPLE_TEST:chn %d, stream file:%s, bufsize: %d\n", 
    //pstVdecThreadParam->s32ChnId, pstVdecThreadParam->cFileName, pstVdecThreadParam->s32MinBufSize);

    pu8Buf = malloc(pstVdecThreadParam->s32MinBufSize);
    if(pu8Buf == NULL)
    {
        printf("SAMPLE_TEST:can't alloc %d in send stream thread:%d\n", pstVdecThreadParam->s32MinBufSize, pstVdecThreadParam->s32ChnId);
        fclose(fpStrm);
        return (HI_VOID *)(HI_FAILURE);
    }     
    fflush(stdout);
	
    u64pts = pstVdecThreadParam->u64PtsInit;
    while (1)
    {
        if (pstVdecThreadParam->eCtrlSinal == VDEC_CTRL_STOP)
        {
            break;
        }
        else if (pstVdecThreadParam->eCtrlSinal == VDEC_CTRL_PAUSE)
        {
            sleep(MIN2(pstVdecThreadParam->s32IntervalTime,1000));
            continue;
        }

        if ( (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME) && (pstVdecThreadParam->enType == PT_MP4VIDEO) )
        {
            bFindStart = HI_FALSE;  
            bFindEnd   = HI_FALSE;
            fseek(fpStrm, s32UsedBytes, SEEK_SET);
            s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
            if (s32ReadLen == 0)
            {
                if (pstVdecThreadParam->bLoopSend)
                {
                    s32UsedBytes = 0;
                    fseek(fpStrm, 0, SEEK_SET);
                    s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
                }
                else
                {
                    break;
                }
            }

            for (i=0; i<s32ReadLen-4; i++)
            {
                if (pu8Buf[i] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1 && pu8Buf[i+3] == 0xB6)
                {
                    bFindStart = HI_TRUE;
                    i += 4;
                    break;
                }
            }

            for (; i<s32ReadLen-4; i++)
            {
                if (pu8Buf[i  ] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1 && pu8Buf[i+3] == 0xB6)
                {
                    bFindEnd = HI_TRUE;
                    break;
                }
            }

            s32ReadLen = i;
            if (bFindStart == HI_FALSE)
            {
                printf("SAMPLE_TEST: chn %d can not find start code! s32ReadLen %d, s32UsedBytes %d. \n", 
					                        pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
            }
            else if (bFindEnd == HI_FALSE)
            {
                s32ReadLen = i+4;
            }
            
        }
        else if ( (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME) && (pstVdecThreadParam->enType == PT_H264) )
        {
            bFindStart = HI_FALSE;  
            bFindEnd   = HI_FALSE;
            fseek(fpStrm, s32UsedBytes, SEEK_SET);
            s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
            if (s32ReadLen == 0)
            {
                if (pstVdecThreadParam->bLoopSend)
                {
                    s32UsedBytes = 0;
                    fseek(fpStrm, 0, SEEK_SET);
                    s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
                }
                else
                {
                    break;
                }
            }
         			
            for (i=0; i<s32ReadLen-8; i++)
            {
                int tmp = pu8Buf[i+3] & 0x1F;
                if (  pu8Buf[i] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1 && 
                       (
                           ((tmp == 5 || tmp == 1) && ((pu8Buf[i+4]&0x80) == 0x80)) ||
                           (tmp == 20 && (pu8Buf[i+7]&0x80) == 0x80)
                        )
                   )            
                {
                    bFindStart = HI_TRUE;
                    i += 8;
                    break;
                }
            }

            for (; i<s32ReadLen-8; i++)
            {
                int tmp = pu8Buf[i+3] & 0x1F;
                if (  pu8Buf[i  ] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1 && 
                            (
                                  tmp == 15 || tmp == 7 || tmp == 8 || tmp == 6 || 
                                  ((tmp == 5 || tmp == 1) && ((pu8Buf[i+4]&0x80) == 0x80)) ||
                                  (tmp == 20 && (pu8Buf[i+7]&0x80) == 0x80)
                              )
                   )                   
                {					
                    bFindEnd = HI_TRUE;
                    break;
                }
            }

            if(i > 0) s32ReadLen = i;
            if (bFindStart == HI_FALSE)
            {
                printf("SAMPLE_TEST: chn %d can not find start code!s32ReadLen %d, s32UsedBytes %d. \n", 
					                        pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
            }
            else if (bFindEnd == HI_FALSE)
            {
                s32ReadLen = i+8;
            }
            
        }
        
        
		else if (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME && pstVdecThreadParam->enType == PT_H265)
        {
            HI_BOOL  bNewPic = HI_FALSE; 
            bFindStart = HI_FALSE;  
            bFindEnd   = HI_FALSE;
            fseek(fpStrm, s32UsedBytes, SEEK_SET);
            s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);

            if (s32ReadLen == 0)
            {
                if (pstVdecThreadParam->bLoopSend == HI_TRUE)
                {
                    s32UsedBytes = 0;
                    fseek(fpStrm, 0, SEEK_SET);
                    s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
                }
                else
                {
                    break;
                }
            }
            
            for (i=0; i<s32ReadLen-6; i++)
            {
                bNewPic = (pu8Buf[i+0] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1 
                    &&(((pu8Buf[i+3]&0x7D) >= 0x0 && (pu8Buf[i+3]&0x7D) <= 0x2A) || (pu8Buf[i+3]&0x1F) == 0x1) 
                    &&((pu8Buf[i+5]&0x80) == 0x80));//first slice segment
                    
                if (bNewPic)
                {
                    bFindStart = HI_TRUE;
                    i += 4;
                    break;
                }
            }

            for (; i<s32ReadLen-6; i++)
            {
                bNewPic = (pu8Buf[i+0] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1 
                    &&(((pu8Buf[i+3]&0x7D) >= 0x0 && (pu8Buf[i+3]&0x7D) <= 0x2A) || (pu8Buf[i+3]&0x1F) == 0x1) 
                    &&((pu8Buf[i+5]&0x80) == 0x80));//first slice segment
                    
                if (  pu8Buf[i  ] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1  
					&&(((pu8Buf[i+3]&0x7D) == 0x40) || ((pu8Buf[i+3]&0x7D) == 0x42) || ((pu8Buf[i+3]&0x7D) == 0x44)|| bNewPic)
                   )
                {
                    bFindEnd = HI_TRUE;
                    break;
                }
            }

            s32ReadLen = i;
            
            if (bFindStart == HI_FALSE)
            {
                printf("hevc can not find start code! %d s32ReadLen 0x%x +++++++++++++\n", pstVdecThreadParam->s32ChnId,s32ReadLen);
            }
            else if (bFindEnd == HI_FALSE)
            {
                s32ReadLen = i+6;
            }
            
        }
        else if (pstVdecThreadParam->enType == PT_MJPEG)
        {
            bFindStart = HI_FALSE;  
            bFindEnd   = HI_FALSE;          
            fseek(fpStrm, s32UsedBytes, SEEK_SET);
            s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
            if (s32ReadLen == 0)
            {
                if (pstVdecThreadParam->bLoopSend)
                {
                    s32UsedBytes = 0;
                    fseek(fpStrm, 0, SEEK_SET);
                    s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
                }
                else
                {
                    break;
                }
            }

           
            for (i=0; i<s32ReadLen-2; i++)
            {
                if (pu8Buf[i] == 0xFF && pu8Buf[i+1] == 0xD8) 
                {  
                    start = i;
                    bFindStart = HI_TRUE;
                    i = i + 2;
                    break;
                }  
            }

            for (; i<s32ReadLen-4; i++)
            {
                if ( (pu8Buf[i] == 0xFF) && (pu8Buf[i+1]& 0xF0) == 0xE0 )
                {   
                     len = (pu8Buf[i+2]<<8) + pu8Buf[i+3];                    
                     i += 1 + len;                  
                }
                else
                {
                    break;
                }
            }

            for (; i<s32ReadLen-2; i++)
            {
                if (pu8Buf[i] == 0xFF && pu8Buf[i+1] == 0xD8)
                {
                    bFindEnd = HI_TRUE;
                    break;
                } 
            }                    
            s32ReadLen = i;
            if (bFindStart == HI_FALSE)
            {
                printf("SAMPLE_TEST: chn %d can not find start code! s32ReadLen %d, s32UsedBytes %d. \n", 
					                        pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
            }
            else if (bFindEnd == HI_FALSE)
            {
                s32ReadLen = i+2;
            }
        }
         else if ((pstVdecThreadParam->enType == PT_JPEG) )
        {
            if (HI_TRUE != sHasReadStream)
            {               
           
                bFindStart = HI_FALSE;  
                bFindEnd   = HI_FALSE; 
                
                fseek(fpStrm, s32UsedBytes, SEEK_SET);
                s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
                if (s32ReadLen == 0)
                {
                    if (pstVdecThreadParam->bLoopSend)
                    {
                        s32UsedBytes = 0;
                        fseek(fpStrm, 0, SEEK_SET);
                        s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
                    }
                    else
                    {
                        break;
                    }
                }

               
                for (i=0; i<s32ReadLen-2; i++)
                {
                    if (pu8Buf[i] == 0xFF && pu8Buf[i+1] == 0xD8) 
                    {  
                        start = i;
                        bFindStart = HI_TRUE;
                        i = i + 2;
                        break;
                    }  
                }

                for (; i<s32ReadLen-4; i++)
                {
                    if ( (pu8Buf[i] == 0xFF) && (pu8Buf[i+1]& 0xF0) == 0xE0 )
                    {   
                         len = (pu8Buf[i+2]<<8) + pu8Buf[i+3];                    
                         i += 1 + len;                  
                    }
                    else
                    {
                        break;
                    }
                }

                for (; i<s32ReadLen-2; i++)
                {
                    if (pu8Buf[i] == 0xFF && pu8Buf[i+1] == 0xD8)
                    {                    
                        bFindEnd = HI_TRUE;
                        break;
                    } 
                }                    
                s32ReadLen = i;
                if (bFindStart == HI_FALSE)
                {
                    printf("SAMPLE_TEST: chn %d can not find start code! s32ReadLen %d, s32UsedBytes %d. \n", 
    					                        pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
                }
                else if (bFindEnd == HI_FALSE)
                {
                    s32ReadLen = i+2;
                }
                 sHasReadStream = HI_TRUE;
            }
        }
        else
        {
            fseek(fpStrm, s32UsedBytes, SEEK_SET);
            s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
            if (s32ReadLen == 0)
            {
                if (pstVdecThreadParam->bLoopSend)
                {
                    s32UsedBytes = 0;
                    fseek(fpStrm, 0, SEEK_SET);
                    s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
                }
                else
                {
                    break;
                }
            }
        }

        stStream.u64PTS  = u64pts;
        stStream.pu8Addr = pu8Buf + start;
		if(pstVdecThreadParam->enType == PT_MJPEG)
		{
		     stStream.u32Len  = s32ReadLen - start; 
		}
		else
		{
            stStream.u32Len  = s32ReadLen; 
		}
        stStream.bEndOfFrame  = (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME)? HI_TRUE: HI_FALSE;
        stStream.bEndOfStream = HI_FALSE;                   
             
        
        //printf("Send One Frame");
        //fflush(stdout);   
        
        s32Ret=HI_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, pstVdecThreadParam->s32MilliSec);
        pstVdecThreadParam->cUserCmd = 0;
        if (HI_SUCCESS != s32Ret)
        {
            usleep(100);
        }
        else
        {
            if(pstVdecThreadParam->enType == PT_MJPEG)
        	{
        	    s32UsedBytes = s32UsedBytes +s32ReadLen;
        	}
			else
		    {
                s32UsedBytes = s32UsedBytes +s32ReadLen + start;	
			}
            u64pts += pstVdecThreadParam->u64PtsIncrease;            
        }
       usleep(1000);
    }

    /* send the flag of stream end */
    memset(&stStream, 0, sizeof(VDEC_STREAM_S) );
    stStream.bEndOfStream = HI_TRUE;
    HI_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, -1);
    
    //printf("SAMPLE_TEST:send steam thread %d return ...\n", pstVdecThreadParam->s32ChnId);
    fflush(stdout);
    if (pu8Buf != HI_NULL)
    {
        free(pu8Buf);
    }
    fclose(fpStrm);
	
    return (HI_VOID *)HI_SUCCESS;
}


HI_VOID * SAMPLE_COMM_VDEC_GetChnLuma(HI_VOID *pArgs)
{
    VDEC_CHN_LUM_S stLumaPix;
    VdecThreadParam *pstVdecThreadParam =(VdecThreadParam *)pArgs;
    HI_BOOL bRunFlag = HI_TRUE;
    HI_S32 s32Ret;
    FILE *fpLuma = HI_NULL;
    HI_CHAR FileName[128];
    HI_CHAR acString[128];

    prctl(PR_SET_NAME, "hi_GetChnLuma", 0, 0, 0);

    snprintf(FileName, 128, "LumaPixChn%d.txt", pstVdecThreadParam->s32ChnId);		
    fpLuma=fopen(FileName, "w+");
    if(fpLuma == NULL)
    {
        printf("SAMPLE_TEST:can't open file %s in get luma thread:%d\n", pstVdecThreadParam->cFileName, pstVdecThreadParam->s32ChnId);
        return (HI_VOID *)(HI_FAILURE);
    }	
	
    while(1)
    {
        switch(pstVdecThreadParam->eCtrlSinal)
        {
            case VDEC_CTRL_START:
                s32Ret = HI_MPI_VDEC_GetChnLuma(pstVdecThreadParam->s32ChnId, &stLumaPix);	
                if (HI_SUCCESS == s32Ret)
                {
                    memset(acString, 0, 128);
                    sprintf(acString, "chn %d,  Pts = %lld,  LumPixSum = %lld,  LumPixAverage=%d!     \n",
                    pstVdecThreadParam->s32ChnId, stLumaPix.u64Pts, stLumaPix.u64LumPixSum,stLumaPix.u32LumPixAverage);
                    fprintf(fpLuma, "%s\n", acString);
                }
                break;

            case VDEC_CTRL_PAUSE:
                sleep(MIN2(pstVdecThreadParam->s32IntervalTime,1000));
                break;

            case VDEC_CTRL_STOP:
                bRunFlag=HI_FALSE;
                break;

            default:
                printf("SAMPLE_TEST:unknow command in get luma thread %d\n", pstVdecThreadParam->s32ChnId);
                bRunFlag=HI_FALSE;
                break;
        }
        usleep(40000);
        if(bRunFlag==HI_FALSE)
        {
            break;
        }
    }   
    printf("SAMPLE_TEST:get LumaPix thread %d return ...\n", pstVdecThreadParam->s32ChnId);
    fclose(fpLuma);	
	
    return (HI_VOID *)HI_SUCCESS;
}


HI_VOID SAMPLE_COMM_VDEC_CmdCtrl(HI_S32 s32ChnNum,VdecThreadParam *pstVdecSend)
{
    HI_S32 i;
    HI_S32 s32FrameRate = 0;
    HI_BOOL /*bIsPause = HI_FALSE, */bVoPause = HI_FALSE;
    VDEC_CHN_STAT_S stStat;
	VO_VIDEO_LAYER_ATTR_S stVoAttr;
    char c=0;
         for (i=0; i<s32ChnNum; i++) 
        pstVdecSend[i].cUserCmd = 0;   
    while(1)    
    {
        printf("\nSAMPLE_TEST:press 'e' to exit; 'p' to pause/resume; 'q' to query!;'s' to step!;'a' to add!;'d' to sub!;\n");

        c = getchar();
        if (10 == c)
        {
            continue;
        }
        getchar();
        if (c == 'e')
        {
            for (i = 0; i < s32ChnNum; i++)
            { pstVdecSend[i].cUserCmd = 0; }
            break;
        }
        //        else if (c == 'r')
        //        {
        //            if (bIsPause)
        //            {
        //                for (i=0; i<s32ChnNum; i++)
        //                pstVdecSend[i].eCtrlSinal = VDEC_CTRL_PAUSE;
        //            }
        //            else
        //            {
        //                for (i=0; i<s32ChnNum; i++)
        //                pstVdecSend[i].eCtrlSinal = VDEC_CTRL_START;
        //            }
        //            bIsPause = !bIsPause;
        //
        //        }
        else if (c == 'p')
        {
            if (bVoPause)
            {
                HI_MPI_VO_ResumeChn(0, 0);
                HI_MPI_VO_ResumeChn(1, 0);
                printf("VO Resume.");
            }
            else
            {
                HI_MPI_VO_PauseChn(0, 0);
                HI_MPI_VO_PauseChn(1, 0);
                printf("VO Pause.");
            }
            bVoPause = !bVoPause;
        }
        else if (c == 'a')
        {
            for (i = 0; i < 2; i++)
            {
                HI_MPI_VO_GetVideoLayerAttr(i, &stVoAttr);
                if (stVoAttr.u32DispFrmRt == 0)
                {
                    continue;
                }
                HI_MPI_VO_GetChnFrameRate(i, 0, &s32FrameRate);
                if (s32FrameRate >= 120)
                {
                    printf("VO FrameRate of chnl 0 of layer %d is larger than 120.", i);
                    continue;
                }
                s32FrameRate += 10;
                HI_MPI_VO_SetChnFrameRate(i, 0, s32FrameRate);
                printf("VO FrameRate of chnl 0 of layer %d is set to %d.", i, s32FrameRate);
            }
        }
        else if (c == 'd')
        {
            for (i = 0; i < 2; i++)
            {
                HI_MPI_VO_GetVideoLayerAttr(i, &stVoAttr);
                if (stVoAttr.u32DispFrmRt == 0)
                {
                    continue;
                }
                HI_MPI_VO_GetChnFrameRate(i, 0, &s32FrameRate);
                if (s32FrameRate < 20)
                {
                    printf("VO FrameRate of chnl 0 of layer %d is less than 10.", i);
                    continue;
                }
                s32FrameRate -= 10;
                HI_MPI_VO_SetChnFrameRate(i, 0, s32FrameRate);
                printf("VO FrameRate of chnl 0 of layer %d is set to %d.", i, s32FrameRate);
            }
        }
        else if (c == 's')
        {
            if (bVoPause == HI_FALSE)
            {
                printf("Firstly press 'p' to pause,then step.");
                continue;
            }
            HI_MPI_VO_StepChn(0, 0);  
            HI_MPI_VO_StepChn(VO_LAYER_PIP, 0); 
            printf("VO Step.");
        }
        else if (c == 'q')
        {
            for (i = 0; i < s32ChnNum; i++)
            {
                HI_MPI_VDEC_Query(pstVdecSend[i].s32ChnId, &stStat);
                PRINTF_VDEC_CHN_STATE(pstVdecSend[i].s32ChnId, stStat);
            }
        }
        fflush(stdout);
    }
}


HI_VOID SAMPLE_COMM_VDEC_StartSendStream(HI_S32 s32ChnNum, VdecThreadParam *pstVdecSend, pthread_t *pVdecThread)
{
    HI_S32  i;
	
    for(i=0; i<s32ChnNum; i++)
    {
        pthread_create(&pVdecThread[i], 0, SAMPLE_COMM_VDEC_SendStream, (HI_VOID *)&pstVdecSend[i]);
    }
}

HI_VOID SAMPLE_COMM_VDEC_StopSendStream(HI_S32 s32ChnNum, VdecThreadParam *pstVdecSend, pthread_t *pVdecThread)
{
    HI_S32  i;

    for(i=0; i<s32ChnNum; i++)
    {	
        HI_MPI_VDEC_StopRecvStream(i);

        pstVdecSend[i].eCtrlSinal=VDEC_CTRL_STOP;
        if (pVdecThread[i])
        {
            pthread_join(pVdecThread[i], HI_NULL);
            pVdecThread[i] = 0;
        }
    }
}


HI_VOID SAMPLE_COMM_VDEC_StartGetLuma(HI_S32 s32ChnNum, VdecThreadParam *pstVdecSend, pthread_t *pVdecThread)
{
    HI_S32  i;

    for(i=0; i<s32ChnNum; i++)
    {
		pthread_create(&pVdecThread[i+VDEC_MAX_CHN_NUM], 0, SAMPLE_COMM_VDEC_GetChnLuma, (HI_VOID *)&pstVdecSend[i]);	
    }
}

HI_VOID SAMPLE_COMM_VDEC_StopGetLuma(HI_S32 s32ChnNum, VdecThreadParam *pstVdecSend, pthread_t *pVdecThread)
{
    HI_S32  i;

    for(i=0; i<s32ChnNum; i++)
    {    
		if (pstVdecSend[i].enType == PT_JPEG || pstVdecSend[i].enType == PT_MJPEG)
		{
	        pstVdecSend[i].eCtrlSinal = VDEC_CTRL_STOP;
	        pthread_join(pVdecThread[i+VDEC_MAX_CHN_NUM], HI_NULL);   
		}
    }
}


HI_S32 SAMPLE_COMM_VDEC_Start(HI_S32 s32ChnNum, VDEC_CHN_ATTR_S *pstAttr)
{
    HI_S32  i;
    HI_U32 u32BlkCnt = 10;
    VDEC_CHN_POOL_S stPool;

    for(i=0; i<s32ChnNum; i++)
    {	
        if(1 == g_s32VBSource)
        {
            CHECK_CHN_RET(HI_MPI_VDEC_SetChnVBCnt(i, u32BlkCnt), i, "HI_MPI_VDEC_SetChnVBCnt");				
        }		
        CHECK_CHN_RET(HI_MPI_VDEC_CreateChn(i, &pstAttr[i]), i, "HI_MPI_VDEC_CreateChn");
        if (2 == g_s32VBSource)
        {
            stPool.hPicVbPool = g_ahVbPool[0];
            stPool.hPmvVbPool = -1;
            CHECK_CHN_RET(HI_MPI_VDEC_AttachVbPool(i, &stPool), i, "HI_MPI_VDEC_AttachVbPool");
        }
        CHECK_CHN_RET(HI_MPI_VDEC_StartRecvStream(i), i, "HI_MPI_VDEC_StartRecvStream");
        //CHECK_CHN_RET(HI_MPI_VDEC_SetDisplayMode(i, VIDEO_DISPLAY_MODE_PREVIEW), i, "HI_MPI_VDEC_SetDisplayMode");
    }

    return HI_SUCCESS;
}


HI_S32 SAMPLE_COMM_VDEC_Stop(HI_S32 s32ChnNum)
{
    HI_S32 i;	

    for(i=0; i<s32ChnNum; i++)
    {
        CHECK_CHN_RET(HI_MPI_VDEC_StopRecvStream(i), i, "HI_MPI_VDEC_StopRecvStream");       
        CHECK_CHN_RET(HI_MPI_VDEC_DestroyChn(i), i, "HI_MPI_VDEC_DestroyChn");
    }

    return HI_SUCCESS;
}




HI_S32 SAMPLE_COMM_VDEC_BindVpss(VDEC_CHN VdChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = HI_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VdChn;

    stDestChn.enModId = HI_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(HI_MPI_SYS_Bind(&stSrcChn, &stDestChn), "HI_MPI_SYS_Bind");

    return HI_SUCCESS;
}



HI_S32 SAMPLE_COMM_VDEC_BindVo(VDEC_CHN VdChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = HI_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VdChn;

    stDestChn.enModId = HI_ID_VOU;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(HI_MPI_SYS_Bind(&stSrcChn, &stDestChn), "HI_MPI_SYS_Bind");

    return HI_SUCCESS;
}



HI_S32 SAMPLE_COMM_VDEC_UnBindVpss(VDEC_CHN VdChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = HI_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VdChn;

    stDestChn.enModId = HI_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "HI_MPI_SYS_UnBind");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VDEC_UnBindVo(VDEC_CHN VdChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = HI_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VdChn;

    stDestChn.enModId = HI_ID_VOU;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "HI_MPI_SYS_UnBind");

    return HI_SUCCESS;
}


/******************************************************************************
* function : Set system memory location
******************************************************************************/
HI_S32 SAMPLE_COMM_VDEC_MemConfig(HI_VOID)
{
    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    HI_CHAR * pcMmzName;
    MPP_CHN_S stMppChnVDEC;

    /* VDEC chn max is 80*/
    for(i=0; i<80; i++)
    {
        stMppChnVDEC.enModId = HI_ID_VDEC;
        stMppChnVDEC.s32DevId = 0;
        stMppChnVDEC.s32ChnId = i;
        
        if(0 == (i%2))
        {
            pcMmzName = NULL;  
        }
        else
        {
            pcMmzName = "ddr1";
        }

        s32Ret = HI_MPI_SYS_SetMemConf(&stMppChnVDEC,pcMmzName);
        if (s32Ret)
        {
            SAMPLE_PRT("HI_MPI_SYS_SetMemConf ERR !\n");
            return HI_FAILURE;
        }
    }  

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

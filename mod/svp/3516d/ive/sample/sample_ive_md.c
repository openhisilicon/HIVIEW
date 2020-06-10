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

#include "svp.h"
extern void* svp_pub;

#include "sample_comm_ive.h"
#include "ivs_md.h"


#define SAMPLE_IVE_MD_IMAGE_NUM 2

typedef struct hiSAMPLE_IVE_MD_S
{
    IVE_SRC_IMAGE_S astImg[SAMPLE_IVE_MD_IMAGE_NUM];
    IVE_DST_MEM_INFO_S stBlob;
    MD_ATTR_S stMdAttr;
    SAMPLE_RECT_ARRAY_S stRegion;

}SAMPLE_IVE_MD_S;

static HI_BOOL s_bStopSignal = HI_FALSE;
static pthread_t s_hMdThread = 0;
static SAMPLE_IVE_MD_S s_stMd;
static SAMPLE_IVE_SWITCH_S s_stMdSwitch = {HI_FALSE,HI_TRUE};
static SAMPLE_VI_CONFIG_S s_stViConfig = {0};

static HI_VOID SAMPLE_IVE_Md_Uninit(SAMPLE_IVE_MD_S *pstMd)
{
    HI_S32 i;
    HI_S32 s32Ret = HI_SUCCESS;

    for (i = 0; i < SAMPLE_IVE_MD_IMAGE_NUM; i++)
    {
        IVE_MMZ_FREE(pstMd->astImg[i].au64PhyAddr[0],pstMd->astImg[i].au64VirAddr[0]);
    }

    IVE_MMZ_FREE(pstMd->stBlob.u64PhyAddr,pstMd->stBlob.u64VirAddr);

    s32Ret = HI_IVS_MD_Exit();
    if(s32Ret != HI_SUCCESS)
    {
       SAMPLE_PRT("HI_IVS_MD_Exit fail,Error(%#x)\n",s32Ret);
       return ;
    }

}

static HI_S32 SAMPLE_IVE_Md_Init(SAMPLE_IVE_MD_S *pstMd,HI_U32 u32Width,HI_U32 u32Height)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i ;
    HI_U32 u32Size;
    HI_U8 u8WndSz;

    for (i = 0;i < SAMPLE_IVE_MD_IMAGE_NUM;i++)
    {
        s32Ret = SAMPLE_COMM_IVE_CreateImage(&pstMd->astImg[i],IVE_IMAGE_TYPE_U8C1,u32Width,u32Height);
        SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, MD_INIT_FAIL,
            "Error(%#x),Create img[%d] image failed!\n", s32Ret,i);
    }
    u32Size = sizeof(IVE_CCBLOB_S);
    s32Ret = SAMPLE_COMM_IVE_CreateMemInfo(&pstMd->stBlob,u32Size);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, MD_INIT_FAIL,
        "Error(%#x),Create blob mem info failed!\n", s32Ret);

    //Set attr info
    pstMd->stMdAttr.enAlgMode = MD_ALG_MODE_BG;
    pstMd->stMdAttr.enSadMode = IVE_SAD_MODE_MB_4X4;
    pstMd->stMdAttr.enSadOutCtrl = IVE_SAD_OUT_CTRL_THRESH;
    pstMd->stMdAttr.u16SadThr = 100 * (1 << 1);//100 * (1 << 2);
    pstMd->stMdAttr.u32Width = u32Width;
    pstMd->stMdAttr.u32Height = u32Height;
    pstMd->stMdAttr.stAddCtrl.u0q16X = 32768;
    pstMd->stMdAttr.stAddCtrl.u0q16Y = 32768;
    pstMd->stMdAttr.stCclCtrl.enMode = IVE_CCL_MODE_4C;
    u8WndSz = ( 1 << (2 + pstMd->stMdAttr.enSadMode));
    pstMd->stMdAttr.stCclCtrl.u16InitAreaThr = u8WndSz * u8WndSz;
    pstMd->stMdAttr.stCclCtrl.u16Step = u8WndSz;

    s32Ret = HI_IVS_MD_Init();
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, MD_INIT_FAIL,
        "Error(%#x),HI_IVS_MD_Init failed!\n", s32Ret);

MD_INIT_FAIL:

    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_IVE_Md_Uninit(pstMd);
    }
    return s32Ret;

}
static HI_VOID * SAMPLE_IVE_MdProc(HI_VOID * pArgs)
{
    HI_S32 s32Ret;
    SAMPLE_IVE_MD_S *pstMd;
    MD_ATTR_S *pstMdAttr;
    //VIDEO_FRAME_INFO_S stBaseFrmInfo;
    VIDEO_FRAME_INFO_S stExtFrmInfo;
    HI_S32 s32VpssGrp = 0;
    HI_S32 as32VpssChn[] = {VPSS_CHN0, VPSS_CHN1};
    HI_S32 s32MilliSec = 20000;
    MD_CHN MdChn = 0;
    VO_LAYER voLayer = 0;
	  VO_CHN voChn = 0;
    HI_BOOL bInstant = HI_TRUE;
    HI_S32 s32CurIdx = 0;
    HI_BOOL bFirstFrm = HI_TRUE;
    pstMd = (SAMPLE_IVE_MD_S *)(pArgs);
    pstMdAttr = &(pstMd->stMdAttr);
    
    SIZE_S stSize;
    PIC_SIZE_E enSize = PIC_CIF;

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSize, &stSize);
    if(HI_SUCCESS != s32Ret)
    {
      SAMPLE_PRT("Error(%#x),SAMPLE_COMM_SYS_GetPicSize failed, enSize:%d, w:%d, h:%d\n",
            s32Ret, enSize, stSize.u32Width, stSize.u32Height);
      return NULL;
    }

    while (HI_FALSE == s_bStopSignal)
    {
        usleep(100*1000);
        
        s32Ret = HI_MPI_VPSS_GetChnFrame(s32VpssGrp, as32VpssChn[1], &stExtFrmInfo, s32MilliSec);
        if(HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Error(%#x),HI_MPI_VPSS_GetChnFrame failed, VPSS_GRP(%d), VPSS_CHN(%d)!\n",
                s32Ret,s32VpssGrp, as32VpssChn[1]);
            continue;
        }

        if(stExtFrmInfo.stVFrame.u32Width != stSize.u32Width 
          || stExtFrmInfo.stVFrame.u32Height != stSize.u32Height)
        {

            SAMPLE_IVE_Md_Uninit(&(s_stMd));
            memset(&s_stMd,0,sizeof(s_stMd));
            
            stSize.u32Width = stExtFrmInfo.stVFrame.u32Width;
            stSize.u32Height = stExtFrmInfo.stVFrame.u32Height;
            
            bFirstFrm = HI_TRUE;
            
            /******************************************
             step 2: Init Md
             ******************************************/
            s32Ret = SAMPLE_IVE_Md_Init(&s_stMd,stSize.u32Width,stSize.u32Height);
            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("Error(%#x),SAMPLE_IVE_Md_Init failed!\n", s32Ret);
                return NULL;
            }
                
            //Create chn
            s32Ret = HI_IVS_MD_CreateChn(MdChn,&(pstMd->stMdAttr));
            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("HI_IVS_MD_CreateChn fail,Error(%#x)\n",s32Ret);
                return NULL;
            }
        }


        if (HI_TRUE != bFirstFrm)
        {
            s32Ret = SAMPLE_COMM_IVE_DmaImage(&stExtFrmInfo,&pstMd->astImg[s32CurIdx],bInstant);
            SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, EXT_RELEASE,
                            "SAMPLE_COMM_IVE_DmaImage fail,Error(%#x)\n",s32Ret);
        }
        else
        {
            s32Ret = SAMPLE_COMM_IVE_DmaImage(&stExtFrmInfo,&pstMd->astImg[1 - s32CurIdx],bInstant);
            SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, EXT_RELEASE,
                            "SAMPLE_COMM_IVE_DmaImage fail,Error(%#x)\n",s32Ret);

            bFirstFrm = HI_FALSE;
            goto CHANGE_IDX;//first frame just init reference frame
        }

        s32Ret = HI_IVS_MD_Process(MdChn,&pstMd->astImg[s32CurIdx],&pstMd->astImg[1 - s32CurIdx],NULL,&pstMd->stBlob);
        SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, EXT_RELEASE,
                        "HI_IVS_MD_Process fail,Error(%#x)\n",s32Ret);
                        
        SAMPLE_COMM_IVE_BlobToRect(SAMPLE_COMM_IVE_CONVERT_64BIT_ADDR(IVE_CCBLOB_S,pstMd->stBlob.u64VirAddr)
                                , &(pstMd->stRegion)
                                , IVE_RECT_NUM
                                , 8
                                , pstMdAttr->u32Width
                                , pstMdAttr->u32Height
                                , pstMdAttr->u32Width
                                , pstMdAttr->u32Height);
        
        int i = 0, j = 0;
        SAMPLE_RECT_ARRAY_S* pstRect = &pstMd->stRegion;
                   
        gsf_svp_mds_t mds;

        mds.cnt = 0;
        mds.pts = stExtFrmInfo.stVFrame.u64PTS/1000;
        mds.w = pstMdAttr->u32Width;
        mds.h = pstMdAttr->u32Height;

        for (i = 0; i < pstRect->u16Num; i++)
        {
            if(pstRect->astRect[i].astPoint[1].s32X - pstRect->astRect[i].astPoint[0].s32X < 80
              || pstRect->astRect[i].astPoint[3].s32Y - pstRect->astRect[i].astPoint[0].s32Y < 80)
              continue;
              
            mds.result[j].thr = 0;
            mds.result[j].rect[0] = pstRect->astRect[i].astPoint[0].s32X;
            mds.result[j].rect[1] = pstRect->astRect[i].astPoint[0].s32Y;
            mds.result[j].rect[2] = pstRect->astRect[i].astPoint[1].s32X - pstRect->astRect[i].astPoint[0].s32X;
            mds.result[j].rect[3] = pstRect->astRect[i].astPoint[3].s32Y - pstRect->astRect[i].astPoint[0].s32Y;
            printf(" MD => j:%d, w:%d, h:%d rect: [x:%d, y:%d, w:%d, h:%d]\n"
                  , j, pstMdAttr->u32Width, pstMdAttr->u32Height
                  , mds.result[j].rect[0]
                  , mds.result[j].rect[1]
                  , mds.result[j].rect[2]
                  , mds.result[j].rect[3]
                  );
            j++;
        }
        
        mds.cnt = j;
        if(mds.cnt > 0)
        {
          printf("publish GSF_EV_SVP_MD cnt:%d \n\n", mds.cnt);

          char buf[sizeof(gsf_msg_t) + sizeof(gsf_svp_mds_t)];
          gsf_msg_t *msg = (gsf_msg_t*)buf;
          
          memset(msg, 0, sizeof(*msg));
          msg->id = GSF_EV_SVP_MD;
          msg->ts = time(NULL)*1000;
          msg->sid = 0;
          msg->err = 0;
          msg->size = sizeof(gsf_svp_mds_t);
          memcpy(msg->data, &mds, msg->size);
          nm_pub_send(svp_pub, (char*)msg, sizeof(*msg)+msg->size); 
        }


        CHANGE_IDX:
            //Change reference and current frame index
            s32CurIdx =    1 - s32CurIdx;

        EXT_RELEASE:
            s32Ret = HI_MPI_VPSS_ReleaseChnFrame(s32VpssGrp,as32VpssChn[1], &stExtFrmInfo);
            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("Error(%#x),HI_MPI_VPSS_ReleaseChnFrame failed,Grp(%d) chn(%d)!\n",
                    s32Ret,s32VpssGrp,as32VpssChn[1]);
            }
    }

     //destroy
     s32Ret = HI_IVS_MD_DestroyChn(MdChn);
     if (HI_SUCCESS != s32Ret)
     {
         SAMPLE_PRT("HI_IVS_MD_DestroyChn fail,Error(%#x)\n",s32Ret);
     }

     return HI_NULL;
}

HI_VOID SAMPLE_IVE_Md(HI_VOID)
{
    SIZE_S stSize;
    PIC_SIZE_E enSize = PIC_D1_PAL;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR acThreadName[16] = {0};

    memset(&s_stMd,0,sizeof(s_stMd));
    
    #if 0 //maohw
    /******************************************
     step 1: start vi vpss venc vo
     ******************************************/
    s32Ret = SAMPLE_COMM_IVE_StartViVpssVencVo(&s_stViConfig,&s_stMdSwitch,&enSize);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_MD_0,
        "Error(%#x),SAMPLE_COMM_IVE_StartViVpssVencVo failed!\n", s32Ret);
    
    enSize = PIC_D1_PAL;
    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSize, &stSize);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_MD_0,
        "Error(%#x),SAMPLE_COMM_SYS_GetPicSize failed!\n", s32Ret);
  
      
    /******************************************
     step 2: Init Md
     ******************************************/
    s32Ret = SAMPLE_IVE_Md_Init(&s_stMd,stSize.u32Width,stSize.u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_MD_0,
        " Error(%#x),SAMPLE_IVE_Md_Init failed!\n", s32Ret);
        
    #endif
        
    s_bStopSignal = HI_FALSE;
    /******************************************
      step 3: Create work thread
     ******************************************/
    snprintf(acThreadName, 16, "IVE_MdProc");
    prctl(PR_SET_NAME, (unsigned long)acThreadName, 0,0,0);
    pthread_create(&s_hMdThread, 0, SAMPLE_IVE_MdProc, (HI_VOID *)&s_stMd);

#if 0 //maohw
    SAMPLE_PAUSE();
    s_bStopSignal = HI_TRUE;
    pthread_join(s_hMdThread, HI_NULL);
    s_hMdThread = 0;

    SAMPLE_IVE_Md_Uninit(&(s_stMd));
    memset(&s_stMd,0,sizeof(s_stMd));

END_MD_0:
  

    SAMPLE_COMM_IVE_StopViVpssVencVo(&s_stViConfig,&s_stMdSwitch);
  #endif
  
  return ;
}

/******************************************************************************
* function : Md sample signal handle
******************************************************************************/
HI_VOID SAMPLE_IVE_Md_HandleSig(HI_VOID)
{
    s_bStopSignal = HI_TRUE;
    if (0 != s_hMdThread)
    {
        pthread_join(s_hMdThread, HI_NULL);
        s_hMdThread = 0;
    }
    SAMPLE_IVE_Md_Uninit(&(s_stMd));
    memset(&s_stMd,0,sizeof(s_stMd));

#if 0 // maohw
    SAMPLE_COMM_IVE_StopViVpssVencVo(&s_stViConfig,&s_stMdSwitch);
#endif
}




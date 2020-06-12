#include<stdio.h>
#include<string.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "svp.h"
extern void* svp_pub;

#include "sample_comm_ive.h"
#include "ivs_md.h"

#include "LPR.h"

static pthread_t s_hMdThread = 0;
static HI_BOOL s_bStopSignal = HI_FALSE;
static void* pHand = NULL;
static void* lpr_task(void* p);


int lpr_start()
{
  pHand = liblpr_Init(1920, 1080, "/tmp");
  return pthread_create(&s_hMdThread, NULL, lpr_task, NULL);
}

int lpr_stop()
{
  s_bStopSignal = HI_TRUE;
  return pthread_join(s_hMdThread, NULL);
}

static void* lpr_task(void* p)
{
  HI_S32 s32Ret, i = 0;
  VIDEO_FRAME_INFO_S stExtFrmInfo;
  HI_S32 s32VpssGrp = 0;
  HI_S32 as32VpssChn[] = {VPSS_CHN0, VPSS_CHN1};
  HI_S32 s32MilliSec = 1000;
  HI_U32 u32Size = 0;
  
  while (HI_FALSE == s_bStopSignal)
  {
      usleep(10*1000);
      
      s32Ret = HI_MPI_VPSS_GetChnFrame(s32VpssGrp, as32VpssChn[1], &stExtFrmInfo, s32MilliSec);
      if(HI_SUCCESS != s32Ret)
      {
          SAMPLE_PRT("Error(%#x),HI_MPI_VPSS_GetChnFrame failed, VPSS_GRP(%d), VPSS_CHN(%d)!\n",
              s32Ret,s32VpssGrp, as32VpssChn[1]);
          continue;
      }
      
      if (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == stExtFrmInfo.stVFrame.enPixelFormat
        || PIXEL_FORMAT_YVU_SEMIPLANAR_420 == stExtFrmInfo.stVFrame.enPixelFormat)    
      {        
          u32Size = (stExtFrmInfo.stVFrame.u32Stride[0]) * (stExtFrmInfo.stVFrame.u32Height) * 3 / 2;        
      }    
      else if(PIXEL_FORMAT_YUV_SEMIPLANAR_422 == stExtFrmInfo.stVFrame.enPixelFormat
        || PIXEL_FORMAT_YVU_SEMIPLANAR_422 == stExtFrmInfo.stVFrame.enPixelFormat)    
      {        
          u32Size = (stExtFrmInfo.stVFrame.u32Stride[0]) * (stExtFrmInfo.stVFrame.u32Height) * 2;        
      }  
      else if(PIXEL_FORMAT_YUV_400 == stExtFrmInfo.stVFrame.enPixelFormat)    
      {        
          u32Size = (stExtFrmInfo.stVFrame.u32Stride[0]) * (stExtFrmInfo.stVFrame.u32Height);   
      } 
      
      HI_CHAR* pVirAddr = (HI_CHAR*) HI_MPI_SYS_Mmap(stExtFrmInfo.stVFrame.u64PhyAddr[0], u32Size); 

      LPR_RECT rcRange;
    	rcRange.m_iLeft= 0 + 200;
    	rcRange.m_iTop= 0 + 200;
    	rcRange.m_iRight = stExtFrmInfo.stVFrame.u32Width - 200;	
    	rcRange.m_iBottom = stExtFrmInfo.stVFrame.u32Height - 200;

      LPR_Result LPRResult[3];
  	  int PlateNum = 3;
      memset(LPRResult,0,sizeof(LPR_Result)*3);

      struct timespec ts1, ts2;
      clock_gettime(CLOCK_MONOTONIC, &ts1);

      s32Ret = liblpr_Recognition(pHand, LPR_FAST, pVirAddr, LPR_YUV420SP
                      , stExtFrmInfo.stVFrame.u32Width, stExtFrmInfo.stVFrame.u32Height
                      , rcRange, LPRResult, &PlateNum);

      clock_gettime(CLOCK_MONOTONIC, &ts2);
      
      printf("s32Ret:%d, PlateNum:%d, cost:%d ms\n"
            , s32Ret, PlateNum
            , (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000));

      if(s32Ret > 0)
      {
        
        char buf[sizeof(gsf_msg_t) + sizeof(gsf_svp_lprs_t)];
        gsf_msg_t *msg = (gsf_msg_t*)buf;
        
        memset(msg, 0, sizeof(*msg));
        msg->id = GSF_EV_SVP_LPR;
        msg->ts = time(NULL)*1000;
        msg->sid = 0;
        msg->err = 0;
        msg->size = sizeof(gsf_svp_lprs_t);
        
        gsf_svp_lprs_t *lprs = (gsf_svp_lprs_t*)msg->data;
        
        lprs->pts = stExtFrmInfo.stVFrame.u64PTS/1000;
        lprs->w = stExtFrmInfo.stVFrame.u32Width;
        lprs->h = stExtFrmInfo.stVFrame.u32Height;
        lprs->cnt = PlateNum;
        
        for(i=0;i<PlateNum;i++)
  			{
  				strcpy(lprs->result[i].number, LPRResult[i].m_cPlatenum);
  				
  				lprs->result[i].rect[0] = LPRResult[i].m_fPlaterect.m_iLeft;
  				lprs->result[i].rect[1] = LPRResult[i].m_fPlaterect.m_iTop;
  				lprs->result[i].rect[2] = LPRResult[i].m_fPlaterect.m_iRight - LPRResult[i].m_fPlaterect.m_iLeft;
  				lprs->result[i].rect[3] = LPRResult[i].m_fPlaterect.m_iBottom - LPRResult[i].m_fPlaterect.m_iTop;
  			
  			  printf("i: %d rect[%d,%d,%d,%d], str[%s]\n"
  			        , i
  			        , lprs->result[i].rect[0]
  			        , lprs->result[i].rect[1]
  			        , lprs->result[i].rect[2]
  			        , lprs->result[i].rect[3]
  			        , lprs->result[i].number);
  			}
        nm_pub_send(svp_pub, (char*)msg, sizeof(*msg)+msg->size);
      }

      HI_MPI_SYS_Munmap(pVirAddr, u32Size);
      HI_MPI_VPSS_ReleaseChnFrame(s32VpssGrp, as32VpssChn[1], &stExtFrmInfo);

  }
}

/******************************************************************************
  Some simple Hisilicon Hi35xx video input functions.

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

#include "mpp.h"
#include "zbar.h"


static pthread_t tid = 0;
static int runing = 0;
static void* zbar_task(void* p);


int zbar_start()
{
  runing = 1;
  return pthread_create(&tid, NULL, zbar_task, NULL);
}

int zbar_stop()
{
  runing = 0;
  return pthread_join(tid, NULL);
}

static void* zbar_task(void* p)
{
  HI_S32 s32Ret, i = 0;
  VIDEO_FRAME_INFO_S stExtFrmInfo;
  HI_S32 s32VpssGrp = 0;
  HI_S32 as32VpssChn[] = {VPSS_CHN0, VPSS_CHN1};
  HI_S32 s32MilliSec = 1000;
  HI_U32 u32Size = 0;
  
  zbar_image_scanner_t *scanner = NULL;
	scanner = zbar_image_scanner_create();
	zbar_image_scanner_set_config(scanner, 0, ZBAR_CFG_ENABLE, 1);
  zbar_image_t *image = zbar_image_create();
  zbar_image_set_format(image, *(int*)"Y800");
  
  while (1 == runing)
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

      zbar_image_set_size(image, stExtFrmInfo.stVFrame.u32Width, stExtFrmInfo.stVFrame.u32Height);
      zbar_image_set_data(image, pVirAddr, stExtFrmInfo.stVFrame.u32Width*stExtFrmInfo.stVFrame.u32Height, NULL);

      struct timespec ts1, ts2;
      clock_gettime(CLOCK_MONOTONIC, &ts1);

      /* scan the image for barcodes */
      int n = zbar_scan_image(scanner, image);
      if(n > 0)
      {
        /* extract results */
        const zbar_symbol_t *symbol = zbar_image_first_symbol(image);
        for(; symbol; symbol = zbar_symbol_next(symbol)) 
        {
            /* do something useful with results */
            zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
            const char *data = zbar_symbol_get_data(symbol);

            printf("decoded %s symbol \"%s\"\n",
                   zbar_get_symbol_name(typ), data);
        }
      }
      clock_gettime(CLOCK_MONOTONIC, &ts2);
      
      printf("n:%d, cost:%d ms\n"
            , n
            , (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000));

      #if 0
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
      #endif

      HI_MPI_SYS_Munmap(pVirAddr, u32Size);
      HI_MPI_VPSS_ReleaseChnFrame(s32VpssGrp, as32VpssChn[1], &stExtFrmInfo);

  }
  
  return NULL;
}





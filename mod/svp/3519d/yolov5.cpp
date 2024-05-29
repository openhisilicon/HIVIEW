#include <sys/file.h>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "sample_svp_npu_process.h"
#include "vpss_capture.h"
#include "yolov5.h"

typedef struct {
  int vpss_grp, vpss_chn;
  VpssCapture vcap;
  cv::Mat image;
  int fd;
}vcap_t;

static vcap_t vcap[YOLO_CHN_MAX];


//save test flag;
static int vcap_save_yuv   = 0;
static int vcap_save_image = 0;
static int vcap_cnt = 0;

extern "C" hi_s32 sample_comm_venc_save_stream(FILE *fd, hi_venc_stream *stream);
static int venc_cb(VENC_STREAM_S* pstStream, void* u)
{
  char* szJpgName = (char*)u;
  
  if(!pstStream)
    return 0;

  if(!szJpgName)
    return 0;
  
  FILE* pFile = fopen(szJpgName, "wb");
  if(pFile)
  {
    flock(fileno(pFile), LOCK_EX);
  	{
  	    hi_u32 i;
  	    for (i = 0; i < pstStream->pack_cnt; i++) {
  	        fwrite(pstStream->pack[i].addr + pstStream->pack[i].offset,
  	               pstStream->pack[i].len - pstStream->pack[i].offset, 1, pFile);

  	        fflush(pFile);
  	    }
  	}
    flock(fileno(pFile), LOCK_UN);
    fclose(pFile);
  }
  
  fprintf(stderr, "save jpeg [%s] done!\n", szJpgName);
  fflush(stderr);
  return 0;
}


int yolov5_init(int vpss_grp[YOLO_CHN_MAX], int vpss_chn[YOLO_CHN_MAX], char *ModelPath)
{
    int i = 0;
    int ret = -1;
   
    for(i = 0; i < YOLO_CHN_MAX; i++)
    {
      vcap[i].vpss_grp = vpss_grp[i];
      vcap[i].vpss_chn = vpss_chn[i];
    }

    {
      ret = sample_svp_npu_init(ModelPath);
      printf("sample_svp_npu_init ret:%d [%s]\n", ret, ModelPath);
    }
    return ret;
}


gsf_mpp_venc_t* __venc_yuv2jpeg = NULL;

int yolov5_detect(yolo_boxs_t _boxs[YOLO_CHN_MAX])
{

    static struct timespec _ts1, _ts2;
    clock_gettime(CLOCK_MONOTONIC, &_ts2);
    int check_fd = (_ts2.tv_sec - _ts1.tv_sec > 3)?1:0;
    
    hi_video_frame_info *hi_frame = NULL, *other_frame = NULL;
    yolo_boxs_t *boxs = _boxs;
    int ret = 0;
    int maxfd = 0;
    struct timeval to;
    fd_set read_fds;
    FD_ZERO(&read_fds);

    for (int i = 0; i < YOLO_CHN_MAX; i++)
    {
      if(vcap[i].vpss_grp == -1)
      {  
        continue;
      }
      
      if(check_fd)
      {
        _ts1 = _ts2;
        
        int fd_new = hi_mpi_vpss_get_chn_fd(vcap[i].vpss_grp, vcap[i].vpss_chn);
        if(vcap[i].fd != fd_new)
        {
          printf("check vpss_grp:%d, vpss_chn:%d, fd:[%d => %d]\n", vcap[i].vpss_grp, vcap[i].vpss_chn, vcap[i].fd, fd_new);
          vcap[i].vcap.destroy();
          vcap[i].fd = vcap[i].vcap.init(vcap[i].vpss_grp, vcap[i].vpss_chn, 0, 0, 1);  //both
        }
      }
      
      if(vcap[i].fd > 0)
      {  
        FD_SET(vcap[i].fd, &read_fds);
        maxfd = (maxfd < vcap[i].fd)?vcap[i].fd:maxfd;
      }
    }
    
    to.tv_sec  = 2; to.tv_usec = 0;
    ret = select(maxfd + 1, &read_fds, NULL, NULL, &to);
    if (ret < 0)
    {
        printf("vpss select failed!\n");
        return -1;
    }
    else if (ret == 0)
    {
        printf("vpss get timeout!\n");
        return 0;
    }
    
    for (int i = 0; i < YOLO_CHN_MAX; i++)
    {
      if(vcap[i].fd <= 0)
      {
        continue;
      }
      
      if (FD_ISSET(vcap[i].fd, &read_fds))
      {
        //printf("vpss get ok! [fd:%d]\n", vcap[i].fd);
        
        struct timespec ts1, ts2; 
        clock_gettime(CLOCK_MONOTONIC, &ts1);
        //vcap[i].vcap.get_frame_lock(&vcap[i].image, &hi_frame, &other_frame);
        vcap[i].vcap.get_frame_lock(NULL, &hi_frame, &other_frame);
        clock_gettime(CLOCK_MONOTONIC, &ts2);

        if(0)
        {
          char info[256] = {0};
          sprintf(info, "get_frame_lock chn:%d, cost:%d ms", i, (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000));
          if(!vcap[i].image.empty())
            sprintf(info, "%s cv_mat[%dx%d]", info, vcap[i].image.cols, vcap[i].image.rows);
          if(hi_frame)
            sprintf(info, "%s hi_frame[%dx%d:%llu ms]", info, hi_frame->video_frame.width, hi_frame->video_frame.height, hi_frame->video_frame.pts/1000);
          if(other_frame)
            sprintf(info, "%s other_frame[%dx%d:%llu ms]", info, other_frame->video_frame.width, other_frame->video_frame.height, other_frame->video_frame.pts/1000);    
          
          printf("%s\n\n", info);
          
          if(vcap_save_image)
          {
            clock_gettime(CLOCK_MONOTONIC, &ts1);
            
            #if 0 // opencv:imwrite;
            if(!vcap[i].image.empty())
            {
              static int j = 0;
              char filename[256];
              sprintf(filename, "vcap/ch%02d_%08d.jpg", i, j++);
              cv::imwrite(filename, vcap[i].image);
            }
            #else // venc:jpeg;
            if(hi_frame)
            {
              static int j = 0;
              char filename[256];
              sprintf(filename, "vcap/ch%02d_%08d.jpg", i, j++);
              
              static int VeChn = 0;
              if(!VeChn)
              {
                VeChn = (HI_VENC_MAX_CHN_NUM-1);
                static gsf_mpp_venc_t venc;
                venc.VencChn    = VeChn;
                venc.srcModId   = HI_ID_VPSS;
                venc.VpssGrp    = -1;
                venc.VpssChn    = -1;
                venc.enPayLoad  = PT_JPEG;
                venc.enSize     = PIC_2688X1520;
                venc.enRcMode   = 0;
                venc.u32Profile = 0;
                venc.bRcnRefShareBuf = HI_TRUE;
                venc.enGopMode  = VENC_GOPMODE_NORMALP;
                venc.u32FrameRate = 30;
                venc.u32Gop       = 30;
                venc.u32BitRate   = 2000;
                venc.u32LowDelay  = 0;
                gsf_mpp_venc_start(&venc);
                __venc_yuv2jpeg = &venc;
              }
              
              gsf_mpp_venc_get_t vget;
              vget.cb = venc_cb;
              vget.u = (void*)filename;
     
     
              static td_u64 __pts = 0;
              static td_u32 __time_ref = 0;         
              hi_frame->video_frame.pts = __pts+= 33000;
              hi_frame->video_frame.time_ref = __time_ref+=2;
              
              if(gsf_mpp_venc_send(VeChn, hi_frame, 100, &vget) < 0)
              {
                printf("gsf_mpp_venc_send error.\n");
              }
            }
            #endif

            clock_gettime(CLOCK_MONOTONIC, &ts2);
            printf("save cost:%d ms\n", (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000));
          }
        }
        
        sample_svp_npu_detect(hi_frame, other_frame, boxs);
        vcap[i].vcap.get_frame_unlock(hi_frame, other_frame);
        
        boxs->chn = i;
        boxs++;
      }
    }
    
    return ((intptr_t)boxs-(intptr_t)_boxs)/sizeof(yolo_boxs_t);
}

int yolov5_deinit()
{
  int ret = sample_svp_npu_destroy();
  printf("sample_svp_npu_destroy ret:%d\n\n", ret);


  if(__venc_yuv2jpeg)
  {
    gsf_mpp_venc_stop(__venc_yuv2jpeg);
    __venc_yuv2jpeg = NULL;
  }  

  for(int i = 0; i < YOLO_CHN_MAX; i++)
  {
    if(vcap[i].fd > 0)
    {  
      ret = vcap[i].vcap.destroy();
      printf("vcap[%d].destroy ret:%d\n\n", i, ret);
    }  
  }
  return 0;
}
 
 
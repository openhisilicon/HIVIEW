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
          
          if(0)//if(!vcap[i].image.empty())
          {
            static int j = 0;
            char filename[256];
            sprintf(filename, "ch%02d_%08d.jpg", i, j++);
            cv::imwrite(filename, vcap[i].image);
          }  
        }
        
        sample_svp_npu_detect(hi_frame, other_frame, boxs);
        
        vcap[i].vcap.get_frame_unlock(hi_frame, other_frame);
        boxs++;
      }
    }
    
    return ((intptr_t)boxs-(intptr_t)_boxs)/sizeof(yolo_boxs_t);
}

int yolov5_deinit()
{
  sample_svp_npu_destroy();
  
  for(int i = 0; i < YOLO_CHN_MAX; i++)
  {
    if(vcap[i].fd > 0)
    {  
      vcap[i].vcap.destroy();  
    }  
  }
  return 0;
}
 
 
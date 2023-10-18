#include <iostream>
#include <opencv2/opencv.hpp>

#include "yolov8c.h"
#include "vpss_capture.h"
#include "yolov8.h"

typedef struct {
  VpssCapture vcap;
  cv::Mat image;
  int fd;
}vcap_t;

static vcap_t vcap[YOLO_CHN_MAX];
static YOLOV8C yolov8;

//save test flag;
static int vcap_save_yuv   = 0;
static int vcap_save_image = 0;

static int vcap_cnt = 0;

int yolov8_init(int vpss_grp[YOLO_CHN_MAX], int vpss_chn[YOLO_CHN_MAX], char *ModelPath)
{
    int i = 0, cnt = 10;
    int ret = -1;
    
    while(1)
    {
      for(i = 0; i < YOLO_CHN_MAX; i++)
      {
        if(vpss_grp[i] == -1 || vpss_chn[i] == -1)
          break;
          
        if(vcap[i].fd <= 0)
        {

          //vcap[i].fd = vcap[i].vcap.init(vpss_grp[i], vpss_chn[i], 640, 640);
          vcap[i].fd = vcap[i].vcap.init(vpss_grp[i], vpss_chn[i]);
          if(vcap[i].fd > 0)
          {
            vcap_cnt++;
            printf("vcap[%d].init ok fd:%d\n", i, vcap[i].fd);
          }
          else
          {
            printf("vcap[%d].init err fd:%d\n", i, vcap[i].fd);
          }
        }
      }
      // all is ok;
      if(vcap_cnt == i)
        break;
      // timeout; 
      if(--cnt <= 0)
        break;
      // retry once;
      if(vcap_cnt > 0)
        cnt = 1;

      sleep(1);
    }
    
    printf("init all:%d, vcap_cnt:%d\n", i, vcap_cnt);
    if(vcap_cnt > 0)
    {
      ret = yolov8.init(ModelPath);
      printf("yolov8.init ret:%d [%s]\n", ret, ModelPath);
    }
    return ret;
}

int yolov8_detect(yolo_boxs_t _boxs[YOLO_CHN_MAX])
{
  
#if YOLOV8TINY
    //yolov8-tiny; 
    static const char* class_names[] = {
        "aeroplane", "bicycle", "bird", "boat",
        "bottle", "bus", "car", "cat", "chair",
        "cow", "diningtable", "dog", "horse",
        "motorbike", "person", "pottedplant",
        "sheep", "sofa", "train", "tvmonitor"
    };
#else
    static const char* class_names[] = {
        "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light",
        "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow",
        "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
        "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard",
        "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple",
        "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch",
        "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone",
        "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear",
        "hair drier", "toothbrush"
    };
#endif
    hi_video_frame_info *frame_info = NULL;
    yolo_boxs_t *boxs = _boxs;
    int ret = 0;
    int maxfd = 0;
    struct timeval to;
    fd_set read_fds;
    FD_ZERO(&read_fds);
    
    for (int i = 0; i < vcap_cnt; i++)
    {
        FD_SET(vcap[i].fd, &read_fds);
        maxfd = (maxfd < vcap[i].fd)?vcap[i].fd:maxfd;
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
    for (int i = 0; i < vcap_cnt; i++)
    {
      if (FD_ISSET(vcap[i].fd, &read_fds))
      {
        //printf("vpss get ok! [fd:%d]\n", vcap[i].fd);
        vcap[i].vcap.get_frame_lock(vcap[i].image, &frame_info);
        //vcap[i].vcap.get_frame(vcap[i].image);
        if(vcap[i].image.empty())
        {
            std::cout << "vpss capture failed!!!\n";
            return -1;
        }
        
        printf("vcap.get_frame_lock chn:%d, image:[%dx%d], frame_info[%dx%d]\n"
            , i, vcap[i].image.cols, vcap[i].image.rows, frame_info->video_frame.width, frame_info->video_frame.height);
        
        //cv::imwrite("test.jpg", vcap[i].image);
        //cv::Mat image = cv::imread("test.jpg");
        if(vcap_save_yuv) // save _image .jpg
        {
          hi_video_frame *frame = &frame_info->video_frame;
          hi_char *g_user_page_addr[2] = { HI_NULL, HI_NULL }; /* 2 Y and C */
          hi_u32 g_size = 0, g_c_size = 0;
          
          hi_pixel_format pixel_format = frame->pixel_format;
          /* When the storage format is a planar format, this variable is used to keep the height of the UV component */
          hi_u32 uv_height = 0;
          hi_bool is_uv_invert = (pixel_format == HI_PIXEL_FORMAT_YUV_SEMIPLANAR_420 ||
              pixel_format == HI_PIXEL_FORMAT_YUV_SEMIPLANAR_422) ? HI_TRUE : HI_FALSE;

          g_size = (frame->stride[0]) * (frame->height);
          if (pixel_format == HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420 || pixel_format == HI_PIXEL_FORMAT_YUV_SEMIPLANAR_420) {
              g_c_size = (frame->stride[1]) * (frame->height) / 2; /* 2 uv height */
              uv_height = frame->height / 2; /* 2 uv height */
          } else if (pixel_format == HI_PIXEL_FORMAT_YVU_SEMIPLANAR_422 ||
              pixel_format == HI_PIXEL_FORMAT_YUV_SEMIPLANAR_422) {
              g_c_size = (frame->stride[1]) * (frame->height);
              uv_height = frame->height;
          } else if (pixel_format == HI_PIXEL_FORMAT_YUV_400) {
              g_c_size = 0;
              uv_height = frame->height;
          }


          g_user_page_addr[0] = (hi_char *)hi_mpi_sys_mmap(frame->phys_addr[0], g_size);
          g_user_page_addr[1] = (hi_char *)hi_mpi_sys_mmap(frame->phys_addr[1], g_c_size);
          if (g_user_page_addr[0] != HI_NULL)
          {
            #if 0
            /* save Y */
            cv::Mat _image(frame->height, frame->stride[0], CV_8UC1, g_user_page_addr[0]);
            #else
            /* save YUV */
            cv::Mat _image1(frame->height + uv_height, frame->stride[0], CV_8UC1);
            memcpy(_image1.data, g_user_page_addr[0], g_size); 
            memcpy(_image1.data+g_size, g_user_page_addr[1], g_c_size);
            
            cv::Mat _image;
            cv::cvtColor(_image1, _image, CV_YUV2BGR_NV12);
            #endif
            
            HI_CHAR szJpgName[128];
            sprintf(szJpgName, "/nfsroot/3403/vcap/ch%d_%dx%d_%llu.jpg", i, frame->stride[0], frame->height, frame->pts);
            cv::imwrite(szJpgName, _image);
            fprintf(stderr, "save _image [%s] done!\n", szJpgName);
            fflush(stderr);
            hi_mpi_sys_munmap(g_user_page_addr[0], g_size);
            hi_mpi_sys_munmap(g_user_page_addr[1], g_c_size);
          }
        }

        struct timespec ts1, ts2;
        clock_gettime(CLOCK_MONOTONIC, &ts1);

        std::vector<Object> bboxs;
        //ret = yolov8.detect(image, bboxs);  
        ret = yolov8.detect(vcap[i].image, bboxs);
        
        boxs->chn = i;
        boxs->w = vcap[i].image.cols;
        boxs->h = vcap[i].image.rows;
        boxs->size = bboxs.size();
        if(boxs->size > YOLO_BOX_MAX)
          printf("@@@@@@@@@@ chn:%d, boxs->size = %d > YOLO_BOX_MAX = %d\n", boxs->chn, boxs->size, YOLO_BOX_MAX);
        boxs->size = (boxs->size > YOLO_BOX_MAX)?YOLO_BOX_MAX:boxs->size;
        
        static char label[256][256] = {0};
        for(int i = 0; i < boxs->size; i++)
        { 
            boxs->box[i].score  = bboxs[i].score;
            boxs->box[i].x = (bboxs[i].x1 < 0)?0:(bboxs[i].x1 > boxs->w-1)?boxs->w-1:bboxs[i].x1;
            boxs->box[i].y = (bboxs[i].y1 < 0)?0:(bboxs[i].y1 > boxs->h-1)?boxs->h-1:bboxs[i].y1;
            boxs->box[i].w = (bboxs[i].x2 < 1)?1:(bboxs[i].x2 > boxs->w-0)?boxs->w-0:bboxs[i].x2;
            boxs->box[i].h = (bboxs[i].y2 < 1)?1:(bboxs[i].y2 > boxs->h-0)?boxs->h-0:bboxs[i].y2;
           
            boxs->box[i].w -= boxs->box[i].x;
            boxs->box[i].h -= boxs->box[i].y;   
            #if 0
            printf("i:%d, result[x1:%0.2f, y1:%0.2f, x2:%0.2f, y2:%0.2f] => box[x:%0.2f, y:%0.2f, w:%0.2f, h:%0.2f]\n"
                  , i
                  , bboxs[i].x1, bboxs[i].y1, bboxs[i].x2, bboxs[i].y2
                  , boxs->box[i].x, boxs->box[i].y, boxs->box[i].w, boxs->box[i].h);
                  
            assert(boxs->box[i].x + boxs->box[i].w <= boxs->w);
            assert(boxs->box[i].y + boxs->box[i].h <= boxs->h);
            #endif
            boxs->box[i].label = class_names[bboxs[i].label];
        }
        
        clock_gettime(CLOCK_MONOTONIC, &ts2);
        #if 1
        printf("boxs->size:%d, cost:%d ms\n", boxs->size, (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000));
        #endif
        
        vcap[i].vcap.get_frame_unlock(frame_info);
        boxs++;
      }
    }
    
    return ((intptr_t)boxs-(intptr_t)_boxs)/sizeof(yolo_boxs_t);
}

int yolov8_deinit()
{
  yolov8.destroy();
  for(int i = 0; i < vcap_cnt; i++)
  {
    vcap[i].vcap.destroy();  
  }
  return 0;
}
 
 
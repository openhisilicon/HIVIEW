#include <iostream>
#include <opencv2/opencv.hpp>

#include "yolov5c.h"
#include "vpss_capture.h"
#include "yolo_sample.h"
#include "zbar.h"
using namespace zbar;

typedef struct {
  VpssCapture vcap;
  cv::Mat image;
  int fd;
}vcap_t;
vcap_t vcap[YOLO_CHN_MAX];
YOLOV5C yolov5;
static int vcap_cnt = 0;
static int qrcode = 0; 

//save test flag;
static int vcap_save_yuv   = 0;
static int vcap_save_image = 0;

int yolo_init(int VpssGrp[YOLO_CHN_MAX], int VpssChn[YOLO_CHN_MAX], char *ModelPath)
{
    int i = 0, cnt = 10;
    int ret = -1;
    
    while(1)
    {
      for(i = 0; i < YOLO_CHN_MAX; i++)
      {
        if(VpssGrp[i] == -1 || VpssChn[i] == -1)
          break;
          
        if(vcap[i].fd <= 0)
        {
          vcap[i].fd = vcap[i].vcap.init(VpssGrp[i], VpssChn[i]);
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
      ret = yolov5.init(ModelPath);
      qrcode = strstr(ModelPath, "qrcode")?1:0;
      printf("yolov5.init ret:%d\n", ret);
    }
    return ret;
}


int yolo_detect(yolo_boxs_t _boxs[YOLO_CHN_MAX])
{
    static const char* class_names[] = {/*"background",*/
        "aeroplane", "bicycle", "bird", "boat",
        "bottle", "bus", "car", "cat", "chair",
        "cow", "diningtable", "dog", "horse",
        "motorbike", "person", "pottedplant",
        "sheep", "sofa", "train", "tvmonitor"
    };
    
    static zbar_image_scanner_t *scanner = NULL;
    static zbar_image_t *zbar_image = NULL;
    VIDEO_FRAME_INFO_S *pstFrame = NULL;
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
        vcap[i].vcap.get_frame_lock(vcap[i].image, &pstFrame);
        if(vcap[i].image.empty())
        {
            std::cout << "vpss capture failed!!!\n";
            return -1;
        }

        struct timespec ts1, ts2;
        clock_gettime(CLOCK_MONOTONIC, &ts1);

        std::vector<BoxInfo> bboxs;
        ret = yolov5.detect(vcap[i].image, bboxs);
        
        boxs->chn = i;
        boxs->w = vcap[i].image.cols;
        boxs->h = vcap[i].image.rows;
        boxs->size = bboxs.size();
        if(boxs->size > YOLO_BOX_MAX)
          printf("@@@@@@@@@@ chn:%d, boxs->size = %d > YOLO_BOX_MAX = %d\n", boxs->chn, boxs->size, YOLO_BOX_MAX);
        boxs->size = (boxs->size > YOLO_BOX_MAX)?YOLO_BOX_MAX:boxs->size;
          
        if(qrcode && !scanner)
        {
        	scanner = zbar_image_scanner_create();
        	zbar_image_scanner_set_config(scanner, ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
          zbar_image = zbar_image_create();
          zbar_image_set_format(zbar_image, *(int*)"Y800");
        }
        
        VIDEO_FRAME_S* pVBuf = &pstFrame->stVFrame;
        PIXEL_FORMAT_E  enPixelFormat = pVBuf->enPixelFormat;
        HI_U32 u32UvHeight = 0, u32Size = 0;
        HI_BOOL bUvInvert = (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat || PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixelFormat) ? HI_TRUE : HI_FALSE;
        if (PIXEL_FORMAT_YVU_SEMIPLANAR_420 == enPixelFormat || PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat)
        {
            u32Size = (pVBuf->u32Stride[0]) * (pVBuf->u32Height) * 3 / 2;
            u32UvHeight = pVBuf->u32Height / 2;
        }
        else if (PIXEL_FORMAT_YVU_SEMIPLANAR_422 == enPixelFormat || PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixelFormat)
        {
            u32Size = (pVBuf->u32Stride[0]) * (pVBuf->u32Height) * 2;
            u32UvHeight = pVBuf->u32Height;
        }
        else if (PIXEL_FORMAT_YUV_400 == enPixelFormat)
        {
            u32Size = (pVBuf->u32Stride[0]) * (pVBuf->u32Height);
            u32UvHeight = pVBuf->u32Height;
        }
        char *srcVirAddr = (char*)HI_MPI_SYS_Mmap(pVBuf->u64PhyAddr[0], u32Size);
        #if 0
        if(boxs->w != pVBuf->u32Width)
        {  
          printf("chn:%d, srcVirAddr:%p, w:%d, h:%d, u32Width:%d, u32Height:%d, u32Stride[0]:%d\n"
                , i, srcVirAddr, boxs->w, boxs->h
                , pVBuf->u32Width, pVBuf->u32Height, pVBuf->u32Stride[0]);
        }
        #endif
        
        static char label[256][256] = {0};
        for(int i = 0; i < boxs->size; i++)
        {
            boxs->box[i].score  = bboxs[i].score;
            boxs->box[i].x = (bboxs[i].box.x > 0 && bboxs[i].box.x < boxs->w)?bboxs[i].box.x:0;
            boxs->box[i].y = (bboxs[i].box.y > 0 && bboxs[i].box.y < boxs->h)?bboxs[i].box.y:0;
            boxs->box[i].w = (boxs->box[i].x + bboxs[i].box.width < boxs->w)?bboxs[i].box.width:boxs->w - boxs->box[i].x;
            boxs->box[i].h = (boxs->box[i].y + bboxs[i].box.height < boxs->h)?bboxs[i].box.height:boxs->h - boxs->box[i].y;
            boxs->box[i].label    = class_names[bboxs[i].label];
     
            if(scanner)
            {
              boxs->box[i].label = label[i];
              label[i][0] = '\0';
              
              int x = boxs->box[i].x;
              int y = boxs->box[i].y;
              int w = boxs->box[i].w;
              int h = boxs->box[i].h;

              x = (float)x/boxs->w * pVBuf->u32Width;
              y = (float)y/boxs->h * pVBuf->u32Height;
              w = (float)w/boxs->w * pVBuf->u32Width;
              h = (float)h/boxs->h * pVBuf->u32Height;
              
              char *dstVirAddr = (char*)malloc(w*h);
              char *dstOffset = dstVirAddr;
              //printf("dstVirAddr:%p, [%d,%d,%d,%d]\n", srcVirAddr, x, y, w, h);
              for(int l = 0; l < h; l++)
              {
                memcpy(dstOffset, srcVirAddr + x + (y+l)*pVBuf->u32Stride[0], w);
                dstOffset += w;
                assert(dstOffset <= dstVirAddr+w*h);
              }
              zbar_image_set_size(zbar_image, w, h);
              zbar_image_set_data(zbar_image, dstVirAddr, w*h, NULL);

              /* scan the image for barcodes */
              int n = zbar_scan_image(scanner, zbar_image);
              if(n > 0)
              {
                /* extract results */
                const zbar_symbol_t *symbol = zbar_image_first_symbol(zbar_image);
                for(; symbol; symbol = zbar_symbol_next(symbol)) 
                {
                    /* do something useful with results */
                    zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
                    const char *data = zbar_symbol_get_data(symbol);
                    //printf("decoded %s symbol \"%s\"\n", zbar_get_symbol_name(typ), data);
                    snprintf(label[i], 255, "%s", data);
                    boxs->box[i].label = label[i];
                }
             }
              free(dstVirAddr);
          	}
        }
        
        #if 0
        clock_gettime(CLOCK_MONOTONIC, &ts2);
        printf("cost:%d ms\n", (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000));
        #endif

        if(vcap_save_yuv) do // save pstFrame .yuv;
        {
          unsigned char TmpBuff[20480];
          char* pMemContent = NULL;
          char* pVBufVirt_Y = srcVirAddr;
          char* pVBufVirt_C = pVBufVirt_Y + (pVBuf->u32Stride[0]) * (pVBuf->u32Height);
          HI_CHAR szYuvName[128];
          sprintf(szYuvName, "/nfsroot/16d/vcap/ch%d_%dx%d_%llu.yuv", i, pVBuf->u32Width, pVBuf->u32Height, pVBuf->u64PTS);
          FILE *pfd = fopen(szYuvName, "wb");
          if(!pfd)
          {
            break;
          }  
          /* save Y ----------------------------------------------------------------*/
          fprintf(stderr, "saving......Y......");
          fflush(stderr);

          for (int h = 0; h < pVBuf->u32Height; h++)
          {
              pMemContent = pVBufVirt_Y + h * pVBuf->u32Stride[0];
              fwrite(pMemContent, pVBuf->u32Width, 1, pfd);
          }

          if (PIXEL_FORMAT_YUV_400 != enPixelFormat)
          {
              fflush(pfd);
              /* save U ----------------------------------------------------------------*/
              fprintf(stderr, "U......");
              fflush(stderr);

              for (int h = 0; h < u32UvHeight; h++)
              {
                  pMemContent = pVBufVirt_C + h * pVBuf->u32Stride[1];

                  if(!bUvInvert) pMemContent += 1;

                  for (int w = 0; w < pVBuf->u32Width / 2; w++)
                  {
                      TmpBuff[w] = *pMemContent;
                      pMemContent += 2;
                  }

                  fwrite(TmpBuff, pVBuf->u32Width / 2, 1, pfd);
              }

              fflush(pfd);

              /* save V ----------------------------------------------------------------*/
              fprintf(stderr, "V......");
              fflush(stderr);

              for (int h = 0; h < u32UvHeight; h++)
              {
                  pMemContent = pVBufVirt_C + h * pVBuf->u32Stride[1];

                  if(bUvInvert) pMemContent += 1;

                  for (int w = 0; w < pVBuf->u32Width / 2; w++)
                  {
                      TmpBuff[w] = *pMemContent;
                      pMemContent += 2;
                  }

                  fwrite(TmpBuff, pVBuf->u32Width / 2, 1, pfd);
              }
          }

          fflush(pfd);
          fclose(pfd);
         
          fprintf(stderr, "save [%s] done!\n", szYuvName);
          fflush(stderr);
        }while(0);
        
        if(vcap_save_image) // save image .jpg
        {
          HI_CHAR szJpgName[128];
          sprintf(szJpgName, "/nfsroot/16d/vcap/ch%d_%dx%d_%llu.jpg", i, pVBuf->u32Width, pVBuf->u32Height, pVBuf->u64PTS);
          cv::imwrite(szJpgName, vcap[i].image);
          fprintf(stderr, "save [%s] done!\n", szJpgName);
          fflush(stderr);  
        }
        
        HI_MPI_SYS_Munmap(srcVirAddr, u32Size);
        vcap[i].vcap.get_frame_unlock(pstFrame);
        boxs++;
      }
    }
    
    return ((unsigned int)boxs-(unsigned int)_boxs)/sizeof(yolo_boxs_t);
}
int yolo_deinit()
{
  yolov5.destroy();
  for(int i = 0; i < vcap_cnt; i++)
  {
    vcap[i].vcap.destroy();  
  }
  return 0;
}
 
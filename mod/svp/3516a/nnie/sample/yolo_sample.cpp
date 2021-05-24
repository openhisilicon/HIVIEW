#include <iostream>
#include <opencv2/opencv.hpp>

#include "yolov5c.h"
#include "vpss_capture.h"
#include "yolo_sample.h"

YOLOV5C yolov5;
VpssCapture vcap;
cv::Mat image;

int yolo_init(int VpssGrp, int VpssChn, char *ModelPath)
{
    int i = 10;
    int ret = 0;
    
    do{
      ret = vcap.init(VpssGrp, VpssChn);
      //printf("vcap.init ret:%d\n", ret);
      if(ret == 0)
      {
        ret = yolov5.init(ModelPath);
        printf("yolov5.init ret:%d\n", ret);
        break;
      }
      sleep(1);
    }while(i-- > 0);
    
    return ret;
}
int yolo_detect(yolo_boxs_t *boxs)
{
    static const char* class_names[] = {/*"background",*/
        "aeroplane", "bicycle", "bird", "boat",
        "bottle", "bus", "car", "cat", "chair",
        "cow", "diningtable", "dog", "horse",
        "motorbike", "person", "pottedplant",
        "sheep", "sofa", "train", "tvmonitor"
    };

    vcap.get_frame(image);
    if(image.empty())
    {
        std::cout << "vpss capture failed!!!\n";
        return -1;
    }

    std::vector<BoxInfo> bboxs;
      
    int ret = yolov5.detect(image, bboxs);
    
    boxs->w = image.cols;
    boxs->h = image.rows;
    boxs->size = bboxs.size();
    if(boxs->size > YOLO_BOX_MAX)
      printf("@@@@@@@@@@ boxs->size = %d > %d\n", boxs->size, YOLO_BOX_MAX);
    boxs->size = (boxs->size > YOLO_BOX_MAX)?YOLO_BOX_MAX:boxs->size;
    for(int i = 0; i < boxs->size; i++)
    {
        boxs->box[i].score  = bboxs[i].score;
        boxs->box[i].x      = bboxs[i].box.x;
        boxs->box[i].y      = bboxs[i].box.y;
        boxs->box[i].w      = bboxs[i].box.width;
        boxs->box[i].h      = bboxs[i].box.height;
        boxs->box[i].label    = class_names[bboxs[i].label];
    }

    return 0;
}
int yolo_deinit()
{
  yolov5.destroy();
  vcap.destroy();
  return 0;
}

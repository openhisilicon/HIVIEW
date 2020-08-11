#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <vector>
#include <sys/ioctl.h>
#include <dirent.h>
#include "ins_nnie_interface.h"
#include "Tensor.h"
#include "util.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"
#include "opencv2/dnn.hpp"
#include "opencv2/imgproc/imgproc_c.h"

void yolov3DetectDemo(const char *model_path, const char *image_path)
{
    NNIE yolov3_mnas;

    yolov3_mnas.init(model_path);

    printf("yolov3 start\n");

    struct timeval tv1;
    struct timeval tv2;
    long t1, t2, time;

    gettimeofday(&tv1, NULL);
    yolov3_mnas.run(image_path);

    gettimeofday(&tv2, NULL);
    t1 = tv2.tv_sec - tv1.tv_sec;
    t2 = tv2.tv_usec - tv1.tv_usec;
    time = (long)(t1 * 1000 + t2 / 1000);
    printf("yolov3 NNIE inference time : %dms\n", time);

    gettimeofday(&tv1, NULL);

    Tensor output0 = yolov3_mnas.getOutputTensor(0);
    Tensor output1 = yolov3_mnas.getOutputTensor(1);
    Tensor output2 = yolov3_mnas.getOutputTensor(2);

    /*yolov3的参数*/
    // int img_width = 416;
    // int img_height = 416;
    // int num_classes = 80;
    // int kBoxPerCell = 3;

    int img_width = 608;
    int img_height = 608;
    int num_classes = 2;
    int kBoxPerCell = 3;

    int feature_index0 = 0;
    int feature_index1 = 1;
    int feature_index2 = 2;

    float conf_threshold = 0.2;
    float nms_threshold = 0.5;
    int is_nms = 1;

    std::vector<int> ids;
    std::vector<cv::Rect> boxes;
    std::vector<float> confidences;

    const std::vector<std::vector<cv::Size2f>> anchors = {
        {{116, 90}, {156, 198}, {373, 326}},
        {{30, 61}, {62, 45}, {59, 119}},
        {{10, 13}, {16, 30}, {33, 23}}};

    parseYolov3Feature(img_width,
                       img_height,
                       num_classes,
                       kBoxPerCell,
                       feature_index0,
                       conf_threshold,
                       anchors[0],
                       output0,
                       ids,
                       boxes,
                       confidences);

    parseYolov3Feature(img_width,
                       img_height,
                       num_classes,
                       kBoxPerCell,
                       feature_index1,
                       conf_threshold,
                       anchors[1],
                       output1,
                       ids,
                       boxes,
                       confidences);

    parseYolov3Feature(img_width,
                       img_height,
                       num_classes,
                       kBoxPerCell,
                       feature_index2,
                       conf_threshold,
                       anchors[2],
                       output2,
                       ids,
                       boxes,
                       confidences);

    std::vector<int> indices;
    /*print result, this sample has 81 classes:
      class 0:background      class 1:person       class 2:bicycle         class 3:car            class 4:motorbike      class 5:aeroplane
      class 6:bus             class 7:train        class 8:truck           class 9:boat           class 10:traffic light
      class 11:fire hydrant   class 12:stop sign   class 13:parking meter  class 14:bench         class 15:bird
      class 16:cat            class 17:dog         class 18:horse          class 19:sheep         class 20:cow
      class 21:elephant       class 22:bear        class 23:zebra          class 24:giraffe       class 25:backpack
      class 26:umbrella       class 27:handbag     class 28:tie            class 29:suitcase      class 30:frisbee
      class 31:skis           class 32:snowboard   class 33:sports ball    class 34:kite          class 35:baseball bat
      class 36:baseball glove class 37:skateboard  class 38:surfboard      class 39:tennis racket class 40bottle
      class 41:wine glass     class 42:cup         class 43:fork           class 44:knife         class 45:spoon
      class 46:bowl           class 47:banana      class 48:apple          class 49:sandwich      class 50orange
      class 51:broccoli       class 52:carrot      class 53:hot dog        class 54:pizza         class 55:donut
      class 56:cake           class 57:chair       class 58:sofa           class 59:pottedplant   class 60bed
      class 61:diningtable    class 62:toilet      class 63:vmonitor       class 64:laptop        class 65:mouse
      class 66:remote         class 67:keyboard    class 68:cell phone     class 69:microwave     class 70:oven
      class 71:toaster        class 72:sink        class 73:refrigerator   class 74:book          class 75:clock
      class 76:vase           class 77:scissors    class 78:teddy bear     class 79:hair drier    class 80:toothbrush*/
    char *cls_names[] = {"background", "person", "bicycle", "car", "motorbike", "aeroplane", "bus", "train", "truck", "boat",

                         "traffic light", "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep",

                         "cow", "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase",

                         "frisbee", "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket",

                         "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "donut", "apple", "sandwich",

                         "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "sofa", "pottedplant",

                         "bed", "diningtable", "toilet", "vmonitor", "laptop", "mouse", "remote", "keyboard", "cell phone", "microwave",

                         "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear", "hair drier", "toothbrush"};
    std::vector<ObjectDetection> detection_results;

    if (is_nms)
    {
        cv::dnn::NMSBoxes(boxes, confidences, conf_threshold, nms_threshold, indices);
    }
    else
    {
        for (int i = 0; i < boxes.size(); ++i)
        {
            indices.push_back(i);
        }
    }

    for (size_t i = 0; i < indices.size(); ++i)
    {
        int idx = indices[i];
        cv::Rect box = boxes[idx];

        // remap box in src input size.
        auto remap_box = RemapBoxOnSrc(cv::Rect2d(box), img_width, img_height);
        ObjectDetection object_detection;
        object_detection.box = remap_box;
        object_detection.cls_id = ids[idx] + 1;
        object_detection.confidence = confidences[idx];
        detection_results.push_back(std::move(object_detection));

        float xmin = object_detection.box.x;
        float ymin = object_detection.box.y;
        float xmax = object_detection.box.x + object_detection.box.width;
        float ymax = object_detection.box.y + object_detection.box.height;
        float confidence = object_detection.confidence;
        int cls_id = object_detection.cls_id;
        char *cls_name = cls_names[cls_id];
        printf("%d %s %.3f %.3f %.3f %.3f %.3f\n", cls_id, cls_name, confidence, xmin, ymin, xmax, ymax);
    }

    gettimeofday(&tv2, NULL);
    t1 = tv2.tv_sec - tv1.tv_sec;
    t2 = tv2.tv_usec - tv1.tv_usec;
    time = (long)(t1 * 1000 + t2 / 1000);
    printf("yolov3 postProcess : %dms\n", time);

    printf("yolov3 finish\n");
}

/******************************************************************************
* function 
******************************************************************************/
int main(int argc, char *argv[])
{

   cv::Mat img = cv::imread("./demo.jpg");
	    cv::Mat gray;
	    if(img.empty()){
	        printf("read img err...\n");
	        return -1;
	    }
	    cv::cvtColor(img, gray, CV_BGR2GRAY);
	    cv::imwrite("demo_gray.jpg", gray);
	    return 0;

return;


 
    const char *image_path = "./data/nnie_image/test/000100_608x608.bgr";
    const char *model_path = "./data/nnie_model/detection/yolov3_my_inst.wk";
    //yolov3DetectDemo(model_path, image_path);
  
    int file_length = 0;
    FILE *fp = fopen(image_path, "rb");
    if (fp == NULL)
    {
        printf("open %s failed\n", image_path);
        return;
    }

    fseek(fp, 0L, SEEK_END);
    file_length = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    unsigned char *data = (unsigned char *)malloc(sizeof(unsigned char) * file_length);

    fread(data, file_length, 1, fp);

    fclose(fp);

    cv::Mat src(608, 608, CV_8UC3),dst;
    cv::cvtColor(src, dst, CV_BGR2RGB);

    cv::imwrite("out.jpg",dst);
    return 1;
}

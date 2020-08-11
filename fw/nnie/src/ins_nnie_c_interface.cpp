#include "stdio.h"
#include <map>

#include "ins_nnie_interface.h"
#include "ins_nnie_c_interface.h"

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
#include "opencv2/core/core_c.h"
#include "opencv2/imgproc/imgproc_c.h"

#include "chef_filepath_op.hpp"
using namespace chef;
//imgproc_c.h

static int s_cnnie_index = 0;
static std::map<int, NNIE *> s_nnie_map;

int CNNIE_init(const char *model_path, const int image_height, const int image_width, CNNIE_RESULT_CB call_back)
{
    s_cnnie_index = s_cnnie_index + 1;
    NNIE *pNNIE = new NNIE();
    s_nnie_map[s_cnnie_index] = pNNIE;

    pNNIE->init(model_path, image_height, image_width);
    pNNIE->setCallBack(call_back);
    return s_cnnie_index;
}

YOLOV3_Result* CNNIE_run_with_file(int nnie_id, int run_id, const char *image_path, const int img_height, const int img_width, int num_classes, int kBoxPerCell, float conf_threshold, float nms_threshold, int *result_count)
{
    if (s_nnie_map.count(nnie_id) == 0)
    {
        return NULL;
    }
    NNIE *pNNIE = s_nnie_map[nnie_id];
    pNNIE->run(image_path);

    Tensor output0 = pNNIE->getOutputTensor(0);
    Tensor output1 = pNNIE->getOutputTensor(1);
    Tensor output2 = pNNIE->getOutputTensor(2);

    int feature_index0 = 0;
    int feature_index1 = 1;
    int feature_index2 = 2;

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
    //printf("new reslut %d \n",indices.size());
    //pYOLOV3Result = new YOLOV3_Result[indices.size()];
    *result_count = indices.size();
    YOLOV3_Result * pYOLOV3Result = (YOLOV3_Result *)malloc(indices.size() * sizeof(YOLOV3_Result));
    //printf("new reslut ok %d \n",pYOLOV3Result);
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
        //printf("%d %.3f %.3f %.3f %.3f %.3f\n", cls_id, confidence, xmin, ymin, xmax, ymax);

        pYOLOV3Result[i].m_class = cls_id;
        pYOLOV3Result[i].m_confidence = cls_id;
        pYOLOV3Result[i].m_rect.m_iLeft = xmin;
        pYOLOV3Result[i].m_rect.m_iTop = ymin;
        pYOLOV3Result[i].m_rect.m_iRight = xmax;
        pYOLOV3Result[i].m_rect.m_iBottom = ymax;

        CNNIE_RESULT_CB callback = pNNIE->getCallBack();
        if (callback != NULL)
        {
            callback(nnie_id, run_id, cls_id, confidence, xmin, ymin, xmax, ymax);
        }
    }

    //printf("new reslut return %d %d\n",indices.size(),pYOLOV3Result);

    return pYOLOV3Result;
}

YOLOV3_Result* CNNIE_run_with_data_bgr(int nnie_id, int run_id, const unsigned char *data, const int img_height, const int img_width, int num_classes, int kBoxPerCell, float conf_threshold, float nms_threshold, int *result_count)
{
    if (s_nnie_map.count(nnie_id) == 0)
    {
        return NULL;
    }
    NNIE *pNNIE = s_nnie_map[nnie_id];

    pNNIE->run(data);

    Tensor output0 = pNNIE->getOutputTensor(0);
    Tensor output1 = pNNIE->getOutputTensor(1);
    Tensor output2 = pNNIE->getOutputTensor(2);

    int feature_index0 = 0;
    int feature_index1 = 1;
    int feature_index2 = 2;

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
    *result_count=indices.size();
    YOLOV3_Result * pYOLOV3Result = (YOLOV3_Result *)malloc(indices.size() * sizeof(YOLOV3_Result));

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
        //printf("%d %.3f %.3f %.3f %.3f %.3f\n", cls_id, confidence, xmin, ymin, xmax, ymax);
        pYOLOV3Result[i].m_class = cls_id;
        pYOLOV3Result[i].m_confidence = cls_id;
        pYOLOV3Result[i].m_rect.m_iLeft = xmin;
        pYOLOV3Result[i].m_rect.m_iTop = ymin;
        pYOLOV3Result[i].m_rect.m_iRight = xmax;
        pYOLOV3Result[i].m_rect.m_iBottom = ymax;

        CNNIE_RESULT_CB callback = pNNIE->getCallBack();
        if (callback != NULL)
        {
            callback(nnie_id, run_id, cls_id, confidence, xmin, ymin, xmax, ymax);
        }
    }

    return pYOLOV3Result;
}

YOLOV3_Result* CNNIE_run_with_data_yuv(int nnie_id, int run_id, const unsigned char *data, const int img_height, const int img_width, int num_classes, int kBoxPerCell, float conf_threshold, float nms_threshold,int *result_count)
{
    struct timeval tv1;
    struct timeval tv2;
    long t1, t2, time;

    gettimeofday(&tv1, NULL);

    //
    if (s_nnie_map.count(nnie_id) == 0)
    {
        return NULL;
    }
    NNIE *pNNIE = s_nnie_map[nnie_id];
    //filepath_op::write_file("test1.yuv",data,img_width * img_height * 3/2);
    //printf("run 0 \n");
    cv::Mat src(img_height * 3 / 2, img_width, CV_8UC1), dst, dst_resize;
    //printf("memcpy data len %d \n", img_width * img_height * 3/2);
    memcpy(src.data, data, img_width * img_height * 3 / 2);
    //filepath_op::write_file("test2.yuv",src.data,img_width * img_height * 3/2);

    //printf("run src 1 %d,%d\n", src.size().width,src.size().height);
    //printf("run dst * %d,%d\n" ,dst.size().width,dst.size().height);

    resize(src, dst, cv::Size(pNNIE->getWidth(), pNNIE->getHeight() * 3 / 2));

    cv::cvtColor(dst, dst_resize, CV_YUV420sp2BGR);

    //printf("run dst 2 %d,%d\n" ,dst.size().width,dst.size().height);
    //resize(dst, dst_resize, cv::Size(pNNIE->getWidth(), pNNIE->getHeight()));
    //printf("run dst_res 3 %d,%d\n",dst_resize.size().width,dst_resize.size().height);

    unsigned char *buffer = new unsigned char[dst_resize.size().width * dst_resize.size().height * 3];

    int step = dst_resize.step;
    int h = dst_resize.rows;
    int w = dst_resize.cols;
    int c = dst_resize.channels();
    int index = 0;
    for (int k = 0; k < c; k++)
    {
        for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j++)
            {
                //fwrite(&image2.data[i*step + j * c + k], sizeof(uint8_t), 1, fp);
                buffer[index] = dst_resize.data[i * step + j * c + k];
                index++;
            }
        }
    }

  YOLOV3_Result* pYOLOV3Result = CNNIE_run_with_data_bgr(nnie_id, run_id, buffer, pNNIE->getHeight(), pNNIE->getWidth(), num_classes, kBoxPerCell, conf_threshold, nms_threshold,result_count);

    //filepath_op::write_file("test.bgr",buffer,dst_resize.size().width*dst_resize.size().height*3);

    delete[] buffer;

    gettimeofday(&tv2, NULL);
    t1 = tv2.tv_sec - tv1.tv_sec;
    t2 = tv2.tv_usec - tv1.tv_usec;
    time = (long)(t1 * 1000 + t2 / 1000);
    //printf("yolov3 NNIE inference time : %dms\n", time);
    //printf("run 4 \n");
    return pYOLOV3Result;
}

void CNNIE_free(YOLOV3_Result *pYOLOV3Result)
{
    if (pYOLOV3Result!=NULL)
    {
        //delete[] pYOLOV3Result;
        free(pYOLOV3Result);
    }
}

void CNNIE_finish(int nnie_id)
{
    if (s_nnie_map.count(nnie_id) == 0)
    {
        return;
    }
    NNIE *pNNIE = s_nnie_map[nnie_id];
    delete pNNIE;
    s_nnie_map.erase(nnie_id);
}
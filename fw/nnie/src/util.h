#ifndef UTIL_H
#define UTIL_H
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"
#include "Tensor.h"
#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct ObjectDetection_S
    {
        int cls_id;
        float confidence;
        cv::Rect2d box;
    } ObjectDetection;

    inline int getFileLength(const char *file_path)
    {
        FILE *FILE;
        int u32FileLen;
        int s32Ret;

        FILE = fopen(file_path, "rb");

        if (NULL != FILE)
        {
            s32Ret = fseek(FILE, 0L, SEEK_END);
            if (0 != s32Ret)
            {
                printf("fseek err!\n");
                fclose(FILE);
                return 0;
            }

            u32FileLen = ftell(FILE);

            s32Ret = fseek(FILE, 0L, SEEK_SET);
            if (0 != s32Ret)
            {
                printf("fseek err!\n");
                fclose(FILE);
                return 0;
            }

            fclose(FILE);
        }
        else
        {
            printf("open file %s fail!\n", file_path);
            u32FileLen = 0;
        }

        return u32FileLen;
    };

    inline float Sigmoid(float x)
    {
        return 1.0 / (1.0 + exp(-x));
    };

    inline void Softmax(std::vector<float> &classes)
    {
        float sum = 0;
        std::transform(classes.begin(), classes.end(), classes.begin(),
                       [&sum](float score) -> float {
                           float exp_score = exp(score);
                           sum += exp_score;
                           return exp_score;
                       });
        std::transform(classes.begin(), classes.end(), classes.begin(),
                       [sum](float score) -> float { return score / sum; });
    };

    inline void parseYolov3Feature(int img_width,
                                   int img_height,
                                   int num_classes,
                                   int kBoxPerCell,
                                   int feature_index,
                                   float conf_threshold,
                                   const std::vector<cv::Size2f> &anchors,
                                   const Tensor feature,
                                   std::vector<int> &ids,
                                   std::vector<cv::Rect> &boxes,
                                   std::vector<float> &confidences)
    {
        const float downscale = static_cast<float>(std::pow(2, feature_index) / 32); // downscale, 1/32, 1/16, 1/8
        int cell_h = feature.height;
        int cell_w = feature.width;
        for (int cy = 0; cy < cell_h; ++cy)
        {
            for (int cx = 0; cx < cell_w; ++cx)
            {
                for (int b = 0; b < kBoxPerCell; ++b)
                {
                    int channel = b * (num_classes + 5);

                    float tc = feature.data[cx + (cy * feature.width) + (channel + 4) * feature.height * feature.width];

                    float confidence = Sigmoid(tc);

                    if (confidence >= conf_threshold)
                    {

                        float tx = feature.data[cx + (cy * feature.width) + channel * feature.height * feature.width];
                        float ty = feature.data[cx + (cy * feature.width) + (channel + 1) * feature.height * feature.width];
                        float tw = feature.data[cx + (cy * feature.width) + (channel + 2) * feature.height * feature.width];
                        float th = feature.data[cx + (cy * feature.width) + (channel + 3) * feature.height * feature.width];
                        float tc = feature.data[cx + (cy * feature.width) + (channel + 4) * feature.height * feature.width];

                        float x = (cx + Sigmoid(tx)) / cell_w;
                        float y = (cy + Sigmoid(ty)) / cell_h;
                        float w = exp(tw) * anchors[b].width * downscale / cell_w;
                        float h = exp(th) * anchors[b].height * downscale / cell_h;
                        // CV_LOG_INFO(NULL, "cx:" << cx << " cy:" << cy);
                        // CV_LOG_INFO(NULL, "box:" << x << " " << y << " " << w << " " << h << " c:" << confidence
                        //                          << " index:" << index);
                        std::vector<float> classes(num_classes);
                        for (int i = 0; i < num_classes; ++i)
                        {
                            classes[i] = feature.data[cx + (cy * feature.width) + (channel + 5 + i) * feature.height * feature.width];
                        }
                        Softmax(classes);
                        auto max_itr = std::max_element(classes.begin(), classes.end());
                        int index = static_cast<int>(max_itr - classes.begin());
                        if (num_classes > 1)
                        {
                            confidence = confidence * classes[index];
                        }

                        int center_x = (int)(x * img_width);
                        int center_y = (int)(y * img_height);
                        int width = (int)(w * img_width);
                        int height = (int)(h * img_height);
                        int left = static_cast<int>(center_x - width / 2);
                        int top = static_cast<int>(center_y - height / 2);
                        // CV_LOG_INFO(NULL,
                        //             "box:" << left << " " << top << " " << width << " " << height << " c:" << confidence
                        //                    << " index:" << index
                        //                    << " p:" << classes[index]);

                        ids.push_back(index);
                        boxes.emplace_back(left, top, width, height);
                        confidences.push_back(confidence);
                    }
                }
            }
        }
    };

    inline void parseYolov2Feature(int img_width,
                                   int img_height,
                                   int num_classes,
                                   int kBoxPerCell,
                                   int feature_index,
                                   float conf_threshold,
                                   const std::vector<cv::Size2f> &anchors,
                                   const Tensor feature,
                                   std::vector<int> &ids,
                                   std::vector<cv::Rect> &boxes,
                                   std::vector<float> &confidences)
    {
        const float downscale = static_cast<float>(std::pow(2, feature_index) / 32); // downscale, 1/32, 1/16, 1/8
        int cell_h = feature.height;
        int cell_w = feature.width;
        for (int cy = 0; cy < cell_h; ++cy)
        {
            for (int cx = 0; cx < cell_w; ++cx)
            {
                for (int b = 0; b < kBoxPerCell; ++b)
                {
                    int channel = b * (num_classes + 5);

                    float tc = feature.data[cx + (cy * feature.width) + (channel + 4) * feature.height * feature.width];

                    float confidence = Sigmoid(tc);

                    if (confidence >= conf_threshold)
                    {

                        float tx = feature.data[cx + (cy * feature.width) + channel * feature.height * feature.width];
                        float ty = feature.data[cx + (cy * feature.width) + (channel + 1) * feature.height * feature.width];
                        float tw = feature.data[cx + (cy * feature.width) + (channel + 2) * feature.height * feature.width];
                        float th = feature.data[cx + (cy * feature.width) + (channel + 3) * feature.height * feature.width];
                        float tc = feature.data[cx + (cy * feature.width) + (channel + 4) * feature.height * feature.width];

                        float x = (cx + Sigmoid(tx)) / cell_w;
                        float y = (cy + Sigmoid(ty)) / cell_h;
                        float w = exp(tw) * anchors[b].width / cell_w;
                        float h = exp(th) * anchors[b].height / cell_h;
                        // CV_LOG_INFO(NULL, "cx:" << cx << " cy:" << cy);
                        // CV_LOG_INFO(NULL, "box:" << x << " " << y << " " << w << " " << h << " c:" << confidence
                        //                          << " index:" << index);
                        std::vector<float> classes(num_classes);
                        for (int i = 0; i < num_classes; ++i)
                        {
                            classes[i] = feature.data[cx + (cy * feature.width) + (channel + 5 + i) * feature.height * feature.width];
                        }
                        Softmax(classes);
                        auto max_itr = std::max_element(classes.begin(), classes.end());
                        int index = static_cast<int>(max_itr - classes.begin());
                        if (num_classes > 1)
                        {
                            confidence = confidence * classes[index];
                        }

                        int center_x = (int)(x * img_width);
                        int center_y = (int)(y * img_height);
                        int width = (int)(w * img_width);
                        int height = (int)(h * img_height);
                        int left = static_cast<int>(center_x - width / 2);
                        int top = static_cast<int>(center_y - height / 2);
                        // CV_LOG_INFO(NULL,
                        //             "box:" << left << " " << top << " " << width << " " << height << " c:" << confidence
                        //                    << " index:" << index
                        //                    << " p:" << classes[index]);

                        ids.push_back(index);
                        boxes.emplace_back(left, top, width, height);
                        confidences.push_back(confidence);
                    }
                }
            }
        }
    };

    inline cv::Rect2d RemapBoxOnSrc(const cv::Rect2d &box, const int img_width, const int img_height)
    {
        float xmin = static_cast<float>(box.x);
        float ymin = static_cast<float>(box.y);
        float xmax = xmin + static_cast<float>(box.width);
        float ymax = ymin + static_cast<float>(box.height);
        cv::Rect2d remap_box;
        remap_box.x = std::max(.0f, xmin);
        remap_box.width = std::min(img_width - 1.0f, xmax) - remap_box.x;
        remap_box.y = std::max(.0f, ymin);
        remap_box.height = std::min(img_height - 1.0f, ymax) - remap_box.y;
        return remap_box;
    };

#ifdef __cplusplus
}
#endif
#endif
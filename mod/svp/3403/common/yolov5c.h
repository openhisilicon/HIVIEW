#ifndef __YOLOV5C_H
#define __YOLOV5C_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <math.h>
#include <iostream>

#ifndef _BoxInfo
#define _BoxInfo
typedef struct BoxInfo
{
	float x1; 
	float y1; 
	float x2; 
	float y2;
	float score;
	int label;
} BoxInfo;
#endif

class YOLOV5C
{
public:
	int init(char* model_path);
	int detect(cv::Mat &image, std::vector<BoxInfo> &bboxs, float confThr=0.5, float nmsThr=0.4);
	int destroy();
};


#endif

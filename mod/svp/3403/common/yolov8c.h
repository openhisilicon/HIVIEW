#ifndef __YOLOV8C_H
#define __YOLOV8C_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <math.h>
#include <iostream>

#ifndef _Object
#define _Object
typedef struct Object
{
	float x1; 
	float y1; 
	float x2; 
	float y2;
	int label;
	float score;
} Object;
#endif

class YOLOV8C
{
public:
	int init(char* model_path);
	int detect(cv::Mat &image, std::vector<Object> &bboxs, float confThr=0.3, float nmsThr=0.45);
	int destroy();
};


#endif

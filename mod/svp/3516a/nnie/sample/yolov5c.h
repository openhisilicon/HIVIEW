#ifndef __YOLOV5C_H
#define __YOLOV5C_H

struct BoxInfo
{
	int label;
	float score;
	cv::Rect2f box;
};

class YOLOV5C
{
public:
	int init(char* model_path);
	int detect(cv::Mat &image, std::vector<BoxInfo> &bboxs, float threshold=0.5);
	int destroy();
};


#endif

#ifndef INS_NNIE_C_INTERFACE_H
#define INS_NNIE_C_INTERFACE_H

#ifdef __cplusplus
	#define NNIE_C_API extern "C"
	#define API_CALL
#else
	#define NNIE_C_API
	#define API_CALL
#endif

/************************************************************************/
/*
	* 作用：NNIE消息通知函数接口指针
	* 参数：
	*   nnie_id:		    nnie 句柄
	*	run_id		        可作为image 输入的id 可以递增
	*   cls_id:			    识别类ID
	*   confidence			识别类
	*   xmin				x
    *   ymin				y
    *   xmax				x
    *   ymax				y
	* 返回值：无
	*/
/************************************************************************/
typedef void (*CNNIE_RESULT_CB)(int nnie_id, int run_id, int cls_id, float confidence, float xmin, float ymin, float xmax, float ymax);




//坐标区域
typedef struct tag_YOLOV3_RECT
{
	int m_iLeft;        //左边
	int m_iTop;         //上边
	int m_iRight;       //右边
	int m_iBottom;      //下边
}YOLOV3_RECT,*pYOLOV3_RECT;

//识别结果
typedef struct tag_YOLOV3_Result
{
	int 		 m_class;						//类别
	float 		 m_confidence;					//置信度
	YOLOV3_RECT  m_rect ;  					    //车牌坐标

}YOLOV3_Result, *pYOLOV3_Result;

//初始化 返回一个ID
NNIE_C_API int CNNIE_init(const char *model_path, const int image_height, const int image_width,CNNIE_RESULT_CB call_back);

NNIE_C_API YOLOV3_Result* CNNIE_run_with_file(int nnie_id, int run_id, const char *image_path, const int img_height, const int img_width, int classes, int kBoxPerCell, float conf_threshold, float nms_threshold,int *result_count);

NNIE_C_API YOLOV3_Result* CNNIE_run_with_data_yuv(int nnie_id, int run_id, const unsigned char *data, const int img_height, const int img_width, int classes, int kBoxPerCell, float conf_threshold, float nms_threshold,int *result_count);

NNIE_C_API YOLOV3_Result* CNNIE_run_with_data_bgr(int nnie_id, int run_id, const unsigned char *data, const int img_height, const int img_width, int classes, int kBoxPerCell, float conf_threshold, float nms_threshold,int *result_count);

NNIE_C_API void CNNIE_free(YOLOV3_Result *pYOLOV3Result);

NNIE_C_API void CNNIE_finish(int nnie_id);

#endif //INS_NNIE_C_INTERFACE_H
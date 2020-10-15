#ifndef __LPR_H
#define __LPR_H

#ifdef __cplusplus
extern "C" {
#endif
#ifdef _WIN32
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#else
#include <strings.h>
#endif

#ifdef _WIN32  
#define DLL_DECLARE __declspec(dllexport)  
#else  
#define DLL_DECLARE  
#endif 
#define MAX_PLATE_NUM 6
#define LPR_PLATE_NUM_LEN     16
#define LPR_COLOR_LEN         8

//图像格式
#define LPR_BGR               0
#define LPR_YUV420P           1
#define LPR_YUV422P           2
#define LPR_UYVY422           3
#define LPR_YUV420SP          4


//识别方式
#define   LPR_NORMAL          0
#define   LPR_FAST            1

//坐标区域
typedef struct tag_LPR_RECT
{
	int m_iLeft;        //左边
	int m_iTop;         //上边
	int m_iRight;       //右边
	int m_iBottom;      //下边
}LPR_RECT,*pLPR_RECT;

//识别结果
typedef struct tag_LPR_Result
{
	char 	  m_cPlatenum[LPR_PLATE_NUM_LEN];		//车牌号码
	char 	  m_cPlatecolor[LPR_COLOR_LEN];  		//车牌颜色
	float     m_fPlaterealty;   					//车牌置信度
	LPR_RECT  m_fPlaterect ;  					    //车牌坐标
	int       m_iPlatetype ;                       //车牌类型
	float 	  m_fLetterrealty[LPR_PLATE_NUM_LEN];	//字符置信度
	float     m_fVertangle;                        //垂直角度
	float     m_fHorzangle;                        //水平角度
	//int       m_iPlatecolor;                        //0---"黑",1--- "蓝",2--- "黄3----", "白", 4-----"绿"
}LPR_Result, *pLPR_Result;

typedef void (*LprEventCallBack)(LPR_Result *pPlateRet,int iPlateNum,void *pData);




/*-------------------------------------------------------------------
函数名：liblpr_Init SDK初始化
参数名：nMaxWidth: 输入图像最大允许宽度
		        nMaxHeight: 输入图像最大允许高度
		        pPath -----key目录路径(可读写)
                    返回值: NULL-----失败非NULL ---成功
 描　述: 只有在此接口调用成功后才能调用识别接口
-------------------------------------------------------------------*/

#if defined(ANDROID) || defined(__ANDROID__)
DLL_DECLARE void *  liblpr_Init(int nMaxWidth, int nMaxHeight);
#else
DLL_DECLARE void *  liblpr_Init(int nMaxWidth, int nMaxHeight,char *pPath);
#endif


/*-------------------------------------------------------------------
函数名:liblpr_Recognition
参数名：
                    LPR_Handle:LPR_Init函数的返回值
    			RecogType: 输入识别方式 LPR_NORMAL---普通LPR_FAST---快速
    			pImage: 输入图像数据
    			ImageFormat:  输入的图像格式具体参看前面的宏定义
    			                     LPR_BGR-----RGB888
    			                     LPR_YUV420P-----YUV420P
    			                     LPR_UYVY422-----YUV422
    			                     LPR_YUV420SP----YUV420SP
    			nWidth: 输入图像宽度(以像素为单位)
    			nHeight: 输入图像高度(以像素为单位)
    			rcRange: 输入识别范围(以像素为单位)要求如下
    			             0<rcRange.m_iLeft<rcRange.m_iRight<nWidth
    			             0<rcRange.m_iTop<rcRange.m_Bottom<nHeight
    			             
    			pPlateResult: 输出识别结果内存由调用者申请
    			pPlateNum: 输入设定识别车牌个数 输出实际识别车牌个数
返回值: 大于0：成功 小于等于0：失败
描　述: 车牌识别接口
//-------------------------------------------------------------------*/
DLL_DECLARE int liblpr_Recognition(void *  LPR_Handle, int RecogType, unsigned char *pImage, int ImageFormat, int nWidth, int nHeight, LPR_RECT rcRange, LPR_Result *pPlateResult, int *pPlateNum);

/*-------------------------------------------------------------------
函数名：LPR_UnInit
参数名：LPR_Handle :LPR_Init函数的返回值
返回值: 无
描　述:车牌识别库的反初始化接口
-------------------------------------------------------------------*/
DLL_DECLARE void  liblpr_UnInit(void * LPR_Handle);



DLL_DECLARE int  liblpr_GetDevID(char * pDevID);




#ifdef __cplusplus
}
#endif

#endif 

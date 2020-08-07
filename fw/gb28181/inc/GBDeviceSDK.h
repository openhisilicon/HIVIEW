#ifndef __GBSERVERSDK_H__
#define __GBSERVERSDK_H__
#include "GBCommon.h"


#ifdef WIN32 // windows
#define strcasecmp  stricmp
#define strncasecmp strnicmp


#ifdef GB_SDK_EXPORTS
#define GB_SDK_API extern "C" __declspec(dllexport)
#else
#define GB_SDK_API extern "C" __declspec(dllimport)
#endif
#define API_CALL __stdcall

#else // linux

#define GB_SDK_API  extern "C"
#define API_CALL
#endif

//////////////////////////////////////////////////////////////////////////
//接口定义
/********************************接口函数声明*********************************/
/************************************************************************/
/*
* 作用：GB28181模块初始化
* 参数：
*	pSipServerInfo：		SIP服务器相关参数
*	pGb28181ChannelID:		视频通道ID数组
*	nChannelNum:			视频通道数量
*	pGb28181AlarmID:		报警通道数组
*	nAlarmNum:				报警通道数量
*	pNotifyCB:				消息函数通知指针
* 返回值：返回GB28181操作结果，状态在“GB_ERROR”中定义
* 备注：无
*/
/************************************************************************/
GB_SDK_API GB_ERROR API_CALL GB_SDK_Init(
            const GB_SIP_SERVER* pSipServerInfo,
            const GB_CHANNEL_ITEM* pGb28181ChannelID,
            int nChannelNum,
            const GB_CHANNEL_ITEM* pGb28181AlarmID,
            int nAlarmNum,
            GB_NOTIFY_CB pfnNotifyCB,
            GB_STREAM_CB pfnStreamCB, 
            void* pUser);

/************************************************************************/
/*
* 作用：GB28181模块启用
* 参数：
*	无
* 返回值：返回GB28181操作结果，状态在“GB_ERROR”中定义
* 备注：无
*/
/************************************************************************/
GB_SDK_API GB_ERROR API_CALL GB_SDK_Start();

/************************************************************************/
/*
* 作用：GB28181模块停止
* 参数：
*	无
* 返回值：返回GB28181操作结果，状态在“GB_ERROR”中定义
* 备注：无
*/
/************************************************************************/
GB_SDK_API GB_ERROR API_CALL GB_SDK_Stop();

/************************************************************************/
/*
* 作用：GB28181模块反初始化
* 参数：
*	无
* 返回值：返回GB28181操作结果，状态在“GB_ERROR”中定义
* 备注：无
*/
/************************************************************************/
GB_SDK_API GB_ERROR API_CALL GB_SDK_Quit();

/************************************************************************/
/*
* 作用：获GB28181模块注册状态
* 参数：
*	无
* 返回值：返回GB28181当前状态，状态在“GB_ERROR”中定义
* 备注：无
*/
/************************************************************************/
GB_SDK_API GB_STATUS API_CALL GB_SDK_GetStatus();

/************************************************************************/
/*
* 作用：根据错误码得到错误描述
* 参数：
*	eError                 错误码，在“GB_ERROR”中定义
* 返回值：返回错误描述
* 备注：无
*/
/************************************************************************/
GB_SDK_API const char* API_CALL GB_SDK_Error2String(GB_ERROR eError);

/************************************************************************/
/*
* 作用：向模块插入实时流媒体帧数据
* 参数：
*	lTaskID				    任务GUID（该值由LivePlayStart()中返回）
*	pFrameHead：			媒体帧头部
*	pData：					媒体数据
*	bPlayback:				是否回放流
* 返回值：返回GB28181操作结果，状态在“GB_ERROR”中定义
* 备注：无
*/
/************************************************************************/
GB_SDK_API GB_ERROR API_CALL GB_SDK_InputData(unsigned long lTaskID, const GBFrameInfo* pFrameHead, const char* pData, bool bPlayback);

/************************************************************************/
/*
* 作用：向模块插入对讲帧数据
* 参数：
*	pTaskGUID：				任务GUID（该值由TalkbackStart()中返回）
*	pFrameHead：			媒体帧头部
*	pData：					媒体数据
* 返回值：返回GB28181操作结果，状态在“GB_ERROR”中定义
* 备注：无
*/
/************************************************************************/
GB_SDK_API GB_ERROR API_CALL GB_SDK_InputTalkbackData(unsigned long lTaskID, const GBFrameInfo* pFrameHead, const char* pData);

/************************************************************************/
/*
* 作用：更新报警状态
* 参数：
*	nAlarmID：				报警通道ID（0，1，2，3...）
*	uAlarmType：			报警类型
*	uAlarmStatus：			报警状态
* 返回值：返回GB28181操作结果，状态在“GB_ERROR”中定义
* 备注：无
*/
/************************************************************************/
GB_SDK_API GB_ERROR API_CALL GB_SDK_UpdateAlarmStatus(int nAlarmID, unsigned short uAlarmType, unsigned short uAlarmStatus);

/************************************************************************/
/*
* 作用：启用/禁用GPS信息上报
* 参数：
*	bEnable：				是否启用
*	uTime：					上报时间间隔，单位为毫秒
* 返回值：返回GB28181操作结果，状态在“GB_ERROR”中定义
* 备注：GB28181库内部默认是不启用
*/
/************************************************************************/
GB_SDK_API GB_ERROR API_CALL GB_SDK_SetGpsInterval(bool bEnable, unsigned int uTime);

/************************************************************************/
/*
* 作用：更新GPS信息
* 参数：
*	pGPSInfo			GPS信息
* 返回值：返回GB28181操作结果，状态在“GB_ERROR”中定义
* 备注：无
*/
/************************************************************************/
GB_SDK_API GB_ERROR API_CALL GB_SDK_SetGpsInfo(const GPS_INFO *pGPSInfo);

/************************************************************************/
/*
* 作用：立即上报GPS信息
* 参数：无
* 返回值：返回GB28181操作结果，状态在“GB_ERROR”中定义
* 备注：无
*/
/************************************************************************/
GB_SDK_API GB_ERROR API_CALL GB_SDK_ReportGpsImmediate();


#endif // __GBSERVERSDK_H__



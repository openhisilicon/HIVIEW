#ifndef __GB28181_COMMON_DEFINE__H__
#define __GB28181_COMMON_DEFINE__H__


#pragma pack( 4)

#define GB_SHORT_LEN	32
#define GB_LONG_LEN	64

const unsigned int GB_MAX_CAMERA_NUM		= 64;

//无效索引
#define INVALID_INDEX(type) (static_cast<type>(~(type)0x0))

enum  GB_ERROR
{
	GB_ERR_SUCCESS,						// success
	GB_ERR_FAILD,						// faild
	GB_ERR_PERMISSION_DENIED,			// permission denied
	GB_ERR_UNAUTHORIZED,   				// unauthorized
	GB_ERR_INVALID_PARAMETER,      		// invalid paramter
	GB_ERR_SYSTEM_BUSY,   				// system busy
	GB_ERR_PENDING,						// pending
	GB_ERR_NETWORK_ERROR,               // network error
	GB_ERR_NOT_EXIST,					// not exist
	GB_ERR_BUFFER_FULL,					// buffer full
	GB_ERR_IGNORE,                      // should ignore
	GB_ERR_NOT_SUPPORTED,               // not supported
	GB_ERR_EXPIRED,                     // expired
    GB_ERR_UNKNOWN_ERROR,				// unknown error
};

typedef enum _sip_proto
{
	PROTOCOL_UDP      = 0, // UDP
	PROTOCOL_TCP      = 1, // TCP
	PROTOCOL_TLS      = 2, // TLS
	PROTOCOL_DTLS     = 3, // DTLS
}SIP_PROTO;

typedef struct _gb_device_init_info
{
    int             nLocalVideoInputNum;
    int             nNetVideoInputNum;
    int             nAudioInputNum;
    int             nSensorInputNum;
    int             nMaxALarm;
    int             nMaxAudio;
    int             nDeviceType;

    int             nProductType;
    int             nDevVer;
    unsigned int    nSoftVer;
    unsigned int    nBuildTime;
    char            szDeviceName [20];
}GB_DEV_INIT_INFO;

typedef enum _gb_status
{
	STATUS_UNREGISTER		= 0,		//no register
	STATUS_REGISTER			= 1,		//registered
}GB_STATUS;

typedef struct _gb_sip_server
{
    char				server_ip[32];			    //上级SIP服务器IP
    unsigned short		server_port;				//上级SIP服务器端口
	SIP_PROTO           sip_protocol;               //SIP传输协议（在“SIP_PROTO”中定义）
    unsigned int		expires;					//过期时间
    char				server_id[GB_SHORT_LEN];	//服务器id
    char				user[GB_SHORT_LEN];		    //注册用户名
    char				passwd[GB_SHORT_LEN];		//注册密码
    char				dvr_id[GB_SHORT_LEN];		//设备ID
    unsigned short		dvr_port;					//设备端口
	unsigned int        keepaliveInterval;          //keepalive时间间隔（单位为秒，默认60秒）
    char				devowner[GB_LONG_LEN];	    //设备归属
    char				civilcode[GB_LONG_LEN];	    //行政区域
    char				installaddr[GB_LONG_LEN];	//安装地址
	unsigned int		defaultStreamType;		    //默认码流类型（0：主码流， 1：子码流）
}GB_SIP_SERVER;

typedef struct _gb_channel_item
{
	int					nChannelID;  //视频通道或者报警通道索引(0,1,2,3...)
	char				gb28181ID[GB_SHORT_LEN];
}GB_CHANNEL_ITEM;

typedef struct _gb_frame_info
{
	int					nChannelID;		//视频通道索引(0,1,2,3...)
	unsigned short		streamType;		//码流类型（在"STREAM_TYPE"中定义）
	unsigned char		byFrameType;	//帧类型（在“FRAME_TYPE”中定义）
	unsigned char		keyFrame;		//0 非关键帧  1：关键帧
	unsigned int		frameIndex;		//帧索引
	unsigned int		width;			//视频宽度（音频帧为0）
	unsigned int		height;			//视频高度（音频帧为0）
	unsigned int		length;			//帧数据长度（不包括帧头）
	long long			time;			//帧时间戳（微秒）
}GBFrameInfo;
#define GBFRAME_LEN		sizeof(GBFrameInfo)

typedef enum _frame_type
{
    FRAME_TYPE_NONE				= 0x00,
    FRAME_TYPE_VIDEO			= 0x01,
    FRAME_TYPE_AUDIO			= 0x02,
    FRAME_TYPE_TALK_AUDIO		= 0x03,
    FRAME_TYPE_END				= 0x04,
}FRAME_TYPE;

typedef struct _frame_info_ex
{
    unsigned long		keyFrame;	//0 不是关键帧

    unsigned long		frameType;
    unsigned long		length;
    unsigned long       width;
    unsigned long       height;

    unsigned char      *pData;

    unsigned long       channel;
    unsigned long		frameIndex;//帧索引,快进模式时，2表示2倍速，4表示4倍速
    unsigned long		streamID;
    long long			time;	   //在帧绝对时间，年月日时分秒浩渺，在改变设备时间时会变化
}FRAME_INFO_EX;	//用于本地使用

typedef struct _ptz_ctrl
{
	unsigned long	chnn;			//哪个通道
	unsigned long	speed;			//控制速度
	unsigned long	cmdType;		//命令类型
	unsigned long	cmdValue;		//命令参数：预置点序号，巡航线序号，轨迹序号等
}PTZ_CTRL;

enum STREAM_TYPE
{
	STREAM_NULL			= -1,
	STREAM_VIDEO	    = 0, // 实时视频
	STREAM_AUDIO,
	STREAM_PLAYBACK,
	STREAM_TALKBACK,

	STREAM_END,	
};


typedef enum _replay_method
{
	METHOD_PLAY		= 0, //播放
	METHOD_PAUSE	= 1, //暂停
	METHOD_SEEK		= 2, //SEEK
}REPLAY_METHOD;
//GPS信息
typedef struct _gps_info
{
	float			fLongitude;			//经度
	float			fLatitude;			//纬度
	float			fSpeed;				//速度（km/h   可选）
	float			fDirection;			//方向（0-360）
	int				nAltiude;			//海拔（单位m）

}GPS_INFO;

// Cmd type
#define NOTIFY_Play				"Play"
#define NOTIFY_Stop				"Stop"
#define NOTIFY_Playback			"Playback"
#define NOTIFY_PlaybackStop		"PlaybackStop"
#define NOTIFY_ManualRecStart	"ManualRecStart"
#define NOTIFY_ManualRecStop	"ManualRecStop"
#define NOTIFY_SetGuard	        "SetGuard"
#define NOTIFY_ResetGuard	    "ResetGuard"
#define NOTIFY_PtzCtl           "PtzCtl"
#define NOTIFY_RecordQuery      "RecordQuery"
#define NOTIFY_Event            "Event"
#define NOTIFY_TalkbackStart    "TalkbackStart"
#define NOTIFY_TalkbackStop     "TalkbackStop"
#define NOTIFY_BroadcastStart   "BroadcastStart"
#define NOTIFY_BroadcastStop    "BroadcastStop"


typedef enum _event_sub_type
{
	EVENT_TYPE_UNKNOWN    = 0,
	EVENT_STREAM_END	  = 1, // stream task end
	EVENT_STREAM_SEEK     = 2, // seek
}EVENT_SUB_TYPE;

/************************************************************************/
	/*
	* 作用：SDK消息通知函数接口指针
	* 参数：
	*   pNotifyType:		notify type
	*	pNotifyXml：		SDK通知外部的数据XML
	*   pRespXml:			外部返回SDK的数据XML
	*   nRespSize：			pRespXml缓冲区长度
	*   pUser：				用户定义数据
	* 返回值：在“GB28181_RET”中定义
	* 备注：详细XML格式在“GB28181_SDK开发手册V1.0.0.1.docx”中定义
	*/
/************************************************************************/
typedef GB_ERROR (*GB_NOTIFY_CB) (const char* pNotifyType,const char* pNotifyXml, char* pRespXml, int nRespSize, void* pUser);

typedef int (*GB_STREAM_CB) (unsigned int nTaskID, const GBFrameInfo *pFrameHead, const char *pData, void* pUser);

#pragma pack()

#endif

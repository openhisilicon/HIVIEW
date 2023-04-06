#ifndef __mpp_h__
#define __mpp_h__

#include "sample_comm.h"

//cfg;
typedef struct {
  char  snsname[32];  // sensor imx335
  int   snscnt;       // sensor count
  int   lane;         // lane mode
  int   wdr;          // wdr mode
  int   res;          // resolution
  int   fps;          // frame rate
  int   slave;        // slave mode
  int   second;       // second channel;
  char  type[32];     // cpu type;
}gsf_mpp_cfg_t;

int gsf_mpp_cfg(char *path, gsf_mpp_cfg_t *cfg);

//vi;
typedef struct {
  HI_BOOL bLowDelay;
  HI_U32 u32SupplementConfig;
  VI_STITCH_GRP_ATTR_S stStitchGrpAttr; // 3516DV300 no-support 
}gsf_mpp_vi_t;

//SAMPLE_COMM_VI_StartVi
int gsf_mpp_vi_start(gsf_mpp_vi_t *vi);
int gsf_mpp_vi_stop();
//HI_S32 HI_MPI_VI_GetPipeFrame(VI_PIPE ViPipe, VIDEO_FRAME_INFO_S *pstVideoFrame, HI_S32 s32MilliSec);
//HI_S32 HI_MPI_VI_GetChnFrame(VI_PIPE ViPipe, VI_CHN ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo, HI_S32 s32MilliSec);
int gsf_mpp_vi_get(int ViPipe, int ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo, int s32MilliSec);


typedef struct {
  void *uargs;
  int (*cb)(HI_U32 Fv1, HI_U32 Fv2, HI_U32 Gain, void* uargs);
}gsf_mpp_af_t;

int gsf_mpp_af_start(gsf_mpp_af_t *af);
int af_get_vd(void);
int af_get_value(HI_U32 FV[2]);

//vpss;
typedef struct {
  VPSS_GRP    VpssGrp;
  
  MOD_ID_E  srcModId;
  union{
    HI_S32  ViPipe;
  };
  union{
    VI_CHN  ViChn;
  };

  HI_BOOL     enable[VPSS_MAX_PHY_CHN_NUM];
  PIC_SIZE_E  enSize[VPSS_MAX_PHY_CHN_NUM];
}gsf_mpp_vpss_t;

//SAMPLE_COMM_VI_Bind_VPSS
int gsf_mpp_vpss_start(gsf_mpp_vpss_t *vpss);
int gsf_mpp_vpss_stop(gsf_mpp_vpss_t *vpss);
//HI_S32 HI_MPI_VPSS_SendFrame VPSS_GRP VpssGrp, VPSS_GRP_PIPE VpssGrpPipe, const VIDEO_FRAME_INFO_S *pstVideoFrame , HI_S32 s32MilliSec);
int gsf_mpp_vpss_send(int VpssGrp, int VpssGrpPipe, VIDEO_FRAME_INFO_S *pstVideoFrame , int s32MilliSec);


enum {
  GSF_MPP_VPSS_CTL_PAUSE = 0, //HI_MPI_VPSS_StartGrp(VpssGrp);
  GSF_MPP_VPSS_CTL_RESUM = 1, //HI_MPI_VPSS_StopGrp(VpssGrp);
  GSF_MPP_VPSS_CTL_CROP  = 2, //HI_MPI_VPSS_SetGrpCrop(VpssGrp, VPSS_CROP_INFO_S *args);
  GSF_MPP_VPSS_CTL_ASPECT = 3, //HI_MPI_VPSS_SetChnAttr(VpssGrp, ASPECT_RATIO_S *args);
  GSF_MPP_VPCH_CTL_ENABLE = 4, //HI_MPI_VPSS_EnableChn(VpssGrp, VpssChn *args);
  GSF_MPP_VPCH_CTL_DISABLE= 5, //HI_MPI_VPSS_DisableChn(VpssGrp, VpssChn *args);
  GSF_MPP_VPSS_CTL_ATTR   = 6, //HI_MPI_VPSS_GetGrpAttr(VpssGrp, VPSS_GRP_ATTR_S *args);
};

int gsf_mpp_vpss_ctl(int VpssGrp, int id, void *args);


//venc;
typedef struct {
  
  VENC_CHN  VencChn;
  
  MOD_ID_E  srcModId;
  union {
    VPSS_GRP  VpssGrp;
    AVS_GRP   AVSGrp;
    VI_PIPE   ViPipe;
    VO_LAYER  VoLayer;
  };
  union {
    VPSS_CHN  VpssChn; // VPSS
    AVS_CHN   AVSChn;  // AVS
    VDEC_CHN  VdChn;   // VDEC
    VI_CHN    ViChn;   // VI
    VO_CHN    VoChn;   // VO
  };
  
  PAYLOAD_TYPE_E  enPayLoad;
  PIC_SIZE_E      enSize;
  HI_S32          enRcMode;
  HI_U32          u32Profile;
  HI_BOOL         bRcnRefShareBuf;
  VENC_GOP_MODE_E enGopMode;
  HI_U32          u32FrameRate;
  HI_U32          u32Gop;
  HI_U32          u32BitRate;
  HI_U32          u32LowDelay;
}gsf_mpp_venc_t;

//SAMPLE_COMM_VPSS_Bind_VENC
int gsf_mpp_venc_start(gsf_mpp_venc_t *venc);
int gsf_mpp_venc_stop(gsf_mpp_venc_t *venc);

typedef struct {
  int(*cb)(VENC_STREAM_S* pstStream, void* u);
  void* u;
}gsf_mpp_venc_get_t;
//HI_MPI_VENC_SendFrame(VENC_CHN VeChn, const VIDEO_FRAME_INFO_S *pstFrame, HI_S32 s32MilliSec);
int gsf_mpp_venc_send(int VeChn, VIDEO_FRAME_INFO_S *pstFrame, int s32MilliSec, gsf_mpp_venc_get_t *get);

typedef struct {
  HI_S32 s32Cnt;
  VENC_CHN VeChn[VENC_MAX_CHN_NUM];
  void *uargs;
  int (*cb)(VENC_CHN VeChn, PAYLOAD_TYPE_E PT, VENC_STREAM_S* pstStream, void* uargs);
}gsf_mpp_recv_t;

//SAMPLE_COMM_VENC_StartGetStreamCb
int gsf_mpp_venc_recv(gsf_mpp_recv_t *recv);
int gsf_mpp_venc_dest();
int gsf_mpp_venc_snap(VENC_CHN VencChn, HI_U32 SnapCnt, int(*cb)(int i, VENC_STREAM_S* pstStream, void* u), void* u);


//isp;
int gsf_mpp_scene_start(char *path, int scenemode);
int gsf_mpp_scene_stop();

enum {
  GSF_MPP_SCENE_CTL_AE = 0, // gsf_scene_ae_t==HI_SCENE_CTL_AE_S
};
int gsf_mpp_scene_ctl(int ViPipe, int id, void *args);

enum {
  GSF_MPP_VENC_CTL_IDR = 0,
  GSF_MPP_VENC_CTL_RST = 1,
};
int gsf_mpp_venc_ctl(int VencChn, int id, void *args);

typedef struct {
  void *uargs;
  int (*cds)(int ViPipe, void* uargs);
  int (*cb)(int ViPipe, int night, void* uargs);
}gsf_mpp_ir_t;

typedef struct {
  int bEnable;
  int u8Hue;
  int u8Luma;
  int u8Contr;
  int u8Satu;
}gsf_mpp_img_csc_t;

typedef struct {
  int  bEnable;
  int  u8Speed;
  int  u8Compensation;
  int  SysGainRangeMax;
  int  SysGainRangeMin;
  int  ExpTimeRangeMax;
  int  ExpTimeRangeMin;
}gsf_mpp_img_ae_t;

typedef struct {
  int  bEnable;
  int  u8strength;
}gsf_mpp_img_dehaze_t;

typedef struct {
  int bEnable;
  int u16TextureFreq;
  int u16EdgeFreq;
  int u8DetailCtrl;
}gsf_mpp_img_sharpen_t;

typedef struct {
  int bEnable;
  int u8LumaThr;
  int u8LumaTarget;
}gsf_mpp_img_hlc_t;

typedef struct {
  int bEnable;
  int enCurveType;
  int TableNo;
}gsf_mpp_img_gamma_t;

typedef struct {
  int bEnable;
  int u16Strength;
  int u16StrengthMax;
  int u16StrengthMin;
}gsf_mpp_img_drc_t;

typedef struct {
  int bEnable;
  int u16BlcCtrl;
  int stHePosWgt_u8Wgt;
  int stHeNegWgt_u8Mean;
}gsf_mpp_img_ldci_t;

typedef struct {
  int bEnable;
  int u8strength;
}gsf_mpp_img_3dnr_t;

typedef struct {
  int bEnable;
}gsf_mpp_img_scene_t;

// gsf_mpp_img_all_t == gsf_img_all_t;
typedef struct {
  int magic;
  gsf_mpp_img_scene_t scene;
  gsf_mpp_img_csc_t   csc;
  gsf_mpp_img_ae_t    ae;
  gsf_mpp_img_dehaze_t dehaze;
  gsf_mpp_img_sharpen_t sharpen;
  gsf_mpp_img_hlc_t   hlc;
  gsf_mpp_img_gamma_t gamma;
  gsf_mpp_img_drc_t   drc;
  gsf_mpp_img_ldci_t  ldci;
  gsf_mpp_img_3dnr_t  _3dnr;
}gsf_mpp_img_all_t;

enum {
  GSF_MPP_ISP_CTL_IR  = 0,    // 0: Day, 1: Night, x: gsf_mpp_ir_t
  GSF_MPP_ISP_CTL_IMG = 1,    // get gsf_mpp_img_all_t;
  GSF_MPP_ISP_CTL_CSC = 2,    // set gsf_mpp_img_csc_t;
  GSF_MPP_ISP_CTL_AE  = 3,    // set gsf_mpp_img_ae_t;
  GSF_MPP_ISP_CTL_DEHAZE = 4, // set gsf_mpp_img_dehaze_t;
  GSF_MPP_ISP_CTL_SCENE  = 5, // set gsf_mpp_img_scene_t;
  GSF_MPP_ISP_CTL_SHARPEN= 6, // set gsf_mpp_img_sharpen_t;
  GSF_MPP_ISP_CTL_HLC    = 7, // set gsf_mpp_img_hlc_t;
  GSF_MPP_ISP_CTL_GAMMA  = 8, // set gsf_mpp_img_gamma_t;
  GSF_MPP_ISP_CTL_DRC    = 9, // set gsf_mpp_img_drc_t;
  GSF_MPP_ISP_CTL_LDCI   = 10,// set gsf_mpp_img_ldci_t;  
  GSF_MPP_ISP_CTL_3DNR   = 11,// set gsf_mpp_img_3dnr_t;
};
int gsf_mpp_isp_ctl(int ViPipe, int id, void *args);

typedef struct {
  int AeChn;
  PAYLOAD_TYPE_E enPayLoad; // PT;
  int adtype; // 0:INNER, 1:I2S, 2:PCM;
  int stereo, sp, br;//stereo, sampleRate, bitRate;
  void *uargs;
  int (*cb)(int AeChn, PAYLOAD_TYPE_E PT, AUDIO_STREAM_S* pstStream, void* uargs);
}gsf_mpp_aenc_t;

int gsf_mpp_audio_start(gsf_mpp_aenc_t *aenc);
int gsf_mpp_audio_stop(gsf_mpp_aenc_t  *aenc);


enum {
  GSF_MPP_RGN_CREATE      = 0, // HI_MPI_RGN_Create
  GSF_MPP_RGN_DESTROY     = 1, // HI_MPI_RGN_Destroy
  GSF_MPP_RGN_SETATTR     = 2, // HI_MPI_RGN_SetAttr
  GSF_MPP_RGN_GETATTR     = 3, // HI_MPI_RGN_GetAttr
  GSF_MPP_RGN_ATTACH      = 4, // HI_MPI_RGN_AttachToChn
  GSF_MPP_RGN_DETACH      = 5, // HI_MPI_RGN_DetachFromChn
  GSF_MPP_RGN_SETDISPLAY  = 6, // HI_MPI_RGN_SetDisplayAttr
  GSF_MPP_RGN_GETDISPLAY  = 7, // HI_MPI_RGN_GetDisplayAttr
};
typedef struct {
    // create;
    RGN_ATTR_S stRegion;
    // attach;
    MPP_CHN_S   stChn;
    // modify;
    RGN_CHN_ATTR_S stChnAttr;
}gsf_mpp_rgn_t;

int gsf_mpp_rgn_ctl(RGN_HANDLE Handle, int id, gsf_mpp_rgn_t *rgn);
int gsf_mpp_rgn_bitmap(RGN_HANDLE Handle, BITMAP_S *bitmap);

int gsf_mpp_rgn_canvas_get(RGN_HANDLE Handle, RGN_CANVAS_INFO_S *pstRgnCanvasInfo);
int gsf_mpp_rgn_canvas_update(RGN_HANDLE Handle);



//vodev;
enum {
  VODEV_HD0 = 0,  // UHD视频设备
  VODEV_HD1 = 1,  // HD视频设备
};

//启动视频输出设备;
int gsf_mpp_vo_start(int vodev, VO_INTF_TYPE_E type, VO_INTF_SYNC_E sync, int wbc);

//停止视频输出设备
int gsf_mpp_vo_stop(int vodev);

//vofb;
enum {
  VOFB_GUI   = 0, // GUI Layer
  VOFB_GUI1  = 1, // GUI1 Layer
  VOFB_MOUSE = 2, // MOUSE Layer
  VOFB_BUTT
};
// FB层操作
int gsf_mpp_fb_start(int vofb, VO_INTF_SYNC_E sync, int hide);


//volayer;
enum {
  VOLAYER_HD0 = 0,  // 视频层0
  VOLAYER_HD1 = 1,  // 视频层1
  VOLAYER_PIP = 2,  // PIP层
  VOLAYER_BUTT
};

// 显示通道布局
typedef enum {
  VO_LAYOUT_NONE  = 0, VO_LAYOUT_10MUX = 10,
  VO_LAYOUT_1MUX  = 1, VO_LAYOUT_12MUX = 12,
  VO_LAYOUT_2MUX  = 2,
  VO_LAYOUT_4MUX  = 4, VO_LAYOUT_16MUX = 16,
  VO_LAYOUT_6MUX  = 6, VO_LAYOUT_25MUX = 25,
  VO_LAYOUT_8MUX  = 8, VO_LAYOUT_36MUX = 36,
  VO_LAYOUT_9MUX  = 9, VO_LAYOUT_64MUX = 64,
  VO_LAYOUT_BUTT
}VO_LAYOUT_E;


//创建图像层显示通道;
int gsf_mpp_vo_layout(int volayer, VO_LAYOUT_E layout, RECT_S *rect);


//vdec;
//解码数据帧属性
typedef struct {
    int size;     // data size;
    int ftype;    // frame type;
    int etype;    // PAYLOAD_TYPE_E;
    int width;    // width;
    int height;   // height;
    int au_chs;   // audio channels;
    unsigned long long pts; // timestamp;
}gsf_mpp_frm_attr_t;

//发送视频数据到显示通道(创建VDEC通道)
int gsf_mpp_vo_vsend(int volayer, int ch, char *data, gsf_mpp_frm_attr_t *attr);

//发送音频数据到 audio 解码输出;
int gsf_mpp_ao_asend(int aodev, int ch, char *data, gsf_mpp_frm_attr_t *attr);

//解码状态;
typedef struct {
    int left_byte;    // vdec
    int left_frame;   // vdec
    int left_pics;    // vdec
    int bufused;      // vo
}gsf_mpp_vo_stat_t;

//获取解码显示状态
int gsf_mpp_vo_stat(int volayer, int ch, gsf_mpp_vo_stat_t *stat);
//设置解码显示FPS
int gsf_mpp_vo_setfps(int volayer, int ch, int fps);
//清除解码显示BUFF
int gsf_mpp_vo_clear(int volayer, int ch);

//VO-BIND-VPSS for DVR;
typedef struct {
  VPSS_GRP  VpssGrp;
  VPSS_CHN  VpssChn;
}gsf_mpp_vo_src_t;

int gsf_mpp_vo_bind(int volayer, int ch, gsf_mpp_vo_src_t *src);

//audio ao_bind_ai;
int gsf_mpp_ao_bind(int aodev, int ch, int aidev, int aich);

//设置通道源图像裁剪区域(用于局部放大)
int gsf_mpp_vo_crop(int volayer, int ch, RECT_S *rect);

//设置VO通道显示区域比例(rect返回调整后的位置)
int gsf_mpp_vo_aspect(int volayer, int ch, RECT_S *rect);

//设置VO通道显示区域(位置&大小);
int gsf_mpp_vo_rect(int volayer, int ch, RECT_S *rect, int priority);


//private for mpp;
extern int SENSOR_TYPE;
extern int SENSOR0_TYPE;
extern int SENSOR1_TYPE;
extern SAMPLE_SNS_TYPE_E g_enSnsType[MAX_SENSOR_NUM];
extern ISP_SNS_OBJ_S* g_pstSnsObj[MAX_SENSOR_NUM];
ISP_SNS_OBJ_S* SAMPLE_COMM_ISP_GetSnsObj(HI_U32 u32SnsId);


#define HI_ACODEC_TYPE_INNER //for audio;

#endif

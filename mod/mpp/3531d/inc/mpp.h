#ifndef __mpp_h__
#define __mpp_h__

#include "sample_comm.h"

typedef hi_vo_intf_type VO_INTF_TYPE_E;
typedef hi_vo_intf_sync VO_INTF_SYNC_E;
typedef hi_vo_intf_sync VO_INTF_SYNC_E;
typedef hi_rect RECT_S;



//sys;
typedef struct {
  char  snsname[32];  // AD_NVP6158
  int   snscnt;       // AD_COUNT
  int   second;       // second
}gsf_mpp_cfg_t;

int gsf_mpp_cfg(char *path, gsf_mpp_cfg_t *cfg);


//vodev;
enum {
  VODEV_HD0 = 0,  // 视频设备
  VODEV_HD1 = 1,
};

//启动视频输出设备
int gsf_mpp_vo_start(int vodev, hi_vo_intf_type type, hi_vo_intf_sync sync, int wbc);

//停止视频输出设备
int gsf_mpp_vo_stop(int vodev);

//vofb;
enum {
  VOFB_GUI   = 0, // GUI Layer
  VOFB_GUI1  = 1, // GUI1 Layer
  VOFB_MOUSE = 2, // MOUSE Layer
  VOFB_MOUSE1 = 3,// MOUSE1 Layer
  VOFB_BUTT
};
// FB层操作
int gsf_mpp_fb_start(int vofb, hi_vo_intf_sync sync, int hide);
int gsf_mpp_fb_hide(int vofb, int hide); //FBIOPUT_SHOW_HIFB
int gsf_mpp_fb_move(int vofb, int xpos, int ypos); // FBIOPAN_DISPLAY
int gsf_mpp_fb_draw(int vofb, void *data /* ARGB1555 */, int w, int h); // draw fb-memory;


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
  VO_LAYOUT_4MUX  = 4, VO_LAYOUT_16MUX = 16,
  VO_LAYOUT_6MUX  = 6, VO_LAYOUT_25MUX = 25,
  VO_LAYOUT_8MUX  = 8, VO_LAYOUT_36MUX = 36,
  VO_LAYOUT_9MUX  = 9, VO_LAYOUT_64MUX = 64,
  VO_LAYOUT_BUTT
}VO_LAYOUT_E;

//创建图像层显示通道;
int gsf_mpp_vo_layout(int volayer, VO_LAYOUT_E layout, hi_rect *rect);
//移动整个图像层显示区域(位置,大小)
int gsf_mpp_vo_move(int volayer, hi_rect *rect);
//设置通道源图像裁剪区域(用于局部放大)
int gsf_mpp_vo_crop(int volayer, int ch, hi_rect *rect);

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

//output format: YUV420
int gsf_mpp_vo_getyuv(int volayer, int ch, char *buf, int size, int *w, int *h);

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

//adec;
//设置当前音频数据通道号(!=ch的数据不送解码)
int gsf_mpp_vo_afilter(int vodev, int ch);
//发送音频数据解码(ch表示音频数据的通道号, ch!=gsf_mpp_vo_afilter(ch)时,直接返回)
int gsf_mpp_vo_asend(int vodev, int ch, char *data, gsf_mpp_frm_attr_t *attr);


typedef hi_venc_chn VENC_CHN;
typedef hi_vpss_grp VPSS_GRP;
typedef hi_vpss_chn VPSS_CHN;
typedef hi_payload_type PAYLOAD_TYPE_E;
typedef hi_pic_size PIC_SIZE_E;
typedef hi_s32 HI_S32;
typedef hi_u32 HI_U32;
typedef hi_s16 HI_S16;
typedef hi_u16 HI_U16;
typedef hi_bool HI_BOOL;
typedef hi_char HI_CHAR;
typedef hi_u8 HI_U8;
typedef hi_venc_gop_mode VENC_GOP_MODE_E;
typedef hi_venc_stream VENC_STREAM_S;
typedef hi_audio_stream AUDIO_STREAM_S;
typedef hi_venc_pack VENC_PACK_S;


typedef hi_mod_id MOD_ID_E;
typedef hi_avs_grp AVS_GRP;
typedef hi_vi_pipe VI_PIPE;
typedef hi_vo_layer VO_LAYER;
typedef hi_avs_chn AVS_CHN;
typedef hi_vi_chn VI_CHN;
typedef hi_vo_chn VO_CHN;
typedef hi_vdec_chn VDEC_CHN;


#define PT_H264 HI_PT_H264
#define PT_H265 HI_PT_H265
#define PT_JPEG HI_PT_JPEG
#define PT_MJPEG HI_PT_MJPEG

#define H264E_NALU_SPS HI_VENC_H264_NALU_SPS
#define H264E_NALU_PPS HI_VENC_H264_NALU_PPS
#define H264E_NALU_SEI HI_VENC_H264_NALU_SEI

#define H265E_NALU_SPS HI_VENC_H265_NALU_SPS
#define H265E_NALU_PPS HI_VENC_H265_NALU_PPS
#define H265E_NALU_VPS HI_VENC_H265_NALU_VPS
#define H265E_NALU_SEI HI_VENC_H265_NALU_SEI

#define VENC_GOPMODE_NORMALP HI_VENC_GOP_MODE_NORMAL_P
#define PIC_2592x1536 PIC_2592x1520 
typedef hi_video_frame_info VIDEO_FRAME_INFO_S;
#define VPSS_MAX_PHY_CHN_NUM HI_VPSS_MAX_PHYS_CHN_NUM


//vi;
typedef struct {
  HI_BOOL bLowDelay;
  HI_U32 u32SupplementConfig;
  //VI_STITCH_GRP_ATTR_S stStitchGrpAttr;
  PIC_SIZE_E venc_pic_size[CHN_NUM_MAX]; //get_default_vpss_chn_attr;
}gsf_mpp_vi_t;

//SAMPLE_COMM_VI_StartVi
int gsf_mpp_vi_start(gsf_mpp_vi_t *vi);
int gsf_mpp_vi_stop();
//HI_S32 HI_MPI_VI_GetPipeFrame(VI_PIPE ViPipe, VIDEO_FRAME_INFO_S *pstVideoFrame, HI_S32 s32MilliSec);
//HI_S32 HI_MPI_VI_GetChnFrame(VI_PIPE ViPipe, VI_CHN ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo, HI_S32 s32MilliSec);
int gsf_mpp_vi_get(int ViPipe, int ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo, int s32MilliSec);


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


//isp;
int gsf_mpp_scene_start(char *path, int scenemode);
int gsf_mpp_scene_stop();


//venc;
//编码参数
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
  
   
  hi_payload_type enPayLoad;
  hi_pic_size     enSize;
  hi_s32          enRcMode;
  hi_u32          u32Profile;
  hi_bool         bRcnRefShareBuf;
  hi_venc_gop_mode enGopMode;
  hi_u32          u32FrameRate;
  hi_u32          u32Gop;
  hi_u32          u32BitRate;
  hi_u32          u32LowDelay;
}gsf_mpp_venc_t;

//启动编码通道
int gsf_mpp_venc_start(gsf_mpp_venc_t *venc);
//停止编码通道
int gsf_mpp_venc_stop(gsf_mpp_venc_t *venc);

enum {
  GSF_MPP_VENC_CTL_IDR = 0,
};
int gsf_mpp_venc_ctl(int VencChn, int id, void *args);

typedef struct {
  hi_s32 s32Cnt;                    //添加到接收线程的通道号个数;
  hi_venc_chn VeChn[HI_VENC_MAX_CHN_NUM]; //添加到接收线程的通道号集合;
  void *uargs;
  int (*cb)(hi_venc_chn VeChn, hi_payload_type PT, hi_venc_stream* pstStream, void* uargs);
}gsf_mpp_recv_t;
//启动接收线程
int gsf_mpp_venc_recv(gsf_mpp_recv_t *recv);
//停止接收线程
int gsf_mpp_venc_dest();
int gsf_mpp_venc_snap(VENC_CHN VencChn, HI_U32 SnapCnt, int(*cb)(int i, VENC_STREAM_S* pstStream, void* u), void* u);


typedef hi_rgn_attr RGN_ATTR_S;
typedef hi_mpp_chn MPP_CHN_S;
typedef hi_rgn_chn_attr RGN_CHN_ATTR_S;
typedef hi_rgn_handle RGN_HANDLE;
typedef hi_rgn_canvas_info RGN_CANVAS_INFO_S;
typedef hi_bmp  BITMAP_S;

#define OVERLAY_RGN HI_RGN_OVERLAY
#define COVER_RGN HI_RGN_COVER
#define AREA_RECT HI_COVER_RECT
#define PIXEL_FORMAT_ARGB_1555 HI_PIXEL_FORMAT_ARGB_1555
#define ALIGN_UP HI_ALIGN_UP



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


//private for mpp;

/////////////////;


#endif //__mpp_h__

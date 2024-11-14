#ifndef __mpp_h__
#define __mpp_h__

#include "sample_comm.h"


typedef hi_rect RECT_S;

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

typedef hi_avs_chn AVS_CHN;
typedef hi_vi_chn VI_CHN;

typedef hi_vdec_chn VDEC_CHN;

#define PT_H264 HI_PT_H264
#define PT_H265 HI_PT_H265
#define PT_JPEG HI_PT_JPEG
#define PT_MJPEG HI_PT_MJPEG
#define PT_AAC HI_PT_AAC
#define PT_G711A HI_PT_G711A
#define PT_G711U HI_PT_G711U
#define PT_BUTT  HI_PT_BUTT



#define H264E_NALU_SPS HI_VENC_H264_NALU_SPS
#define H264E_NALU_PPS HI_VENC_H264_NALU_PPS
#define H264E_NALU_SEI HI_VENC_H264_NALU_SEI

#define H265E_NALU_SPS HI_VENC_H265_NALU_SPS
#define H265E_NALU_PPS HI_VENC_H265_NALU_PPS
#define H265E_NALU_VPS HI_VENC_H265_NALU_VPS
#define H265E_NALU_SEI HI_VENC_H265_NALU_SEI

#define VENC_GOPMODE_NORMALP HI_VENC_GOP_MODE_NORMAL_P
typedef hi_video_frame_info VIDEO_FRAME_INFO_S;
#define VPSS_MAX_PHY_CHN_NUM HI_VPSS_MAX_PHYS_CHN_NUM

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

typedef ot_vi_stitch_grp_attr VI_STITCH_GRP_ATTR_S;
#define VENC_MAX_CHN_NUM  HI_VENC_MAX_CHN_NUM
typedef hi_venc_stream_buf_info VENC_STREAM_BUF_INFO_S;


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
  int   aiisp;        // aiisp mode;
}gsf_mpp_cfg_t;

int gsf_mpp_cfg(char *path, gsf_mpp_cfg_t *cfg);

//vi;
typedef struct {
  HI_BOOL bLowDelay;
  HI_U32 u32SupplementConfig;
  VI_STITCH_GRP_ATTR_S stStitchGrpAttr;  //stitch; 
  PIC_SIZE_E venc_pic_size[CHN_NUM_MAX]; //get_default_vpss_chn_attr;
}gsf_mpp_vi_t;

//SAMPLE_COMM_VI_StartVi
int gsf_mpp_vi_start(gsf_mpp_vi_t *vi);
int gsf_mpp_vi_stop();
//HI_S32 HI_MPI_VI_GetPipeFrame(VI_PIPE ViPipe, VIDEO_FRAME_INFO_S *pstVideoFrame, HI_S32 s32MilliSec);
//HI_S32 HI_MPI_VI_GetChnFrame(VI_PIPE ViPipe, VI_CHN ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo, HI_S32 s32MilliSec);
int gsf_mpp_vi_get(int ViPipe, int ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo, int s32MilliSec);


typedef struct {
  int  ViPipe;
  void *uargs;
  int (*cb)(HI_U32 Fv1, HI_U32 Fv2, HI_U32 Gain, void* uargs);
}gsf_mpp_af_t;

int gsf_mpp_af_start(gsf_mpp_af_t *af);


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
int gsf_mpp_vpss_sendgd(int VpssGrp, int VpssGrpPipe, /*guide_usb_frame_data_t*/void *pstVideoFrame, int s32MilliSec);

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


//mcf;
typedef struct {
  ;
}gsf_mpp_mcf_t;

int gsf_mpp_mcf_start(gsf_mpp_mcf_t *mcf);
int gsf_mpp_mcf_stop(gsf_mpp_mcf_t *mcf);




//venc;
typedef struct {
  
  VENC_CHN  VencChn;
  
  MOD_ID_E  srcModId;
  union {
    VPSS_GRP  VpssGrp;
    AVS_GRP   AVSGrp;
    VI_PIPE   ViPipe;
  };
  union {
    VPSS_CHN  VpssChn; // VPSS
    AVS_CHN   AVSChn;  // AVS
    VDEC_CHN  VdChn;   // VDEC
    VI_CHN    ViChn;   // VI
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


typedef struct { // ==gsf_scene_ae_t
  float compensation_mul; // 0.5 - 1.5;
}gsf_mpp_scene_ae_t;

// gsf_mpp_scene_all_t == gsf_scene_all_t;
typedef struct {
  int magic;
  gsf_mpp_scene_ae_t ae;
}gsf_mpp_scene_all_t;

enum {
  GSF_MPP_SCENE_CTL_ALL= 0, // gsf_mpp_scene_all_t
  GSF_MPP_SCENE_CTL_AE = 1, // gsf_mpp_scene_ae_t==HI_SCENE_CTL_AE_S
};
int gsf_mpp_scene_ctl(int ViPipe, int id, void *args);


typedef struct {
  int fps;
  int bitrate;
}gsf_mpp_venc_rc_t;

enum {
  GSF_MPP_VENC_CTL_IDR = 0,
  GSF_MPP_VENC_CTL_RST = 1,
  GSF_MPP_VENC_CTL_RC  = 2, //gsf_mpp_venc_rc_t;
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
  int s32DistortionRatio;
}gsf_mpp_img_ldc_t;

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
  gsf_mpp_img_ldc_t   ldc;
}gsf_mpp_img_all_t;

typedef struct {
  int bFlip;
  int bMirror;
}gsf_mpp_img_flip_t;
    
typedef struct {
  int bEnable;
  int enMode;    //DIS_MODE_4_DOF_GME = 0 ;//DIS_MODE_6_DOF_GME; //DIS_MODE_4_DOF_GME;
  int enPdtType; //DIS_PDT_TYPE_IPC = 0; //DIS_PDT_TYPE_DV; DIS_PDT_TYPE_DRONE;
}gsf_mpp_img_dis_t;

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
  GSF_MPP_ISP_CTL_FLIP   = 12,// set gsf_mpp_img_flip_t;
  GSF_MPP_ISP_CTL_DIS    = 13,// set gsf_mpp_img_dis_t;
  GSF_MPP_ISP_CTL_LDC    = 14,// set gsf_mpp_img_ldc_t;  
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

//发送音频数据到 audio 解码输出;
int gsf_mpp_ao_asend(int aodev, int ch, int flag, char *data, gsf_mpp_frm_attr_t *attr);

//send pcm to ao;
int gsf_mpp_ao_send_pcm(int aodev, int ch, int flag, unsigned char *data, int size);

//audio ao_bind_ai;
int gsf_mpp_ao_bind(int aodev, int ch, int aidev, int aich);

//private for mpp;
#include "hi_sns_ctrl.h"

extern int SENSOR_TYPE;
extern int SENSOR0_TYPE;
extern int SENSOR1_TYPE;
extern hi_isp_sns_obj *sample_comm_isp_get_sns_obj(sample_sns_type sns_type);

#define OT_ACODEC_TYPE_INNER //for audio;
#define BOARD_TYPE DMEB_BGA //for BOARD;

//-DSENSOR0_TYPE=SC4336P_MIPI_4M_30FPS_10BIT -DSENSOR1_TYPE=SC4336P_MIPI_4M_30FPS_10BIT -DOT_ACODEC_TYPE_INNER -DOT_VQE_USE_STATIC_MODULE_REGISTER 


///////////////////

#endif

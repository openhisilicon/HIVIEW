#ifndef __mpp_h__
#define __mpp_h__

#include "sample_comm.h"

//sys;
typedef struct {
  int max_vdcnt;
}gsf_mpp_cfg_t;

int gsf_mpp_cfg(char *path, gsf_mpp_cfg_t *cfg);


//vodev;
enum {
  VODEV_HD0 = 0,  // 视频设备
};

//启动视频输出设备
int gsf_mpp_vo_start(int vodev, VO_INTF_TYPE_E type, VO_INTF_SYNC_E sync, int wbc);

//停止视频输出设备
int gsf_mpp_vo_stop(int vodev);

//vofb;
enum {
  VOFB_GUI   = 0, // GUI Layer
  VOFB_MOUSE = 1, // MOUSE Layer
  VOFB_BUTT
};
// FB层操作
int gsf_mpp_fb_start(int vofb, VO_INTF_SYNC_E sync, int hide);
int gsf_mpp_fb_hide(int vofb, int hide); //FBIOPUT_SHOW_HIFB
int gsf_mpp_fb_move(int vofb, int xpos, int ypos); // FBIOPAN_DISPLAY
int gsf_mpp_fb_draw(int vofb, void *data /* ARGB1555 */, int w, int h); // draw fb-memory;


//volayer;
enum {
  VOLAYER_HD0 = 0,  // 视频层
  VOLAYER_PIP = 1,  // PIP层
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
int gsf_mpp_vo_layout(int volayer, VO_LAYOUT_E layout, RECT_S *rect);
//移动整个图像层显示区域(位置,大小)
int gsf_mpp_vo_move(int volayer, RECT_S *rect);
//设置通道源图像裁剪区域(用于局部放大)
int gsf_mpp_vo_crop(int volayer, int ch, RECT_S *rect);

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

//venc;
//编码参数
typedef struct {
  VENC_CHN        VencChn;  // vech;
  VPSS_GRP        VpssGrp;  // -1: VO(WBC), >= 0: VPGRP;
  VPSS_CHN        VpssChn;  
  PAYLOAD_TYPE_E  enPayLoad;
  PIC_SIZE_E      enSize;
  HI_S32          enRcMode;
  HI_U32          u32Profile;
  HI_BOOL         bRcnRefShareBuf;
  VENC_GOP_MODE_E enGopMode;
  HI_U32          u32FrameRate;
  HI_U32          u32Gop;
  HI_U32          u32BitRate;
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
  HI_S32 s32Cnt;                    //添加到接收线程的通道号个数;
  VENC_CHN VeChn[VENC_MAX_CHN_NUM]; //添加到接收线程的通道号集合;
  void *uargs;
  int (*cb)(VENC_CHN VeChn, PAYLOAD_TYPE_E PT, VENC_STREAM_S* pstStream, void* uargs);
}gsf_mpp_recv_t;
//启动接收线程
int gsf_mpp_venc_recv(gsf_mpp_recv_t *recv);
//停止接收线程
int gsf_mpp_venc_dest();


//private for mpp;

/////////////////;


#endif //__mpp_h__

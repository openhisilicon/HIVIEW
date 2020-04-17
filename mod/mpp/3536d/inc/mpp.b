#ifndef __mpp_h__
#define __mpp_h__

#include "sample_comm.h"

/*
 * 1, 代码不考虑支持多种芯片系列, 比如3536X系列不考虑3531X; 
 * 2, 为方便更新SDK,尽量复用sample_comm中现有的代码;(可修改,请增加注释);
 */

//vodev;
enum {
  VODEV_HD0 = 0,  // 视频设备
};

//启动视频输出设备
int gsf_mpp_vo_start(int vodev, VO_INTF_TYPE_E type, VO_INTF_SYNC_E sync);
//停止视频输出设备
int gsf_mpp_vo_stop(int vodev);
//修改视频输出设置分辨率
int gsf_mpp_vo_resolu(int vodev, VO_INTF_SYNC_E sync);
//WBC使能
int gsf_mpp_vo_wbc(int vodev, int en);

//vofb;
enum {
  VOFB_GUI   = 0, // GUI Layer
  VOFB_MOUSE = 1, // MOUSE Layer
};
// FB层操作
int gsf_mpp_fb_start(int vofb, VO_INTF_SYNC_E sync, int hide);
int gsf_mpp_fb_stop(int vofb);
int gsf_mpp_fb_hide(int vofd);
int gsf_mpp_fb_move(int vofb, int xpos, int ypos);
int gsf_mpp_fb_draw(int vofb, void *data /* ARGB1555 */, int w, int h);


//volayer;
enum {
  VOLAYER_HD0 = 0,  // 视频层
  VOLAYER_PIP = 1,  // PIP层
};

// 显示通道布局
typedef enum {
  VO_SPLIT_NONE  = 0, VO_SPLIT_10MUX = 10,
  VO_SPLIT_1MUX  = 1, VO_SPLIT_12MUX = 12,
  VO_SPLIT_4MUX  = 4, VO_SPLIT_16MUX = 16,
  VO_SPLIT_6MUX  = 6, VO_SPLIT_25MUX = 25,
  VO_SPLIT_8MUX  = 8, VO_SPLIT_36MUX = 36,
  VO_SPLIT_9MUX  = 9, VO_SPLIT_64MUX = 64,
  VO_SPLIT_BUTT
}VO_SPLIT_E;

//创建图像层显示通道
//创建VPSS/VO通道, split=VO_SPLIT_NONE时,销毁所有显示通道;
//当vdch!=NULL时,绑定指定解码通道号(用于多个VPSS绑定一个VDEC的场景,如画中画且同一个视频源)
int gsf_mpp_vo_split(int volayer, VO_SPLIT_E split, RECT_S *rect, int vdch[VO_SPLIT_BUTT]);
//移动整个图像层显示区域(位置,大小)
int gsf_mpp_vo_move(int volayer, RECT_S *rect);
//设置通道源图像裁剪区域(用于局部放大)
int gsf_mpp_vo_crop(int volayer, int ch, RECT_S *rect);

//vdec;
//解码数据帧属性
typedef struct {
    int size;     // data size;
    int ftype;    // frame type;
    int etype;    // encode type;
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

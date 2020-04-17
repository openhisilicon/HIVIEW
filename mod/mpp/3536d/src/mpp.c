#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#include "mpp.h"

typedef struct {
  int adec;
  int ao;
  // ...
}vo_audio_t;

typedef struct {
  int en;
  int depth;
  VO_WBC_MODE_E   mode;
  VO_WBC_ATTR_S   attr;
  VO_WBC_SOURCE_S src;
  // ...
}vo_wbc_t;

typedef struct {
  int etype;
  int width;
  int height;
  int vdec;
  int vpss;
  // ...
}vo_ch_t;

typedef struct {
  RECT_S     rect;
  VO_LAYOUT_E cnt;
  vo_ch_t    chs[VO_MAX_CHN_NUM];
  // ...
}vo_layer_t;

typedef struct {
  // lock;
  pthread_mutex_t lock;
  // vodev;
  VO_INTF_TYPE_E intf;
  VO_INTF_SYNC_E sync;
  
  vo_wbc_t wbc;
  vo_audio_t audio;
  
  // volayer;
  vo_layer_t layer[VOLAYER_BUTT];
  
  // ...
}vo_mng_t;

static vo_mng_t vo_mng[VO_MAX_DEV_NUM];

static int layer2vdev[VO_MAX_LAYER_NUM] = {
  [VOLAYER_HD0] = VODEV_HD0,
  [VOLAYER_PIP] = VODEV_HD0
};


int gsf_mpp_cfg(char *path, gsf_mpp_cfg_t *cfg)
{
  
  char loadstr[64];
  sprintf(loadstr, "/var/app/ko/load3536dv100 -i");
  printf("%s => loadstr: %s\n", __func__, loadstr);
  system(loadstr);
  
  VB_CONF_S stVbConf, stModVbConf;
  HI_S32 ret = 0, s32Ret = HI_SUCCESS;

  SIZE_S stSize;
 
  VO_VIDEO_LAYER_ATTR_S stVoLayerAttr;
  HI_U32 u32VdCnt = 9;
  
  HI_BOOL bVdecCompress = HI_TRUE;

  stSize.u32Width = HD_WIDTH;
  stSize.u32Height = HD_HEIGHT;

  /************************************************
  step1:  init SYS and common VB 
  *************************************************/
  SAMPLE_COMM_VDEC_Sysconf(&stVbConf, &stSize);
  s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
  if(s32Ret != HI_SUCCESS)
  {
      SAMPLE_PRT("init sys fail for %#x!\n", s32Ret);
      ret = s32Ret;
      goto END1;
  }

  /************************************************
    step2:  init mod common VB
  *************************************************/
  SAMPLE_COMM_VDEC_ModCommPoolConf(&stModVbConf, PT_H265, &stSize, u32VdCnt, bVdecCompress);	
  s32Ret = SAMPLE_COMM_VDEC_InitModCommVb(&stModVbConf);
  if(s32Ret != HI_SUCCESS)
  {			
      SAMPLE_PRT("init mod common vb fail for %#x!\n", s32Ret);
      ret = s32Ret;
      goto END1;
  }
  
  int i = 0;
  for(i = 0; i < VO_MAX_DEV_NUM; i++)
  {
    pthread_mutex_init(&vo_mng[i].lock, NULL);
    //pthread_mutex_destroy(&mutex);
  }

  return 0;
END1:
  SAMPLE_COMM_SYS_Exit(); 
  return ret;
}


//启动视频输出设备
int gsf_mpp_vo_start(int vodev, VO_INTF_TYPE_E type, VO_INTF_SYNC_E sync, int wbc)
{
  int err = 0;

  HI_S32 s32Ret = HI_SUCCESS;
  
  HI_S32 VoDev = SAMPLE_VO_DEV_DHD0;
  HI_S32 VoLayer = SAMPLE_VO_LAYER_VHD0;
  
  pthread_mutex_lock(&vo_mng[VoDev].lock);

  /************************************************
  step5:  start VO
  *************************************************/
  VO_PUB_ATTR_S stVoPubAttr = {0};
  stVoPubAttr.enIntfSync = sync;
  stVoPubAttr.enIntfType = type;

  s32Ret = SAMPLE_COMM_VO_StartDev(VoDev, &stVoPubAttr);
  if(s32Ret != HI_SUCCESS)
  {		
      SAMPLE_PRT("vdec bind vpss fail for %#x!\n", s32Ret);
      err = s32Ret;
      goto END4_1;
  }

  if (HI_SUCCESS != SAMPLE_COMM_VO_HdmiStart(stVoPubAttr.enIntfSync))
  {
      SAMPLE_PRT("Start SAMPLE_COMM_VO_HdmiStart failed!\n");
      err = s32Ret;
      goto END4_2;
  }

  VO_VIDEO_LAYER_ATTR_S stVoLayerAttr = {0};
  
  s32Ret = SAMPLE_COMM_VO_GetWH(stVoPubAttr.enIntfSync, \
      &stVoLayerAttr.stDispRect.u32Width, &stVoLayerAttr.stDispRect.u32Height, &stVoLayerAttr.u32DispFrmRt);
  if (s32Ret != HI_SUCCESS)
  {
      SAMPLE_PRT("failed with %#x!\n", s32Ret);
      err = s32Ret;
      goto  END4_2;
  }
  stVoLayerAttr.stImageSize.u32Width = stVoLayerAttr.stDispRect.u32Width;
  stVoLayerAttr.stImageSize.u32Height = stVoLayerAttr.stDispRect.u32Height;
  stVoLayerAttr.bClusterMode = HI_FALSE;
  stVoLayerAttr.bDoubleFrame = HI_FALSE;
  stVoLayerAttr.enPixFormat = SAMPLE_PIXEL_FORMAT;    
  s32Ret = SAMPLE_COMM_VO_StartLayer(VoLayer, &stVoLayerAttr);
  if(s32Ret != HI_SUCCESS)
  {		
      SAMPLE_PRT("SAMPLE_COMM_VO_StartLayer fail for %#x!\n", s32Ret);
      err = s32Ret;
      goto END4_3;
  }

  vo_mng[VoDev].intf = stVoPubAttr.enIntfSync;
  vo_mng[VoDev].sync = stVoPubAttr.enIntfType;

  pthread_mutex_unlock(&vo_mng[VoDev].lock);
  return 0;
END4_3: 
  SAMPLE_COMM_VO_StopLayer(VoLayer);
END4_2: 
  SAMPLE_COMM_VO_HdmiStop();
END4_1:
  SAMPLE_COMM_VO_StopDev(VoDev);
  
  pthread_mutex_unlock(&vo_mng[VoDev].lock);
  return err;
}

int gsf_mpp_vo_stop(int vodev)
{
  int err = 0;  

  HI_S32 VoDev = SAMPLE_VO_DEV_DHD0;
  HI_S32 VoLayer = SAMPLE_VO_LAYER_VHD0;
  
  vo_mng_t *vdev = &vo_mng[layer2vdev[VoLayer]];

  pthread_mutex_lock(&vdev->lock);
  
END4_4:
  SAMPLE_COMM_VO_StopChn(VoLayer, vdev->layer[VoLayer].cnt);
END4_3: 
  SAMPLE_COMM_VO_StopLayer(VoLayer);
END4_2: 
#ifndef HI_FPGA
  SAMPLE_COMM_VO_HdmiStop();
#endif
END4_1:
  SAMPLE_COMM_VO_StopDev(VoDev);
    
  pthread_mutex_unlock(&vdev->lock);
  return err;
}


//创建显示通道
int gsf_mpp_vo_layout(int volayer, VO_LAYOUT_E layout, RECT_S *rect)
{
  int i = 0;
  int err = 0;
  HI_S32 s32Ret = HI_SUCCESS;
   
  vo_mng_t *vdev = &vo_mng[layer2vdev[volayer]];
  
  if(vdev->layer[volayer].cnt == layout)
  {
    return 0;
  }
  
  
  pthread_mutex_lock(&vdev->lock);
  
  for(i = 0; i < vdev->layer[volayer].cnt; i++)
  {
    // unbind vo && vpss && vdec;
    SAMPLE_COMM_VO_UnBindVpss(volayer, i, i, VPSS_CHN0);
    SAMPLE_COMM_VDEC_UnBindVpss(i, i);
     
    // only set width = height = 0, tell gsf_mpp_vo_vsend to recreate vdec&vpss;
    vdev->layer[volayer].chs[i].width = vdev->layer[volayer].chs[i].height = 0;
  }
  
  SAMPLE_COMM_VO_StopChn(volayer, vdev->layer[volayer].cnt);	
  SAMPLE_COMM_VPSS_Stop(vdev->layer[volayer].cnt, 1);
  SAMPLE_COMM_VDEC_Stop(vdev->layer[volayer].cnt);
  
  vdev->layer[volayer].cnt = layout;
  SAMPLE_COMM_VO_StartChn(volayer, vdev->layer[volayer].cnt);

  pthread_mutex_unlock(&vdev->lock);
  
  return err;
}
//移动整个显示区域(位置,大小)
int gsf_mpp_vo_move(int volayer, RECT_S *rect)
{
  int err = 0;
  // 
  return err;
}

//裁剪通道源图像区域到显示通道(局部放大)
int gsf_mpp_vo_crop(int volayer, int ch, RECT_S *rect)
{
  int err = 0;
  // 
  return err;
}

//发送视频数据到指定ch;
int gsf_mpp_vo_vsend(int volayer, int ch, char *data, gsf_mpp_frm_attr_t *attr)
{
  int err = 0;
  HI_S32 s32Ret = HI_SUCCESS;
  
  vo_mng_t *vdev = &vo_mng[layer2vdev[volayer]];
  
  if(vdev->layer[volayer].cnt == VO_LAYOUT_NONE
    || attr->width == 0
    || attr->height == 0
    || (attr->etype != PT_H264 
        && attr->etype != PT_H265 
        && attr->etype != PT_MP4VIDEO
        && attr->etype != PT_MJPEG
        && attr->etype != PT_JPEG))
  {
    return -1;
  }
  
  pthread_mutex_lock(&vdev->lock);
  
  if(vdev->layer[volayer].chs[ch].width != attr->width 
    || vdev->layer[volayer].chs[ch].height != attr->height
    || vdev->layer[volayer].chs[ch].etype != attr->etype)
  {
    vdev->layer[volayer].chs[ch].width  = attr->width;
    vdev->layer[volayer].chs[ch].height = attr->height;
    vdev->layer[volayer].chs[ch].etype  = attr->etype;

    // VOLAYER_HD0: [0 - VPSS_MAX_GRP_NUM/2]
    // VOLAYER_PIP: [VPSS_MAX_GRP_NUM/2 - VPSS_MAX_GRP_NUM];

    // unbind vo && vpss && vdec;
    SAMPLE_COMM_VO_UnBindVpss(volayer, ch, ch, VPSS_CHN0);
    SAMPLE_COMM_VDEC_UnBindVpss(ch, ch);
    
    // stop  vpss;
    s32Ret = HI_MPI_VPSS_StopGrp(ch);
    s32Ret = HI_MPI_VPSS_DisableChn(ch, VPSS_CHN0);
    s32Ret = HI_MPI_VPSS_DestroyGrp(ch);
    // stop  vdec;
    HI_MPI_VDEC_StopRecvStream(ch);
    HI_MPI_VDEC_DestroyChn(ch);
    
    /////////////////////////////
    
    // start vdec;
    VDEC_CHN_ATTR_S stVdecChnAttr = {0};

    stVdecChnAttr.enType       = attr->etype;
    stVdecChnAttr.u32BufSize   = 3 * attr->width * attr->height;
    stVdecChnAttr.u32Priority  = 5;
    stVdecChnAttr.u32PicWidth  = attr->width;
    stVdecChnAttr.u32PicHeight = attr->height;
    if (PT_H264 == stVdecChnAttr.enType || PT_MP4VIDEO == stVdecChnAttr.enType)
    {
        stVdecChnAttr.stVdecVideoAttr.enMode=VIDEO_MODE_FRAME;
        stVdecChnAttr.stVdecVideoAttr.u32RefFrameNum = 2;
        stVdecChnAttr.stVdecVideoAttr.bTemporalMvpEnable = 0;
    }
    else if (PT_JPEG == stVdecChnAttr.enType || PT_MJPEG == stVdecChnAttr.enType)
    {
        stVdecChnAttr.stVdecJpegAttr.enMode = VIDEO_MODE_FRAME;
        stVdecChnAttr.stVdecJpegAttr.enJpegFormat = JPG_COLOR_FMT_YCBCR420;
    }
    else if(PT_H265 == stVdecChnAttr.enType)
    {
        stVdecChnAttr.stVdecVideoAttr.enMode=VIDEO_MODE_STREAM;
        stVdecChnAttr.stVdecVideoAttr.u32RefFrameNum = 2;
        stVdecChnAttr.stVdecVideoAttr.bTemporalMvpEnable = 1;
    }
    
    HI_MPI_VDEC_SetChnVBCnt(ch, 10);
    HI_MPI_VDEC_CreateChn(ch, &stVdecChnAttr);
    HI_MPI_VDEC_StartRecvStream(ch);
    
    // start vpss;
    VPSS_GRP_ATTR_S stGrpAttr = {0};
    VPSS_CHN_ATTR_S stChnAttr = {0};
    
    /*** create vpss group ***/
    stGrpAttr.u32MaxW = ALIGN_UP(attr->width,  16);
    stGrpAttr.u32MaxH = ALIGN_UP(attr->height, 16);
    stGrpAttr.enPixFmt = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    stGrpAttr.bIeEn = HI_FALSE;
    stGrpAttr.bNrEn = HI_FALSE;
    stGrpAttr.bDciEn = HI_FALSE;
    stGrpAttr.bHistEn = HI_FALSE;
    stGrpAttr.bEsEn = HI_FALSE;
    stGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
    
    s32Ret = HI_MPI_VPSS_CreateGrp(ch, &stGrpAttr);
    /*** enable vpss chn, with frame ***/
    stChnAttr.bSpEn = HI_FALSE;
    stChnAttr.bUVInvert = HI_FALSE;
    stChnAttr.bBorderEn = HI_TRUE;
    stChnAttr.stBorder.u32Color = 0xffffff;
    stChnAttr.stBorder.u32LeftWidth = 2;
    stChnAttr.stBorder.u32RightWidth = 2;
    stChnAttr.stBorder.u32TopWidth = 2;
    stChnAttr.stBorder.u32BottomWidth = 2;
    s32Ret = HI_MPI_VPSS_SetChnAttr(ch, VPSS_CHN0, &stChnAttr);
    s32Ret = HI_MPI_VPSS_EnableChn(ch, VPSS_CHN0);
    s32Ret = HI_MPI_VPSS_StartGrp(ch);

    // bind  vdec && vpss && vo;
    s32Ret = SAMPLE_COMM_VDEC_BindVpss(ch, ch);
    s32Ret = SAMPLE_COMM_VO_BindVpss(volayer, ch, ch, VPSS_CHN0);
    
  }
  
  pthread_mutex_unlock(&vdev->lock);
  
  // send vdec;
  VDEC_STREAM_S stStream = {0};
  stStream.u64PTS  = 0;
  stStream.pu8Addr = data;
  stStream.u32Len  = attr->size;
  stStream.bEndOfFrame  = HI_TRUE;
  stStream.bEndOfStream = HI_FALSE;   
  s32Ret = HI_MPI_VDEC_SendStream(ch, &stStream, 0);

  #if 0
  /* send the flag of stream end */
  memset(&stStream, 0, sizeof(VDEC_STREAM_S) );
  stStream.bEndOfStream = HI_TRUE;
  HI_MPI_VDEC_SendStream(ch, &stStream, -1);
  #endif

  return err;
}

//获取解码显示状态
int gsf_mpp_vo_stat(int volayer, int ch, gsf_mpp_vo_stat_t *stat)
{
  int err = 0;
  // 
  return err;
}
//设置解码显示FPS
int gsf_mpp_vo_setfps(int volayer, int ch, int fps)
{
  int err = 0;
  // 
  return err;
}
//清除解码显示BUFF
int gsf_mpp_vo_clear(int volayer, int ch)
{
  int err = 0;
  // 
  return err;
}

//adec;
//设置当前音频数据通道号(!=ch的数据不送解码)
int gsf_mpp_vo_afilter(int vodev, int ch)
{
  int err = 0;
  return err;
}
//发送音频数据解码(ch表示音频数据的通道号)
int gsf_mpp_vo_asend(int vodev, int ch, char *data, gsf_mpp_frm_attr_t *attr)
{
  int err = 0;
  // start adec;
  return err;
}
//启动编码通道
int gsf_mpp_venc_start(gsf_mpp_venc_t *venc)
{
  int err = 0;
  return err;
}
//停止编码通道
int gsf_mpp_venc_stop(gsf_mpp_venc_t *venc)
{
  int err = 0;
  return err;
}

int gsf_mpp_venc_ctl(int VencChn, int id, void *args)
{
  int err = 0;
  return err;
}

//启动接收线程
int gsf_mpp_venc_recv(gsf_mpp_recv_t *recv)
{
  int err = 0;
  return err;
}
//停止接收线程
int gsf_mpp_venc_dest()
{
  int err = 0;
  return err;
}


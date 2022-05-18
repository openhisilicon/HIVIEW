#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <linux/fb.h>

#include "hifb.h"
#include "mpp.h"

static hi_u32 vdec_chn_num = 8;
static sample_vdec_attr sample_vdec[HI_VDEC_MAX_CHN_NUM];

int gsf_mpp_cfg(char *path, gsf_mpp_cfg_t *cfg)
{
  int ret = 0;
  
  char loadstr[128];
  sprintf(loadstr, "%s/ko/load3531dv200 -i", path);
  printf("%s => loadstr: %s\n", __func__, loadstr);
  system(loadstr);
  
  
  //SAMPLE_AD_TYPE cfg->snsname;cfg->snscnt;
  
  
  extern hi_void sample_venc_handle_sig(hi_s32 signo);
  signal(SIGINT, sample_venc_handle_sig);
  signal(SIGTERM, sample_venc_handle_sig);
  
  return ret;
}

static sample_venc_vpss_chn_attr vpss_param;
static sample_venc_vb_attr vb_attr = {0};
static sample_vi_config vi_cfg;

//SAMPLE_COMM_VI_StartVi
int gsf_mpp_vi_start(gsf_mpp_vi_t *vi)
{
  hi_s32 ret = 0, i = 0;
  hi_size enc_size[CHN_NUM_MAX];
  hi_size vi_size = {0};
  hi_pic_size pic_size[CHN_NUM_MAX] = {PIC_1080P, PIC_D1_NTSC};

  for (i = 0; i < CHN_NUM_MAX; i++) {
      //check venc_pic_size;
      pic_size[i] = (vi->venc_pic_size[i])?vi->venc_pic_size[i]:pic_size[i];
    
      ret = sample_comm_sys_get_pic_size(pic_size[i], &enc_size[i]);
      if (ret != HI_SUCCESS) {
          SAMPLE_PRT("sample_comm_sys_get_pic_size failed!\n");
          return ret;
      }
  }

  // get vi param
  extern hi_void get_default_vi_cfg(sample_vi_config *vi_cfg);
  get_default_vi_cfg(&vi_cfg);
  extern hi_size get_vi_size_from_mode(sample_vi_mode vi_mode);
  vi_size = get_vi_size_from_mode(vi_cfg.vi_mode);
  if (vi_size.width == 0 || vi_size.height == 0) {
      SAMPLE_PRT("call get_vi_size_from_mode failed! vi mode = %d\n", vi_cfg.vi_mode);
      return HI_FAILURE;
  }
  
  // get vpss param
  extern hi_void get_default_vpss_chn_attr(hi_size vi_size, hi_size enc_size[], hi_s32 len,
            sample_venc_vpss_chn_attr *vpss_chan_attr);
  get_default_vpss_chn_attr(vi_size, enc_size, CHN_NUM_MAX, &vpss_param);

  /******************************************
    step 1: init sys alloc common vb
  ******************************************/
  extern hi_void get_vb_attr(const hi_size *vi_size, const sample_venc_vpss_chn_attr *vpss_chn_attr,
            sample_venc_vb_attr *vb_attr);
  get_vb_attr(&vi_size, &vpss_param, &vb_attr);

  //init common VB(for VPSS and VO) from sample_init_sys_and_vb();
  extern hi_s32 get_vb_attr_vdec(hi_u32 vdec_chn_num, sample_venc_vb_attr *vb_attr);
  get_vb_attr_vdec(vdec_chn_num, &vb_attr);
 
  extern hi_s32 sample_venc_sys_init(sample_venc_vb_attr *vb_attr);
  if ((ret = sample_venc_sys_init(&vb_attr)) != HI_SUCCESS) {
      SAMPLE_PRT("Init SYS ret for %#x!\n", ret);
      return ret;
  }
  
  extern hi_s32 sample_venc_vi_init(sample_vi_config *vi_cfg);
  if ((ret = sample_venc_vi_init(&vi_cfg)) != HI_SUCCESS) {
      SAMPLE_PRT("Init VI ret for %#x!\n", ret);
      goto EXIT_SYS_STOP;
  }

  return ret;

EXIT_SYS_STOP:
    sample_comm_sys_exit();
  return ret;
}

int gsf_mpp_vi_stop()
{
  int ret = 0;
  extern hi_void sample_venc_vi_deinit(sample_vi_config *vi_cfg);
  sample_venc_vi_deinit(&vi_cfg);
  sample_comm_sys_exit();
  return ret;
}
//HI_S32 HI_MPI_VI_GetPipeFrame(VI_PIPE ViPipe, VIDEO_FRAME_INFO_S *pstVideoFrame, HI_S32 s32MilliSec);
//HI_S32 HI_MPI_VI_GetChnFrame(VI_PIPE ViPipe, VI_CHN ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo, HI_S32 s32MilliSec);
int gsf_mpp_vi_get(int ViPipe, int ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo, int s32MilliSec)
{
  int ret = 0;
  return ret;
}


//vpss;
extern hi_void sample_venc_vpss_deinit(hi_vpss_grp vpss_grp, sample_venc_vpss_chn_attr *vpss_chan_cfg);

//SAMPLE_COMM_VI_Bind_VPSS
int gsf_mpp_vpss_start(gsf_mpp_vpss_t *vpss)
{
  int ret = 0;
  hi_vi_pipe vi_pipe  = vpss->ViPipe;
  hi_vi_chn vi_chn    = vpss->ViChn;
  hi_vpss_grp vpss_grp= vpss->VpssGrp;
  
  for(int i = 0; i < HI_VPSS_MAX_PHYS_CHN_NUM; i++)
  {
    if(!vpss->enable[i])
      continue;
    // set vpss_param ???;
  }
  
  extern hi_s32 sample_venc_vpss_init(hi_vpss_grp vpss_grp, sample_venc_vpss_chn_attr *vpss_chan_cfg);
  if ((ret = sample_venc_vpss_init(vpss_grp, &vpss_param)) != HI_SUCCESS) {
      SAMPLE_PRT("Init VPSS err for %#x!\n", ret);
      goto EXIT;
  }
  
  //if ((ret = sample_comm_vi_bind_vpss(vi_pipe, vi_chn, vpss_grp, 0)) != HI_SUCCESS) 
  if ((ret = sample_comm_vi_bind_vpss(vi_pipe+4, (vi_pipe+4)*4, vpss_grp, 0)) != HI_SUCCESS) 
  {
      SAMPLE_PRT("VI Bind VPSS err for %#x!\n", ret);
      goto EXIT_VPSS_STOP;
  }
  
EXIT:
  return ret;
  
EXIT_VPSS_STOP:
  sample_venc_vpss_deinit(vpss_grp, &vpss_param);
  return ret;
}

int gsf_mpp_vpss_stop(gsf_mpp_vpss_t *vpss)
{
  int ret = 0;
  hi_vi_pipe vi_pipe  = vpss->ViPipe;
  hi_vi_chn vi_chn    = vpss->ViChn;
  hi_vpss_grp vpss_grp= vpss->VpssGrp;

  sample_comm_vi_un_bind_vpss(vi_pipe, vi_chn, vpss_grp, 0);
  sample_venc_vpss_deinit(vpss_grp, &vpss_param);
  return ret;
}
//HI_S32 HI_MPI_VPSS_SendFrame VPSS_GRP VpssGrp, VPSS_GRP_PIPE VpssGrpPipe, const VIDEO_FRAME_INFO_S *pstVideoFrame , HI_S32 s32MilliSec);
int gsf_mpp_vpss_send(int VpssGrp, int VpssGrpPipe, VIDEO_FRAME_INFO_S *pstVideoFrame , int s32MilliSec)
{
  int ret = 0;
  return ret;
}

int gsf_mpp_scene_start(char *path, int scenemode)
{
  int ret = 0;
  return ret;
}

int gsf_mpp_scene_stop()
{
  int ret = 0;
  return ret;
}


//启动编码通道
int gsf_mpp_venc_start(gsf_mpp_venc_t *venc)
{
  int ret = 0;
  hi_venc_gop_mode gop_mode;
  hi_venc_gop_attr gop_attr;
  
  hi_vpss_grp vpss_grp = venc->VpssGrp;
  hi_vpss_chn vpss_chn = venc->VpssChn;
  hi_venc_chn venc_chn = venc->VencChn;
  
  sample_comm_venc_chnl_param venc_create_param = {0};

  gop_mode = HI_VENC_GOP_MODE_NORMAL_P;
  if ((ret = sample_comm_venc_get_gop_attr(gop_mode, &gop_attr)) != HI_SUCCESS) {
      SAMPLE_PRT("Venc Get GopAttr for %#x!\n", ret);
      return ret;
  }

  venc_create_param.gop_attr                  = gop_attr;
  venc_create_param.type                      = venc->enPayLoad;// HI_PT_H265;
  venc_create_param.size                      = venc->enSize; // BIG_STREAM_SIZE//SMALL_STREAM_SIZE
  venc_create_param.rc_mode                   = SAMPLE_RC_CBR;//venc->enRcMode;//SAMPLE_RC_CBR;
  venc_create_param.profile                   = 0;
  venc_create_param.is_rcn_ref_share_buf      = HI_TRUE;
  venc_create_param.frame_rate                = venc->u32FrameRate; /* 30 is a number */
  venc_create_param.gop                       = venc->u32Gop; /* 30 is a number */
  
  /* encode h.264 */
  ret = sample_comm_venc_start(venc_chn, &venc_create_param);
  if (ret != HI_SUCCESS) {
      SAMPLE_PRT("Venc Start failed for %#x!\n", ret);
      goto EXIT;
  }

  if ((ret = sample_comm_vpss_bind_venc(vpss_grp, vpss_chn, venc_chn)) != HI_SUCCESS) {
      SAMPLE_PRT("sample_comm_vpss_bind_venc failed for %#x!\n", ret);
      goto EXIT_VENC_H264_STOP;
  }
  
  return ret;

EXIT_VENC_H264_STOP:
    sample_comm_venc_stop(venc_chn);
EXIT:
  return ret;
}
//停止编码通道
int gsf_mpp_venc_stop(gsf_mpp_venc_t *venc)
{
  int ret = 0;
  hi_venc_gop_mode gop_mode;
  hi_venc_gop_attr gop_attr;

  hi_vpss_grp vpss_grp = venc->VpssGrp;
  hi_vpss_chn vpss_chn = venc->VpssChn;
  hi_venc_chn venc_chn = venc->VencChn;

  sample_comm_vpss_un_bind_venc(vpss_grp, vpss_chn, venc_chn);
  sample_comm_venc_stop(venc_chn);
  return ret;
}

int gsf_mpp_venc_ctl(int VencChn, int id, void *args)
{
  int ret = 0;
  return ret;
}

//启动接收线程
int gsf_mpp_venc_recv(gsf_mpp_recv_t *recv)
{
  int ret = 0;

  if ((ret = sample_comm_venc_start_get_streamCB(recv->VeChn, recv->s32Cnt, recv->cb, recv->uargs)) != HI_SUCCESS) 
  {
      SAMPLE_PRT("Start Venc failed!\n");
  } 
  
  return ret;
}
//停止接收线程
int gsf_mpp_venc_dest()
{
  int ret = 0;
  sample_comm_venc_stop_get_stream(CHN_NUM_MAX);
  return ret;
}

int gsf_mpp_venc_snap(VENC_CHN VencChn, HI_U32 SnapCnt, int(*cb)(int i, VENC_STREAM_S* pstStream, void* u), void* u)
{
  int ret = 0;
  return ret;
}



int gsf_mpp_rgn_ctl(RGN_HANDLE Handle, int id, gsf_mpp_rgn_t *rgn)
{
  int ret = 0;
  return ret;
}
int gsf_mpp_rgn_bitmap(RGN_HANDLE Handle, BITMAP_S *bitmap)
{
  int ret = 0;
  return ret;
}
int gsf_mpp_rgn_canvas_get(RGN_HANDLE Handle, RGN_CANVAS_INFO_S *pstRgnCanvasInfo)
{
  int ret = 0;
  return ret;
}
int gsf_mpp_rgn_canvas_update(RGN_HANDLE Handle)
{
  int ret = 0;
  return ret;
}


typedef struct {
  int adec;
  int ao;
  // ...
}vo_audio_t;

typedef struct {
  int en;
  int depth;
  hi_vo_wbc_mode   mode;
  hi_vo_wbc_attr   attr;
  hi_vo_wbc_src_type src;
  // ...
}vo_wbc_t;

typedef enum {
  VO_SRC_VP = 0,
  VO_SRC_VDVP,
}VO_SRC_E;

typedef struct {
  int etype;
  int width;
  int height;
  VO_SRC_E src_type;
  int src_grp;
  int src_chn;
  // ...
}vo_ch_t;

typedef struct {
  hi_rect     rect;
  VO_LAYOUT_E cnt;
  vo_ch_t    chs[HI_VO_MAX_CHN_NUM];
  // ...
}vo_layer_t;

typedef struct {
  // lock;
  pthread_mutex_t lock;
  
  // vodev;
  hi_vo_intf_type intf;
  hi_vo_intf_sync sync;
  sample_vo_config vo_config;
  
  vo_wbc_t wbc;
  vo_audio_t audio;
  
  // volayer;
  vo_layer_t layer[VOLAYER_BUTT];
  
  // ...
}vo_mng_t;

static vo_mng_t vo_mng[HI_VO_MAX_PHYS_DEV_NUM];

static int layer2vdev[HI_VO_MAX_PHYS_LAYER_NUM] = {
  [VOLAYER_HD0] = VODEV_HD0,
  [VOLAYER_HD1] = VODEV_HD1,
  [VOLAYER_PIP] = VODEV_HD0
};


static struct fb_bitfield s_r16 = {10, 5, 0};
static struct fb_bitfield s_g16 = {5, 5, 0};
static struct fb_bitfield s_b16 = {0, 5, 0};
static struct fb_bitfield s_a16 = {15, 1, 0};

static struct fb_bitfield s_a32 = {24,8,0};
static struct fb_bitfield s_r32 = {16,8,0};
static struct fb_bitfield s_g32 = {8,8,0};
static struct fb_bitfield s_b32 = {0,8,0};

static int vo_fd[VOFB_BUTT];

int gsf_mpp_fb_start(int vofb, VO_INTF_SYNC_E sync, int hide)
{
    int i = 0, j = 0, x = 0, y = 0;
    HI_CHAR file[32] = "/dev/fb0";
    
    switch (vofb)
    {
        case VOFB_GUI:
            strcpy(file, "/dev/fb0");
            break;
        case VOFB_GUI1:
            strcpy(file, "/dev/fb1");
            break;
        case VOFB_MOUSE:
            strcpy(file, "/dev/fb2");
            break;
        case VOFB_MOUSE1:
            strcpy(file, "/dev/fb3");
            break;
        default:
            strcpy(file, "/dev/fb0");
            break;
    }
  
    /* 1. open framebuffer device overlay 0 */
    int fd = open(file, O_RDWR, 0);
    if(fd < 0)
    {
        SAMPLE_PRT("open %s failed!\n",file);
        return -1;
    } 

    HI_BOOL bShow = HI_FALSE;
    if (ioctl(fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
    {
        SAMPLE_PRT("FBIOPUT_SHOW_HIFB failed!\n");
        return -1;
    }
    #if 0
    HIFB_CAPABILITY_S stCap;
    if ( ioctl(fd, FBIOGET_CAPABILITY_HIFB, &stCap) < 0)
    {
        SAMPLE_PRT("FBIOGET_CAPABILITY_HIFB failed!\n");
        return -1;
    }
    #endif
    /* 2. set the screen original position */
    hi_fb_point stPoint = {0, 0};
    stPoint.x_pos = 0;
    stPoint.y_pos = 0;

    if (ioctl(fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0)
    {
        SAMPLE_PRT("set screen original show position failed!\n");
        close(fd);
        return -1;
    }

    /* 3. get the variable screen info */
    struct fb_var_screeninfo var;
    if (ioctl(fd, FBIOGET_VSCREENINFO, &var) < 0)
    {
        SAMPLE_PRT("Get variable screen info failed!\n");
		    close(fd);
        return -1;
    }
    
    int u32Width = 1280;
    int u32Height = 720;
    int u32VoFrmRate = 60;
    sample_comm_vo_get_width_height(sync, &u32Width, &u32Height, &u32VoFrmRate);
    printf("vofb:%d, u32Width:%d, u32Height:%d\n", vofb, u32Width, u32Height);
    switch (vofb)
    {
        case VOFB_GUI:
        case VOFB_GUI1:
            var.xres_virtual = u32Width;
            var.yres_virtual = u32Height;//u32Height*2;
            var.xres = u32Width;
            var.yres = u32Height;
            break;
        case VOFB_MOUSE:
        case VOFB_MOUSE1:
            var.xres_virtual = 48;
            var.yres_virtual = 48;
            var.xres = 48;
            var.yres = 48;
            break;
        default:
            break;
    }
    #if 1
    var.transp= s_a32;
    var.red   = s_r32;
    var.green = s_g32;
    var.blue  = s_b32;
    var.bits_per_pixel = 32;
    #else
    var.transp= s_a16;
    var.red   = s_r16;
    var.green = s_g16;
    var.blue  = s_b16;
    var.bits_per_pixel = 16;
    #endif
    var.activate = FB_ACTIVATE_NOW;
    
    /* 5. set the variable screeninfo */
    if (ioctl(fd, FBIOPUT_VSCREENINFO, &var) < 0)
    {
        SAMPLE_PRT("Put variable screen info failed!\n");
		    close(fd);
        return -1;
    }
     
    /* 6. get the fix screen info */
    struct fb_fix_screeninfo fix;
    if (ioctl(fd, FBIOGET_FSCREENINFO, &fix) < 0)
    {
        SAMPLE_PRT("Get fix screen info failed!\n");
		    close(fd);
        return -1;
    }
    int u32FixScreenStride = fix.line_length;   /*fix screen stride*/
    
    hi_fb_alpha stAlpha={0};
    if (ioctl(fd, FBIOGET_ALPHA_HIFB,  &stAlpha))
    {
        SAMPLE_PRT("Get alpha failed!\n");
        close(fd);
        return -1;
    }
    stAlpha.alpha_en = HI_TRUE;
    stAlpha.alpha_chn_en = HI_FALSE;
    stAlpha.alpha0 = 0x0;//0xff; // 当最高位为0时,选择该值作为Alpha
    stAlpha.alpha1 = 0xff;//0x0; // 当最高位为1时,选择该值作为Alpha
    stAlpha.global_alpha = 0x0;//在Alpha通道使能时起作用
    
    if (ioctl(fd, FBIOPUT_ALPHA_HIFB,  &stAlpha) < 0)
    {
        SAMPLE_PRT("Set alpha failed!\n");
        close(fd);
        return -1;
    }

    hi_fb_colorkey stColorKey;
    stColorKey.enable = HI_FALSE;
    stColorKey.value = 0x0000;
    if (ioctl(fd, FBIOPUT_COLORKEY_HIFB, &stColorKey) < 0)
    {
        SAMPLE_PRT("FBIOPUT_COLORKEY_HIFB failed!\n");
        close(fd);
        return -1;
    }

    /* 7. map the physical video memory for user use */
    HI_U8 *pShowScreen = mmap(HI_NULL, fix.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(MAP_FAILED == pShowScreen)
    {
        SAMPLE_PRT("mmap framebuffer failed!\n");
		    close(fd);
        return -1;
    }
    
    if(vo_mng[0].intf == HI_VO_INTF_MIPI)
    {
      ;
    }
  
    memset(pShowScreen, 0x00, fix.smem_len);
    
    #if 0
    void *ptemp;
    ptemp = (pShowScreen + 100*u32FixScreenStride);
    for (y = 0; y < 100; y++)
    {
        for (x = 0; x < 800; x++)
        {
          *((HI_U32*)ptemp + y*u32FixScreenStride/4 + x) = 0xFFff0000;
        }
    }
    
    ptemp = (pShowScreen + 200*u32FixScreenStride);
    for (y = 0; y < 100; y++)
    {
        for (x = 0; x < 800; x++)
        {
          *((HI_U32*)ptemp + y*u32FixScreenStride/4 + x) = 0xFF00ff00;
        }
    }
    
    ptemp = (pShowScreen + 300*u32FixScreenStride);
    for (y = 0; y < 100; y++)
    {
        for (x = 0; x < 800; x++)
        {
          *((HI_U32*)ptemp + y*u32FixScreenStride/4 + x) = 0xFF0000ff;
        }
    }
    #endif
    
    munmap(pShowScreen, fix.smem_len);
    
    if(!hide)
    {
      bShow = HI_TRUE;
      if (ioctl(fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
      {
          SAMPLE_PRT("FBIOPUT_SHOW_HIFB failed!\n");
          close(fd);
          return -1;
      }
    }
    
    vo_fd[vofb] = fd;//close(fd);
    return 0;
}

int gsf_mpp_fb_hide(int vofb, int hide)
{
  // FBIOPUT_SHOW_HIFB
  return 0;
}

int gsf_mpp_fb_move(int vofb, int xpos, int ypos)
{
  // FBIOPAN_DISPLAY
  return 0;
}
int gsf_mpp_fb_draw(int vofb, void *data /* ARGB1555 */, int w, int h)
{
  // draw fb-memory;
  return 0;
}

//启动视频输出设备;
int gsf_mpp_vo_start(int vodev, VO_INTF_TYPE_E type, VO_INTF_SYNC_E sync, int wbc)
{
    HI_S32 i, ret = HI_SUCCESS;
    
    hi_u32 vpss_grp_num = 0;
    
    #if 0 // move to gsf_mpp_vi_start();
    /************************************************
    step1:  init SYS, init common VB(for VPSS and VO), init module VB(for VDEC)
    *************************************************/
    extern hi_s32 sample_init_sys_and_vb(sample_vdec_attr *sample_vdec, hi_u32 vdec_chn_num, hi_payload_type type, hi_u32 len);
    ret = sample_init_sys_and_vb(&sample_vdec[0], vdec_chn_num, HI_PT_H264, HI_VDEC_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    #else //init module VB(for VDEC)
    extern hi_s32 sample_init_module_vb(sample_vdec_attr *sample_vdec, hi_u32 vdec_chn_num, hi_payload_type type,
              hi_u32 len);
    ret = sample_init_module_vb(&sample_vdec[0], vdec_chn_num, HI_PT_H264, HI_VDEC_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("init mod vb fail for %#x!\n", ret);
        return ret;
    }
    #endif
    
    /************************************************
    step4:  start VO
    *************************************************/
    
    extern hi_s32 sample_start_vo(sample_vo_config *vo_config, hi_u32 vpss_grp_num, hi_bool is_pip);
    ret = sample_start_vo(&vo_mng[vodev].vo_config, vpss_grp_num, HI_FALSE);
    if (ret != HI_SUCCESS) {
        goto END3;
    }
    
    vo_mng[vodev].intf = vo_mng[vodev].vo_config.vo_intf_type;
    vo_mng[vodev].sync = vo_mng[vodev].vo_config.intf_sync;
    printf("vodev:%d, intf:%d, sync:%d, u32Width:%d, u32Height:%d\n"
            , vodev, vo_mng[vodev].intf, vo_mng[vodev].sync
            , vo_mng[vodev].vo_config.image_size.width, vo_mng[vodev].vo_config.image_size.height);         
            
    return ret;
END3:
    sample_comm_sys_exit();
    return ret;
}

//停止视频输出设备
int gsf_mpp_vo_stop(int vodev)
{
    vo_mng_t *vdev = &vo_mng[vodev];
    
    return sample_comm_vo_stop_vo(&vdev->vo_config);
}

//创建视频层显示通道;
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
    sample_comm_vpss_un_bind_vo(vdev->layer[volayer].chs[i].src_grp, vdev->layer[volayer].chs[i].src_chn, volayer, i);
    if(vdev->layer[volayer].chs[i].src_type >= VO_SRC_VDVP)
    {
      sample_comm_vdec_un_bind_vpss(i, i);
      HI_BOOL chen[VPSS_MAX_PHY_CHN_NUM] = {HI_TRUE,HI_TRUE};
      sample_common_vpss_stop(i, chen, HI_VPSS_MAX_CHN_NUM);
    }
    
    // only set width = height = 0, tell gsf_mpp_vo_vsend to recreate vdec&vpss;
    vdev->layer[volayer].chs[i].width = vdev->layer[volayer].chs[i].height = 0;
  }

  if(vdev->layer[volayer].cnt)
    sample_comm_vo_stop_chn(volayer, vdev->layer[volayer].cnt);
  
  if(vdev->layer[volayer].chs[i].src_type >= VO_SRC_VDVP)
  {
    sample_comm_vdec_stop(vdev->layer[volayer].cnt);
  }
  
  vdev->layer[volayer].cnt = layout;
  sample_comm_vo_start_chn(volayer, vdev->layer[volayer].cnt);
  
  pthread_mutex_unlock(&vdev->lock);

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
    vdev->layer[volayer].chs[ch].src_type = VO_SRC_VDVP;
    vdev->layer[volayer].chs[ch].src_grp = ch;
    vdev->layer[volayer].chs[ch].src_chn = HI_VPSS_CHN0;
    
    // VOLAYER_HD0: [0 - VPSS_MAX_GRP_NUM/2]
    // VOLAYER_PIP: [VPSS_MAX_GRP_NUM/2 - VPSS_MAX_GRP_NUM];

    // unbind vo && vpss && vdec;
    sample_comm_vpss_un_bind_vo(ch, HI_VPSS_CHN0, volayer, ch);
    sample_comm_vdec_un_bind_vpss(ch, ch);
    
    // stop  vpss;
    HI_BOOL chen[VPSS_MAX_PHY_CHN_NUM] = {HI_TRUE,HI_TRUE};
    sample_common_vpss_stop(ch, chen, HI_VPSS_MAX_CHN_NUM);
    
    // stop  vdec;
    hi_mpi_vdec_stop_recv_stream(ch);
    hi_mpi_vdec_destroy_chn(ch);
    
    // start vdec;
    if(1)
    {
      printf("%s => start vdec ch:%d, etype:%d, width:%d, height:%d\n", __func__, ch, attr->etype, attr->width, attr->height);
      
      extern hi_vb_src g_vdec_vb_src;
      extern hi_bool g_progressive_en;
      extern hi_vb_pool g_pic_vb_pool[HI_VB_MAX_POOLS];
      extern hi_vb_pool g_tmv_vb_pool[HI_VB_MAX_POOLS];
      hi_vdec_chn_pool pool;
      hi_vdec_chn_attr chn_attr[HI_VDEC_MAX_CHN_NUM];
      hi_vdec_chn_param chn_param;
      hi_vdec_mod_param mod_param;
      hi_mpi_vdec_get_mod_param(&mod_param);

      mod_param.vb_src = g_vdec_vb_src;
      mod_param.pic_mod_param.progressive_en = g_progressive_en;
      hi_mpi_vdec_set_mod_param(&mod_param);
      
      extern hi_void sample_comm_vdec_config_attr(hi_s32 i, hi_vdec_chn_attr *chn_attr, sample_vdec_attr *sample_vdec, hi_u32 arr_len);
      sample_comm_vdec_config_attr(ch, &chn_attr[0], &sample_vdec[0], HI_VDEC_MAX_CHN_NUM);

      CHECK_CHN_RET(hi_mpi_vdec_create_chn(ch, &chn_attr[ch]), ch, "hi_mpi_vdec_create_chn");

      if ((g_vdec_vb_src == HI_VB_SRC_USER) && (ch < HI_VB_MAX_POOLS)) {
          pool.pic_vb_pool = g_pic_vb_pool[ch];
          pool.tmv_vb_pool = g_tmv_vb_pool[ch];
          CHECK_CHN_RET(hi_mpi_vdec_attach_vb_pool(ch, &pool), ch, "hi_mpi_vdec_attach_vb_pool");
      }

      CHECK_CHN_RET(hi_mpi_vdec_get_chn_param(ch, &chn_param), ch, "hi_mpi_vdec_get_chn_param");
      if (sample_vdec[ch].type == HI_PT_H264 || sample_vdec[ch].type == HI_PT_H265) {
          chn_param.video_param.dec_mode = sample_vdec[ch].sapmle_vdec_video.dec_mode;
          chn_param.video_param.compress_mode = HI_COMPRESS_MODE_NONE;
          chn_param.video_param.video_format = HI_VIDEO_FORMAT_TILE_64x16;
          if (chn_param.video_param.dec_mode == HI_VIDEO_DEC_MODE_IPB) {
              chn_param.video_param.out_order = HI_VIDEO_OUT_ORDER_DISPLAY;
          } else {
              chn_param.video_param.out_order = HI_VIDEO_OUT_ORDER_DEC;
          }
      } else {
          chn_param.pic_param.pixel_format = sample_vdec[ch].sapmle_vdec_picture.pixel_format;
          chn_param.pic_param.alpha = sample_vdec[ch].sapmle_vdec_picture.alpha;
      }
      chn_param.display_frame_num = sample_vdec[ch].display_frame_num;
      CHECK_CHN_RET(hi_mpi_vdec_set_chn_param(ch, &chn_param), ch, "hi_mpi_vdec_get_chn_param");

      CHECK_CHN_RET(hi_mpi_vdec_start_recv_stream(ch), ch, "hi_mpi_vdec_start_recv_stream");

      hi_mpi_vdec_set_display_mode(ch, HI_VIDEO_DISPLAY_MODE_PREVIEW);
    }

    // start vpss;
    if(1)
    {
      hi_s32 ret = HI_SUCCESS;
      hi_vpss_grp vpss_grp = ch;
      hi_vpss_chn vpss_chn;
      hi_vpss_grp_attr grp_attr = {0};
      hi_vpss_chn_attr chn_attr[HI_VPSS_MAX_PHYS_CHN_NUM] = {0};
      hi_bool chn_enable[HI_VPSS_MAX_PHYS_CHN_NUM] = {0};

      grp_attr.max_width = 1920;//size->width;
      grp_attr.max_height = 1088;//size->height;
      grp_attr.nr_en = HI_FALSE;
      grp_attr.dei_mode = HI_VPSS_DEI_MODE_OFF;
      grp_attr.pixel_format = SAMPLE_PIXEL_FORMAT;
      grp_attr.frame_rate.src_frame_rate = -1;
      grp_attr.frame_rate.dst_frame_rate = -1;

      for (vpss_chn = 0; vpss_chn < 2; ++vpss_chn) {
          chn_enable[vpss_chn] = HI_TRUE;
          chn_attr[vpss_chn].border_en = HI_TRUE;
          chn_attr[vpss_chn].chn_mode = HI_VPSS_CHN_MODE_USER;
          chn_attr[vpss_chn].pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
          chn_attr[vpss_chn].width = 1920;
          chn_attr[vpss_chn].height = 1088;
          chn_attr[vpss_chn].frame_rate.src_frame_rate = -1;
          chn_attr[vpss_chn].frame_rate.dst_frame_rate = -1;
          chn_attr[vpss_chn].compress_mode = HI_COMPRESS_MODE_NONE;

          chn_attr[vpss_chn].border_attr.color = COLOR_RGB_WHITE;
          chn_attr[vpss_chn].border_attr.top_width = 2;
          chn_attr[vpss_chn].border_attr.bottom_width = 2;
          chn_attr[vpss_chn].border_attr.left_width = 2;
          chn_attr[vpss_chn].border_attr.right_width = 2;
      }
      
      ret = sample_common_vpss_start(vpss_grp, chn_enable, &grp_attr, chn_attr, HI_VPSS_MAX_PHYS_CHN_NUM);
      if (ret != HI_SUCCESS) {
          SAMPLE_PRT("sample_common_vpss_start vpss_grp:%d, failed with %#x!\n", vpss_grp, ret);
      }
        
      // bind  vdec && vpss && vo;
      ret = sample_comm_vdec_bind_vpss(ch, ch);
      ret = sample_comm_vpss_bind_vo(ch, HI_VPSS_CHN0, volayer, ch);
    }
  }
  pthread_mutex_unlock(&vdev->lock);
  
  // send vdec;
  if(1)
  {
    hi_vdec_stream stream = {0};
    stream.pts  = attr->pts;//0;
    stream.addr = data;
    stream.len  = attr->size;
    stream.end_of_frame = HI_TRUE;
    stream.end_of_stream = HI_FALSE;
    stream.need_display = 1;
    s32Ret = hi_mpi_vdec_send_stream(ch, &stream, 0);
    //printf("hi_mpi_vdec_send_stream ch:%d, ret:0x%x\n", ch, s32Ret);
  }

  return err;
}


int gsf_mpp_ao_asend(int aodev, int ch, char *data, gsf_mpp_frm_attr_t *attr)
{
  //audio dec bind vo;
  return 0;
}



//移动整个显示区域(位置,大小)
int gsf_mpp_vo_move(int volayer, hi_rect *rect)
{
  int ret = 0;
  // 
  return ret;
}

//裁剪通道源图像区域到显示通道(局部放大)
int gsf_mpp_vo_crop(int volayer, int ch, hi_rect *rect)
{
  int ret = 0;
  // 
  return ret;
}

//获取解码显示状态
int gsf_mpp_vo_stat(int volayer, int ch, gsf_mpp_vo_stat_t *stat)
{
  int ret = 0;
  //hi_s32 HI_MPI_VO_QueryChnStat(VO_LAYER VoLayer, VO_CHN VoChn, VO_QUERY_STATUS_S *pstStatus);
  return ret;
}
//设置解码显示FPS
int gsf_mpp_vo_setfps(int volayer, int ch, int fps)
{
  int ret = 0;
  // hi_s32 HI_MPI_VO_SetChnFrameRate(VO_LAYER VoLayer, VO_CHN VoChn, hi_s32 s32ChnFrmRate);
  return ret;
}
//清除解码显示BUFF
int gsf_mpp_vo_clear(int volayer, int ch)
{
  int err = 0;

  // reset vdec;
  err = hi_mpi_vdec_stop_recv_stream(ch);
  err = hi_mpi_vdec_reset_chn(ch);
  err = hi_mpi_vdec_start_recv_stream(ch);
  
  // clear vo;
  HI_BOOL bClearAll = HI_TRUE;
  err = hi_mpi_vo_clear_chn_buf(volayer, ch, bClearAll);
  printf("hi_mpi_vo_clear_chn_buf err:%d, ch:%d\n", err, ch);
  return err;
}

//adec;
//设置当前音频数据通道号(!=ch的数据不送解码)
int gsf_mpp_vo_afilter(int vodev, int ch)
{
  int ret = 0;
  return ret;
}
//发送音频数据解码(ch表示音频数据的通道号)
int gsf_mpp_vo_asend(int vodev, int ch, char *data, gsf_mpp_frm_attr_t *attr)
{
  int ret = 0;
  // start adec;
  return ret;
}

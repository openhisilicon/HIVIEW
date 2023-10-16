#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <dlfcn.h>
#include <linux/fb.h>

#include "ot_type.h"
#include "ot_scene.h"
#include "hi_mpi_awb.h"
#include "hi_mpi_ae.h"
#include "hi_mpi_isp.h"
#include "hi_mpi_venc.h"

#include "scene_loadparam.h"
#include "ot_scenecomm_log.h"

#include "sample_comm.h"
//#include "sample_isp.h"
#include "hifb.h"
#include "mpp.h"

//#include "mppex.h"

typedef struct {
  int   type;
  char* name;
  char* lib;
  char* obj;
}SAMPLE_MPP_SENSOR_T;

int SENSOR_TYPE;
int SENSOR0_TYPE;
int SENSOR1_TYPE;
hi_isp_sns_obj* SENSOR_OBJ = NULL;
hi_isp_sns_obj *sample_comm_isp_get_sns_obj(sample_sns_type sns_type)
{
  return SENSOR_OBJ;
}

static SAMPLE_MPP_SENSOR_T libsns[SNS_TYPE_BUTT] = {
    {OV_OS08A20_MIPI_8M_30FPS_12BIT,         "os08a20-0-0-8-30",  "libsns_os08a20.so",    "g_sns_os08a20_obj"},
    {OV_OS08A20_MIPI_8M_30FPS_12BIT_WDR2TO1, "os08a20-0-1-8-30",  "libsns_os08a20.so",    "g_sns_os08a20_obj"}, 
    {OV_OS04A10_MIPI_4M_30FPS_12BIT,         "os04a10-0-0-4-30",  "libsns_os04a10.so",    "g_sns_os04a10_obj"}, 
    {OV_OS08B10_MIPI_8M_30FPS_12BIT,         "os08b10-0-0-8-30",  "libsns_os08b10.so",    "g_sns_os08b10_obj"}, 
    {OV_OS08B10_MIPI_8M_30FPS_12BIT_WDR2TO1, "os08b10-0-1-8-30",  "libsns_os08b10.so",    "g_sns_os08b10_obj"},
    {OV_OS05A10_SLAVE_MIPI_4M_30FPS_12BIT,   "os05a10-0-0-4-30",  "libsns_os05a10_2l_slave.so", "g_sns_os05a10_2l_slave_obj"},
    {SONY_IMX347_SLAVE_MIPI_4M_30FPS_12BIT,  "imx347-0-0-4-30",   "libsns_imx347_slave.so", "g_sns_imx347_slave_obj"},
    {SONY_IMX485_MIPI_8M_30FPS_12BIT,        "imx485-0-0-8-30",   "libsns_imx485.so",     "g_sns_imx485_obj"},
    {SONY_IMX485_MIPI_8M_30FPS_10BIT_WDR3TO1,"imx485-0-1-8-30",   "libsns_imx485.so",     "g_sns_imx485_obj"},
    {SONY_IMX334_MIPI_8M_30FPS_12BIT,        "imx334-0-0-8-30",   "libsns_imx334.so",     "g_sns_imx334_obj"},
    {SONY_IMX334_MIPI_8M_60FPS_12BIT,        "imx334-0-0-8-60",   "libsns_imx334.so",     "g_sns_imx334_obj"},
    {SONY_IMX378_MIPI_8M_30FPS_10BIT,        "imx378-0-0-8-30",   "libsns_imx378.so",     "g_sns_imx378_obj"},
    {SONY_IMX585_MIPI_8M_30FPS_12BIT,        "imx585-0-0-8-30",   "libsns_imx585.so",     "g_sns_imx585_obj"},
    {SONY_IMX482_MIPI_2M_30FPS_12BIT,        "imx482-0-0-2-30",   "libsns_imx482.so",     "g_sns_imx482_obj"},
    {MIPI_YUV422_2M_60FPS_8BIT,              "yuv422-0-0-2-60",     NULL,                 NULL},
    {MIPI_YUV422_half8M_60FPS_8BIT,          "yuv422-1-0-8-60",     NULL,                 NULL},
    {OV_OS04A10_2L_MIPI_4M_30FPS_10BIT,      "os04a10-2-0-4-30",  "libsns_os04a10_2l.so",    "g_sns_os04a10_2l_obj"}, 
  };


static SAMPLE_MPP_SENSOR_T* SAMPLE_MPP_SERSOR_GET(char* name)
{
  int i = 0;
  for(i = 0; i < SNS_TYPE_BUTT; i++)
  {
    //printf("libsns[%d].name:%s, name:%s\n", i, libsns[i].name, name);
    if(strstr(libsns[i].name, name))
    {
      return &libsns[i];
    }
  }
  return NULL;
}


static void * dl = NULL;
static int snscnt = 0;

hi_void sample_venc_handle_sig2(hi_s32 signo)
{
  int ret = 0;
  
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  if (signo == SIGINT || signo == SIGTERM) 
  {
      //mppex_hook_destroy();
      ret = gsf_mpp_scene_stop();
      printf("gsf_mpp_scene_stop ret = %x\n", ret);
      ret = gsf_mpp_venc_dest();
      printf("gsf_mpp_venc_dest ret = %x\n", ret);
      ret = sample_comm_audio_destory_all_trd();
      printf("sample_comm_audio_destory_all_trd ret = %x\n", ret);
      sample_comm_all_isp_stop();
      printf("sample_comm_all_isp_stop\n");
      ret = sample_comm_vo_hdmi_stop();
      printf("sample_comm_vo_hdmi_stop ret = %x\n", ret);
      sample_comm_sys_exit();
      printf("sample_comm_sys_exit\n");
  }
  exit(-1);
}

#include <libgen.h>
#include "hi_common_hnr.h"
static hi_s32 hnr_cnt = 0;
static hi_hnr_cfg hnr_cfg[4] = {{0}};
static hi_vi_pipe hnr_pipe[4] = {0, 1, 2, 3};  /* 1-long frame of wdr */
static hi_vb_pool g_vb_pool[4] = {HI_VB_INVALID_POOL_ID,HI_VB_INVALID_POOL_ID,HI_VB_INVALID_POOL_ID,HI_VB_INVALID_POOL_ID};

//HI_VI_VIDEO_MODE_NORM && normal_blend = HI_TRUE
static sample_hnr_param hnr_param = {.is_wdr_mode = HI_FALSE, .ref_mode = HI_HNR_REF_MODE_NORM, .normal_blend = HI_TRUE, .video_mode = HI_VI_VIDEO_MODE_NORM};

//HI_VI_VIDEO_MODE_ADVANCED
//static sample_hnr_param hnr_param = {.is_wdr_mode = HI_FALSE, .ref_mode = HI_HNR_REF_MODE_NORM, .normal_blend = HI_FALSE, .video_mode = HI_VI_VIDEO_MODE_ADVANCED};


int gsf_mpp_cfg_sns(char *path, gsf_mpp_cfg_t *cfg)
{
  int i = 0;
  char snsstr[64];
  sprintf(snsstr, "%s-%d-%d-%d-%d"
          , cfg->snsname, cfg->lane, cfg->wdr, cfg->res, cfg->fps);

  //mppex_hook_sns(cfg);
  
  SAMPLE_MPP_SENSOR_T* sns = SAMPLE_MPP_SERSOR_GET(snsstr);
  if(!sns)
  {
    printf("%s => snsstr:%s, unknow.\n", __func__, snsstr);
    return -1;
  } 
  
  snscnt = cfg->snscnt;
  char loadstr[256] = {0};
  char snsname[64] = {0};
  
  if(strstr(cfg->snsname, "imx334") || strstr(cfg->snsname, "imx378"))
    strncpy(snsname, "os08a20", sizeof(snsname)-1);
  else if(strstr(cfg->snsname, "imx585") || strstr(cfg->snsname, "imx482"))
    strncpy(snsname, "imx485", sizeof(snsname)-1);
  else  
    strncpy(snsname, cfg->snsname, sizeof(snsname)-1);
    
  sprintf(loadstr, "%s/ko/load_3403v100 -i -sensor0 %s", path, snsname);

  for(i = 1; i < snscnt; i++)
  {
    sprintf(loadstr, "%s -sensor%d %s", loadstr, i, snsname);
  }
  
  if(cfg->second == 1 && cfg->snscnt == 1)
  {
    sprintf(loadstr, "%s -sensor3 bt1120", loadstr);
  }
  
  printf("%s => loadstr: %s\n", __func__, loadstr);
  system(loadstr);
  
  SENSOR_TYPE = SENSOR0_TYPE = SENSOR1_TYPE = sns->type;
  
  if(cfg->second && cfg->snscnt == 1)
  {
    SENSOR1_TYPE = (cfg->second == 1)?BT1120_YUV422_2M_60FPS_8BIT:
                   (cfg->second == 2)?BT656_YUV422_0M_60FPS_8BIT:
                   (cfg->second == 3)?BT601_YUV422_0M_60FPS_8BIT:
                                      T0_RAW_0M_60FPS_16BIT;
  }
  
  hnr_cnt = cfg->hnr;
  

  if(dl)
  {
    dlclose(dl);
    dlerror();
  }
  
  if(sns->lib)
  {
    dl = dlopen(sns->lib, RTLD_LAZY);
    if(dl == NULL)
    {
      printf("err dlopen %s\n", sns->lib);
      goto __err;
    }
    
    SENSOR_OBJ = dlsym(dl, sns->obj);
    if(NULL != dlerror())
    {
        printf("err dlsym %s\n", sns->obj);
        goto __err;
    }
  }
  printf("%s => snsstr:%s, sensor_type:%d, load:%s\n"
        , __func__, snsstr, SENSOR_TYPE, sns->lib?:"");
  
  signal(SIGINT, sample_venc_handle_sig2);
  signal(SIGTERM, sample_venc_handle_sig2);
  
  return 0;
__err:
	if(dl)
	{
    dlclose(dl);
  }
  dlerror();
  return -1;
}

int gsf_mpp_cfg(char *path, gsf_mpp_cfg_t *cfg)
{
  return gsf_mpp_cfg_sns(path, cfg);
}

static hi_u32 vdec_chn_num = 8;
static sample_vdec_attr sample_vdec[HI_VDEC_MAX_CHN_NUM];
static sample_venc_vpss_chn_attr vpss_param;
static sample_venc_vb_attr vb_attr = {0};
static sample_vi_cfg vi_cfg[4];

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
          sample_print("sample_comm_sys_get_pic_size failed!\n");
          return ret;
      }
  }

  //LANE_DIVIDE_MODE_3 for 4x2lane;

  // get vi param
  for(i = 0; i < snscnt; i++)
  {
    sample_comm_vi_get_default_vi_cfg(SENSOR0_TYPE, &vi_cfg[i]);
    if(i != 0)
    {
      const hi_vi_dev vi_dev = 2; /* dev2 for sensor1 */
      const hi_vi_pipe vi_pipe = 1; /* dev2 bind pipe1 */
      
      printf("sample_vi_get_two_sensor_vi_cfg() beg.\n");
      
      vi_cfg[0].mipi_info.divide_mode = LANE_DIVIDE_MODE_1;

      vi_cfg[i].sns_info.bus_id = 5; /* i2c5 */
      vi_cfg[i].sns_info.sns_clk_src = 1;
      vi_cfg[i].sns_info.sns_rst_src = 1;

      sample_comm_vi_get_mipi_info_by_dev_id(SENSOR0_TYPE, vi_dev, &vi_cfg[i].mipi_info);
      vi_cfg[i].dev_info.vi_dev = vi_dev;
      vi_cfg[i].bind_pipe.pipe_id[0] = vi_pipe;
      vi_cfg[i].grp_info.grp_num = 1;
      vi_cfg[i].grp_info.fusion_grp[0] = 1;
      vi_cfg[i].grp_info.fusion_grp_attr[0].pipe_id[0] = vi_pipe;
      printf("sample_vi_get_two_sensor_vi_cfg() end.\n");
    }
  }
  
  if(SENSOR0_TYPE != SENSOR1_TYPE)
  {
    i = 1;
    sample_comm_vi_get_default_vi_cfg(SENSOR1_TYPE, &vi_cfg[i]);

    const hi_vi_dev vi_dev = 3; /* dev3 for bt1120 */
    const hi_vi_pipe vi_pipe = 1; /* dev3 bind pipe1 */
    
    printf("sample_vi_get_bt1120_vi_cfg() beg.\n");
    vi_cfg[i].sns_info.bus_id = 5; /* i2c5 */
    vi_cfg[i].sns_info.sns_clk_src = 1;
    vi_cfg[i].sns_info.sns_rst_src = 1;
    vi_cfg[i].mipi_info.mipi_dev = -1;
    vi_cfg[i].dev_info.vi_dev = vi_dev;
    vi_cfg[i].bind_pipe.pipe_id[0] = vi_pipe;
    vi_cfg[i].grp_info.grp_num = 1;
    vi_cfg[i].grp_info.fusion_grp[0] = 1;
    vi_cfg[i].grp_info.fusion_grp_attr[0].pipe_id[0] = vi_pipe;
    printf("sample_vi_get_bt1120_vi_cfg() end.\n");
  }
  
  // get vpss param
  vi_size = vi_cfg[0].dev_info.dev_attr.in_size;
  extern hi_void get_default_vpss_chn_attr(hi_size *vi_size, hi_size enc_size[], hi_s32 len,
            sample_venc_vpss_chn_attr *vpss_chan_attr);
  get_default_vpss_chn_attr(&vi_size, enc_size, CHN_NUM_MAX, &vpss_param);

  /******************************************
    step 1: init sys alloc common vb
  ******************************************/
  extern hi_void get_vb_attr(const hi_size *vi_size, const sample_venc_vpss_chn_attr *vpss_chn_attr,
            sample_venc_vb_attr *vb_attr);
  get_vb_attr(&vi_size, &vpss_param, &vb_attr);

  //init common VB(for VPSS and VO) from sample_init_sys_and_vb();
  extern hi_s32 get_vb_attr_vdec(hi_u32 vdec_chn_num, sample_venc_vb_attr *vb_attr);
  get_vb_attr_vdec(vdec_chn_num, &vb_attr);
 
  //HI_VB_SUPPLEMENT_BNR_MOT_MASK
  extern hi_s32 sample_venc_sys_init(sample_venc_vb_attr *vb_attr);
  if ((ret = sample_venc_sys_init(&vb_attr)) != HI_SUCCESS) {
      sample_print("Init SYS ret for %#x!\n", ret);
      return ret;
  }
  
  sample_sns_type sns_type = SENSOR0_TYPE;
  
  if(hnr_cnt)
  {
    hi_vi_pipe vi_pipe = 0; /* 1-long frame of wdr */ 
    //HI_VI_VIDEO_MODE_NORM: pipe_attr.compress_mode = HI_COMPRESS_MODE_NONE
    extern hi_void sample_hnr_get_default_cfg(sample_sns_type sns_type, hi_vi_pipe vi_pipe, hi_vi_video_mode video_mode,
              hi_size *size, sample_vi_cfg *vi_cfg);
    sample_hnr_get_default_cfg(sns_type, vi_pipe, hnr_param.video_mode, &vi_size, &vi_cfg[0]);
    
    sample_print("hnr_param.video_mode:%d, normal_blend:%d\n", hnr_param.video_mode, hnr_param.normal_blend);
    
    ret = sample_comm_vi_set_vi_vpss_mode(HI_VI_OFFLINE_VPSS_OFFLINE, hnr_param.video_mode);
    if (ret != HI_SUCCESS) {
      sample_print("Init vpss_mode ret for %#x!\n", ret);
      return ret;
    }
  }
 
  for(i = 0; i < snscnt; i++)
  {
    extern hi_s32 sample_venc_vi_init(sample_vi_cfg *vi_cfg);
    if ((ret = sample_venc_vi_init(&vi_cfg[i])) != HI_SUCCESS) {
        sample_print("Init VI i:%d ret:%#x!\n", i, ret);
        goto EXIT_SYS_STOP;
    }
  }
  
  if(SENSOR0_TYPE != SENSOR1_TYPE)
  {
    i = 1;
    extern hi_s32 sample_venc_vi_init(sample_vi_cfg *vi_cfg);
    if ((ret = sample_venc_vi_init(&vi_cfg[i])) != HI_SUCCESS) {
        sample_print("Init VI i:%d ret:%#x!\n", i, ret);
        goto EXIT_SYS_STOP;
    }
  }

  for(i = 0; i < hnr_cnt; i++)
  {
    hi_s32 blk_size;
    hi_vb_pool vb_pool = HI_VB_INVALID_POOL_ID;
    hi_vb_pool_cfg vb_pool_cfg = {0};
    
    //if(i == 0)
    {
      blk_size = hi_hnr_get_pic_buf_size(vi_size.width, vi_size.height);
      vb_pool_cfg.blk_size = blk_size;
      vb_pool_cfg.blk_cnt = 10*2; /* 12: vb cnt */
      vb_pool_cfg.remap_mode = HI_VB_REMAP_MODE_NONE;
      vb_pool = hi_mpi_vb_create_pool(&vb_pool_cfg);
      if (vb_pool == HI_VB_INVALID_POOL_ID) {
          sample_print("create user pool failed!\n");
          return HI_FAILURE;
      }
    }
    
    hi_vi_pipe vi_pipe = hnr_pipe[i];
    ret = hi_mpi_vi_pipe_set_vb_src(vi_pipe, HI_VB_SRC_USER);
    if (ret != HI_SUCCESS) {
        sample_print("vi pipe set vb src failed.\n");
        return ret;
    }

    ret = hi_mpi_vi_pipe_attach_vb_pool(vi_pipe, vb_pool);
    if (ret != HI_SUCCESS) {
        sample_print("vi pipe attach vb pool failed.\n");
        return ret;
    }

    g_vb_pool[i] = (vb_pool)?vb_pool:g_vb_pool[i];
 
 #if 0 // g_sns_imx485_obj.pfn_set_blc_clamp
    sample_sns_type sns_type = SENSOR0_TYPE;
    extern hi_void sample_hnr_sensor_cfg(hi_vi_pipe vi_pipe, sample_sns_type sns_type);
    sample_hnr_sensor_cfg(vi_pipe, sns_type);
 #endif   
  }

  return ret;

EXIT_SYS_STOP:
    sample_comm_sys_exit();
  return ret;
}

int gsf_mpp_vi_stop()
{
  int ret = 0, i = 0;
  extern hi_void sample_venc_vi_deinit(sample_vi_cfg *vi_cfg);
  
  for(i = 0; i < snscnt; i++)
  {
    sample_venc_vi_deinit(&vi_cfg[i]);
  
    if (g_vb_pool[i] != HI_VB_INVALID_POOL_ID) {
      hi_mpi_vb_destroy_pool(g_vb_pool[i]);
      g_vb_pool[i] = HI_VB_INVALID_POOL_ID;
    }
  }
  
  if(SENSOR0_TYPE != SENSOR1_TYPE)
  {
    i = 1;
    sample_venc_vi_deinit(&vi_cfg[i]);
  
    if (g_vb_pool[i] != HI_VB_INVALID_POOL_ID) {
      hi_mpi_vb_destroy_pool(g_vb_pool[i]);
      g_vb_pool[i] = HI_VB_INVALID_POOL_ID;
    }
  }

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
  
  if(vi_pipe == 1 && SENSOR0_TYPE != SENSOR1_TYPE)
  {
    for(int i = 0; i < HI_VPSS_MAX_PHYS_CHN_NUM; i++)
    {
      hi_size enc_size = {0};
      ret = sample_comm_sys_get_pic_size(vpss->enSize[i], &enc_size);
      vpss_param.enable[i] = vpss->enable[i]; 
      vpss_param.output_size[i].width = enc_size.width;
      vpss_param.output_size[i].height = enc_size.height;
    }
    vpss_param.pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_422;
  }
  
  extern hi_s32 sample_venc_vpss_init(hi_vpss_grp vpss_grp, sample_venc_vpss_chn_attr *vpss_chan_cfg);
  if ((ret = sample_venc_vpss_init(vpss_grp, &vpss_param)) != HI_SUCCESS) {
      sample_print("Init VPSS err for %#x!\n", ret);
      goto EXIT;
  }
  
  if ((ret = sample_comm_vi_bind_vpss(vi_pipe, vi_chn, vpss_grp, 0)) != HI_SUCCESS) 
  {
      sample_print("VI Bind VPSS err for %#x!\n", ret);
      goto EXIT_VPSS_STOP;
  }
  
  if(hnr_cnt)
  {
    hi_low_delay_info low_delay_info;
    low_delay_info.enable     = HI_TRUE;
    low_delay_info.line_cnt   = 200; /* 200: lowdelay line */
    low_delay_info.one_buf_en = HI_FALSE;
    ret = hi_mpi_vpss_set_low_delay_attr(vpss_grp, 0, &low_delay_info);
    if (ret != HI_SUCCESS) {
      sample_print("Init VPSS low_delay err for %#x!\n", ret);
    }
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

//启动编码通道
int gsf_mpp_venc_start(gsf_mpp_venc_t *venc)
{
  int ret = 0;
  hi_venc_gop_mode gop_mode;
  hi_venc_gop_attr gop_attr;
  
  hi_vpss_grp vpss_grp = venc->VpssGrp;
  hi_vpss_chn vpss_chn = venc->VpssChn;
  hi_venc_chn venc_chn = venc->VencChn;
  
  sample_comm_venc_chn_param venc_create_param = {0};

  gop_mode = HI_VENC_GOP_MODE_NORMAL_P;
  if ((ret = sample_comm_venc_get_gop_attr(gop_mode, &gop_attr)) != HI_SUCCESS) {
      sample_print("Venc Get GopAttr for %#x!\n", ret);
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
      sample_print("Venc Start failed for %#x!\n", ret);
      goto EXIT;
  }

  if ((ret = sample_comm_vpss_bind_venc(vpss_grp, vpss_chn, venc_chn)) != HI_SUCCESS) {
      sample_print("sample_comm_vpss_bind_venc failed for %#x!\n", ret);
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
      sample_print("Start Venc failed!\n");
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
  return sample_comm_venc_snap_processCB(VencChn, SnapCnt, cb, u);
}

static int g_scenebEnable = 0;

int gsf_mpp_scene_start(char *path, int scenemode)
{
    char _path[256] = {0};
    
    strncpy(_path, path, sizeof(_path)-1);
    strcat(_path, (snscnt==2)?".2ch":"");
    
    char *dir_name = dirname(_path);
    
    sample_sns_type sns_type = SENSOR0_TYPE;
    hi_size in_size;
  
    for(int i = 0; i < hnr_cnt; i++)
    {
      hi_vi_pipe vi_pipe = hnr_pipe[i];  /* 1-long frame of wdr */
      hi_vi_pipe master_pipe = vi_pipe;  //0;
            
      extern int sample_hnr_set_bin_path(char *path);
      sample_hnr_set_bin_path(dir_name); //for normal_hnr.bin file;
      
      sample_comm_vi_get_size_by_sns_type(sns_type, &in_size);

      extern hi_s32 sample_hnr_set_blc(hi_vi_pipe vi_pipe, sample_sns_type sns_type);
      if (sample_hnr_set_blc(master_pipe, sns_type) != HI_SUCCESS)
      {
        printf("sample_hnr_set_blc failed\n");
      }

      extern hi_s32 sample_hnr_start(hi_vi_pipe vi_pipe, hi_size in_size, hi_hnr_cfg *hnr_cfg, sample_hnr_param *hnr_param);
      if (sample_hnr_start(vi_pipe, in_size, &hnr_cfg[i], &hnr_param) != HI_SUCCESS) 
      {
        printf("sample_hnr_start failed\n");
      }
      extern hi_s32 sample_hnr_set_long_frame_mode(hi_vi_pipe vi_pipe, hi_bool is_wdr_mode);
      if (sample_hnr_set_long_frame_mode(master_pipe, hnr_param.is_wdr_mode) != HI_SUCCESS)
      {
        printf("sample_hnr_set_long_frame_mode failed\n");
      }
    }
    
    HI_S32 ret = HI_SUCCESS;
    ot_scene_param stSceneParam;
    ot_scene_video_mode stVideoMode;
    
    set_dir_name(dir_name); // for FPN_frame_xxx.raw file;
    
    ret = ot_scene_create_param(path, &stSceneParam, &stVideoMode);
    if (ret != HI_SUCCESS) {
        printf("SCENETOOL_CreateParam failed\n");
        return HI_FAILURE;
    }

    ret = ot_scene_init(&stSceneParam);
    if (ret != HI_SUCCESS) {
        printf("ot_scene_init failed\n");
        return HI_FAILURE;
    }

    ret = ot_scene_set_scene_mode(&(stVideoMode.video_mode[scenemode]));
    if (ret != HI_SUCCESS) {
        printf("HI_SRDK_SCENEAUTO_Start failed\n");
        return HI_FAILURE;
    }

    printf("The sceneauto is started, hnr_cnt:%d\n", hnr_cnt);
    g_scenebEnable = 1;
    return ret;
}
int gsf_mpp_scene_stop()
{
  hi_s32 ret = ot_scene_deinit();
  if (ret != HI_SUCCESS) {
      printf("ot_scene_deinit failed\n");
  }
  for(int i = 0; i < hnr_cnt; i++)
  {
    extern hi_s32 sample_hnr_stop(hi_vi_pipe vi_pipe, hi_hnr_cfg *hnr_cfg, sample_hnr_param *hnr_param);
    sample_hnr_stop(hnr_pipe[i], &hnr_cfg[i], &hnr_param);
  }
  printf("The scene sceneauto is stop, hnr_cnt:%d\n", hnr_cnt);
  return ret;
}


int gsf_mpp_isp_ctl(int ViPipe, int id, void *args)
{
  int ret = -1;
  
  if(ViPipe < 0 || ViPipe >= 4)
    return ret;
      
  switch(id)
  {
    case GSF_MPP_ISP_CTL_IMG:
      {
        gsf_mpp_img_all_t *all = (gsf_mpp_img_all_t*)args;
        
        all->scene.bEnable = g_scenebEnable;
        
        hi_isp_exposure_attr exp_attr;
        ret = hi_mpi_isp_get_exposure_attr(ViPipe, &exp_attr);
        all->ae.u8Speed         = exp_attr.auto_attr.speed;
        all->ae.u8Compensation  = exp_attr.auto_attr.compensation;
        all->ae.SysGainRangeMax = exp_attr.auto_attr.sys_gain_range.max;
        all->ae.SysGainRangeMin = exp_attr.auto_attr.sys_gain_range.min;
        all->ae.ExpTimeRangeMax = exp_attr.auto_attr.exp_time_range.max;
        all->ae.ExpTimeRangeMin = exp_attr.auto_attr.exp_time_range.min;

        hi_isp_sharpen_attr shp_attr;
        ret = hi_mpi_isp_get_sharpen_attr(ViPipe, &shp_attr);
        all->sharpen.u16TextureFreq = shp_attr.manual_attr.texture_freq;
        all->sharpen.u16EdgeFreq    = shp_attr.manual_attr.edge_freq;   
        all->sharpen.u8DetailCtrl   = shp_attr.manual_attr.detail_ctrl; 
        
        printf("all->scene.bEnable:%d\n", all->scene.bEnable);
      }
      break;
    case GSF_MPP_ISP_CTL_AE:
      {
        gsf_mpp_img_ae_t *ae = (gsf_mpp_img_ae_t*)args;
        
        printf("ae->bEnable:%d, u8Compensation:%d\n", ae->bEnable, ae->u8Compensation);  
        if(!ae->bEnable)
        {
          ret = 0;  
          break;
        }

        hi_isp_exposure_attr exp_attr;
        ret = hi_mpi_isp_get_exposure_attr(ViPipe, &exp_attr);

        //exp_attr.bypass = 0;
        exp_attr.auto_attr.speed              = ae->u8Speed;
        exp_attr.auto_attr.compensation       = ae->u8Compensation;
        exp_attr.auto_attr.sys_gain_range.max = ae->SysGainRangeMax;
        exp_attr.auto_attr.sys_gain_range.min = ae->SysGainRangeMin;
        exp_attr.auto_attr.exp_time_range.max = ae->ExpTimeRangeMax;
        exp_attr.auto_attr.exp_time_range.min = ae->ExpTimeRangeMin;
        
        ret = hi_mpi_isp_set_exposure_attr(ViPipe, &exp_attr);
      }
      break;
     case GSF_MPP_ISP_CTL_SHARPEN:
      {
        gsf_mpp_img_sharpen_t *sharpen = (gsf_mpp_img_sharpen_t*)args;

        printf("sharpen->bEnable:%d,u16TextureFreq:%d\n", sharpen->bEnable, sharpen->u16TextureFreq);  

        hi_isp_sharpen_attr shp_attr;
        ret = hi_mpi_isp_get_sharpen_attr(ViPipe, &shp_attr);
        
        //shp_attr.en = HI_TRUE;
        shp_attr.op_type = (sharpen->bEnable)?HI_OP_MODE_MANUAL:HI_OP_MODE_AUTO;
        shp_attr.manual_attr.texture_freq   = sharpen->u16TextureFreq;
        shp_attr.manual_attr.edge_freq      = sharpen->u16EdgeFreq;
        shp_attr.manual_attr.detail_ctrl    = sharpen->u8DetailCtrl;
        ret = hi_mpi_isp_set_sharpen_attr(ViPipe, &shp_attr);
      }
      break;       
     case GSF_MPP_ISP_CTL_FLIP:
      {
        gsf_mpp_img_flip_t *flip = (gsf_mpp_img_flip_t*)args;

        hi_vi_chn_attr chn_attr;
        ret = hi_mpi_vi_get_chn_attr(ViPipe, 0, &chn_attr);
        printf("GET ret:0x%x, flip_en:%d, mirror_en:%d\n", ret, chn_attr.flip_en, chn_attr.mirror_en);

        chn_attr.flip_en = flip->bFlip;
        chn_attr.mirror_en = flip->bMirror;

        ret = hi_mpi_vi_set_chn_attr(ViPipe, 0, &chn_attr);
        printf("SET ret:0x%x, flip_en:%d, mirror_en:%d\n", ret, chn_attr.flip_en, chn_attr.mirror_en);
      }
      break;

     default:
     break;
  }
  return ret;
}



int gsf_mpp_scene_ctl(int ViPipe, int id, void *args)
{
  int ret = -1;
  
  if(ViPipe < 0 || ViPipe >= 4)
    return ret;
    
  switch(id)
  {
    case GSF_MPP_SCENE_CTL_AE:
      { 
        extern HI_SCENE_CTL_AE_S g_scene_ctl_ae[4];
        g_scene_ctl_ae[ViPipe] = *((HI_SCENE_CTL_AE_S*)args);
        ret = 0;
        printf("g_scene_ctl_ae[%d]: %0.2f\n", ViPipe, g_scene_ctl_ae[ViPipe].compensation_mul);
      }
      break;
  }
  return ret;
}


static gsf_mpp_aenc_t _aenc;
int gsf_mpp_audio_start(gsf_mpp_aenc_t *aenc)
{
  int ret = 0;
  static int init = 0;
  
  if(!init)
  {
    init = 1;
    extern hi_s32 sample_audio_init();
    sample_audio_init();
  }  
  if(aenc == NULL)
  {
    printf("test sample_audio_ai_ao\n");
    static pthread_t aio_pid;
    extern hi_s32 sample_audio_ai_ao(hi_void);
    return pthread_create(&aio_pid, 0, (void*(*)(void*))sample_audio_ai_ao, NULL);
  }
  else
  {
    //extern hi_s32 sample_audio_ai_aenc(gsf_mpp_aenc_t *aenc);
    //_aenc = *aenc;
    //return sample_audio_ai_aenc(aenc);
    printf("unimplemented sample_audio_ai_aenc\n");
  }
  return 0;
  
  return ret;
}
int gsf_mpp_audio_stop(gsf_mpp_aenc_t  *aenc)
{
  int ret = 0;
  if(aenc == NULL)
  {
    ;  
  }
  else
  {
    //extern hi_s32 sample_audio_ai_aenc_stoop(gsf_mpp_aenc_t *aenc);
    //return sample_audio_ai_aenc_stoop(aenc);
    printf("unimplemented sample_audio_ai_aenc_stoop\n");
  }
  return ret;
}


int gsf_mpp_rgn_ctl(RGN_HANDLE Handle, int id, gsf_mpp_rgn_t *rgn)
{
  int ret = 0;
  switch(id)
  {
    case GSF_MPP_RGN_CREATE:
      ret = hi_mpi_rgn_create(Handle, &rgn->stRegion);
      break;
    case GSF_MPP_RGN_DESTROY:
      ret = hi_mpi_rgn_destroy(Handle);
      break;
    case GSF_MPP_RGN_SETATTR:
      ret = hi_mpi_rgn_set_attr(Handle, &rgn->stRegion);
      break;
    case GSF_MPP_RGN_GETATTR:
      ret = hi_mpi_rgn_get_attr(Handle, &rgn->stRegion);
      break;
    case GSF_MPP_RGN_ATTACH:
      ret = hi_mpi_rgn_attach_to_chn(Handle, &rgn->stChn, &rgn->stChnAttr);
      break;
    case GSF_MPP_RGN_DETACH:
      ret = hi_mpi_rgn_detach_from_chn(Handle, &rgn->stChn);
      break;
    case GSF_MPP_RGN_SETDISPLAY:
      ret = hi_mpi_rgn_set_display_attr(Handle, &rgn->stChn, &rgn->stChnAttr);
      break;
    case GSF_MPP_RGN_GETDISPLAY:
      ret = hi_mpi_rgn_get_display_attr(Handle, &rgn->stChn, &rgn->stChnAttr);
      break;
  }
  return ret;
}
int gsf_mpp_rgn_bitmap(RGN_HANDLE Handle, BITMAP_S *bitmap)
{
  int ret = 0;
  ret = hi_mpi_rgn_set_bmp(Handle, bitmap);
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
  sample_vo_cfg vo_config;
  
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
        sample_print("init mod vb fail for %#x!\n", ret);
        return ret;
    }
    #endif
    
    /************************************************
    step4:  start VO
    *************************************************/
    //maohw
    vo_mng[vodev].vo_config.vo_intf_type      = type;
    vo_mng[vodev].vo_config.intf_sync         = sync;
    vo_mng[vodev].vo_config.pic_size          = (sync == HI_VO_OUT_1080P60)?PIC_1080P:PIC_3840X2160;
    
    extern hi_s32 sample_start_vo(sample_vo_cfg *vo_config, hi_u32 vpss_grp_num, hi_bool is_pip);
    ret = sample_start_vo(&vo_mng[vodev].vo_config, vpss_grp_num, HI_FALSE);
    if (ret != HI_SUCCESS) {
        printf("sample_start_vo err ret:%d\n", ret);
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
      //extern hi_bool g_progressive_en;
      extern hi_vb_pool g_pic_vb_pool[HI_VB_MAX_POOLS];
      extern hi_vb_pool g_tmv_vb_pool[HI_VB_MAX_POOLS];
      hi_vdec_chn_pool pool;
      hi_vdec_chn_attr chn_attr[HI_VDEC_MAX_CHN_NUM];
      hi_vdec_chn_param chn_param;
      hi_vdec_mod_param mod_param;
      hi_mpi_vdec_get_mod_param(&mod_param);

      mod_param.vb_src = g_vdec_vb_src;
      //mod_param.pic_mod_param.progressive_en = g_progressive_en;
      hi_mpi_vdec_set_mod_param(&mod_param);
      
      extern hi_void sample_comm_vdec_config_attr(hi_s32 i, hi_vdec_chn_attr *chn_attr, sample_vdec_attr *sample_vdec, hi_u32 arr_len);
      sample_comm_vdec_config_attr(ch, &chn_attr[0], &sample_vdec[0], HI_VDEC_MAX_CHN_NUM);

      check_chn_return(hi_mpi_vdec_create_chn(ch, &chn_attr[ch]), ch, "hi_mpi_vdec_create_chn");

      if ((g_vdec_vb_src == HI_VB_SRC_USER) && (ch < HI_VB_MAX_POOLS)) {
          pool.pic_vb_pool = g_pic_vb_pool[ch];
          pool.tmv_vb_pool = g_tmv_vb_pool[ch];
          check_chn_return(hi_mpi_vdec_attach_vb_pool(ch, &pool), ch, "hi_mpi_vdec_attach_vb_pool");
      }

      check_chn_return(hi_mpi_vdec_get_chn_param(ch, &chn_param), ch, "hi_mpi_vdec_get_chn_param");
      if (sample_vdec[ch].type == HI_PT_H264 || sample_vdec[ch].type == HI_PT_H265) {
          chn_param.video_param.dec_mode = sample_vdec[ch].sample_vdec_video.dec_mode;
          chn_param.video_param.compress_mode = HI_COMPRESS_MODE_NONE;
          chn_param.video_param.video_format = HI_VIDEO_FORMAT_TILE_64x16;
          if (chn_param.video_param.dec_mode == HI_VIDEO_DEC_MODE_IPB) {
              chn_param.video_param.out_order = HI_VIDEO_OUT_ORDER_DISPLAY;
          } else {
              chn_param.video_param.out_order = HI_VIDEO_OUT_ORDER_DEC;
          }
      } else {
          chn_param.pic_param.pixel_format = sample_vdec[ch].sample_vdec_picture.pixel_format;
          chn_param.pic_param.alpha = sample_vdec[ch].sample_vdec_picture.alpha;
      }
      chn_param.display_frame_num = sample_vdec[ch].display_frame_num;
      check_chn_return(hi_mpi_vdec_set_chn_param(ch, &chn_param), ch, "hi_mpi_vdec_get_chn_param");

      check_chn_return(hi_mpi_vdec_start_recv_stream(ch), ch, "hi_mpi_vdec_start_recv_stream");

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
          sample_print("sample_common_vpss_start vpss_grp:%d, failed with %#x!\n", vpss_grp, ret);
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

//设置当前音频数据通道号(!=ch的数据不送解码)
int gsf_mpp_ao_filter(int vodev, int ch)
{
  int ret = 0;
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


int gsf_mpp_vo_bind(int volayer, int ch, gsf_mpp_vo_src_t *src)
{
  HI_S32 s32Ret = HI_SUCCESS;
  vo_mng_t *vdev = &vo_mng[layer2vdev[volayer]];
   
  pthread_mutex_lock(&vdev->lock);

  // unbind vo && vpss && vdec;
  int vpss_grp = vdev->layer[volayer].chs[ch].src_grp;
  int vpss_chn = vdev->layer[volayer].chs[ch].src_chn;
  
  if(vpss_grp != -1)
  {
    sample_comm_vpss_un_bind_vo(vpss_grp, vpss_chn, volayer, ch);
    if(vdev->layer[volayer].chs[ch].src_type >= VO_SRC_VDVP)
    {
      // stop vpss;
      sample_comm_vdec_un_bind_vpss(ch, vpss_grp);
      HI_BOOL chen[VPSS_MAX_PHY_CHN_NUM] = {HI_TRUE,HI_TRUE};
      sample_common_vpss_stop(vpss_grp, chen, HI_VPSS_MAX_CHN_NUM);
      
      //stop vdec;
      hi_mpi_vdec_stop_recv_stream(ch);
      hi_mpi_vdec_destroy_chn(ch);
    }
  }
  // only set width = height = 0, tell gsf_mpp_vo_vsend to recreate vdec&vpss;
  vdev->layer[volayer].chs[ch].width = vdev->layer[volayer].chs[ch].height = 0;
  vdev->layer[volayer].chs[ch].src_grp = src->VpssGrp;
  vdev->layer[volayer].chs[ch].src_chn = src->VpssChn;
  vdev->layer[volayer].chs[ch].src_type = VO_SRC_VP;
  s32Ret = sample_comm_vpss_bind_vo(src->VpssGrp, src->VpssChn, volayer, ch);
  
  pthread_mutex_unlock(&vdev->lock);
  return 0;
}

//audio ao_bind_ai;
int gsf_mpp_ao_bind(int aodev, int ch, int aidev, int aich)
{
  int ret = 0;
  return ret;
}

//设置通道源图像裁剪区域(用于局部放大)
int gsf_mpp_vo_crop(int volayer, int ch, RECT_S *rect)
{
  int ret = 0;
  return ret;
}

//设置VO通道显示区域比例(rect返回调整后的位置)
int gsf_mpp_vo_aspect(int volayer, int ch, RECT_S *rect)
{
  int ret = 0;
  return ret;
}

//设置VO通道显示区域(位置&大小);
int gsf_mpp_vo_rect(int volayer, int ch, RECT_S *rect, int priority)
{
  int ret = 0;
  return ret;
}

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
        default:
            strcpy(file, "/dev/fb0");
            break;
    }
  
    /* 1. open framebuffer device overlay 0 */
    int fd = open(file, O_RDWR, 0);
    if(fd < 0)
    {
        sample_print("open %s failed!\n",file);
        return -1;
    } 

    HI_BOOL bShow = HI_FALSE;
    if (ioctl(fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
    {
        sample_print("FBIOPUT_SHOW_HIFB failed!\n");
        return -1;
    }
    #if 0
    HIFB_CAPABILITY_S stCap;
    if ( ioctl(fd, FBIOGET_CAPABILITY_HIFB, &stCap) < 0)
    {
        sample_print("FBIOGET_CAPABILITY_HIFB failed!\n");
        return -1;
    }
    #endif
    /* 2. set the screen original position */
    hi_fb_point stPoint = {0, 0};
    stPoint.x_pos = 0;
    stPoint.y_pos = 0;

    if (ioctl(fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0)
    {
        sample_print("set screen original show position failed!\n");
        close(fd);
        return -1;
    }

    /* 3. get the variable screen info */
    struct fb_var_screeninfo var;
    if (ioctl(fd, FBIOGET_VSCREENINFO, &var) < 0)
    {
        sample_print("Get variable screen info failed!\n");
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
            var.xres_virtual = 48;
            var.yres_virtual = 48;
            var.xres = 48;
            var.yres = 48;
            break;
        default:
            break;
    }
    #if 1
    #warning "FB_COLOR_DEPTH == 32"
    var.transp= s_a32;
    var.red   = s_r32;
    var.green = s_g32;
    var.blue  = s_b32;
    var.bits_per_pixel = 32;
    #else
    #warning "FB_COLOR_DEPTH == 16"
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
        sample_print("Put variable screen info failed!\n");
		    close(fd);
        return -1;
    }
     
    /* 6. get the fix screen info */
    struct fb_fix_screeninfo fix;
    if (ioctl(fd, FBIOGET_FSCREENINFO, &fix) < 0)
    {
        sample_print("Get fix screen info failed!\n");
		    close(fd);
        return -1;
    }
    int u32FixScreenStride = fix.line_length;   /*fix screen stride*/
    
    hi_fb_alpha stAlpha={0};
    if (ioctl(fd, FBIOGET_ALPHA_HIFB,  &stAlpha))
    {
        sample_print("Get alpha failed!\n");
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
        sample_print("Set alpha failed!\n");
        close(fd);
        return -1;
    }

    hi_fb_colorkey stColorKey;
    stColorKey.enable = HI_FALSE;
    stColorKey.value = 0x0000;
    if (ioctl(fd, FBIOPUT_COLORKEY_HIFB, &stColorKey) < 0)
    {
        sample_print("FBIOPUT_COLORKEY_HIFB failed!\n");
        close(fd);
        return -1;
    }

    /* 7. map the physical video memory for user use */
    HI_U8 *pShowScreen = mmap(HI_NULL, fix.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(MAP_FAILED == pShowScreen)
    {
        sample_print("mmap framebuffer failed!\n");
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
          sample_print("FBIOPUT_SHOW_HIFB failed!\n");
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

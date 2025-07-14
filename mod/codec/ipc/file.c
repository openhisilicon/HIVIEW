#if defined(GSF_CPU_3516d) || defined(GSF_CPU_3559) || defined(GSF_CPU_3519d)

#include "codec.h"
#include "cfg.h"
#include "msg_func.h"
#include "mpp.h"

#include "fw/comm/inc/proc.h"
#include "mod/rec/src/file.h"
#include "mod/rec/src/file.c"

#define PT_VENC(t) \
            (t == GSF_ENC_H264)? PT_H264:\
            (t == GSF_ENC_H265)? PT_H265:\
            (t == GSF_ENC_JPEG)? PT_JPEG:\
            (t == GSF_ENC_MJPEG)? PT_MJPEG:\
            PT_H264

#define FRAME_MAX_SIZE (1000*1024)
static int _task_runing = 1;
static void* vdec_task(void *param)
{
  char *filename = (char*)param;
  char * frame_buf = (char *)malloc(FRAME_MAX_SIZE);
	if (frame_buf == NULL)
	{
		printf("frame_buf malloc failed\n");
		return NULL;
	}
	
  rec_media_info_t media;
	fd_av_t* fd = fd_av_ropen(filename, 0, &media);
	if (fd != NULL)
	{
		printf("open success [%s]\n",filename);
		printf("media.aenc : %d; media.venc: %d ;\n",media.aenc, media.venc);		
		printf("media.v.w: %d; media.v.h: %d;  media.v.fps: %d\n",media.v.w,media.v.h,media.v.fps);
		printf("media.a.sp: %d; media.a.bps: %d; miedia.a.chs: %d\n",media.a.sp,media.a.bps,media.a.chs);
		printf("vtrack : %d; atrack: %d\n",fd->vtrack,fd->atrack);
	}
	else 
  {
		printf("open failed [%s]\n", filename);
		return NULL;
  }
  
  while(_task_runing)
  {
    //从MP4文件中读取，一帧数据 
    rec_rw_info_t info;
    int frame_size = FRAME_MAX_SIZE;
	  int ret = fd_av_rread(fd, frame_buf, &frame_size, &info);
	  if(ret < 0) //文件读取结束
	  {
	    printf("rread end.\n");
	    break;
	  }
    //printf("rread ret:%d, info.type:%d, enc:%d, ms:%u, size:%d\n", ret, info.type, info.enc, info.ms, frame_size);
            
	  if(info.enc == GSF_ENC_H264 || info.enc == GSF_ENC_H265)
	  {
  	  gsf_mpp_frm_attr_t attr = {0};
      attr.size   = frame_size;   // data size;
      attr.ftype  = info.type;    // frame type;
      attr.pts    = info.ms*1000; // pts ms*1000;
      attr.etype  = PT_VENC(info.enc);// PAYLOAD_TYPE_E;
      attr.width  = info.v.w;     // width;
      attr.height = info.v.h;     // height;
    
  	  ret = gsf_mpp_vo_vsend(VOLAYER_HD0, 1, 0, frame_buf, &attr);
  	  printf("vsend ret:%d, video size:%d, pts:%llu\n", ret, attr.size, attr.pts);
  	  
  	  usleep(20*1000);  //info.v.fps;
    }
  }
  return NULL;
}

static pthread_t pid;
int file_vdec(void)
{
  //////// test file vdec => vo //////// 
  static char mp4_filename[256] = {0};
  proc_absolute_path(mp4_filename);
  strcat(mp4_filename, "/../cfg/video.mp4");
  if (access(mp4_filename, 0) < 0)
  {
    return -1;
  }

  int ly = VO_LAYOUT_2MUX;      
  gsf_mpp_vo_layout(VOLAYER_HD0, ly, NULL);
  extern int vo_ly_set(int ly);
  vo_ly_set(ly);
  gsf_mpp_vo_src_t src0 = {0, 0};
  gsf_mpp_vo_bind(VOLAYER_HD0, 0, &src0);

  //for vo_1 full screen;
  gsf_resolu_t res;
  extern int vo_res_get(gsf_resolu_t *res);
  vo_res_get(&res);  

  #if 1 //show sensor 
  RECT_S vo_0 = {res.w/2, res.h/2, res.w/2, res.h/2};
  gsf_mpp_vo_rect(VOLAYER_HD0, 0, &vo_0, 1); // vo_0 for sensor;
  #endif
  
  RECT_S vo_1 = {0, 0, res.w, res.h};
  gsf_mpp_vo_rect(VOLAYER_HD0, 1, &vo_1, 0); // vo_1 for vdec;

  return pthread_create(&pid, 0, vdec_task, (void*)mp4_filename);
}


#endif // defined(GSF_CPU_3516d) || defined(GSF_CPU_3559) || defined(GSF_CPU_3519d)
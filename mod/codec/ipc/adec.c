//Test audio decoding and output

#if defined(GSF_CPU_3516d) || defined(GSF_CPU_3559) || defined(GSF_CPU_3519d)  || defined(GSF_CPU_3516c)

#include "fw/comm/inc/proc.h"
#include "codec.h"
#include "cfg.h"
#include "msg_func.h"
#include "mpp.h"

#include "inc/frm.h"

#if defined(GSF_CPU_3519d) || defined(GSF_CPU_3516c)
static int aodev = SAMPLE_AUDIO_INNER_AO_DEV;
#else
static int aodev = SAMPLE_AUDIO_INNER_HDMI_AO_DEV;
#endif

static int _task_runing = 1;
void* adec_task(void *param)
{
  char *filename = (char*)param;
  FILE* pfd = fopen(filename, "rb");
  if(!pfd)
    return NULL;
  
  HI_U32 s32Ret = 0, ch = 0;
  HI_U32 u32Len = 640;
  HI_U8 u8Data[640];
  
  //wait hdmi-display screen;
  sleep(5);

  while(_task_runing)
  {
    gsf_mpp_frm_attr_t attr = {0};
    attr.etype = PT_AAC;
    
    HI_U32 u32ReadLen = fread(u8Data, 1, u32Len, pfd);
    if (u32ReadLen <= 0)
    {
      attr.size = 0;
      s32Ret = gsf_mpp_ao_asend(aodev, ch, 1, u8Data, &attr);
      printf("file EOF.\n");
      break;
    }
    
    attr.size = u32ReadLen;
    s32Ret = gsf_mpp_ao_asend(aodev, ch, 1, u8Data, &attr);
    if (HI_SUCCESS != s32Ret)
    {
      printf("asend err ret:%d\n", s32Ret);
      break;
    }
  }
  return NULL;
}

static pthread_t pid;
int adec_start()
{
  //////// test audio dec => ao //////// 
  static char adec_filename[256] = {0};
  proc_absolute_path(adec_filename);
  strcat(adec_filename, "/../cfg/audio2ch48k.mpa"); //aac-lc;
  if (access(adec_filename, 0) < 0)
  {
    return -1;
  }  

  printf("test audio dec => ao from adec_filename:[%s]\n", adec_filename);
  return pthread_create(&pid, 0, adec_task, (void*)adec_filename);
}

#else
int adec_start() {return -1;}
#endif // defined(GSF_CPU_3516d) || defined(GSF_CPU_3559)
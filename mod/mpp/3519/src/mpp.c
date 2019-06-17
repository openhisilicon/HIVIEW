#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

#include "sample_comm.h"
#include "mpp.h"

int SENSOR_TYPE;
ISP_SNS_OBJ_S* g_pstSnsObj[2];

int SAMPLE_COMM_MPP_Cfg(SAMPLE_MPP_CONFIG_S *cfg)
{
  SENSOR_TYPE = cfg->sensor_type;

  char* libsns[128][2] = {
    [SONY_IMX290_MIPI_1080P_30FPS] = {"libsns_imx290.so", "stSnsImx290Obj"},
  };

  if(!libsns[SENSOR_TYPE][0])
    return -1;
  
  
  void * dl = dlopen(libsns[SENSOR_TYPE][0], RTLD_LAZY);
  if(dl == NULL)
  {
    printf("err dlopen %s\n", libsns[SENSOR_TYPE][0]);
    goto __err;
  }
  
  g_pstSnsObj[0] = g_pstSnsObj[1] = dlsym(dl, libsns[SENSOR_TYPE][0][1]);
  
  if(NULL != dlerror())
  {
      printf("err dlsym g_pstSnsObj\n");
      goto __err;
  }
  printf("%s => sensor_type:%d, load:%s\n", __func__, SENSOR_TYPE, libsns[SENSOR_TYPE][0]);
  
  return 0;
__err:
	if(dl)
	{
    dlclose(dl);
		dl = NULL;
  }
  dlerror();
  return -1;
}
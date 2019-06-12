#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

#include "sample_comm.h"
#include "mpp.h"

int SENSOR_TYPE;
int (*sensor_set_inifile_path)(const char *pcPath);
int (*sensor_register_callback)(void);


int SAMPLE_COMM_MPP_Cfg(SAMPLE_MPP_CONFIG_S *cfg)
{
  SENSOR_TYPE = cfg->sensor_type;

  char* libsns[128] = {
    [SONY_IMX185_MIPI_1080P_30FPS] = "libsns_imx185.so",
  };

  if(!libsns[SENSOR_TYPE])
    return -1;
  
  
  void * dl = dlopen(libsns[SENSOR_TYPE], RTLD_LAZY);
  if(dl == NULL)
  {
    printf("err dlopen %s\n", libsns[SENSOR_TYPE]);
    goto __err;
  }
  
  sensor_set_inifile_path = dlsym(dl, "sensor_set_inifile_path");
  if(NULL != dlerror())
  {
      printf("err dlsym sensor_set_inifile_path\n");
      goto __err;
  }
  
  sensor_register_callback = dlsym(dl, "sensor_register_callback");
  if(NULL != dlerror())
  {
      printf("err dlsym sensor_register_callback\n");
      goto __err;
  }
  printf("%s => sensor_type:%d, load:%s\n", __func__, SENSOR_TYPE, libsns[SENSOR_TYPE]);
  
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
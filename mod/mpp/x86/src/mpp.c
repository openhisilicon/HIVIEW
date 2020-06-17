#include <stdio.h>
#include "sample_comm.h"
#include "mpp.h"

gsf_mpp_cfg_t mpp_cfg;

int SENSOR_TYPE;
int (*sensor_set_inifile_path)(const char *pcPath);
int (*sensor_register_callback)(void);

int gsf_mpp_cfg(char *path, gsf_mpp_cfg_t *cfg)
{
  SENSOR_TYPE = cfg->sensor_type;
  sensor_set_inifile_path  = NULL;
  sensor_register_callback = NULL;
  
  printf("%s => load libsns.so sensor_type:%d\n", __func__, SENSOR_TYPE);
  
  return 0;
}

int SAMPLE_COMM_ISP_Init(int  enWDRMode)
{
  printf("%s => enWDRMode:%d\n", __func__, enWDRMode);
}
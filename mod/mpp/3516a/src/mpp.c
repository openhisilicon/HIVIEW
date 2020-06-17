#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

#include "sample_comm.h"
#include "mpp.h"

int SENSOR_TYPE;
int (*sensor_set_inifile_path)(const char *pcPath);
int (*sensor_register_callback)(void);

typedef struct {
  int   type;
  char* name;
  char* lib;
  char* obj;
}SAMPLE_MPP_SENSOR_T;



static SAMPLE_MPP_SENSOR_T libsns[32] = {
    {APTINA_AR0130_DC_720P_30FPS,          "ar0130-0-0-1-30",   NULL,                     NULL},
    {APTINA_9M034_DC_720P_30FPS,           "9m034-0-0-1-30",    NULL,                     NULL}, 
    {SAMPLE_VI_MODE_1_D1,                  "bt656-0-0-0-30",    NULL,                     NULL}, 
    {SAMPLE_VI_MODE_BT1120_1080I,          "bt1120i-0-0-2-30",  NULL,                     NULL}, 
    {SAMPLE_VI_MODE_BT1120_720P,           "bt1120-0-0-1-30",   NULL,                     NULL}, 
    {SAMPLE_VI_MODE_BT1120_1080P,          "bt1120-0-0-2-30",   NULL,                     NULL}, 
    {PANASONIC_MN34220_SUBLVDS_1080P_30FPS,"mn34220-1-0-2-30",  "libsns_mn34220.so",      NULL}, 
    {PANASONIC_MN34220_SUBLVDS_720P_120FPS,"mn34220-1-0-1-120", "libsns_mn34220.so",      NULL}, 
    {SONY_IMX178_LVDS_1080P_30FPS,         "imx178-0-0-2-30",   "libsns_imx178.so",       NULL}, 
    {SONY_IMX185_MIPI_1080P_30FPS,         "imx185-0-0-2-30",   "libsns_imx185.so",       NULL}, 
    {PANASONIC_MN34220_MIPI_1080P_30FPS,   "mn34220-0-0-2-30",  "libsns_mn34220_mipi.so", NULL}, 
    {PANASONIC_MN34220_MIPI_720P_120FPS,   "mn34220-0-0-1-120", "libsns_mn34220_mipi.so", NULL}, 
    {SONY_IMX178_LVDS_5M_30FPS,            "imx178-0-0-5-60",   "libsns_imx178_37M.so",   NULL}, 
    {SONY_IMX117_LVDS_1080P_30FPS,         "imx117-0-0-2-30",   "libsns_imx117.so",       NULL}, 
    {SONY_IMX117_LVDS_720P_30FPS,          "imx117-0-0-1-30",   "libsns_imx117.so",       NULL}, 
    {SONY_IMX123_LVDS_QXGA_30FPS,          "imx123-0-0-3-30",   "libsns_imx123.so",       NULL}, 
    {APTINA_AR0230_HISPI_1080P_30FPS,      "ar0230-0-0-2-30",   "libsns_ar0230.so",       NULL}, 
    {APTINA_AR0237_HISPI_1080P_30FPS,      "ar0237-0-0-2-30",   "libsns_ar0237.so",       NULL},
    {APTINA_AR0330_MIPI_1080P_30FPS,       "ar0330-0-0-2-30",   "libsns_ar0330.so",       NULL},
    {APTINA_AR0330_MIPI_1536P_25FPS,       "ar0330-0-0-3-30",   "libsns_ar0330.so",       NULL},
    {APTINA_AR0330_MIPI_1296P_25FPS,       "ar0330-0-0-3-30",   "libsns_ar0330.so",       NULL},
    {OMNIVISION_OV4689_MIPI_4M_30FPS,      "ov4689-0-0-4-30",   "libsns_ov4689.so",       NULL},
    {OMNIVISION_OV4689_MIPI_1080P_30FPS,   "ov4689-0-0-2-30",   "libsns_ov4689.so",       NULL},
    {OMNIVISION_OV5658_MIPI_5M_30FPS,      "ov5658-0-0-5-30",   "libsns_ov5658.so",       NULL},
  };

static SAMPLE_MPP_SENSOR_T* SAMPLE_MPP_SERSOR_GET(char* name)
{
  int i = 0;
  for(i = 0; i < SAMPLE_SNS_TYPE_BUTT; i++)
  {
    if(strstr(libsns[i].name, name))
    {
      return &libsns[i];
    }
  }
  return NULL;
}

static void * dl = NULL;

int gsf_mpp_cfg(char *path, gsf_mpp_cfg_t *cfg)
{
  char snsstr[64];
  sprintf(snsstr, "%s-%d-0-%d-%d"
          , cfg->snsname, cfg->lane, cfg->wdr, cfg->res, cfg->fps);

  SAMPLE_MPP_SENSOR_T* sns = SAMPLE_MPP_SERSOR_GET(snsstr);
  if(!sns)
  {
    printf("%s => snsstr:%s, unknow.\n", __func__, snsstr);
    return -1;
  } 
 
  SENSOR_TYPE = sns->type;
  
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
  }
  printf("%s => snsstr:%s, sensor_type:%d, load:%s\n"
        , __func__, snsstr, SENSOR_TYPE, sns->lib?:"");
  
  return 0;
__err:
	if(dl)
	{
    dlclose(dl);
  }
  dlerror();
  return -1;
}
#ifndef __mpp_h__
#define __mpp_h__


//api;
typedef struct {
  char  snsname[32];  // sensor imx335
  int   snscnt;       // sensor count
  int   lane;         // lane mode
  int   wdr;          // wdr mode
  int   res;          // resolution
  int   fps;          // frame rate
}gsf_mpp_cfg_t;

int gsf_mpp_cfg(char *path, gsf_mpp_cfg_t *cfg);

int gsf_mpp_isp_init();
int gsf_mpp_isp_run();
int gsf_mpp_isp_ctl();
int gsf_mpp_scene_run();
int gsf_mpp_scene_stop();
int gsf_mpp_isp_stop();



int gsf_mpp_venc_init();    
int gsf_mpp_venc_run();     
int gsf_mpp_venc_ctl();     
int gsf_mpp_venc_stop();    
                        
int gsf_mpp_aenc_init();    
int gsf_mpp_aenc_run();     
int gsf_mpp_aenc_ctl();     
int gsf_mpp_aenc_stop();    




//for mpp;
extern int SENSOR_TYPE;
extern int (*sensor_set_inifile_path)(const char *pcPath);
extern int (*sensor_register_callback)(void);


#endif

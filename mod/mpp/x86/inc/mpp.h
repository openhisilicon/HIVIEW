#ifndef __mpp_h__
#define __mpp_h__

typedef struct {
  int sensor_type;
}SAMPLE_MPP_CONFIG_S;

extern int SENSOR_TYPE;
extern int (*sensor_set_inifile_path)(const char *pcPath);
extern int (*sensor_register_callback)(void);

int SAMPLE_COMM_MPP_Cfg(SAMPLE_MPP_CONFIG_S *cfg);

#endif

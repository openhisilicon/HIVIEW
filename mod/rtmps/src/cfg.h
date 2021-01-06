#ifndef __cfg_h__
#define __cfg_h__

#include "rtmps.h"


extern gsf_rtmps_t rtmps_parm;
extern char rtmps_parm_path[128];
int json_parm_load(char *filename, gsf_rtmps_t *cfg);
int json_parm_save(char *filename, gsf_rtmps_t *cfg);

#endif
#ifndef __cfg_h__
#define __cfg_h__

#include "rtsps.h"


extern gsf_rtsps_t rtsps_parm;
extern char rtsps_parm_path[128];
int json_parm_load(char *filename, gsf_rtsps_t *cfg);
int json_parm_save(char *filename, gsf_rtsps_t *cfg);

#endif
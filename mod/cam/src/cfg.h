#ifndef __cfg_h__
#define __cfg_h__

#include "cam.h"


extern gsf_cam_parm_t cam_parm;

int jscon_parm_load(char *filename, gsf_cam_parm_t *cfg);
int jscon_parm_save(char *filename, gsf_cam_parm_t *cfg);

#endif
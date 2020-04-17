#ifndef __cfg_h__
#define __cfg_h__

#include "onvif.h"

extern gsf_onvif_cfg_t onvif_cfg;
extern char onvif_parm_path[128];

int json_parm_load(char *filename, gsf_onvif_cfg_t *cfg);
int json_parm_save(char *filename, gsf_onvif_cfg_t *cfg);

#endif
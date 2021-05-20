#ifndef __cfg_h__
#define __cfg_h__

#include "sips.h"


extern gsf_sips_t sips_parm;
extern char sips_parm_path[128];
int json_parm_load(char *filename, gsf_sips_t *cfg);
int json_parm_save(char *filename, gsf_sips_t *cfg);

#endif
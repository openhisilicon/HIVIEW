#ifndef __cfg_h__
#define __cfg_h__


#include "svp.h"

extern gsf_svp_t svp_cfg;
extern char svp_parm_path[128];

int json_parm_load(char *filename, gsf_svp_t *cfg);
int json_parm_save(char *filename, gsf_svp_t *cfg);

#endif
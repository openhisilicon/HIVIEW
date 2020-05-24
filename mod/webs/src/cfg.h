#ifndef __cfg_h__
#define __cfg_h__

#include "webs.h"


extern gsf_webs_cfg_t webs_cfg;
extern char webs_parm_path[128];

int json_parm_load(char *filename, gsf_webs_cfg_t *cfg);
int json_parm_save(char *filename, gsf_webs_cfg_t *cfg);

#endif
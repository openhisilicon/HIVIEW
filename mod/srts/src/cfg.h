#ifndef __cfg_h__
#define __cfg_h__

#include "srts.h"


extern gsf_srts_t srts_parm;
extern char srts_parm_path[128];
int json_parm_load(char *filename, gsf_srts_t *cfg);
int json_parm_save(char *filename, gsf_srts_t *cfg);

#endif
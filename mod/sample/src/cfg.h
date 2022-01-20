#ifndef __cfg_h__
#define __cfg_h__

#include "sample.h"


extern gsf_sample_t sample_parm;
extern char sample_parm_path[128];
int json_parm_load(char *filename, gsf_sample_t *cfg);
int json_parm_save(char *filename, gsf_sample_t *cfg);

#endif
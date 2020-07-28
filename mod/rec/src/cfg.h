#ifndef __cfg_h__
#define __cfg_h__

#include "rec.h"


extern gsf_rec_parm_t rec_parm;
extern char rec_parm_path[128];
int json_parm_load(char *filename, gsf_rec_parm_t *cfg);
int json_parm_save(char *filename, gsf_rec_parm_t *cfg);

#endif
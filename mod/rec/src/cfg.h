#ifndef __cfg_h__
#define __cfg_h__

#include "rec.h"


extern gsf_rec_parm_t rec_parm;

int json_parm_load(char *filename, gsf_rec_parm_t *cfg);
int json_parm_save(char *filename, gsf_rec_parm_t *cfg);

#endif
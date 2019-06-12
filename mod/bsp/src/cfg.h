#ifndef __cfg_h__
#define __cfg_h__

#include "bsp.h"

extern gsf_bsp_def_t bsp_def;
extern gsf_bsp_parm_t bsp_parm;

int jscon_def_load(char *filename, gsf_bsp_def_t *cfg);
int jscon_def_save(char *filename, gsf_bsp_def_t *cfg);
int jscon_parm_load(char *filename, gsf_bsp_parm_t *cfg);
int jscon_parm_save(char *filename, gsf_bsp_parm_t *cfg);

#endif
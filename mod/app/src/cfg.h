#ifndef __cfg_h__
#define __cfg_h__


#include "app.h"

extern gsf_app_nvr_t app_nvr;
extern char app_parm_path[128];

int json_parm_load(char *filename, gsf_app_nvr_t *cfg);
int json_parm_save(char *filename, gsf_app_nvr_t *cfg);

#endif
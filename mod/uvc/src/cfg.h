#ifndef __cfg_h__
#define __cfg_h__

#include "uvc.h"


extern gsf_uvc_t uvc_parm;
extern char uvc_parm_path[128];
int json_parm_load(char *filename, gsf_uvc_t *cfg);
int json_parm_save(char *filename, gsf_uvc_t *cfg);

#endif
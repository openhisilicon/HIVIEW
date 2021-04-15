#ifndef __mod_call_h__
#define __mod_call_h__

#include "mod/codec/inc/codec.h"
#include "mod/rec/inc/rec.h"
#include "mod/app/inc/app.h"

#include "mod/rtsps/inc/rtsps.h"
#include "mod/rtmps/inc/rtmps.h"
#include "mod/onvif/inc/onvif.h"
#include "mod/svp/inc/svp.h"
#include "mod/webs/inc/webs.h"

int mod_call(char *str, char *args, char *in, char *out, int osize);

#endif //__mod_call_h__
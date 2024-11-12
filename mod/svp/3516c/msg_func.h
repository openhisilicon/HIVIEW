#ifndef __msg_func_h__
#define __msg_func_h__

#include "inc/msg.h"


/**
 *  msg_func_t
 *  
 *  \param req, rsp;
 *  \return fill rsp->err, rsp->size;
 */
typedef void (msg_func_t)(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize);



/**
 *  msg_func_proc
 *  
 *  \param req, rsp;
 *  \return TRUE: call func, FALSE: none func;
 */
int msg_func_proc(gsf_msg_t *in, int isize, gsf_msg_t *out, int *osize);



#endif //__msg_func_h__

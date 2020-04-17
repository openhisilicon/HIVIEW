//os

//top
#include "inc/gsf.h"

//mod
#include "onvif.h"

//myself
#include "cfg.h"
#include "msg_func.h"
#include "onvif_api.h"

typedef struct {
  UT_hash_handle hh;
  char name[256];
  int timeout;
  void* dev;
  int refcnt;
  gsf_onvif_media_url_t media_url;
}nvc_conn_ctx_t;

static nvc_conn_ctx_t *conn_ctxs;
static pthread_mutex_t conn_mutex = PTHREAD_MUTEX_INITIALIZER;

static void msg_func_cfg(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    gsf_onvif_cfg_t *cfg = (gsf_onvif_cfg_t*)req->data;
  }
  else
  {
    gsf_onvif_cfg_t *cfg = (gsf_onvif_cfg_t*)rsp->data;
  }
   
   
}


static int callback(nvc_probe_item_t *item, void *user_args)
{
	if (strlen(item->ip) > 1)
	{	
		printf(" probe: ip: %s, port: %d \n", item->ip, item->port);
	}
	else
		printf(" probe: ip is NULL. \n");
	return 0;
}


static void msg_func_probe(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  gsf_onvif_probe_t *p = (gsf_onvif_probe_t*)req->data;
  gsf_onvif_probe_item_t *item = (gsf_onvif_probe_item_t*)rsp->data;
  
  pthread_mutex_lock(&conn_mutex);
  nvc_probe(0, 3, callback, NULL);
  pthread_mutex_unlock(&conn_mutex);
}

static void msg_func_open(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  gsf_onvif_url_t *u = (gsf_onvif_url_t*)req->data;
  gsf_onvif_media_url_t *m = (gsf_onvif_media_url_t*)rsp->data;
  
  pthread_mutex_lock(&conn_mutex);
  char *name = u->url;
  nvc_conn_ctx_t *ctx = NULL;
  HASH_FIND_STR(conn_ctxs, name, ctx);
  if (ctx == NULL)
  {
    ctx = calloc(1, sizeof(*ctx));
    strcpy(ctx->name, name);
    ctx->timeout = u->timeout;
    HASH_ADD_STR(conn_ctxs, name, ctx);
    
    ctx->dev = nvc_dev_open(name, ctx->timeout);
    
    nvc_media_url_t me = {0};
  	me.proto = 0;
  	me.trans = 3;
    nvc_media_url_get(ctx->dev, 0, 0, &me);
    strcpy(ctx->media_url.st1, me.url);
  }
  
  ctx->refcnt++;
  *m = ctx->media_url;
  rsp->size = sizeof(gsf_onvif_media_url_t);
  pthread_mutex_unlock(&conn_mutex);
  
  return;
}

static void msg_func_close(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  gsf_onvif_url_t *u = (gsf_onvif_url_t*)req->data;
  
  pthread_mutex_lock(&conn_mutex);
  char *name = u->url;
  nvc_conn_ctx_t *ctx = NULL;
  HASH_FIND_STR(conn_ctxs, name, ctx);
  if (ctx && --ctx->refcnt == 0)
  {
    HASH_DEL(conn_ctxs, ctx);
    
    if(ctx->dev)
    {
      nvc_dev_close(ctx->dev);
    }
    free(ctx);
  }
  pthread_mutex_unlock(&conn_mutex);
}

static void msg_func_list(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  gsf_onvif_conn_t *conn = (gsf_onvif_conn_t*)rsp->data;
  
  pthread_mutex_lock(&conn_mutex);
  nvc_conn_ctx_t *ctx, *tmp;
  HASH_ITER(hh, conn_ctxs, ctx, tmp)
  {
    printf("refcnt:%d, name:[%s]\n", ctx->refcnt, ctx->name);
  }
  pthread_mutex_unlock(&conn_mutex);
}

static void msg_func_clear(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  pthread_mutex_lock(&conn_mutex);
  nvc_conn_ctx_t *ctx, *tmp;
  HASH_ITER(hh, conn_ctxs, ctx, tmp)
  {
    HASH_DEL(conn_ctxs, ctx);
    printf("refcnt:%d, name:[%s]\n", ctx->refcnt, ctx->name);
    // nvc_dev_close
    free(ctx);
  }
  pthread_mutex_unlock(&conn_mutex);
}


static msg_func_t *msg_func[GSF_ID_ONVIF_END] = {
    [GSF_ID_MOD_CLI]        = NULL,
    [GSF_ID_ONVIF_CFG]      = msg_func_cfg,    
    [GSF_ID_ONVIF_C_PROBE]  = msg_func_probe,  
    [GSF_ID_ONVIF_C_OPEN]   = msg_func_open,
    [GSF_ID_ONVIF_C_CLOSE]  = msg_func_close,
    [GSF_ID_ONVIF_C_LIST]   = msg_func_list,
    [GSF_ID_ONVIF_C_CLEAR]  = msg_func_clear,
    [GSF_ID_ONVIF_C_PTZCTL] = NULL,
    [GSF_ID_ONVIF_C_PRESET] = NULL,
    [GSF_ID_ONVIF_C_SETTIME]= NULL,
    [GSF_ID_ONVIF_C_IMGATTR]= NULL,
 };


int msg_func_proc(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
    if(req->id < 0 || req->id >= GSF_ID_ONVIF_END)
    {
        return FALSE;
    }
    
    if(msg_func[req->id] == NULL)
    {
        return FALSE;
    }   
    
    msg_func[req->id](req, isize, rsp, osize);
    
    return TRUE;
}

//os
#include <assert.h>
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

static nvc_conn_ctx_t *ctxs_hash = NULL;
static pthread_mutex_t ctxs_mutex = PTHREAD_MUTEX_INITIALIZER;

static void msg_func_cfg(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    gsf_onvif_cfg_t *cfg = (gsf_onvif_cfg_t*)req->data;
    onvif_cfg = *cfg;
    json_parm_save(onvif_parm_path, &onvif_cfg);
    rsp->err  = 0;
    rsp->size = 0;
  }
  else
  {
    gsf_onvif_cfg_t *cfg = (gsf_onvif_cfg_t*)rsp->data;
    *cfg = onvif_cfg;
    rsp->err  = 0;
    rsp->size = sizeof(gsf_onvif_cfg_t);
  }
}


static int callback(nvc_probe_item_t *item, void *user_args)
{
  gsf_onvif_probe_item_t *items = (gsf_onvif_probe_item_t*)user_args;
  
	if (strlen(item->ip) > 1)
	{	
		printf(" probe: ip: %s, port: %d \n", item->ip, item->port);
	}
	else
	{
	  printf(" probe: ip is NULL. \n");
	}
	
	return 0;
}


static void msg_func_probe(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  gsf_onvif_probe_t *p = (gsf_onvif_probe_t*)req->data;
  gsf_onvif_probe_item_t *items = (gsf_onvif_probe_item_t*)rsp->data;
  
  nvc_probe(p->type, p->timeout, callback, items);
}

static void msg_func_open(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  gsf_onvif_url_t *u = (gsf_onvif_url_t*)req->data;
  gsf_onvif_media_url_t *m = (gsf_onvif_media_url_t*)rsp->data;
  
  char *name = u->url;
  nvc_conn_ctx_t *ctx = NULL, *_ctx = NULL;
  
  pthread_mutex_lock(&ctxs_mutex);
  HASH_FIND_STR(ctxs_hash, name, ctx);
  if(ctx)
  {
    ctx->refcnt++;
    *m = ctx->media_url;
    rsp->size = sizeof(gsf_onvif_media_url_t);
    pthread_mutex_unlock(&ctxs_mutex);
    printf("ref => name:[%s], refcnt:%d, st1:[%s]\n"
          , ctx->name, ctx->refcnt, ctx->media_url.st1);
    return;
  }
  pthread_mutex_unlock(&ctxs_mutex);
  
  ctx = calloc(1, sizeof(*ctx));
  strcpy(ctx->name, name);
  ctx->timeout = u->timeout;
  ctx->refcnt++;
  
  ctx->dev = nvc_dev_open(name, ctx->timeout);
  printf("new => name:[%s], dev:[%p]\n", ctx->name, ctx->dev);
  if(ctx->dev)
  {
    nvc_media_url_t me = {0};
  	me.proto = 0; //uc;
  	me.trans = 2; //rtsp;
    
    int ret = nvc_media_url_get(ctx->dev, 0, 0, &me);
    strcpy(ctx->media_url.st1, me.url);
    
    int ret2 = nvc_media_url_get(ctx->dev, 0, 1, &me);
    strcpy(ctx->media_url.st2, me.url);
    
    printf("get => name:[%s], st1:[%s], st2:[%s]\n"
          , ctx->name, ctx->media_url.st1, ctx->media_url.st2);
          
    if(ret == 0)
    {
      pthread_mutex_lock(&ctxs_mutex);
      HASH_FIND_STR(ctxs_hash, name, _ctx);
      if(!_ctx)
      {
        HASH_ADD_STR(ctxs_hash, name, ctx);
        pthread_mutex_unlock(&ctxs_mutex);
        *m = ctx->media_url;
        rsp->size = sizeof(gsf_onvif_media_url_t);
        printf("add => name:[%s], refcnt:%d, st1:[%s]\n"
              , ctx->name, ctx->refcnt, ctx->media_url.st1);
        return;
      }
      pthread_mutex_unlock(&ctxs_mutex);
    }
    printf("free => name:[%s], _ctx:%p, st1:[%s]\n"
          , ctx->name, _ctx, ctx->media_url.st1);
    nvc_dev_close(ctx->dev);
    free(ctx);
  }
  rsp->err = 1;
  rsp->size = 0;
}


static void msg_func_close(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  gsf_onvif_url_t *u = (gsf_onvif_url_t*)req->data;
  
  char *name = u->url;
  nvc_conn_ctx_t *ctx = NULL;
  
  pthread_mutex_lock(&ctxs_mutex);
  HASH_FIND_STR(ctxs_hash, name, ctx);
  if (ctx && --ctx->refcnt == 0)
  {
    HASH_DEL(ctxs_hash, ctx);
    
    printf("del => name:[%s], refcnt:%d, st1:[%s]\n"
          , ctx->name, ctx->refcnt, ctx->media_url.st1);
    if(ctx->dev)
    {
      nvc_dev_close(ctx->dev);
    }
    free(ctx);
  }
  pthread_mutex_unlock(&ctxs_mutex);
  
}

static void msg_func_list(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  gsf_onvif_conn_t *conn = (gsf_onvif_conn_t*)rsp->data;
  
  nvc_conn_ctx_t *ctx, *tmp;
  pthread_mutex_lock(&ctxs_mutex);
  HASH_ITER(hh, ctxs_hash, ctx, tmp)
  {
    printf("list => refcnt:%d, name:[%s]\n", ctx->refcnt, ctx->name);
  }
  pthread_mutex_unlock(&ctxs_mutex);
  
}

static void msg_func_clear(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  nvc_conn_ctx_t *ctx, *tmp;
  
  pthread_mutex_lock(&ctxs_mutex);
  HASH_ITER(hh, ctxs_hash, ctx, tmp)
  {
    HASH_DEL(ctxs_hash, ctx);
    printf("clear => refcnt:%d, name:[%s]\n", ctx->refcnt, ctx->name);
    if(ctx->dev)
    {
      nvc_dev_close(ctx->dev);
    }
    free(ctx);
  }
  pthread_mutex_unlock(&ctxs_mutex);
}




static void msg_func_ptzctl(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  gsf_onvif_ptz_ctl_t *ptz = (gsf_onvif_ptz_ctl_t*)req->data;

  int found = 0;
  char *name = ptz->url;
  nvc_conn_ctx_t *ctx = NULL;
  
  pthread_mutex_lock(&ctxs_mutex);
  HASH_FIND_STR(ctxs_hash, name, ctx);
  if (ctx)
  {
    found = 1;
    ctx->refcnt++;
  }
  pthread_mutex_unlock(&ctxs_mutex);
 
 if(found)
 {
    int ret = nvc_ptz_ctl(ctx->dev, 0, 0, &ptz->ctl);
    printf("nvc_ptz_ctl ret:%d, cmd:%d, speed:%d, val:%d, name:%s\n",
            ret, ptz->ctl.cmd, ptz->ctl.speed, ptz->ctl.val, ptz->ctl.name);
    
    pthread_mutex_lock(&ctxs_mutex);
    if (ctx && --ctx->refcnt == 0)
    {
      HASH_DEL(ctxs_hash, ctx);
      printf("del => name:[%s], refcnt:%d, st1:[%s]\n"
            , ctx->name, ctx->refcnt, ctx->media_url.st1);
      if(ctx->dev)
      {
        nvc_dev_close(ctx->dev);
      }
      free(ctx);
    }
    pthread_mutex_unlock(&ctxs_mutex);
 }
}



static msg_func_t *msg_func[GSF_ID_ONVIF_END] = {
    [GSF_ID_MOD_CLI]        = NULL,
    [GSF_ID_ONVIF_CFG]      = msg_func_cfg,    
    [GSF_ID_ONVIF_C_PROBE]  = msg_func_probe,  
    [GSF_ID_ONVIF_C_OPEN]   = msg_func_open,
    [GSF_ID_ONVIF_C_CLOSE]  = msg_func_close,
    [GSF_ID_ONVIF_C_LIST]   = msg_func_list,
    [GSF_ID_ONVIF_C_CLEAR]  = msg_func_clear,
    [GSF_ID_ONVIF_C_PTZCTL] = msg_func_ptzctl,
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

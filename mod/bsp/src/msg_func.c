//os

//top
#include "inc/gsf.h"

//mod
#include "bsp.h"
#include "cfg.h"

//myself
#include "msg_func.h"


static gsf_mod_reg_t mods[GSF_MOD_ID_END] = {0};

static void msg_func_cli(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
    int ret = 0;
    switch(req->sid)
    {
        case GSF_CLI_POWEROFF:
            break;
        case GSF_CLI_RESTART:
            break;
        case GSF_CLI_REGISTER:
            {
                gsf_mod_reg_t *reg = (gsf_mod_reg_t*)req->buf;
                printf("REGISTER [mid:%d, uri:%s]\n", reg->mid, reg->uri);
                mods[reg->mid] = *reg;
                rsp->size = sizeof(mods);
                memcpy(rsp->buf, mods, sizeof(mods));
            }
            break;
        default:
            break;
    }
}

static void msg_func_time(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
}

static void msg_func_net(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
}

static void msg_func_upg(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
}

static void msg_func_cfg(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  gsf_bsp_def_t *cfg = (gsf_bsp_def_t*)rsp->buf;
  
  *cfg = bsp_def;
  rsp->size = sizeof(gsf_bsp_def_t);
}

static msg_func_t *msg_func[GSF_ID_BSP_END] = {
    [GSF_ID_MOD_CLI]    = msg_func_cli,
    [GSF_ID_BSP_TIME]   = msg_func_time,
    [GSF_ID_BSP_ETH]    = msg_func_net,
    [GSF_ID_BSP_UPG]    = msg_func_upg,
    [GSF_ID_BSP_DEF]    = msg_func_cfg,
};


int msg_func_proc(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
    if(req->id < 0 || req->id >= GSF_ID_BSP_END)
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

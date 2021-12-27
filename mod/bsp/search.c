#include <unistd.h>
#include <stdlib.h>
#include "inc/gsf.h"
#include "bsp.h"

#include "sadp.h"     // discover
#include "upg.h"      // upgrade

GSF_LOG_GLOBAL_INIT("SEARCH", 8*1024);

// route add -net 224.0.0.0 netmask 240.0.0.0 dev ens33

// gcc -g  -o tt  search.c  src/sadp.c -I/mnt/hgfs/works/gsf-20190612/gsf -Isrc -Iinc -L/mnt/hgfs/works/gsf-20190612/gsf/lib/x86  -lcomm -lnm -lm -lpthread

// export LD_LIBRARY_PATH=../../lib/x86;./tt ens33 238.238.238.238 3


static int sadp_recv_func(gsf_sadp_msg_t *in, gsf_msg_t* out
          , int *osize/*IN&OUT*/, gsf_sadp_peer_t* peer)
{
  int ret = 0;
  
  printf(" <<<< in->ver:%04X, modid:%d, msg.size:%d, peer:[%s:%d]\n"
        , in->ver, in->modid, in->msg.size, peer->ipaddr, peer->port);

  *osize = 0;
  return 0;
}

#define SADP_ADDR "238.238.238.238"

int main(int argc, char *argv[])
{
  if(argc < 4)
  {
    printf("pls: %s eth0/ztyxa66jmd %s/192.168.1.2 3\n", argv[0], SADP_ADDR);
    return 0;
  }

  gsf_sadp_ini_t puini = {
    .ethname= "eth0",
    .lcaddr = "0.0.0.0",
    .mcaddr = SADP_ADDR,
    .mcport = 8888,
    .cb = sadp_recv_func,
  };
 
  strncpy(puini.ethname,  argv[1], sizeof(puini.ethname)-1); // "ztyxa66jmd"
 
  int ret = sadp_pu_init(&puini);
  printf("sadp_pu_init ret:%d\n", ret);

  gsf_sadp_peer_t dst1;
  strcpy(dst1.ipaddr, argv[2]);
  dst1.port = 8888;
  

  gsf_sadp_msg_t in = {.ver = GSF_SADP_VER_REQ|GSF_SADP_VER_MC, .modid = GSF_MOD_ID_BSP, .msg = {.id = GSF_ID_BSP_DEF}};

  char outbuf[64*1024] = {0};
  gsf_msg_t *out = (gsf_msg_t*)outbuf;
  int osize = sizeof(outbuf);

  gsf_sadp_peer_t *dst = &dst1;
  
  while(1)
  {
    sadp_cu_gset(dst, &in, out, &osize, atoi(argv[3]));
    printf("\n\n");
    sleep(1);
  }



  return 0;
}

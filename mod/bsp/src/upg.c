#include <stdio.h>
#include "upg.h"
#include "cfg.h"


static pthread_t gpid;
static int(*gcb)(int progress, void *u);
static void *gu;

void* upg_task(void *parm)
{
  char str[128];
  sprintf(str, "%s", "/tmp/upg/install.sh");
  FILE* fd = popen(str, "r");
  
  while(1)
  {
      if (fgets(str, sizeof(str), fd))
    	{
    	  int progress = 0;
    		sscanf(str, "progress=%d", &progress);
    		printf("progress = %d\n", progress);
    		if(gcb)
    		{
    		  gcb(progress, gu);
    		}
    	}
    	else
    	{
    	  printf("exit.\n");
    	  break;
    	}
  }
}

int upg_start(char *path, int(*cb)(int progress, void *u), void *u)
{
  gcb = cb;
  gu = u;
  
  char str[128];
  sprintf(str, "mkdir -p /tmp/upg/;miniunz -o -p 123 %s -d /tmp/upg/;chmod +x /tmp/upg/install.sh", path);
  system(str);
  
  return pthread_create(&gpid, NULL, upg_task, (void*)NULL);
}

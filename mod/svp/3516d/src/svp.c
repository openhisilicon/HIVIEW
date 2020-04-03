#include <unistd.h>
#include "sample_ive_main.h" 
#include "sample_nnie_main.h"

extern int sample_ive_main(int argc, char *argv[]);
extern int sample_nnie_main(int argc, char *argv[]);

int main(int argc, char *argv[])
{
  
  SAMPLE_IVE_Md();
  
  while(1)
  {
    sleep(1);
  }
  
  return 0;
}
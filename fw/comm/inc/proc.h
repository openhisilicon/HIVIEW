#ifndef __PROC_H__
#define __PROC_H__

#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/prctl.h>

#define proc_absolute_path(p) ({\
    int i = 0;\
    char path[128] = {0};\
    int cnt = readlink("/proc/self/exe", path, 128);\
    if(cnt > 0 && cnt < 128) \
    for(i = cnt; i >= 0; --i)\
    {\
        if(path[i]=='/')\
        {\
            path[i + 1]='\0';\
            break;\
        }\
    }\
    strcpy(p, path);\
  })
#define proc_name_set(n) \
        prctl(PR_SET_NAME, (unsigned long)n, 0,0,0);

#endif // __PROC_H__
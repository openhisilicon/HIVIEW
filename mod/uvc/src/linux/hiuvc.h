#ifndef __HIUVC_H__
#define __HIUVC_H__

typedef struct hiuvc
{
    int (*init)(void);
    int (*open)(void);
    int (*close)(void);
    int (*run)(void);
} hiuvc;

hiuvc *get_hiuvc(void);

#endif //__HIUVC_H__

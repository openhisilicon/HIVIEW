#ifndef __HI_UAC_H__
#define __HI_UAC_H__

typedef struct hiuac
{
    int (*init)(void);
    int (*open)(void);
    int (*close)(void);
    int (*run)(void);
} hiuac;

hiuac *get_hiuac(void);

#endif //__HI_UAC_H__

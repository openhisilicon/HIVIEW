#ifndef __HI_AUDIO_H__
#define __HI_AUDIO_H__

typedef struct audio_control_ops {
    int (*init)(void);
    int (*deinit)(void);
    int (*startup)(void);
    int (*shutdown)(void);
} audio_control_ops_st;

typedef struct hiaudio
{
    struct audio_control_ops *mpi_ac_ops;
    int audioing;
} hiaudio;

/* audio control functions */
extern int hiaudio_register_mpi_ops(struct audio_control_ops *ac_ops);

extern int hiaudio_init(void);
extern int hiaudio_deinit(void);
extern int hiaudio_startup(void);
extern int hiaudio_shutdown(void);

#endif //__HI_AUDIO_H__


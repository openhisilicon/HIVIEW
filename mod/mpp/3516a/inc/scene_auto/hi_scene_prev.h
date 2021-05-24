#ifndef _HI_SCENE_PREV_H_
#define _HI_SCENE_PREV_H_

typedef struct HiSCENE_PREV_S
{
    SAMPLE_VI_CONFIG_S stViConfig;
    SAMPLE_VO_CONFIG_S stVoConfig;
    VPSS_GRP           VpssGrp   ;
    VPSS_CHN           VpssChn   ;
    VO_CHN             VoChn     ;
    HI_BOOL            abChnEnable[VPSS_MAX_PHY_CHN_NUM] ;
    VENC_CHN           VencChn[1];
}SCENE_PREV_S;

HI_S32 SAMPLE_SCENE_START_PREV(SCENE_PREV_S *pstScenePrev);
HI_S32 SAMPLE_SCENE_STOP_PREV(SCENE_PREV_S *pstScenePrev);
#endif

#ifndef _HI_IR_AUTO_PREV_H_
#define _HI_IR_AUTO_PREV_H_

typedef struct HiIR_AUTO_PREV_S
{
    SAMPLE_VI_CONFIG_S stViConfig;
    SAMPLE_VO_CONFIG_S stVoConfig;
    VPSS_GRP           VpssGrp   ;
    VPSS_CHN           VpssChn   ;
    VO_CHN             VoChn     ;
    HI_BOOL            abChnEnable[VPSS_MAX_PHY_CHN_NUM] ;
    VENC_CHN           VencChn[1];
}IR_AUTO_PREV_S;

#endif

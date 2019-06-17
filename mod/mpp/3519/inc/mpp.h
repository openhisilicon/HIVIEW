#ifndef __mpp_h__
#define __mpp_h__

typedef struct {
  int sensor_type;
}SAMPLE_MPP_CONFIG_S;

extern int SENSOR_TYPE;
extern ISP_SNS_OBJ_S *g_pstSnsObj[2];

int SAMPLE_COMM_MPP_Cfg(SAMPLE_MPP_CONFIG_S *cfg);

#endif

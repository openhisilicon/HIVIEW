#ifndef  __detection_h__
#define  __detection_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "inc/gsf.h"

//for json cfg;
#include "mod/detection/inc/sjb_detection.h"


#define GSF_IPC_DETECTION      "ipc:///tmp/detection_rep"
#define GSF_PUB_DETECTION      "ipc:///tmp/detection_pub"
enum {
    //GSF_ID_UAV_CFG    = 1,  // [gsf_uav_t, gsf_uav_t]
    //GSF_ID_UAV_CMD    = 2,  // [gsf_rtsp_url_t, gsf_shmid_t]

    GSF_ID_DETECTION_END
};


enum  {
    GSF_EV_DETECTION_YOLO3 = 1
};

typedef struct {
  int cls;        //thr 
  float confidence;
  int rect[4];    //区域位置
}gsf_yolov3_result_t;


typedef struct {
  int pts;   // u64PTS/1000
  int cnt;   // count
  int w, h;
  gsf_yolov3_result_t result[10];
}gsf_detection_yolov3_t;



#ifdef __cplusplus
}
#endif

#endif
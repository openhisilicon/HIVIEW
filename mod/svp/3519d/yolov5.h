#ifndef __yolov5_h__
#define __yolov5_h__

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifndef YOLO_BOX_MAX
#define YOLO_BOX_MAX 64
typedef struct {
  int chn;
  int w, h;
  int size;
  struct{
    float score;
    float x;
    float y;
    float w;
    float h;
    const char *label;
    }box[YOLO_BOX_MAX];
}yolo_boxs_t;
#endif

#define YOLO_CHN_MAX 9
int yolov5_init(int vpss_grp[YOLO_CHN_MAX], int vpss_chn[YOLO_CHN_MAX], char *ModelPath);
int yolov5_detect(yolo_boxs_t boxs[YOLO_CHN_MAX]);
int yolov5_deinit();

#ifdef __cplusplus
}
#endif  /* __cplusplus */



#endif // __yolov5_h__

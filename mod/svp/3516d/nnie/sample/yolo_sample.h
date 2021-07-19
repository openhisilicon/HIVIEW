#ifndef __yolo_sampe_h
#define __yolo_sample_h

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

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


#define YOLO_CHN_MAX 4
int yolo_init(int VpssGrp[YOLO_CHN_MAX], int VpssChn[YOLO_CHN_MAX], char *ModelPath);
int yolo_detect(yolo_boxs_t boxs[YOLO_CHN_MAX]);
int yolo_deinit();

#ifdef __cplusplus
}
#endif  /* __cplusplus */



#endif // __yolo_sample_h


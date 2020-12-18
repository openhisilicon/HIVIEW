#ifndef __yolo_sampe_h
#define __yolo_sample_h

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define YOLO_BOX_MAX 64
typedef struct {
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

int yolo_init(int VpssGrp, int VpssChn, char *ModelPath);
int yolo_detect(yolo_boxs_t *boxs);
int yolo_deinit();

#ifdef __cplusplus
}
#endif  /* __cplusplus */



#endif // __yolo_sample_h


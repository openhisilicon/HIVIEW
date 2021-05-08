#ifndef __AFACE_H
#define __AFACE_H

#ifdef __cplusplus  
extern "C" {
#endif

#include <stdio.h>
#include <stdbool.h>
#include "hi_comm_video.h"

#define MAX_TRACKER_SIZE 50
#define FEATURE_LENGTH   512

struct FuncEnable
{
	bool face_detect;
	bool person_detect;
	bool face_match;
	bool age;
	bool gender;
};

struct FaceInfo
{
	float x[MAX_TRACKER_SIZE];            // face bounding box top left x
	float y[MAX_TRACKER_SIZE];            // face bounding box top left y
	float w[MAX_TRACKER_SIZE];            // face bounding box width
	float h[MAX_TRACKER_SIZE];            // face bounding box height
	
	int ID[MAX_TRACKER_SIZE];             // tracker id
	int quality[MAX_TRACKER_SIZE];        // face quality    (0 -- 100)
	int confidence[MAX_TRACKER_SIZE];     // face confidence (0 -- 100)

	int yaw[MAX_TRACKER_SIZE];			  // 0 - 90 degree
	int pitch[MAX_TRACKER_SIZE];		  // 0 - 90 degree
};


typedef struct {
  void* (*init)(char *model_path, struct FuncEnable *funcEnable);
  int (*detect)(void *handle, VIDEO_FRAME_S* pVBuf, int minsize, struct FaceInfo *faceArray);
  int (*features)(void *handle, char image_path[], float* feature, int *gender, int *age, int *expression);
  int (*features_by_buffer)(void *handle, unsigned char *buffer, unsigned int len, float* feature, int *gender, int *age, int *expression);
  int (*features_by_bbox)(void *handle, char image_path[], float x, float y, float w, float h, float* feature, int *gender, int *age, int *expression);
  int (*features_by_bbox_buffer)(void *handle, unsigned char *buffer, unsigned int len, float x, float y, float w, float h, float* feature, int *gender, int *age, int *expression);
  float (*feature_dist)(void *handle, float* feature1, float* feature2);
  int (*destroy)(void *handle);
}aface_t;


extern aface_t aface;

#define aface_init(arg...)                    ((aface.init)?aface.init(arg):NULL)
#define aface_detect(arg...)                  ((aface.detect)?aface.detect(arg):-1)
#define aface_features(arg...)                ((aface.features)?aface.features(arg):-1)
#define aface_features_by_buffer(arg...)      ((aface.features_by_buffer)?aface.features_by_buffer(arg):-1)
#define aface_features_by_bbox(arg...)        ((aface.features_by_bbox)?aface.features_by_bbox(arg):-1)
#define aface_features_by_bbox_buffer(arg...) ((aface.features_by_bbox_buffer)?aface.features_by_bbox_buffer(arg):-1)    
#define aface_feature_dist(arg...)            ((aface.feature_dist)?aface.feature_dist(arg):0)
#define aface_destroy(arg...)                 ((aface.destroy)?aface.destroy(arg):-1)

#ifdef __cplusplus
}
#endif

#endif

#include "aface.h"


aface_t aface = {
  .init = NULL,
  .detect = NULL,
  .features = NULL,
  .features_by_buffer = NULL,
  .features_by_bbox = NULL,
  .features_by_bbox_buffer = NULL,
  .feature_dist = NULL,
};

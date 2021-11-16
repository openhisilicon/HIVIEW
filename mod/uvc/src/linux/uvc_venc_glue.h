/*
 * glue for uvc and venc by frame cache
 */

#ifndef __UVC_VENC_GLUE_H__
#define __UVC_VENC_GLUE_H__

#include "frame_cache.h"

void add_multi_payload_header(frame_node_t *node, unsigned long frame_type);
void set_multi_payload_size(frame_node_t *node);

#endif //__UVC_VENC_GLUE_H__

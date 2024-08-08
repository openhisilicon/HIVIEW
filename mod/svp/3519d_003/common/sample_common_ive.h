/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef SAMPLE_COMMON_IVE_H
#define SAMPLE_COMMON_IVE_H

#include "hi_common_svp.h"
#include "hi_common_ive.h"
#include "hi_mpi_ive.h"
#include "sample_comm.h"
#include "sample_common_svp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#define HI_VIDEO_WIDTH                          352
#define HI_VIDEO_HEIGHT                         288
#define HI_IVE_ALIGN                            16
#define HI_IVE_CHAR_CALW                        8
#define HI_IVE_CHAR_CALH                        8
#define HI_IVE_CHAR_NUM                         (HI_IVE_CHAR_CALW * HI_IVE_CHAR_CALH)
#define HI_IVE_FILE_NAME_LEN                    256
#define HI_VPSS_CHN_NUM                         2

#define HI_MAX_LOOP_IMG_H                       3

#define HI_SAMPLE_IVE_IMAGE_CHN_ONE             1
#define HI_SAMPLE_IVE_IMAGE_CHN_TWO             2
#define HI_SAMPLE_IVE_IMAGE_CHN_THREE           3
#define HI_SAMPLE_IVE_IMAGE_CHN_FOUR            4
#define HI_SAMPLE_IVE_MAX_CCL_REGION_NUM        254
#define HI_SAMPLE_IVE_DIV_TWO                   2
#define HI_SAMPLE_POINT_IDX_ZERO                0
#define HI_SAMPLE_POINT_IDX_ONE                 1
#define HI_SAMPLE_POINT_IDX_TWO                 2
#define HI_SAMPLE_POINT_IDX_THREE               3

#define HI_SAMPLE_ADDR_IDX_ZERO                 0
#define HI_SAMPLE_ADDR_IDX_ONE                  1
#define HI_SAMPLE_ADDR_IDX_TWO                  2
#define HI_SAMPLE_IVE_MAX_POOL_CNT              128
#define HI_SAMPLE_IVE_DRAW_THICK                2

#define HI_SAMPLE_IVE_PIPE_IDX_ZERO             0
#define HI_SAMPLE_IVE_PIPE_IDX_ONE              1
#define HI_SAMPLE_IVE_PIPE_IDX_TWO              2
#define HI_SAMPLE_IVE_PIPE_IDX_THREE            3

#define HI_SAMPLE_IVE_1080P_WIDTH               1920
#define HI_SAMPLE_IVE_1080P_HEIGHT              1080

#define HI_SAMPLE_IVE_MASK_NUM                  25
#define HI_SAMPLE_IDX_ZERO                      0
#define HI_SAMPLE_IDX_ONE                       1
#define HI_SAMPLE_IDX_TWO                       2
#define HI_SAMPLE_IDX_THREE                     3
#define HI_SAMPLE_IDX_FOUR                      4
#define HI_SAMPLE_IDX_FIVE                      5

#define HI_SAMPLE_NUM_ZERO                      0
#define HI_SAMPLE_NUM_ONE                       1
#define HI_SAMPLE_NUM_TWO                       2
#define HI_SAMPLE_NUM_THREE                     3
#define HI_SAMPLE_NUM_FOUR                      4
#define HI_SAMPLE_NUM_FIVE                      5
#define HI_SAMPLE_NUM_SEVEN                     7
#define HI_SAMPLE_QUARTER_OF_1M                 256
#define HI_SAMPLE_1M                            1024
#define HI_SAMPLE_4M                            (4 * 1024)
#define HI_SAMPLE_MAX_SRC_FRAME_RATE            30
#define HI_SAMPLE_MAX_DST_FRAME_RATE            30

#define hi_sample_align_back(x, a) ((a) * (((x) / (a))))
#define hi_sample_ive_max(a, b) (((a) > (b)) ? (a) : (b))
#define hi_sample_ive_min(a, b) (((a) < (b)) ? (a) : (b))

typedef struct {
    hi_s32 linear_num;
    hi_s32 thresh_num;
    hi_point *linear_point;
} hi_sample_ive_linear_data;

typedef struct {
    hi_u32 ele_size; /* element size */
    hi_u32 loop_c;  /* loop times of c dimension */
    hi_u32 loop_h[HI_SVP_IMG_ADDR_NUM]; /* loop times of h dimension */
} hi_sample_rw_image_loop_info;

typedef struct {
    hi_size src;
    hi_size dst;
} hi_sample_src_dst_size;

/*
 * function : Mpi check init
 */
hi_s32 sample_common_ive_check_mpi_init(hi_void);

/*
 * function : Mpi exit
 */
hi_void sample_common_ive_mpi_exit(hi_void);

/*
 * function :Read file
 */
hi_s32 sample_common_ive_read_file(hi_svp_img *img, FILE *fp);

/*
 * function :Write file
 */
hi_s32 sample_common_ive_write_file(hi_svp_img *img, FILE *fp);

/*
 * function :Calc stride
 */
hi_u32 sample_common_ive_calc_stride(hi_u32 width, hi_u8 align);

/*
 * function : Copy blob to rect
 */
hi_s32 sample_common_ive_blob_to_rect(hi_ive_ccblob *blob, hi_sample_svp_rect_info *rect,
    hi_u16 rect_max_num, hi_u16 area_thr_step, hi_sample_src_dst_size *src_dst_size);

/*
 * function : Create ive image
 */
hi_s32 sample_common_ive_create_image(hi_svp_img *img, hi_svp_img_type type,
    hi_u32 width, hi_u32 height);

/*
 * function : Create memory info
 */
hi_s32 sample_common_ive_create_mem_info(hi_svp_mem_info *mem_info, hi_u32 size);

/*
 * function : Create ive image by cached
 */
hi_s32 sample_common_ive_create_image_by_cached(hi_svp_img *img,
    hi_svp_img_type type, hi_u32 width, hi_u32 height);

/*
 * function : Dma frame info to  ive image
 */
hi_s32 sample_common_ive_dma_image(hi_video_frame_info *frame_info, hi_svp_dst_img *dst,
    hi_bool is_instant);
#endif

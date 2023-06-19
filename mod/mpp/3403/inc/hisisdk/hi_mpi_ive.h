/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_IVE_H__
#define __HI_MPI_IVE_H__

#include "hi_debug.h"
#include "hi_common_ive.h"
#include "hi_common.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
 *   Prototype    : hi_mpi_ive_dma
 *   Description  : Direct memory access (DMA):
 *                  1. Direct memory copy;
 *                  2. Copy with interval bytes;
 *                  3. Memset using 3 bytes;
 *                  4. Memset using 8 bytes;
 *   Parameters   : hi_ive_handle          *handle         Returned handle ID of a task.
 *                  hi_svp_data            *src            Input source data.The input data is treated as U8C1 data.
 *                  hi_svp_dst_data        *dst            Output result data.
 *                  hi_ive_dma_ctrl        *ctrl           DMA control parameter.
 *                  hi_bool                is_instant      Flag indicating whether to generate an interrupt.
 *                                                         If the output result blocks the next operation,
 *                                                         set is_instant to HI_TRUE.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 *
 */
hi_s32 hi_mpi_ive_dma(hi_ive_handle *handle, const hi_svp_data *src, const hi_svp_dst_data *dst,
    const hi_ive_dma_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_filter
 *   Description  : 5x5 template filter.
 *   Parameters   : hi_ive_handle           *handle          Returned handle ID of a task
 *                  hi_svp_src_img          *src             Input source data.
 *                                                            The U8C1,SP420 and SP422 input formats are supported.
 *                  hi_svp_dst_img          *dst             Output result, of same type with the input.
 *                  hi_ive_filter_ctrl      *ctrl            Control parameters of filter
 *                  hi_bool                 is_instant       For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_filter(hi_ive_handle *handle, const hi_svp_src_img *src, const hi_svp_dst_img *dst,
    const hi_ive_filter_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_csc
 *   Description  : YUV2RGB\YUV2HSV\YUV2LAB\RGB2YUV color space conversion are supported.
 *   Parameters   : hi_ive_handle           *handle        Returned handle ID of a task
 *                  hi_svp_src_img          *src           Input source data:
 *                                                       1. SP420\SP422 type for YUV2RGB\YUV2HSV\YUV2LAB;
 *                                                       2. U8C3_PACKAGE\U8C3_PLANAR type for RGB2YUV;
 *                  hi_svp_dst_img          *dst           Output result:
 *                                                       1. U8C3_PACKAGE\U8C3_PLANAR typed for YUV2RGB\YUV2HSV\YUV2LAB;
 *                                                       2. SP420\SP422 type for RGB2YUV;
 *                  hi_ive_csc_ctrl         *ctrl          Control parameters for CSC
 *                  hi_bool                 is_instant     For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_csc(hi_ive_handle *handle, const hi_svp_src_img *src, const hi_svp_dst_img *dst,
    const hi_ive_csc_ctrl *ctrl, hi_bool is_instant);


/*
 *   Prototype    : hi_mpi_ive_filter_and_csc
 *   Description  : Only support YUV2RGB color space conversion.
 *   Parameters   : hi_ive_handle               *handle       Returned handle ID of a task.
 *                  hi_svp_src_img              *src          Input source data.Only SP420\SP422 type are supported.
 *                  hi_svp_dst_img              *dst          Output result.Only U8C3_PACKAGE\U8C3_PLANAR are supported.
 *                  hi_ive_filter_and_csc_ctrl  *ctrl         Control parameters.
 *                  hi_bool                     is_instant    For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_filter_and_csc(hi_ive_handle *handle, const hi_svp_src_img *src, const hi_svp_dst_img *dst,
    const hi_ive_filter_and_csc_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_sobel
 *   Description  : SOBEL is used to extract the gradient information.
 *   Parameters   : hi_ive_handle           *handle      Returned handle ID of a task
 *                  hi_svp_src_img          *src         Input source data. Only the U8C1 input image is supported.
 *                  hi_svp_dst_img          *dst_h       The (horizontal) result of input image filtered
 *                                                     by the input mask;
 *                  hi_svp_dst_img          *dst_v       The (vertical) result  of input image filtered
 *                                                     by the transposed mask;
 *                  hi_ive_sobel_ctrl       *ctrl        Control parameters
 *                  hi_bool                 is_instant   For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_sobel(hi_ive_handle *handle, const hi_svp_src_img *src, const hi_svp_dst_img *dst_h,
    const hi_svp_dst_img *dst_v, const hi_ive_sobel_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_mag_and_ang
 *   Description  : mag_and_ang is used to extract the edge information.
 *   Parameters   : hi_ive_handle           *handle         Returned handle ID of a task
 *                  hi_svp_src_img          *src            Input source data. Only the U8C1 input format is supported.
 *                  hi_svp_dst_img          *dst_mag        Output magnitude.
 *                  hi_svp_dst_img          *dst_ang        Output angle.
 *                                                           If the output mode is set to magnitude only,
 *                                                           this item can be set to null.
 *                  hi_ive_mag_and_ang_ctrl *ctrl           Control parameters
 *                  hi_bool                 is_instant      For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_mag_and_ang(hi_ive_handle *handle, const hi_svp_src_img *src, const hi_svp_dst_img *dst_mag,
    const hi_svp_dst_img *dst_ang, const hi_ive_mag_and_ang_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_dilate
 *   Description  : 5x5 template dilate. Only the U8C1 binary image input is supported.
 *                  Or else the result is not expected.
 *   Parameters   : hi_ive_handle           *handle          Returned handle ID of a task
 *                  hi_svp_src_img          *src             Input binary image, which consists of 0 or 255;
 *                  hi_svp_dst_img          *dst             Output result.
 *                  hi_ive_dilate_ctrl      *ctrl            Control parameters.
 *                  hi_bool                 is_instant       For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_dilate(hi_ive_handle *handle, const hi_svp_src_img *src, const hi_svp_dst_img *dst,
    const hi_ive_dilate_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_erode
 *   Parameters   : 5x5 template erode. Only the U8C1 binary image input is supported.Or else the result is not correct.
 *   Input        : hi_ive_handle               *handle       Returned handle ID of a task
 *                  hi_svp_src_img              *src          Input binary image, which consists of 0 or 255;
 *                  hi_svp_dst_img              *dst          Output result.
 *                  hi_ive_erode_ctrl           *ctrl         Control parameters
 *                  hi_bool                     is_instant    For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_erode(hi_ive_handle *handle, const hi_svp_src_img *src, const hi_svp_dst_img *dst,
    const hi_ive_erode_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_threshold
 *   Description  : Thresh operation to the input image.
 *   Parameters   : hi_ive_handle           *handle         Returned handle ID of a task
 *                  hi_svp_src_img          *src            Input source data. Only the U8C1 input format is supported.
 *                  hi_svp_dst_img          *dst            Output result
 *                  hi_ive_threshold_ctrl   *ctrl           Control parameters
 *                  hi_bool                 is_instant      For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_threshold(hi_ive_handle *handle, const hi_svp_src_img *src, const hi_svp_dst_img *dst,
    const hi_ive_threshold_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_and
 *   Description  : Binary images' And operation.
 *   Parameters   : hi_ive_handle       *handle        Returned handle ID of a task
 *                  hi_svp_src_img      *src1          The input source1. Only U8C1 input format is supported.
 *                  hi_svp_src_img      *src2          The input source2.Only U8C1 input format is supported.
 *                  hi_svp_dst_img      *dst           Output result of " src1 & src2 ".
 *                  hi_bool             is_instant     For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_and(hi_ive_handle *handle, const hi_svp_src_img *src1, const hi_svp_src_img *src2,
    const hi_svp_dst_img *dst, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_sub
 *   Description  : Two gray images' Sub operation.
 *   Parameters   : hi_ive_handle       *handle       Returned handle ID of a task
 *                  hi_svp_src_img      *src1         Minuend of the input source.Only the U8C1 input format
 *                                                     is supported.
 *                  hi_svp_src_img      *src2         Subtrahend of the input source.Only the U8C1 input format
 *                                                     is supported.
 *                  hi_svp_dst_img      *dst          Output result of src1 minus src2
 *                  hi_ive_sub_ctrl     *ctrl         Control parameter
 *                  hi_bool             is_instant    For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_sub(hi_ive_handle *handle, const hi_svp_src_img *src1, const hi_svp_src_img *src2,
    const hi_svp_dst_img *dst, const hi_ive_sub_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_or
 *   Description  : Two binary images' Or operation.
 *   Parameters   : hi_ive_handle           *handle     Returned handle ID of a task
 *                  hi_svp_src_img          *src1         Input source1. Only the U8C1 input format is supported.
 *                  hi_svp_src_img          *src2         Input source2. Only the U8C1 input format is supported.
 *                  hi_svp_dst_img          *dst          Output result src1 or src2
 *                  hi_bool                 is_instant    For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_or(hi_ive_handle *handle, const hi_svp_src_img *src1, const hi_svp_src_img *src2,
    const hi_svp_dst_img *dst, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_integ
 *   Description  : Calculate the input gray image's integral image.
 *   Parameters   : hi_ive_handle         *handle         Returned handle ID of a task
 *                  hi_svp_src_img        *src            Input source data.Only the U8C1 input format is supported.
 *                  hi_svp_dst_img        *dst            Output result.Can be U32C1 or U64C1, relied on the
 *                                                        control parameter.
 *                  hi_ive_integ_ctrl     *ctrl           Integ Control
 *                  hi_bool               is_instant      For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_integ(hi_ive_handle *handle, const hi_svp_src_img *src, const hi_svp_dst_img *dst,
    const hi_ive_integ_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_hist
 *   Description  : Calculate the input gray image's histogram.
 *   Parameters   : hi_ive_handle          *handle         Returned handle ID of a task
 *                  hi_svp_src_img         *src            Input source data. Only the U8C1 input format is supported.
 *                  hi_svp_dst_mem_info    *dst            Output result.
 *                  hi_bool                is_instant      For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_hist(hi_ive_handle *handle, const hi_svp_src_img *src, const hi_svp_dst_mem_info *dst,
    hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_threshold_s16
 *   Description  : S16 image's THRESH operation.
 *   Parameters   : hi_ive_handle             *handle         Returned handle ID of a task
 *                  hi_svp_src_img            *src            Input source data.Only the S16 input format is supported.
 *                  hi_svp_dst_img            *dst            Output result.
 *                  hi_ive_threshold_s16_ctrl *ctrl           Control parameters
 *                  hi_bool                   is_instant      For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_threshold_s16(hi_ive_handle *handle, const hi_svp_src_img *src, const hi_svp_dst_img *dst,
    const hi_ive_threshold_s16_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_threshold_u16
 *   Description  : U16 image's THRESH operation.
 *   Parameters   : hi_ive_handle              *handle        Returned handle ID of a task
 *                  hi_svp_src_img             *src           Input source data. Only the U16 input format is supported.
 *                  hi_svp_dst_img             *dst           Output result
 *                  hi_ive_threshold_u16_ctrl  *ctrl          Control parameters
 *                  hi_bool                    is_instant     For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_threshold_u16(hi_ive_handle *handle, const hi_svp_src_img *src, const hi_svp_dst_img *dst,
    const hi_ive_threshold_u16_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_16bit_to_8bit
 *   Description  : Scale the input 16bit data to the output 8bit data.
 *   Parameters   : hi_ive_handle              *handle            Returned handle ID of a task
 *                  hi_svp_src_img             *src               Input source data.Only U16C1\S16C1 input is supported.
 *                  hi_svp_dst_img             *dst               Output result
 *                  hi_ive_16bit_to_8bit_ctrl  *ctrl              control parameter
 *                  hi_bool                    is_instant         For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_16bit_to_8bit(hi_ive_handle *handle, const hi_svp_src_img *src, const hi_svp_dst_img *dst,
    const hi_ive_16bit_to_8bit_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_ord_stat_filter
 *   Description  : Order Statistic Filter. It can be used as median\max\min value filter.
 *   Parameters   : hi_ive_handle                 *handle           Returned handle ID of a task
 *                  hi_svp_src_img                *src              Input source data. Only U8C1 input is supported
 *                  hi_svp_dst_img                *dst              Output result
 *                  hi_ive_ord_stat_filter_ctrl   *ctrl             Control parameter
 *                  hi_bool                       is_instant        For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_order_stats_filter(hi_ive_handle *handle, const hi_svp_src_img *src,
    const hi_svp_dst_img *dst, const hi_ive_order_stats_filter_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_map
 *   Description  : Map a image to another through a lookup table.
 *   Parameters   : hi_ive_handle         *handle       Returned handle ID of a task
 *                  hi_svp_src_img        *src          Input source. Only the U8C1 input format is supported.
 *                  hi_svp_src_mem_info   *map          Input lookup table. Must be an U8 array of size 256.
 *                  hi_svp_dst_img        *dst          Output result.
 *                  hi_ive_map_ctrl       *ctrl         Map control parameter.
 *                  hi_bool               is_instant    For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_map(hi_ive_handle *handle, const hi_svp_src_img *src, const hi_svp_src_mem_info *map,
    const hi_svp_dst_img *dst, const hi_ive_map_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_equalize_hist
 *   Description  : Enhance the input image's contrast through histogram equalization.
 *   Parameters   : hi_ive_handle               *handle              Returned handle ID of a task
 *                  hi_svp_src_img              *src                 Input source.Only U8C1 input format is supported.
 *                  hi_svp_dst_img              *dst                 Output result.
 *                  hi_ive_equalize_hist_ctrl   *ctrl                EqualizeHist control parameter.
 *                  hi_bool                     is_instant           For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_equalize_hist(hi_ive_handle *handle, const hi_svp_src_img *src, const hi_svp_dst_img *dst,
    const hi_ive_equalize_hist_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_add
 *   Description  : Two gray images' Add operation.
 *   Parameters   : hi_ive_handle        *handle     Returned handle ID of a task
 *                  hi_svp_src_img       *src1       Augend of the input source.Only the U8C1 input format is supported.
 *                  hi_svp_src_img       *src2       Addend of the input source.Only the U8C1 input format is supported.
 *                  hi_svp_dst_img       *dst        Output result of src1 plus src2
 *                  hi_ive_add_ctrl      *ctrl       Control parameter
 *                  hi_bool              is_instant    For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_add(hi_ive_handle *handle, const hi_svp_src_img *src1, const hi_svp_src_img *src2,
    const hi_svp_dst_img *dst, const hi_ive_add_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_xor
 *   Description  : Two binary images' Xor operation.
 *   Parameters   : hi_ive_handle         *handle     Returned handle ID of a task
 *                  hi_svp_src_img        *src1       The input source1.Only the U8C1 input format is supported.
 *                  hi_svp_src_img        *src2       The input source2.
 *                  hi_svp_dst_img        *dst        Output result
 *                  hi_bool               is_instant  For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_xor(hi_ive_handle *handle, const hi_svp_src_img *src1, const hi_svp_src_img *src2,
    const hi_svp_dst_img *dst, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_ncc
 *   Description  : Calculate two gray images' NCC (Normalized Cross Correlation).
 *   Parameters   : hi_ive_handle          *handle       Returned handle ID of a task
 *                  hi_svp_src_img         *src1         Input source1. Only the U8C1 input format is supported.
 *                  hi_svp_src_img         *src2         Input source2. Must be of the same type, size of source1.
 *                  hi_svp_dst_mem_info    *dst          Output result
 *                  hi_bool                is_instant    For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_ncc(hi_ive_handle *handle, const hi_svp_src_img *src1, const hi_svp_src_img *src2,
    const hi_svp_dst_mem_info *dst, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_ccl
 *   Description  : Connected Component Labeling. Only 8-Connected method is supported.
 *   Parameters   : hi_ive_handle         *handle        Returned handle ID of a task
 *                  hi_svp_img            *src_dst       Input source
 *                  hi_svp_dst_mem_info   *blob          Output result of detected region;
 *                  hi_ive_ccl_ctrl       *ctrl          CCL control parameter
 *                  hi_bool               is_instant     For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_ccl(hi_ive_handle *handle, const hi_svp_img *src_dst, const hi_svp_dst_mem_info *blob,
    const hi_ive_ccl_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_gmm
 *   Description  : Separate foreground and background using GMM(Gaussian Mixture Model) method;
 *                  Gray or RGB GMM are supported.
 *   Parameters   : hi_ive_handle        *handle      Returned handle ID of a task
 *                  hi_svp_src_img       *src         Input source. Only support U8C1 or U8C3_PACKAGE input.
 *                  hi_svp_dst_img       *fg          Output foreground (Binary) image.
 *                  hi_svp_dst_img       *bg          Output background image. Of the same type of pstSrc.
 *                  hi_svp_mem_info      *model       Model data.
 *                  hi_ive_gmm_ctrl      *ctrl        Control parameter.
 *                  hi_bool              is_instant   For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_gmm(hi_ive_handle *handle, const hi_svp_src_img *src, const hi_svp_dst_img *fg,
    const hi_svp_dst_img *bg, const hi_svp_mem_info *model, const hi_ive_gmm_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_gmm2
 *   Description  : Separate foreground and background using GMM(Gaussian Mixture Model) method;
 *                  Gray or RGB GMM are supported.
 *   Parameters   : hi_ive_handle           *handle            Returned handle ID of a task
 *                  hi_svp_src_img          *src               Only U8C1 or U8C3_PACKAGE input are supported.
 *                  hi_svp_src_img          *factor            U16C1 input, low-8bits is sensitivity factor,
 *                                                             and high-8bits is life update factor.
 *                  hi_svp_dst_img          *fg                Output foreground (Binary) image.
 *                  hi_svp_dst_img          *bg                Output background image. With same type of pstSrc.
 *                  hi_svp_dst_img          *match_model_info  Output U8C1 match model info image. Low-1bit is match
 *                                                             flag, and high-7bits is max freq index.
 *                  hi_svp_mem_info         *model             Model data.
 *                  hi_ive_gmm2_ctrl        *ctrl              Control parameter.
 *                  hi_bool                 is_instant         For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_gmm2(hi_ive_handle *handle, const hi_svp_src_img *src, const hi_svp_src_img *factor,
    const hi_svp_dst_img *fg, const hi_svp_dst_img *bg, const hi_svp_dst_img *match_model_info,
    const hi_svp_mem_info *model, const hi_ive_gmm2_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_canny_hys_edge
 *   Description  : The first part of canny Edge detection. Including step: gradient calculation,
 *                  magnitude and angle calculation, hysteresis threshold, NMS(Non-Maximum Suppression)
 *   Parameters   : hi_ive_handle                   *handle       Returned handle ID of a task
 *                  hi_svp_src_img                  *src          Input source. Only the U8C1 input format is supported
 *                  hi_svp_dst_img                  *edge         Output result.
 *                  hi_svp_dst_mem_info             *stack        OutPut stack for CannyEdge
 *                  hi_ive_canny_hys_edge_ctrl      *ctrl         Control parameter.
 *                  hi_bool                         is_instant    For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_canny_hys_edge(hi_ive_handle *handle, const hi_svp_src_img *src, const hi_svp_dst_img *edge,
    const hi_svp_dst_mem_info *stack, const hi_ive_canny_hys_edge_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_canny_edge
 *   Description  : The second part of canny Edge detection: trace strong edge by weak edge.
 *   Parameters   : hi_svp_img          *edge        Input and Output source. Only the U8C1 format is supported
 *                  hi_svp_mem_info     *stack       stack for CannyEdge
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_canny_edge(const hi_svp_img *edge, const hi_svp_mem_info *stack);

/*
 *   Prototype    : hi_mpi_ive_lbp
 *   Description  : LBP calculation using the original method and a extensional method.
 *   Parameters   : hi_ive_handle       *handle      Returned handle ID of a task
 *                  hi_svp_src_img      *src         Input source.Only the U8C1 input format is supported.
 *                  hi_svp_dst_img      *dst         Output result
 *                  hi_ive_lbp_ctrl     *ctrl        Control parameter
 *                  hi_bool             is_instant   For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_lbp(hi_ive_handle *handle, const hi_svp_src_img *src, const hi_svp_dst_img *dst,
    const hi_ive_lbp_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_norm_grad
 *   Description  : Gradient calculation and the output is normalized to S8.
 *   Parameters   : hi_ive_handle         *handle          Returned handle ID of a task
 *                  hi_svp_src_img        *src             Input source data
 *                  hi_svp_dst_img        *dst_h           The (horizontal) result of input image filtered
 *                                                         by the input mask;
 *                  hi_svp_dst_img        *dst_v           The (vertical) result  of input image filtered
 *                                                         by the transposed mask;
 *                  hi_svp_dst_img        *dst_hv          Output the horizontal and vertical component in
 *                                                         single image in package format.
 *                  hi_ive_norm_grad_ctrl *ctrl            Control parameter
 *                  hi_bool               is_instant       For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_norm_grad(hi_ive_handle *handle, const hi_svp_src_img *src, const hi_svp_dst_img *dst_h,
    const hi_svp_dst_img *dst_v, const hi_svp_dst_img *dst_hv, const hi_ive_norm_grad_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_lk_optical_flow_pyr
 *   Description  : Calculate LK Optical Flow using multi-layer of the pyramid-images.
 *   Parameters   : hi_ive_handle                 *handle          Returned handle ID of a task
 *                  hi_svp_src_img                src_prev_pyr     Prev-frame's pyramid. Must be U8C1 images.
 *                  hi_svp_src_img                src_next_pyr     Next-frame's pyramid. Same size and type
 *                                                                 with src_prev_pyr.
 *                  hi_svp_src_mem_info           *prev_points     Interesting points on src_prev_pyr[0].
 *                  hi_svp_mem_info               *next_points     Output points. When use_init_flow is true,
 *                                                                 must have the same size of prev_points as input.
 *                  hi_svp_dst_mem_info           *status          The status of feature points
 *                  hi_svp_dst_mem_info           *err             The similarity between the successfully trucked
 *                                                                 points in next_points with corresponding points
 *                                                                 in prev_points.
 *                  hi_ive_lk_optical_flow_pyr_ctrl *ctrl          Control parameters.
 *                  hi_bool                         is_instant     For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_lk_optical_flow_pyr(hi_ive_handle *handle, const hi_svp_src_img src_prev_pyr[],
    const hi_svp_src_img src_next_pyr[], const hi_svp_src_mem_info *prev_points, const hi_svp_mem_info *next_points,
    const hi_svp_dst_mem_info *status, const hi_svp_dst_mem_info *err, const hi_ive_lk_optical_flow_pyr_ctrl *ctrl,
    hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_st_cand_corner
 *   Description  : The first part of corners detection using Shi-Tomasi-like method: calculate candidate corners.
 *   Parameters   : hi_ive_handle                *handle           Returned handle ID of a task
 *                  hi_svp_src_img               *src              Input source data
 *                  hi_svp_dst_img               *cand_corner      Output result of eig
 *                  hi_ive_st_cand_corner_ctrl   *ctrl             Control parameter
 *                  hi_bool                      is_instant        For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_st_cand_corner(hi_ive_handle *handle, const hi_svp_src_img *src,
    const hi_svp_dst_img *cand_corner, const hi_ive_st_cand_corner_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_st_corner
 *   Description  : The second part of corners detection using Shi-Tomasi-like method: select corners by certain rules.
 *                  hi_svp_src_img         *cand_corner     Input source data
 *                  hi_svp_dst_mem_info      *corner           Output result of Corner
 *                  hi_ive_st_corner_ctrl    *ctrl             Control parameter
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_st_corner(const hi_svp_src_img *cand_corner, const hi_svp_dst_mem_info *corner,
    const hi_ive_st_corner_ctrl *ctrl);

/*
 *   Prototype    : hi_mpi_ive_grad_fg
 *   Description  : Calculate the gradient of frontground
 *   Parameters   : hi_ive_handle        *handle          Returned handle ID of a task
 *                  hi_svp_src_img       *bg_diff_fg      Background subtraction foreground image
 *                  hi_svp_src_img       *cur_grad        Current gradient image, both horizontally and vertically
 *                                                          graded in accordance with [xyxyxy ...] format
 *                  hi_svp_src_img       *bg_grad         Background gradient image
 *                  hi_svp_dst_img       *grad_fg         Gradient foreground image
 *                  hi_ive_grad_fg_ctrl  *ctrl            Gradient calculation parameters
 *                  hi_bool              is_instant       For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_grad_fg(hi_ive_handle *handle, const hi_svp_src_img *bg_diff_fg, const hi_svp_src_img *cur_grad,
    const hi_svp_src_img *bg_grad, const hi_svp_dst_img *grad_fg, const hi_ive_grad_fg_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_match_bg_model
 *   Description  : Match background model
 *   Parameters   : hi_ive_handle              *handle          Returned handle ID of a task
 *                  hi_svp_src_img             *cur_img         Current grayscale image
 *                  hi_svp_data                *bg_model        Background model data
 *                  hi_svp_img                 *fg_flag         Foreground status image
 *                  hi_svp_dst_img             *bg_diff_fg      Foreground image obtained by background matching,
 *                                                                the background pixel value is 0, the foreground pixel
 *                                                                value is the gray difference value
 *                  hi_svp_dst_img             *frame_diff_fg     Foreground image obtained by interframe difference,
 *                                                                the background pixel value is 0, the foreground pixel
 *                                                                value is the gray difference value
 *                  hi_svp_dst_mem_info        *state_data       result status data
 *                  hi_ive_match_bg_model_ctrl *ctrl            Background matching parameters
 *                  hi_bool                    is_instant       For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_match_bg_model(hi_ive_handle *handle, const hi_svp_src_img *cur_img,
    const hi_svp_data *bg_model, const hi_svp_img *fg_flag, const hi_svp_dst_img *bg_diff_fg,
    const hi_svp_dst_img *frame_diff_fg, const hi_svp_dst_mem_info *state_data,
    const hi_ive_match_bg_model_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_update_bg_model
 *   Description  : Update background model
 *   Parameters   : hi_ive_handle               *handle             Returned handle ID of a task
 *                  hi_svp_data                 *bg_model           Background model data
 *                  hi_svp_img                  *fg_flag            Foreground status image
 *                  hi_svp_dst_img              *bg_img             Background grayscale image
 *                  hi_svp_dst_img              *chg_status_img     Change state life image, for still detection
 *                  hi_svp_dst_img              *chg_status_fg      Change state grayscale image, for still detection
 *                  hi_svp_dst_img              *chg_status_life    Change state foreground image, for still detection
 *                  hi_svp_dst_mem_info         *state_data         result status data
 *                  hi_ive_update_bg_model_ctrl *ctrl               Background update parameters
 *                  hi_bool                     is_instant          For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_update_bg_model(hi_ive_handle *handle, const hi_svp_data *bg_model, const hi_svp_img *fg_flag,
    const hi_svp_dst_img *bg_img, const hi_svp_dst_img *chg_status_img, const hi_svp_dst_img *chg_status_fg,
    const hi_svp_dst_img *chg_status_life, const hi_svp_dst_mem_info *state_data,
    const hi_ive_update_bg_model_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_ann_mlp_load_model
 *   Description  : Load ANN_MLP model data from ".bin" file.
 *   Parameters   : hi_char                  *file_name        ANN_MLP model file name, must be ".bin" file.
 *                  hi_ive_ann_mlp_model      *model           ANN_MLP model data.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_ann_mlp_load_model(const hi_char *file_name, hi_ive_ann_mlp_model *model);

/*
 *   Prototype    : hi_mpi_ive_ann_mlp_unload_model
 *   Description  : Unload ANN_MLP model data.
 *   Parameters   : hi_ive_ann_mlp_model      *model        ANN_MLP model data.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 *
 */
hi_void hi_mpi_ive_ann_mlp_unload_model(const hi_ive_ann_mlp_model *model);

/*
 *   Prototype    : hi_mpi_ive_ann_mlp_predict
 *   Description  : ANN predict
 *   Parameters   : hi_ive_handle           *handle                 Returned handle ID of a task
 *                  hi_svp_src_data         *src                    Input samples
 *                  hi_svp_lut              *actv_func_table        Look-up table for active function
 *                  hi_ive_ann_mlp_model    *model                  ANN_MLP model
 *                  hi_svp_dst_data         *dst                    Output layers of every input sample
 *                  hi_bool                 is_instant              For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_ann_mlp_predict(hi_ive_handle *handle, const hi_svp_src_data *src,
    const hi_svp_lut *actv_func_table, const hi_ive_ann_mlp_model *model,
    const hi_svp_dst_data *dst, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_svm_load_model
 *   Description  : Load SVM model data from ".bin" file.
 *   Parameters   : hi_char               *file_name        SVM model file name, must be ".bin" file.
 *                  hi_ive_svm_model      *svm_model        SVM model data.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_svm_load_model(const hi_char *file_name, hi_ive_svm_model *svm_model);

/*
 *   Prototype    : hi_mpi_ive_svm_unload_model
 *   Description  : Unload SVM model data.
 *   Parameters   : hi_ive_svm_model      *svm_model         SVM model data.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_void hi_mpi_ive_svm_unload_model(const hi_ive_svm_model *svm_model);

/*
 *   Prototype    : hi_mpi_ive_svm_predict
 *   Description  : SVM predict
 *   Parameters   : hi_ive_handle           *handle          Returned handle ID of a task
 *                  hi_svp_src_data         *src             Input sample
 *                  hi_svp_lut              *kernel_table    Look-up table for active function
 *                  hi_ive_svm_model        *svm_model       SVM model
 *                  hi_svp_dst_data         *dst_vote        Output Votes' array of each class
 *                  hi_bool                 is_instant           For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_svm_predict(hi_ive_handle *handle, const hi_svp_src_data *src,
    const hi_svp_lut *kernel_table, const hi_ive_svm_model *svm_model,
    const hi_svp_dst_data *dst_vote, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_sad
 *   Description  : Sum of absolute differences.
 *   Parameters   : hi_ive_handle       *handle      Returned handle ID of a task
 *                  hi_svp_src_img      *src1        The input source1.Only the U8C1 input format is supported.
 *                  hi_svp_src_img      *src2        The input source2.Only the U8C1 input format is supported.
 *                  hi_svp_dst_img      *sad         Output result of sad value.Only the U8C1/U16C1 format is supported.
 *                  hi_svp_dst_img      *threshold   Output result of threshold.Only the U8C1 format is supported.
 *                  hi_ive_sad_ctrl     *ctrl         Control parameter
 *                  hi_bool             is_instant    For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_sad(hi_ive_handle *handle, const hi_svp_src_img *src1, const hi_svp_src_img *src2,
    const hi_svp_dst_img *sad, const hi_svp_dst_img *threshold, const hi_ive_sad_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_resize
 *   Description  : Resize.
 *   Parameters   : hi_ive_handle           *handle     Returned handle ID of a task
 *                  hi_svp_src_img          src[]       The input source.Only the U8C1/U8C3_PLANAR input format
 *                                                   is supported.
 *                  hi_svp_dst_img          dst[]       Output result.Only the U8C1/U8C3_PLANAR format is supported.
 *                  hi_ive_resize_ctrl      *ctrl       Control parameter
 *                  hi_bool                 is_instant  For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_resize(hi_ive_handle *handle, const hi_svp_src_img src[], const hi_svp_dst_img dst[],
    const hi_ive_resize_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_cnn_load_model
 *   Description  : Load CNN model data from ".bin" file.
 *   Parameters   : hi_char             *file_name     CNN model file name, must be ".bin" file.
 *                  hi_ive_cnn_model    *model         CNN model data.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_cnn_load_model(const hi_char *file_name, hi_ive_cnn_model *model);

/*
 *   Prototype    : hi_mpi_ive_cnn_unload_model
 *   Description  : Unload CNN model data and release memory.
 *   Parameters   : hi_ive_cnn_model *model         CNN model data.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_void hi_mpi_ive_cnn_unload_model(const hi_ive_cnn_model *model);

/*
 *   Prototype    : hi_mpi_ive_cnn_predict
 *   Description  : Perform CNN prediction on input sample(s), and output responses for corresponding sample(s)
 *   Parameters   : hi_ive_handle       *handle     Returned handle ID of a task
 *                  hi_svp_src_img      src[]       Input images array. Only the U8C1 and U8C3_PLANAR
 *                                                  input are supported
 *                  hi_ive_cnn_model    *model      CNN model data
 *                  hi_svp_dst_data     *dst        Output vectors of CNN_Predict
 *                  hi_ive_cnn_ctrl     *ctrl       CNN control parameter
 *                  hi_bool             is_instant  For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_cnn_predict(hi_ive_handle *handle, const hi_svp_src_img src[], const hi_ive_cnn_model *model,
    const hi_svp_dst_data *dst, const hi_ive_cnn_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_cnn_get_result
 *   Description  : Calculate classification and conf with CNN output by softmax regression.
 *   Parameters   : hi_svp_src_data         *src    The result of CNN_Predict output.
 *                  hi_svp_dst_mem_info     *dst    The prediction about classification label index and conf.
 *                  hi_ive_cnn_model        *model  CNN model data
 *                  hi_ive_cnn_ctrl         *ctrl   CNN control parameter
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_cnn_get_result(const hi_svp_src_data *src, const hi_svp_dst_mem_info *dst,
    const hi_ive_cnn_model *model, const hi_ive_cnn_ctrl *ctrl);

/*
 *   Prototype    : hi_mpi_ive_query
 *   Description  : This API is used to query the status of a called function by using the returned
 *                  IveHandle of the function.
 *                  In block mode, the system waits until the function that is being queried is called.
 *                  In non-block mode, the current status is queried and no action is taken.
 *   Parameters   : hi_ive_handle     handle          IveHandle of a called function. It is entered by users.
 *                  hi_bool           *is_finish      Returned status
 *                  hi_bool           is_block        Flag indicating the block mode or non-block mode
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_query(hi_ive_handle handle, hi_bool *is_finish, hi_bool is_block);

/*
 *   Prototype    : hi_mpi_ive_persp_trans
 *   Description  : Perspective transform
 *   Parameters   : hi_ive_handle               *handle        Returned handle ID of a task.
 *                  hi_svp_src_img              *src           The input source.Only the U8C1/YUVSP420 input format
 *                                                             is supported..
 *                  hi_svp_rect_u32             roi[]          Roi array.
 *                  hi_svp_src_mem_info         point_pair[]   Point pair array.
 *                  hi_svp_dst_img              dst[]          Output result.Only the U8C1/YUVSP420/U8C3_PACKAGE
 *                                                             ormat is supported.
 *                  hi_ive_persp_trans_ctrl     *ctrl          PerspTrans control parameter.
 *                  hi_bool                     is_instant     For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_persp_trans(hi_ive_handle *handle, const hi_svp_src_img *src, const hi_svp_rect_u32 roi[],
    const hi_svp_src_mem_info point_pair[], const hi_svp_dst_img dst[], const hi_ive_persp_trans_ctrl *ctrl,
    hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_kcf_get_mem_size
 *   Description  : KCF get object memory size
 *   Parameters  :  hi_u32 max_obj_num    The maximum numbers of tracking object.
 *                  hi_u32 *size          memory size.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_kcf_get_mem_size(hi_u32 max_obj_num, hi_u32 *size);

/*
 *   Prototype    : hi_mpi_ive_kcf_create_obj_list
 *   Description  : KCF create object list
 *   Parameters   : hi_svp_mem_info     *mem            The memory of object for object list.
 *                  hi_u32              max_obj_num     The maximum numbers of tracking object (list node).
 *                  hi_ive_kcf_obj_list *obj_list       The object list.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_kcf_create_obj_list(const hi_svp_mem_info *mem, hi_u32 max_obj_num,
    hi_ive_kcf_obj_list *obj_list);

/*
 *   Prototype    : hi_mpi_ive_kcf_destroy_obj_list
 *   Description  : KCF destroy object list
 *   Parameters  :  hi_ive_kcf_obj_list    *obj_list    The object list.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_kcf_destroy_obj_list(hi_ive_kcf_obj_list *obj_list);

/*
 *   Prototype    : hi_mpi_ive_kcf_create_gauss_peak
 *   Description  : KCF calculate gauss peak
 *   Parameters  :  hi_u3q5             padding         The multiple of roi rectangle's width and height.
 *                  hi_svp_dst_mem_info *gauss_peak     The memory of gauss peak.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_kcf_create_gauss_peak(hi_u3q5 padding, const hi_svp_dst_mem_info *gauss_peak);

/*
 *   Prototype    : hi_mpi_ive_kcf_create_cos_win
 *   Description  : KCF calculate cos window x and y.
 *   Parameters  :  hi_svp_dst_mem_info     *cos_win_x     The memory of cos window x.
 *                  hi_svp_dst_mem_info     *cos_win_y     The memory of cos window y.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_kcf_create_cos_win(const hi_svp_dst_mem_info *cos_win_x,
    const hi_svp_dst_mem_info *cos_win_y);

/*
 *   Prototype    : hi_mpi_ive_kcf_get_train_obj
 *   Description  : KCF get train object.
 *   Parameters  :  hi_u3q5             padding         The multiple of roi rectangle's width and height.
 *                  hi_ive_roi_info     roi_info[]      The array of roi information.
 *                  hi_u32              obj_num         The numbers of roi_info and object.
 *                  hi_svp_mem_info     *cos_win_x      The memory of cos window x.
 *                  hi_svp_mem_info     *cos_win_y      The memory of cos window y.
 *                  hi_svp_mem_info     *gauss_peak     The memory of gauss peak.
 *                  hi_ive_kcf_obj_list *obj_list       The object list.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_kcf_get_train_obj(hi_u3q5 padding, const hi_ive_roi_info roi_info[], hi_u32 obj_num,
    const hi_svp_mem_info *cos_win_x, const hi_svp_mem_info *cos_win_y, const hi_svp_mem_info *gauss_peak,
    hi_ive_kcf_obj_list *obj_list);

/*
 *   Prototype    : hi_mpi_ive_kcf_process
 *   Description  : KCF process.
 *   Parameters   : hi_ive_handle           *handle         Returned handle ID of a task
 *                  hi_svp_src_img          *src            Input source.Only the YUV420SP input format is supported.
 *                  hi_ive_kcf_obj_list     *obj_list       The object list.
 *                  hi_ive_kcf_proc_ctrl     *ctrl           Control parameter.
 *                  hi_bool                 is_instant      For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_kcf_proc(hi_ive_handle *handle, const hi_svp_src_img *src,
    hi_ive_kcf_obj_list *obj_list, const hi_ive_kcf_proc_ctrl *ctrl, hi_bool is_instant);

/*
 *   Prototype    : hi_mpi_ive_kcf_get_obj_bbox
 *   Description  : KCF get track object bbox information.
 *   Parameters  :  hi_ive_kcf_obj_list     *obj_list           The object list.
 *                  hi_ive_kcf_bbox         bbox                The output bbox of object.
 *                  hi_u32                  *bbox_obj_num       The valid numbers of output bbox.
 *                  hi_ive_kcf_bbox_ctrl    *ctrl               Control parameter.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_kcf_get_obj_bbox(hi_ive_kcf_obj_list *obj_list, hi_ive_kcf_bbox bbox[],
    hi_u32 *bbox_obj_num, const hi_ive_kcf_bbox_ctrl *ctrl);

/*
 *   Prototype    : hi_mpi_ive_kcf_judge_obj_bbox_track_state
 *   Description  : KCF judge object tracking state.
 *   Parameters  :  hi_ive_roi_info     *roi_info       The object list.
 *                  hi_ive_kcf_bbox     bbox            The bbox of object.
 *                  hi_bool             *is_track_ok    The tracking state of object.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_kcf_judge_obj_bbox_track_state(const hi_ive_roi_info *roi_info,
    const hi_ive_kcf_bbox *bbox, hi_bool *is_track_ok);

/*
 *   Prototype    : hi_mpi_ive_kcf_obj_update
 *   Description  : KCF update object.
 *   Parameters  :  hi_ive_kcf_obj_list     *obj_list       The object list.
 *                  hi_ive_kcf_bbox         bbox            The bbox information of update object.
 *                  hi_u32                  bbox_obj_num    The update numbers of bbox.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_kcf_obj_update(hi_ive_kcf_obj_list *obj_list, const hi_ive_kcf_bbox bbox[],
    hi_u32 bbox_obj_num);

/*
 *   Prototype    : hi_mpi_ive_hog
 *   Description  : get Hog feature.
 *   Parameters  :  hi_ive_handle           *handle         Returned handle ID of a task.
 *                  hi_svp_src_img          *src            Input source.Only the YUV420SP input format is supported.
 *                  hi_svp_rect_u32         roi[]           The array of roi.
 *                  hi_svp_dst_blob         dst[]           The array of Hog feature.
 *                  hi_ive_hog_ctrl         *ctrl           Control parameter.
 *                  hi_bool                 is_instant      For details, see hi_mpi_ive_dma.
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_mpi_ive_hog(hi_ive_handle *handle, const hi_svp_src_img *src, const hi_svp_rect_u32 roi[],
    const hi_svp_dst_blob dst[], const hi_ive_hog_ctrl *ctrl, hi_bool is_instant);

#ifdef __cplusplus
}
#endif
#endif /* __HI_MPI_IVE_H__ */

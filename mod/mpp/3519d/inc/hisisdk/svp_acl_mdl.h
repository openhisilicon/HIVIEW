/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef SVP_ACL_MODEL_H
#define SVP_ACL_MODEL_H

#include <stddef.h>
#include <stdint.h>

#include "svp_acl_base.h"
#include "svp_acl_rt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SVP_ACL_MAX_DIM_CNT          128
#define SVP_ACL_MAX_TENSOR_NAME_LEN  128
#define SVP_ACL_MAX_BATCH_NUM        1
#define SVP_ACL_MAX_HW_NUM           100
#define SVP_ACL_MAX_SHAPE_COUNT      1
#define SVP_ACL_INVALID_NODE_INDEX   0xFFFFFFFF

#define SVP_ACL_MDL_LOAD_FROM_FILE            1
#define SVP_ACL_MDL_LOAD_FROM_FILE_WITH_MEM   2
#define SVP_ACL_MDL_LOAD_FROM_MEM             3
#define SVP_ACL_MDL_LOAD_FROM_MEM_WITH_MEM    4
#define SVP_ACL_MDL_LOAD_FROM_FILE_WITH_Q     5
#define SVP_ACL_MDL_LOAD_FROM_MEM_WITH_Q      6

#define SVP_ACL_NET_TYPE_CNN        0x1
#define SVP_ACL_NET_TYPE_RECURRENT  (0x1 << 1)

#define SVP_ACL_DYNAMIC_TENSOR_NAME "svp_acl_mbatch_shape_data"
#define SVP_ACL_DYNAMIC_AIPP_NAME "svp_acl_dynamic_aipp_data"

typedef struct svp_acl_mdl_dataset svp_acl_mdl_dataset;
typedef struct svp_acl_mdl_desc svp_acl_mdl_desc;
typedef struct svp_acl_mdl_config_handle svp_acl_mdl_config_handle;
typedef struct svp_acl_mdl_aipp svp_acl_mdl_aipp;
typedef struct svp_acl_aipp_extend_info svp_acl_aipp_extend_info;

typedef enum {
    SVP_ACL_MDL_PRIORITY_INT32 = 0,
    SVP_ACL_MDL_LOAD_TYPE_SIZET,
    SVP_ACL_MDL_PATH_PTR, /* *< pointer to model load path with deep copy */
    SVP_ACL_MDL_MEM_ADDR_PTR, /* *< pointer to model memory with shallow copy */
    SVP_ACL_MDL_MEM_SIZET,
    SVP_ACL_MDL_WEIGHT_ADDR_PTR, /* *< pointer to weight memory of model with shallow copy */
    SVP_ACL_MDL_WEIGHT_SIZET,
    SVP_ACL_MDL_WORKSPACE_ADDR_PTR, /* *< pointer to workspace memory of model with shallow copy */
    SVP_ACL_MDL_WORKSPACE_SIZET,
    SVP_ACL_MDL_INPUTQ_NUM_SIZET,
    SVP_ACL_MDL_INPUTQ_ADDR_PTR, /* *< pointer to inputQ with shallow copy */
    SVP_ACL_MDL_OUTPUTQ_NUM_SIZET,
    SVP_ACL_MDL_OUTPUTQ_ADDR_PTR, /* *< pointer to outputQ with shallow copy */
    SVP_ACL_MDL_PRIORITY_PREEMP_EN_BOOL, /* RW; Range: [0, 1], Default: 1; the task can preemp the low-priority task */
    SVP_ACL_MDL_PRIORITY_UP_STEP_TIMEOUT_UINT32,
    SVP_ACL_MDL_PRIORITY_UP_TOP_TIMEOUT_UINT32,
} svp_acl_mdl_config_attr;

typedef enum {
    SVP_ACL_YUV420SP_U8 = 0x1,
    SVP_ACL_XRGB8888_U8 = 0x2,
    SVP_ACL_RGB888_U8 = 0x3,
    SVP_ACL_YUV400_U8 = 0x4,
    SVP_ACL_NC1HWC0DI_FP16 = 0x5, /* not support */
    SVP_ACL_NC1HWC0DI_S8 = 0x6, /* not support */
    SVP_ACL_ARGB8888_U8 = 0x7, /* not support */
    SVP_ACL_YUYV_U8 = 0x8, /* not support */
    SVP_ACL_YUV422SP_U8 = 0x9,
    SVP_ACL_AYUV444_U8 = 0xA, /* not support */
    SVP_ACL_RAW10 = 0xB, /* not support */
    SVP_ACL_RAW12 = 0xC, /* not support */
    SVP_ACL_RAW16 = 0xD, /* not support */
    SVP_ACL_RAW24 = 0xE, /* not support */
    SVP_ACL_FEATURE_MAP_U8 = 0x100,
    SVP_ACL_FEATURE_MAP_S8 = 0x101,
    SVP_ACL_FEATURE_MAP_U16 = 0x102,
    SVP_ACL_FEATURE_MAP_S16 = 0x103,
    SVP_ACL_FEATURE_MAP_FP16 = 0x104,
    SVP_ACL_FEATURE_MAP_FP32 = 0x105,
    SVP_ACL_YVU420SP_U8 = 0x106,
    SVP_ACL_YVU422SP_U8 = 0x107,
    SVP_ACL_BGR_PLANAR_U8 = 0x108,
    SVP_ACL_RGB_PLANAR_U8 = 0x109,
    SVP_ACL_BGR_PACKAGE_U8 = 0x10A,
    SVP_ACL_XRGB_PLANAR_U8 = 0x10B,
    SVP_ACL_XBGR_PLANAR_U8 = 0x10C,
    SVP_ACL_RGBX_PLANAR_U8 = 0x10D,
    SVP_ACL_BGRX_PLANAR_U8 = 0x10E,
    SVP_ACL_XBGR_PACKAGE_U8 = 0x10F,
    SVP_ACL_RGBX_PACKAGE_U8 = 0x110,
    SVP_ACL_BGRX_PACKAGE_U8 = 0x111,
    SVP_ACL_RGGB_RAW8 = 0x112,
    SVP_ACL_GRBG_RAW8 = 0x113,
    SVP_ACL_GBRG_RAW8 = 0x114,
    SVP_ACL_BGGR_RAW8 = 0x115,
    SVP_ACL_RGGB_RAW10 = 0x116,
    SVP_ACL_GRBG_RAW10 = 0x117,
    SVP_ACL_GBRG_RAW10 = 0x118,
    SVP_ACL_BGGR_RAW10 = 0x119,
    SVP_ACL_RGGB_RAW12 = 0x11a,
    SVP_ACL_GRBG_RAW12 = 0x11b,
    SVP_ACL_GBRG_RAW12 = 0x11c,
    SVP_ACL_BGGR_RAW12 = 0x11d,
    SVP_ACL_RGGB_RAW14 = 0x11e,
    SVP_ACL_GRBG_RAW14 = 0x11f,
    SVP_ACL_GBRG_RAW14 = 0x120,
    SVP_ACL_BGGR_RAW14 = 0x121,
    SVP_ACL_RGGB_RAW16 = 0x122,
    SVP_ACL_GRBG_RAW16 = 0x123,
    SVP_ACL_GBRG_RAW16 = 0x124,
    SVP_ACL_BGGR_RAW16 = 0x125,
    SVP_ACL_UV_PLANNAR_U8 = 0x126,  /* not support */
    SVP_ACL_VU_PLANNAR_U8 = 0x127,  /* not support */
    SVP_ACL_UV_PACKAGE_U8 = 0x128,
    SVP_ACL_VU_PACKAGE_U8 = 0x129,

    SVP_ACL_AIPP_RESERVED = 0xFFFF
} svp_acl_aipp_format;

typedef svp_acl_aipp_format svp_acl_aipp_input_format;

typedef enum {
    SVP_ACL_DATA_WITHOUT_AIPP = 0,
    SVP_ACL_DATA_WITH_STATIC_AIPP,
    SVP_ACL_DATA_WITH_DYNAMIC_AIPP,
    SVP_ACL_DYNAMIC_AIPP_NODE
} svp_acl_mdl_aipp_type;

typedef struct svp_acl_mdl_io_dims {
    char name[SVP_ACL_MAX_TENSOR_NAME_LEN]; /* tensor name */
    size_t dim_count;  /* dim array count */
    int64_t dims[SVP_ACL_MAX_DIM_CNT]; /* dim data array */
} svp_acl_mdl_io_dims;

typedef struct {
    svp_acl_mdl_io_dims src_dims;       /* input dims before model transform */
    size_t src_size;                    /* input size before model transform */
    svp_acl_mdl_io_dims aipp_out_dims;  /* aipp output dims */
    size_t aipp_out_size;               /* aipp output size */
} svp_acl_aipp_dims;

typedef struct {
    size_t hw_count; /* height&width array count */
    uint64_t hw[SVP_ACL_MAX_HW_NUM][2]; /* height&width data array */
} svp_acl_mdl_hw;

typedef enum {
    AIPP_PADDING_SWITCH_OFF = 0,
    AIPP_PADDING_SWITCH_CONSTANT,
    AIPP_PADDING_SWITCH_REPLICATION,
    AIPP_PADDING_SWITCH_REFLECTION,
    AIPP_PADDING_SWITCH_SYMMETRIC,
    AIPP_PADDING_SWITCH_BUTT
} svp_acl_aipp_padding_switch;

typedef enum {
    AIPP_RESIZE_SWITCH_OFF = 0,
    AIPP_RESIZE_SWITCH_NEAREST,
    AIPP_RESIZE_SWITCH_BILINEAR,
    AIPP_RESIZE_SWITCH_BUTT
} svp_acl_aipp_resize_switch;

typedef struct {
    svp_acl_aipp_format aipp_format;
    int32_t src_image_width;
    int32_t src_image_height;
    int8_t crop_switch;
    int32_t crop_start_width;
    int32_t crop_start_height;
    int32_t crop_width;
    int32_t crop_height;
    int8_t resize_switch;
    int32_t resize_output_width;
    int32_t resize_output_height;
    int8_t padding_switch;
    int32_t left_padding_size;
    int32_t right_padding_size;
    int32_t top_padding_size;
    int32_t bottom_padding_size;
    int8_t csc_switch;
    int8_t rbuv_swap_switch;
    int8_t ax_swap_switch;
    int8_t cmp_switch;
    int32_t matrix_r0c0;
    int32_t matrix_r0c1;
    int32_t matrix_r0c2;
    int32_t matrix_r1c0;
    int32_t matrix_r1c1;
    int32_t matrix_r1c2;
    int32_t matrix_r2c0;
    int32_t matrix_r2c1;
    int32_t matrix_r2c2;
    int32_t output_bias0;
    int32_t output_bias1;
    int32_t output_bias2;
    int32_t input_bias0;
    int32_t input_bias1;
    int32_t input_bias2;
    float mean_chn0;
    float mean_chn1;
    float mean_chn2;
    float mean_chn3;
    float min_chn0;
    float min_chn1;
    float min_chn2;
    float min_chn3;
    float var_reci_chn0;
    float var_reci_chn1;
    float var_reci_chn2;
    float var_reci_chn3;
    svp_acl_format src_format;
    svp_acl_data_type src_data_type;
    size_t src_dim_num;
    size_t shape_count;
    svp_acl_aipp_dims out_dims[SVP_ACL_MAX_SHAPE_COUNT];
    svp_acl_aipp_extend_info *aipp_extend;
} svp_acl_aipp_info;

typedef struct svp_acl_mdl_batch {
    size_t batch_count; /* batch array count, only support 1 */
    uint64_t batch[SVP_ACL_MAX_BATCH_NUM]; /* batch data array */
} svp_acl_mdl_batch;

typedef enum {
    SVP_ACL_BUFFER_REUSE_UNDEFINED = -1,
    SVP_ACL_BUFFER_REUSE_NONE = 0,
    SVP_ACL_BUFFER_REUSE_DISCONT = 1,
    SVP_ACL_BUFFER_REUSE_IN_CONT = 2,
    SVP_ACL_BUFFER_REUSE_OUT_CONT = 3,
    SVP_ACL_BUFFER_REUSE_IN_OUT_CONT = 4,
    SVP_ACL_BUFFER_REUSE_INOUT_CONT = 5,
} svp_acl_buffer_reuse_type;

typedef enum {
    SVP_ACL_INOUT_REUSE_UNDEFINED = -1,
    SVP_ACL_INOUT_REUSE_NONE = 0,
    SVP_ACL_INOUT_REUSE_DISCONT = 1,
    SVP_ACL_INOUT_REUSE_CONT = 2,
} svp_acl_inout_reuse_type;

/**
 * @ingroup svp_acl
 * @brief Create data of type svp_acl_mdl_desc
 *
 * @retval the svp_acl_mdl_desc pointer
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_mdl_desc *svp_acl_mdl_create_desc();

/**
 * @ingroup svp_acl
 * @brief destroy data of type svp_acl_mdl_desc
 *
 * @param model_desc [IN]   Pointer to svp_acl_mdl_desc to be destroyed
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_destroy_desc(svp_acl_mdl_desc *model_desc);

/**
 * @ingroup svp_acl
 * @brief Get svp_acl_mdl_desc data of the model according to the model ID
 *
 * @param  model_desc [OUT]   svp_acl_mdl_desc pointer
 * @param  model_id [IN]      model id
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_get_desc(svp_acl_mdl_desc *model_desc, uint32_t model_id);

/**
 * @ingroup svp_acl
 * @brief Get the number of the inputs of
 *        the model according to data of svp_acl_mdl_desc
 *
 * @param  model_desc [IN]   svp_acl_mdl_desc pointer
 *
 * @retval input size with svp_acl_mdl_desc
 */
SVP_ACL_FUNC_VISIBILITY size_t svp_acl_mdl_get_num_inputs(const svp_acl_mdl_desc *model_desc);

/**
 * @ingroup svp_acl
 * @brief Get the number of the output of
 *        the model according to data of svp_acl_mdl_desc
 *
 * @param  model_desc [IN]   svp_acl_mdl_desc pointer
 *
 * @retval output size with svp_acl_mdl_desc
 */
SVP_ACL_FUNC_VISIBILITY size_t svp_acl_mdl_get_num_outputs(const svp_acl_mdl_desc *model_desc);

/**
 * @ingroup svp_acl
 * @brief Get the size of the specified input according to
 *        the data of type svp_acl_mdl_desc
 *
 * @param  model_desc [IN]  svp_acl_mdl_desc pointer
 * @param  index [IN] the size of the number of inputs to be obtained,
 *         the index value starts from 0
 *
 * @retval Specify the size of the input
 */
SVP_ACL_FUNC_VISIBILITY size_t svp_acl_mdl_get_input_size_by_index(const svp_acl_mdl_desc *model_desc,
    size_t index);

/**
 * @ingroup svp_acl
 * @brief Get the size of the specified output according to
 *        the data of type svp_acl_mdl_desc
 *
 * @param model_desc [IN]   svp_acl_mdl_desc pointer
 * @param index [IN]  the size of the number of outputs to be obtained,
 *        the index value starts from 0
 *
 * @retval Specify the size of the output
 */
SVP_ACL_FUNC_VISIBILITY size_t svp_acl_mdl_get_output_size_by_index(const svp_acl_mdl_desc *model_desc,
    size_t index);

/**
 * @ingroup svp_acl
 * @brief Get the number of the output of
 *        the model according to data of svp_acl_mdl_desc
 *
 * @param  model_desc [IN]   svp_acl_mdl_desc pointer
 *
 * @retval output buffer_resue_type with svp_acl_mdl_desc
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_buffer_reuse_type svp_acl_mdl_get_reuse_buffer_type(
    const svp_acl_mdl_desc *model_desc);

/**
 * @ingroup svp_acl
 * @brief Get the size of the specified input according to
 *        the data of type svp_acl_mdl_desc
 *
 * @param  model_desc [IN]  svp_acl_mdl_desc pointer
 * @param  index [IN] the size of the number of inputs to be obtained,
 *         the index value starts from 0
 *
 * @retval Specify the inout_reuse_type of the input
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_inout_reuse_type svp_acl_mdl_get_input_reuse_type(
    const svp_acl_mdl_desc *model_desc, size_t index);

/**
 * @ingroup svp_acl
 * @brief Get the size of the specified output according to
 *        the data of type svp_acl_mdl_desc
 *
 * @param model_desc [IN]   svp_acl_mdl_desc pointer
 * @param index [IN]  the size of the number of outputs to be obtained,
 *        the index value starts from 0
 *
 * @retval Specify the inout_reuse_type of the output
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_inout_reuse_type svp_acl_mdl_get_output_reuse_type(
    const svp_acl_mdl_desc *model_desc, size_t index);

/**
 * @ingroup svp_acl
 * @brief Create data of type svp_acl_mdl_dataset
 *
 * @retval the svp_acl_mdl_dataset pointer
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_mdl_dataset *svp_acl_mdl_create_dataset();

/**
 * @ingroup svp_acl
 * @brief destroy data of type svp_acl_mdl_dataset
 *
 * @param  dataset [IN]  Pointer to svp_acl_mdl_dataset to be destroyed
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_destroy_dataset(const svp_acl_mdl_dataset *dataset);

/**
 * @ingroup svp_acl
 * @brief Add svp_acl_data_buffer to svp_acl_mdl_dataset
 *
 * @param dataset [OUT]    svp_acl_mdl_dataset address of svp_acl_data_buffer to be added
 * @param data_buffer [IN]  svp_acl_data_buffer address to be added
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_add_dataset_buffer(svp_acl_mdl_dataset *dataset,
    svp_acl_data_buffer *data_buffer);

/**
 * @ingroup svp_acl
 * @brief Get the number of svp_acl_data_buffer in svp_acl_mdl_dataset
 *
 * @param dataset [IN]   svp_acl_mdl_dataset pointer
 *
 * @retval the number of svp_acl_data_buffer
 */
SVP_ACL_FUNC_VISIBILITY size_t svp_acl_mdl_get_dataset_num_buffers(const svp_acl_mdl_dataset *dataset);

/**
 * @ingroup svp_acl
 * @brief Get the svp_acl_data_buffer in svp_acl_mdl_dataset by index
 *
 * @param dataset [IN]   svp_acl_mdl_dataset pointer
 * @param index [IN]     the index of svp_acl_data_buffer
 *
 * @retval Get successfully, return the address of svp_acl_data_buffer
 * @retval Failure return NULL
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_data_buffer *svp_acl_mdl_get_dataset_buffer(const svp_acl_mdl_dataset *dataset,
    size_t index);

/**
 * @ingroup svp_acl
 * @brief Load offline model data from memory and manage the memory of
 * model running internally by the system
 *
 * @par Function
 * After the system finishes loading the model,
 * the model ID returned is used as a mark to identify the model
 * during subsequent operations
 *
 * @param model [IN]      Model data stored in memory
 * @param model_size [IN]  model data size
 * @param model_id [OUT]   Model ID generated after
 *        the system finishes loading the model
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_load_from_mem(const void *model, size_t model_size,
    uint32_t *model_id);

/**
 * @ingroup svp_acl
 * @brief Execute model synchronous inference until the inference result is returned
 *
 * @param  model_id [IN]   ID of the model to perform inference
 * @param  input [IN]     Input data for model inference
 * @param  output [OUT]   Output data for model inference
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_execute(uint32_t model_id, const svp_acl_mdl_dataset *input,
    const svp_acl_mdl_dataset *output);

/**
 * @ingroup svp_acl
 * @brief Execute model asynchronous inference until the inference result is returned
 *
 * @param  model_id [IN]   ID of the model to perform inference
 * @param  input [IN]     Input data for model inference
 * @param  output [OUT]   Output data for model inference
 * @param  stream [IN]    stream
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 *
 * @see svp_acl_mdl_load_from_mem
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_execute_async(uint32_t model_id,
    const svp_acl_mdl_dataset *input, const svp_acl_mdl_dataset *output, svp_acl_rt_stream stream);

/**
 * @ingroup svp_acl
 * @brief unload model with model id
 *
 * @param  model_id [IN]   model id to be unloaded
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_unload(uint32_t model_id);

/**
 * @ingroup svp_acl
 * @brief In dynamic batch scenarios,
 * it is used to set the number of images processed
 * at one time during model inference
 *
 * @param  model_id [IN]     model id
 * @param  dataset [IN|OUT] data for model inference
 * @param  index [IN]       index of dynamic tensor
 * @param  batch_size [IN]   Number of images processed at a time during model
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 *
 * @see svp_acl_mdl_load_from_mem | svp_acl_mdl_get_input_index_by_name
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_dynamic_batch_size(uint32_t model_id,
    svp_acl_mdl_dataset *dataset, size_t index, uint64_t batch_size);

/**
 * @ingroup svp_acl
 * @brief In recurrent net scenarios,
 * it is used to set the T dim size of recurrent net
 *
 * @param  model_id [IN]    model id
 * @param  dataset [IN]    data for model inference
 * @param  total_t [IN]     T dim size
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 *
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_total_t(uint32_t model_id, svp_acl_mdl_dataset *dataset,
    uint64_t total_t);

/**
 * @ingroup svp_acl
 * @brief Sets the H and W of the specified input of the model
 *
 * @param  model_id [IN]     model id
 * @param  dataset [IN|OUT]  data for model inference
 * @param  index [IN]        index of dynamic tensor
 * @param  height [IN]       model height
 * @param  width [IN]        model width
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see svp_acl_mdl_load_from_mem | svp_acl_mdl_get_input_index_by_name
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_dynamic_hw_size(uint32_t model_id,
    svp_acl_mdl_dataset *dataset, size_t index, uint64_t height, uint64_t width);

/**
 * @ingroup svp_acl
 * @brief get dynamic height&width info
 *
 * @param model_desc [IN]  model description
 * @param index [IN]  input tensor index
 * @param hw [OUT]  dynamic height&width info
 *
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_get_dynamic_hw(const svp_acl_mdl_desc *model_desc,
    size_t index, svp_acl_mdl_hw *hw);

/**
 * @ingroup svp_acl
 * @brief get input security info
 *
 * @param model_desc [IN]  model description
 * @param index [IN]  input tensor index
 * @param class_id [OUT]  secure class id
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_get_input_class_id_by_index(const svp_acl_mdl_desc *model_desc,
    size_t index, size_t *class_id);

/**
 * @ingroup svp_acl
 * @brief get output security info
 *
 * @param model_desc [IN]  model description
 * @param index [IN]  output tensor index
 * @param class_id [OUT]  secure class id
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_get_output_class_id_by_index(const svp_acl_mdl_desc *model_desc,
    size_t index, size_t *class_id);

/**
 * @ingroup svp_acl
 * @brief get input dims info
 *
 * @param model_desc [IN]  model description
 * @param index [IN]  input tensor index
 * @param dims [OUT]  dims info
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_get_input_dims(const svp_acl_mdl_desc *model_desc,
    size_t index, svp_acl_mdl_io_dims *dims);

/**
 * @ingroup svp_acl
 * @brief get output dims info
 *
 * @param model_desc [IN] model description
 * @param index [IN]     output tensor index
 * @param dims [OUT]     dims info
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_get_output_dims(const svp_acl_mdl_desc *model_desc,
    size_t index, svp_acl_mdl_io_dims *dims);

/**
 * @ingroup svp_acl
 * @brief get input name by index
 *
 * @param model_desc [IN]  model description
 * @param index [IN]      input tensor index
 *
 * @retval input tensor name,the same life cycle with model_id
 */
SVP_ACL_FUNC_VISIBILITY const char *svp_acl_mdl_get_input_name_by_index(const svp_acl_mdl_desc *model_desc,
    size_t index);

/**
 * @ingroup svp_acl
 * @brief get output name by index
 *
 * @param model_desc [IN]  model description
 * @param index [IN]      output tensor index
 *
 * @retval output tensor name,the same life cycle with model_id
 */
SVP_ACL_FUNC_VISIBILITY const char *svp_acl_mdl_get_output_name_by_index(const svp_acl_mdl_desc *model_desc,
    size_t index);

/**
 * @ingroup svp_acl
 * @brief get input format by index
 *
 * @param model_desc [IN]  model description
 * @param index [IN]      input tensor index
 *
 * @retval input tensor format
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_format svp_acl_mdl_get_input_format(const svp_acl_mdl_desc *model_desc,
    size_t index);

/**
 * @ingroup svp_acl
 * @brief get output format by index
 *
 * @param model_desc [IN]  model description
 * @param index [IN]      output tensor index
 *
 * @retval output tensor format
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_format svp_acl_mdl_get_output_format(const svp_acl_mdl_desc *model_desc,
    size_t index);

/**
 * @ingroup svp_acl
 * @brief get input data type by index
 *
 * @param model_desc [IN]  model description
 * @param index [IN]  input tensor index
 *
 * @retval input tensor data type
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_data_type svp_acl_mdl_get_input_data_type(const svp_acl_mdl_desc *model_desc,
    size_t index);

/**
 * @ingroup svp_acl
 * @brief get output data type by index
 *
 * @param model_desc [IN]  model description
 * @param index [IN]  output tensor index
 *
 * @retval output tensor data type
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_data_type svp_acl_mdl_get_output_data_type(const svp_acl_mdl_desc *model_desc,
    size_t index);

/**
 * @ingroup svp_acl
 * @brief get input tensor index by name
 *
 * @param model_desc [IN]  model description
 * @param name [IN]    input tensor name
 * @param index [OUT]  input tensor index
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_get_input_index_by_name(const svp_acl_mdl_desc *model_desc,
    const char *name, size_t *index);

/**
 * @ingroup svp_acl
 * @brief get output tensor index by name
 *
 * @param model_desc [IN]  model description
 * @param name [IN]  output tensor name
 * @param index [OUT]  output tensor index
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_get_output_index_by_name(const svp_acl_mdl_desc *model_desc,
    const char *name, size_t *index);

/**
 * @ingroup svp_acl
 * @brief get input tensor default stride by idx
 *
 * @param model_desc [IN]  model description
 * @param index [IN]  tensor index
 * @param stride [OUT]  output default stride
 * @retval default stride value.
 */
SVP_ACL_FUNC_VISIBILITY size_t svp_acl_mdl_get_input_default_stride(const svp_acl_mdl_desc *model_desc,
    size_t index);

/**
 * @ingroup svp_acl
 * @brief get output tensor default stride by idx
 *
 * @param model_desc [IN]  model description
 * @param index [IN]  tensor index
 * @param stride [OUT]  output default stride
 * @retval default stride value.
 */
SVP_ACL_FUNC_VISIBILITY size_t svp_acl_mdl_get_output_default_stride(const svp_acl_mdl_desc *model_desc,
    size_t index);

/**
 * @ingroup svp_acl
 * @brief init dump
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_init_dump();

/**
 * @ingroup svp_acl
 * @brief set param of dump
 *
 * @param dump_cfg_path [IN]   the path of dump config
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_dump(const char *dump_cfg_path);

/**
 * @ingroup svp_acl
 * @brief finalize dump.
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_finalize_dump();

/**
 * @ingroup svp_acl
 * @brief load model with config
 *
 * @param handle [IN]    pointer to model config handle
 * @param model_id [OUT]  pointer to model id
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_load_with_config(const svp_acl_mdl_config_handle *handle,
    uint32_t *model_id);

/**
 * @ingroup svp_acl
 * @brief set config to model
 *
 * @param model_id [IN]  pointer to model id
 * @param handle [IN]    pointer to model config handle
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_model_config(uint32_t model_id,
    const svp_acl_mdl_config_handle *handle);

/**
 * @ingroup svp_acl
 * @brief create model config handle of type svp_acl_mdl_config_handle
 *
 * @retval the svp_acl_mdl_config_handle pointer
 *
 * @see svp_acl_mdl_destroy_config_handle
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_mdl_config_handle *svp_acl_mdl_create_config_handle();

/**
 * @ingroup svp_acl
 * @brief destroy data of type svp_acl_mdl_config_handle
 *
 * @param handle [IN]   pointer to model config handle
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 *
 * @see svp_acl_mdl_create_config_handle
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_destroy_config_handle(svp_acl_mdl_config_handle *handle);

/**
 * @ingroup svp_acl
 * @brief set config for model
 *
 * @param handle [OUT]    pointer to model config handle
 * @param attr [IN]       config attr in model config handle to be set
 * @param attr_value [IN]  pointer to model config value
 * @param value_size [IN]  memory size of attr_value
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_config_opt(svp_acl_mdl_config_handle *handle,
    svp_acl_mdl_config_attr attr, const void *attr_value, size_t value_size);

/**
 * @ingroup svp_acl
 * @brief get config for model
 *
 * @param handle [OUT]    pointer to model config handle
 * @param attr [IN]       config attr in model config handle to be set
 * @param attr_value [IN]  pointer to model config value
 * @param value_size [IN]  memory size of attr_value
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_get_config_opt(svp_acl_mdl_config_handle *handle,
    svp_acl_mdl_config_attr attr, void *attr_value, size_t value_size);

/**
 * @ingroup svp_acl
 * @brief get om batch info
 *
 * @param model_desc [in]   svp_acl_mdl_desc pointer
 * @param batch [OUT]  om batch info
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_get_dynamic_batch(const svp_acl_mdl_desc *model_desc,
    svp_acl_mdl_batch *batch);

/**
 * @ingroup svp_acl
 * @brief create data of type svp_acl_mdl_aipp
 *
 * @param batch_size [IN] batch size of model
 *
 * @retval the svp_acl_mdl_aipp pointer
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_mdl_aipp *svp_acl_mdl_create_aipp(uint64_t batch_size);

/**
 * @ingroup svp_acl
 * @brief destroy data of type svp_acl_mdl_aipp
 *
 * @param aipp_set [IN]  pointer for svp_acl_mdl_aipp to be destroyed
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_destroy_aipp(svp_acl_mdl_aipp *aipp_set);

/**
 * @ingroup svp_acl
 * @brief set input_format of type svp_acl_mdl_aipp
 *
 * @param aipp_set [OUT]  pointer for svp_acl_mdl_aipp
 * @param input_format [IN]    The input format of aipp
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 *
 * @see svp_acl_mdl_create_aipp
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_aipp_input_format(svp_acl_mdl_aipp *aipp_set,
    svp_acl_aipp_input_format input_format);

/**
 * @ingroup svp_acl
 * @brief set csc params of type svp_acl_mdl_aipp
 *
 * @param aipp_set [OUT]      pointer for svp_acl_mdl_aipp
 * @param csc_switch [IN]     csc switch
 * @param matrix_r0c0 [IN]    csc_matrix_r0_c0
 * @param matrix_r0c1 [IN]    csc_matrix_r0_c1
 * @param matrix_r0c2 [IN]    csc_matrix_r0_c2
 * @param matrix_r1c0 [IN]    csc_matrix_r1_c0
 * @param matrix_r1c1 [IN]    csc_matrix_r1_c1
 * @param matrix_r1c2 [IN]    csc_matrix_r1_c2
 * @param matrix_r2c0 [IN]    csc_matrix_r2_c0
 * @param matrix_r2c1 [IN]    csc_matrix_r2_c1
 * @param matrix_r2c2 [IN]    csc_matrix_r2_c2
 * @param output_bias_r0 [IN]  output bias for RGB to YUV, element of row 0, unsigned number
 * @param output_bias_r1 [IN]  output bias for RGB to YUV, element of row 1, unsigned number
 * @param output_bias_r2 [IN]  output bias for RGB to YUV, element of row 2, unsigned number
 * @param input_bias_r0 [IN]   input bias for YUV to RGB, element of row 0, unsigned number
 * @param input_bias_r1 [IN]   input bias for YUV to RGB, element of row 1, unsigned number
 * @param input_bias_r2 [IN]   input bias for YUV to RGB, element of row 2, unsigned number
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 *
 * @see svp_acl_mdl_create_aipp
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_aipp_csc_params(svp_acl_mdl_aipp *aipp_set,
    int8_t csc_switch, int16_t matrix_r0c0, int16_t matrix_r0c1, int16_t matrix_r0c2,
    int16_t matrix_r1c0, int16_t matrix_r1c1, int16_t matrix_r1c2,
    int16_t matrix_r2c0, int16_t matrix_r2c1, int16_t matrix_r2c2,
    uint8_t output_bias_r0, uint8_t output_bias_r1, uint8_t output_bias_r2,
    uint8_t input_bias_r0, uint8_t input_bias_r1, uint8_t input_bias_r2);

/**
 * @ingroup svp_acl
 * @brief set rb/ub swap switch of type svp_acl_mdl_aipp
 *
 * @param aipp_set [OUT]  pointer for svp_acl_mdl_aipp
 * @param rbuv_swap_switch [IN] rb/ub swap switch
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 *
 * @see svp_acl_mdl_create_aipp
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_aipp_rbuv_swap_switch(svp_acl_mdl_aipp *aipp_set,
    int8_t rbuv_swap_switch);

/**
 * @ingroup svp_acl
 * @brief set RGBA->ARGB, YUVA->AYUV swap switch of type svp_acl_mdl_aipp
 *
 * @param aipp_set [OUT]  pointer for svp_acl_mdl_aipp
 * @param ax_swap_switch [IN]   RGBA->ARGB, YUVA->AYUV swap switch
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 *
 * @see svp_acl_mdl_create_aipp
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_aipp_ax_swap_switch(svp_acl_mdl_aipp *aipp_set,
    int8_t ax_swap_switch);

/**
 * @ingroup svp_acl
 * @brief set source image of type svp_acl_mdl_aipp
 *
 * @param aipp_set [OUT]     pointer for svp_acl_mdl_aipp
 * @param image_width [IN]   source image width
 * @param image_height [IN]  source image height
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 *
 * @see svp_acl_mdl_create_aipp
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_aipp_src_image_size(svp_acl_mdl_aipp *aipp_set,
    int32_t image_width, int32_t image_height);

/**
 * @ingroup svp_acl
 * @brief set resize switch of type svp_acl_mdl_aipp
 *
 * @param aipp_set [OUT]  pointer for svp_acl_mdl_aipp
 * @param scf_switch [IN]   resize switch
 * @param input_width [IN]  input width of scf
 * @param input_height [IN] input height of scf
 * @param output_width [IN]  output width of scf
 * @param output_height [IN] output height of scf
 * @param batch_index [IN]   batch parameter index
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 *
 * @see svp_acl_mdl_create_aipp
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_aipp_scf_params(svp_acl_mdl_aipp *aipp_set,
    int8_t scf_switch, int32_t input_width, int32_t input_height, int32_t output_width, int32_t output_height,
    uint64_t batch_index);

/**
 * @ingroup svp_acl
 * @brief set crop params of type svp_acl_mdl_aipp
 *
 * @param aipp_set [OUT]  pointer for svp_acl_mdl_aipp
 * @param crop_switch [IN]     crop switch
 * @param width_start_pos [IN]  the start horizontal position of cropping
 * @param height_start_pos [IN] the start vertical position of cropping
 * @param width [IN]      crop width
 * @param height [IN]     crop height
 * @param batch_index [IN]     batch parameter index
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 *
 * @see svp_acl_mdl_create_aipp
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_aipp_crop_params(svp_acl_mdl_aipp *aipp_set,
    int8_t crop_switch, int32_t width_start_pos, int32_t height_start_pos, int32_t width, int32_t height,
    uint64_t batch_index);

/**
 * @ingroup svp_acl
 * @brief set paddingParams of type svp_acl_mdl_aipp
 *
 * @param aipp_set [OUT]    pointer for svp_acl_mdl_aipp
 * @param switch [IN]       padding switch, see svp_acl_aipp_padding_switch
 * @param top [IN]          top padding size
 * @param bottom [IN]       bottom padding size
 * @param left [IN]         left padding size
 * @param right [IN]        right padding size
 * @param batch_index [IN]  batch parameter index
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 *
 * @see svp_acl_mdl_create_aipp
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_aipp_padding_params(svp_acl_mdl_aipp *aipp_set,
    int8_t padding_switch, int32_t top, int32_t bottom, int32_t left, int32_t right, uint64_t batch_index);

/**
 * @ingroup svp_acl
 * @brief set pixel mean of type svp_acl_mdl_aipp
 *
 * @param aipp_set [OUT]   pointer for svp_acl_mdl_aipp
 * @param mean_chn0 [IN]   mean value of channel 0
 * @param mean_chn1 [IN]   mean value of channel 1
 * @param mean_chn2 [IN]   mean value of channel 2
 * @param mean_chn3 [IN]   mean value of channel 3
 * @param batch_index [IN] batch parameter index
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 *
 * @see svp_acl_mdl_create_aipp
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_aipp_dtc_pixel_mean(svp_acl_mdl_aipp *aipp_set,
    float mean_chn0, float mean_chn1, float mean_chn2, float mean_chn3, uint64_t batch_index);

/**
 * @ingroup svp_acl
 * @brief set pixel min of type svp_acl_mdl_aipp
 *
 * @param aipp_set [OUT] pointer for svp_acl_mdl_aipp
 * @param min_chn0 [IN]  min value of channel 0
 * @param min_chn1 [IN]  min value of channel 1
 * @param min_chn2 [IN]  min value of channel 2
 * @param min_chn3 [IN]  min value of channel 3
 * @param batch_index [IN] batch parameter index
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 *
 * @see svp_acl_mdl_create_aipp
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_aipp_dtc_pixel_min(svp_acl_mdl_aipp *aipp_set,
    float min_chn0, float min_chn1, float min_chn2, float min_chn3, uint64_t batch_index);

/**
 * @ingroup svp_acl
 * @brief set pixel variance recipe of type svp_acl_mdl_aipp
 *
 * @param aipp_set [OUT]   pointer for svp_acl_mdl_aipp
 * @param reci_chn0 [IN]   sfr_dtc_pixel_variance_reci_ch0
 * @param reci_chn1 [IN]   sfr_dtc_pixel_variance_reci_ch1
 * @param reci_chn2 [IN]   sfr_dtc_pixel_variance_reci_ch2
 * @param reci_chn3 [IN]   sfr_dtc_pixel_variance_reci_ch3
 * @param batch_index [IN] batch parameter index
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 *
 * @see svp_acl_mdl_create_aipp
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_aipp_pixel_var_reci(svp_acl_mdl_aipp *aipp_set,
    float reci_chn0, float reci_chn1, float reci_chn2, float reci_chn3, uint64_t batch_index);

/**
 * @ingroup svp_acl
 * @brief set aipp parameters to model
 *
 * @param model_id [IN]   model id
 * @param dataset [IN]    pointer of dataset
 * @param index [IN]      index of input for aipp data(SVP_ACL_DYNAMIC_AIPP_NODE)
 * @param aipp_set [IN]   pointer for svp_acl_mdl_aipp
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 *
 * @see svp_acl_mdl_load_from_mem | svp_acl_mdl_get_input_index_by_name | svp_acl_mdl_create_aipp
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_input_aipp(uint32_t model_id,
    svp_acl_mdl_dataset *dataset, size_t index, const svp_acl_mdl_aipp *aipp_set);

/**
 * @ingroup svp_acl
 * @brief set aipp parameters to model
 *
 * @param model_id [IN] model id
 * @param dataset [IN]  pointer of dataset
 * @param index [IN]    index of output for aipp data(SVP_ACL_DYNAMIC_AIPP_NODE)
 * @param aipp_set [IN] pointer for svp_acl_mdl_aipp
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 *
 * @see svp_acl_mdl_load_from_mem | svp_acl_mdl_get_output_index_by_name | svp_acl_mdl_create_aipp
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_output_aipp(uint32_t model_id,
    svp_acl_mdl_dataset *dataset, size_t index, const svp_acl_mdl_aipp *aipp_set);

/**
 * @ingroup svp_acl
 * @brief set aipp parameters to model
 *
 * @param model_id [IN]        model id
 * @param dataset [IN]        Pointer of dataset
 * @param index [IN]          index of input for data which linked dynamic aipp(SVP_ACL_DATA_WITH_DYNAMIC_AIPP)
 * @param aipp_set [IN]   Pointer for svp_acl_mdl_aipp
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 *
 * @see svp_acl_mdl_load_from_mem | svp_acl_mdl_get_input_index_by_name | svp_acl_mdl_create_aipp
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_aipp_by_input_index(uint32_t model_id,
    svp_acl_mdl_dataset *dataset, size_t index, const svp_acl_mdl_aipp *aipp_set);

/**
 * @ingroup svp_acl
 * @brief get input aipp type
 *
 * @param model_id [IN] model id
 * @param index [IN]    index of input
 * @param type [OUT]    aipp type for input.refrer to svp_acl_mdl_aipp_type(enum)
 * @param attached_index [OUT]     index for dynamic attached data valid when type is SVP_ACL_DATA_WITH_DYNAMIC_AIPP,
 *        invalid value is SVP_ACL_INVALID_NODE_INDEX
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 *
 * @see svp_acl_mdl_load_from_mem | svp_acl_mdl_get_input_index_by_name | svp_acl_mdl_create_aipp
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_get_input_aipp_type(uint32_t model_id, size_t index,
    svp_acl_mdl_aipp_type *type, size_t *attached_index);

/**
 * @ingroup svp_acl
 * @brief get output aipp type
 *
 * @param model_id [IN] model id
 * @param index [IN]    index of output
 * @param type [OUT]    aipp type for input.refrer to svp_acl_mdl_aipp_type(enum)
 * @param attached_index [OUT]     index for dynamic attached data valid when type is SVP_ACL_DATA_WITH_DYNAMIC_AIPP,
 *        invalid value is SVP_ACL_INVALID_NODE_INDEX
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 *
 * @see svp_acl_mdl_load_from_mem | svp_acl_mdl_get_output_index_by_name | svp_acl_mdl_create_aipp
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_get_output_aipp_type(uint32_t model_id, size_t index,
    svp_acl_mdl_aipp_type *type, size_t *attached_index);

/**
 * @ingroup svp_acl
 * @brief get input static aipp parameters from model
 *
 * @param model_id [IN]     model id
 * @param index [IN]        index of input
 * @param aipp_info [OUT]   Pointer for static aipp info
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_get_first_aipp_info(uint32_t model_id,
    size_t index, svp_acl_aipp_info *aipp_info);

/**
 * @ingroup svp_acl
 * @brief get output static aipp parameters from model
 *
 * @param model_id [IN]     model id
 * @param index [IN]        index of output
 * @param aipp_info [OUT]   Pointer for static aipp info
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval other values failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_get_last_aipp_info(uint32_t model_id,
    size_t index, svp_acl_aipp_info *aipp_info);

#ifdef __cplusplus
}
#endif

#endif /* SVP_ACL_MODEL_H */

/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
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
#define SVP_ACL_MAX_HW_NUM           128

#define SVP_ACL_MDL_LOAD_FROM_FILE            1
#define SVP_ACL_MDL_LOAD_FROM_FILE_WITH_MEM   2
#define SVP_ACL_MDL_LOAD_FROM_MEM             3
#define SVP_ACL_MDL_LOAD_FROM_MEM_WITH_MEM    4
#define SVP_ACL_MDL_LOAD_FROM_FILE_WITH_Q     5
#define SVP_ACL_MDL_LOAD_FROM_MEM_WITH_Q      6

#define SVP_ACL_NET_TYPE_CNN        0x1
#define SVP_ACL_NET_TYPE_RECURRENT  (0x1 << 1)

#define SVP_ACL_DYNAMIC_TENSOR_NAME "svp_acl_mbatch_shape_data"

typedef struct svp_acl_mdl_dataset svp_acl_mdl_dataset;
typedef struct svp_acl_mdl_desc svp_acl_mdl_desc;
typedef struct svp_acl_mdl_config_handle svp_acl_mdl_config_handle;

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
    SVP_ACL_MDL_OUTPUTQ_ADDR_PTR /* *< pointer to outputQ with shallow copy */
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
    SVP_ACL_AIPP_RESERVED = 0xFFFF
} svp_acl_aipp_input_format;

typedef struct svp_acl_mdl_io_dims {
    char name[SVP_ACL_MAX_TENSOR_NAME_LEN]; /* tensor name */
    size_t dim_count;  /* dim array count */
    int64_t dims[SVP_ACL_MAX_DIM_CNT]; /* dim data array */
} svp_acl_mdl_io_dims;

typedef struct svp_acl_aipp_info {
    svp_acl_aipp_input_format input_format;
} svp_acl_aipp_info;

typedef struct svp_acl_mdl_batch {
    size_t batch_count; /* batch array count, only support 1 */
    uint64_t batch[SVP_ACL_MAX_BATCH_NUM]; /* batch data array */
} svp_acl_mdl_batch;

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
 * @retval OtherValues Failure
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
 * @retval OtherValues Failure
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
SVP_ACL_FUNC_VISIBILITY size_t svp_acl_mdl_get_input_size_by_index(const svp_acl_mdl_desc *model_desc, size_t index);

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
SVP_ACL_FUNC_VISIBILITY size_t svp_acl_mdl_get_output_size_by_index(const svp_acl_mdl_desc *model_desc, size_t index);

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
 * @retval OtherValues Failure
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
 * @retval OtherValues Failure
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
 * @retval OtherValues Failure
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
 * @retval OtherValues Failure
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
 * @retval OtherValues Failure
 *
 * @see svp_acl_mdl_load_from_mem
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_execute_async(uint32_t model_id, const svp_acl_mdl_dataset *input,
    const svp_acl_mdl_dataset *output, svp_acl_rt_stream stream);

/**
 * @ingroup svp_acl
 * @brief unload model with model id
 *
 * @param  model_id [IN]   model id to be unloaded
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
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
 * @retval OtherValues Failure
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
 * @retval OtherValues Failure
 *
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_total_t(uint32_t model_id, svp_acl_mdl_dataset *dataset,
    uint64_t total_t);

/**
 * @ingroup svp_acl
 * @brief get input dims info
 *
 * @param model_desc [IN]  model description
 * @param index [IN]  input tensor index
 * @param dims [OUT]  dims info
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_get_input_dims(const svp_acl_mdl_desc *model_desc, size_t index,
    svp_acl_mdl_io_dims *dims);

/**
 * @ingroup svp_acl
 * @brief get output dims info
 *
 * @param model_desc [IN] model description
 * @param index [IN]     output tensor index
 * @param dims [OUT]     dims info
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_get_output_dims(const svp_acl_mdl_desc *model_desc, size_t index,
    svp_acl_mdl_io_dims *dims);

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
SVP_ACL_FUNC_VISIBILITY svp_acl_format svp_acl_mdl_get_input_format(const svp_acl_mdl_desc *model_desc, size_t index);

/**
 * @ingroup svp_acl
 * @brief get output format by index
 *
 * @param model_desc [IN]  model description
 * @param index [IN]      output tensor index
 *
 * @retval output tensor format
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_format svp_acl_mdl_get_output_format(const svp_acl_mdl_desc *model_desc, size_t index);

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
 * @retval OtherValues Failure
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
 * @retval OtherValues Failure
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
SVP_ACL_FUNC_VISIBILITY size_t svp_acl_mdl_get_input_default_stride(const svp_acl_mdl_desc *model_desc, size_t index);

/**
 * @ingroup svp_acl
 * @brief get output tensor default stride by idx
 *
 * @param model_desc [IN]  model description
 * @param index [IN]  tensor index
 * @param stride [OUT]  output default stride
 * @retval default stride value.
 */
SVP_ACL_FUNC_VISIBILITY size_t svp_acl_mdl_get_output_default_stride(const svp_acl_mdl_desc *model_desc, size_t index);

/**
 * @ingroup svp_acl
 * @brief init dump
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_init_dump();

/**
 * @ingroup svp_acl
 * @brief set param of dump
 *
 * @param dump_cfg_path [IN]   the path of dump config
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_dump(const char *dump_cfg_path);

/**
 * @ingroup svp_acl
 * @brief finalize dump.
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_finalize_dump();

/**
 * @ingroup svp_acl
 * @brief load model with config
 *
 * @param handle [IN]    pointer to model config handle
 * @param modelId [OUT]  pointer to model id
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
*/
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_load_with_config(const svp_acl_mdl_config_handle *handle,
    uint32_t *model_id);

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
 * @retval OtherValues Failure
 *
 * @see svp_acl_mdl_create_config_handle
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_destroy_config_handle(svp_acl_mdl_config_handle *handle);

/**
 * @ingroup svp_acl
 * @brief set config for model load
 *
 * @param handle [OUT]    pointer to model config handle
 * @param attr [IN]       config attr in model config handle to be set
 * @param attr_value [IN]  pointer to model config value
 * @param value_size [IN]  memory size of attr_value
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_set_config_opt(svp_acl_mdl_config_handle *handle,
    svp_acl_mdl_config_attr attr, const void *attr_value, size_t value_size);

/**
 * @ingroup svp_acl
 * @brief get static aipp parameters from model
 *
 * @param model_id [IN]     model id
 * @param index [IN]        index of tensor
 * @param aipp_info [OUT]   Pointer for static aipp info
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_get_first_aipp_info(uint32_t model_id,
    size_t index, svp_acl_aipp_info *aipp_info);

/**
 * @ingroup svp_acl
 * @brief get om batch info
 *
 * @param model_desc [in]   svp_acl_mdl_desc pointer
 * @param batch [OUT]  om batch info
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_mdl_get_dynamic_batch(const svp_acl_mdl_desc *model_desc,
    svp_acl_mdl_batch *batch);
#ifdef __cplusplus
}
#endif

#endif /* SVP_ACL_MODEL_H */

/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef SVP_ACL_BASE_H
#define SVP_ACL_BASE_H

#include <stdint.h>
#include <stdlib.h>
#ifdef SVP_FUNC_VISIBILITY
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SVP_FUNC_VISIBILITY
#define SVP_ACL_FUNC_VISIBILITY __attribute__((visibility("default")))
#else
#define SVP_ACL_FUNC_VISIBILITY
#endif

typedef void *svp_acl_rt_stream;
typedef void *svp_acl_rt_event;
typedef void *svp_acl_rt_context;
typedef int svp_acl_error;
typedef uint16_t svp_acl_float16;
typedef struct svp_acl_data_buffer svp_acl_data_buffer;

#define SVP_ACL_ERROR_NONE 0
#define SVP_ACL_SUCCESS 0

#define SVP_ACL_ERROR_INVALID_PARAM  100000
#define SVP_ACL_ERROR_UNINITIALIZE  100001
#define SVP_ACL_ERROR_REPEAT_INITIALIZE  100002
#define SVP_ACL_ERROR_INVALID_FILE  100003
#define SVP_ACL_ERROR_WRITE_FILE  100004
#define SVP_ACL_ERROR_INVALID_FILE_SIZE  100005
#define SVP_ACL_ERROR_PARSE_FILE  100006
#define SVP_ACL_ERROR_FILE_MISSING_ATTR  100007
#define SVP_ACL_ERROR_FILE_ATTR_INVALID  100008
#define SVP_ACL_ERROR_INVALID_DUMP_CONFIG 100009
#define SVP_ACL_ERROR_INVALID_PROFILING_CONFIG  100010
#define SVP_ACL_ERROR_INVALID_MODEL_ID  100011
#define SVP_ACL_ERROR_DESERIALIZE_MODEL  100012
#define SVP_ACL_ERROR_PARSE_MODEL  100013
#define SVP_ACL_ERROR_READ_MODEL_FAILURE  100014
#define SVP_ACL_ERROR_MODEL_SIZE_INVALID  100015
#define SVP_ACL_ERROR_MODEL_MISSING_ATTR  100016
#define SVP_ACL_ERROR_MODEL_INPUT_NOT_MATCH  100017
#define SVP_ACL_ERROR_MODEL_OUTPUT_NOT_MATCH  100018
#define SVP_ACL_ERROR_MODEL_NOT_DYNAMIC  100019
#define SVP_ACL_ERROR_OP_TYPE_NOT_MATCH  100020
#define SVP_ACL_ERROR_OP_INPUT_NOT_MATCH  100021
#define SVP_ACL_ERROR_OP_OUTPUT_NOT_MATCH  100022
#define SVP_ACL_ERROR_OP_ATTR_NOT_MATCH  100023
#define SVP_ACL_ERROR_OP_NOT_FOUND  100024
#define SVP_ACL_ERROR_OP_LOAD_FAILED  100025
#define SVP_ACL_ERROR_UNSUPPORTED_DATA_TYPE  100026
#define SVP_ACL_ERROR_FORMAT_NOT_MATCH  100027
#define SVP_ACL_ERROR_BIN_SELECTOR_NOT_REGISTERED  100028
#define SVP_ACL_ERROR_KERNEL_NOT_FOUND 100029
#define SVP_ACL_ERROR_BIN_SELECTOR_ALREADY_REGISTERED 100030
#define SVP_ACL_ERROR_KERNEL_ALREADY_REGISTERED  100031
#define SVP_ACL_ERROR_INVALID_QUEUE_ID 100032
#define SVP_ACL_ERROR_REPEAT_SUBSCRIBE  100033
#define SVP_ACL_ERROR_STREAM_NOT_SUBSCRIBE  100034
#define SVP_ACL_ERROR_THREAD_NOT_SUBSCRIBE  100035
#define SVP_ACL_ERROR_WAIT_CALLBACK_TIMEOUT 100036
#define SVP_ACL_ERROR_REPEAT_FINALIZE  100037
#define SVP_ACL_ERROR_NOT_STATIC_AIPP  100038
#define SVP_ACL_ERROR_COMPILING_STUB_MODE  100039
#define SVP_ACL_ERROR_GROUP_NOT_SET  100040
#define SVP_ACL_ERROR_GROUP_NOT_CREATE  100041
#define SVP_ACL_ERROR_PROF_ALREADY_RUN  100042
#define SVP_ACL_ERROR_PROF_NOT_RUN  100043
#define SVP_ACL_ERROR_DUMP_ALREADY_RUN  100044
#define SVP_ACL_ERROR_DUMP_NOT_RUN  100045
#define SVP_ACL_ERROR_PROF_REPEAT_SUBSCRIBE 148046
#define SVP_ACL_ERROR_PROF_API_CONFLICT 148047
#define SVP_ACL_ERROR_INVALID_MAX_OPQUEUE_NUM_CONFIG 148048
#define SVP_ACL_ERROR_UNSUPPORTED_API 148049

#define SVP_ACL_ERROR_BAD_ALLOC  200000
#define SVP_ACL_ERROR_API_NOT_SUPPORT  200001
#define SVP_ACL_ERROR_INVALID_DEVICE  200002
#define SVP_ACL_ERROR_MEMORY_ADDRESS_UNALIGNED  200003
#define SVP_ACL_ERROR_RESOURCE_NOT_MATCH  200004
#define SVP_ACL_ERROR_INVALID_RESOURCE_HANDLE  200005
#define SVP_ACL_ERROR_FEATURE_UNSUPPORTED  200006
#define SVP_ACL_ERROR_PROF_MODULES_UNSUPPORTED 200007

#define SVP_ACL_ERROR_STORAGE_OVER_LIMIT  300000

#define SVP_ACL_ERROR_INTERNAL_ERROR  500000
#define SVP_ACL_ERROR_FAILURE  500001
#define SVP_ACL_ERROR_GE_FAILURE  500002
#define SVP_ACL_ERROR_RT_FAILURE  500003
#define SVP_ACL_ERROR_DRV_FAILURE  500004
#define SVP_ACL_ERROR_PROFILING_FAILURE  500005
#define SVP_ACL_ERROR_RT_REPORT_TIMEOUT  507012

#define SVP_ACL_UNKNOWN_RANK 0xFFFFFFFFFFFFFFFE

typedef enum {
    SVP_ACL_DT_UNDEFINED = -1,
    SVP_ACL_FLOAT = 0,
    SVP_ACL_FLOAT16 = 1,
    SVP_ACL_INT8 = 2,
    SVP_ACL_INT32 = 3,
    SVP_ACL_UINT8 = 4,
    SVP_ACL_INT16 = 6,
    SVP_ACL_UINT16 = 7,
    SVP_ACL_UINT32 = 8,
    SVP_ACL_INT64 = 9,
    SVP_ACL_UINT64 = 10,
    SVP_ACL_DOUBLE = 11,
    SVP_ACL_BOOL = 12,
    SVP_ACL_STRING = 13,

    SVP_ACL_INT10 = 100,
    SVP_ACL_UINT10 = 101,
    SVP_ACL_INT12 = 102,
    SVP_ACL_UINT12 = 103,
    SVP_ACL_INT14 = 104,
    SVP_ACL_UINT14 = 105,
    SVP_ACL_INT24 = 106,
    SVP_ACL_UINT24 = 107,
    SVP_ACL_UINT4 = 108,
} svp_acl_data_type;

typedef enum {
    SVP_ACL_FORMAT_UNDEFINED = -1,
    SVP_ACL_FORMAT_NCHW = 0,
    SVP_ACL_FORMAT_NHWC = 1,
    SVP_ACL_FORMAT_ND = 2,
    SVP_ACL_FORMAT_NC1HWC0 = 3,
    SVP_ACL_FORMAT_FRACTAL_Z = 4,
    SVP_ACL_FORMAT_NC1HWC0_C04 = 12,
    SVP_ACL_FORMAT_FRACTAL_NZ = 29,
} svp_acl_format;

/**
 * @ingroup svp_acl
 * @brief create data of svp_acl_data_buffer
 *
 * @param data [IN]    pointer to data
 * @li Need to be managed by the user,
 *  call svp_acl_rt_malloc interface to apply for memory,
 *  call svp_acl_rt_free interface to release memory
 * @param size [IN]      size of data in bytes
 * @param stride [IN]    stride of data in bytes
 * @retval pointer to created instance. nullptr if run out of memory
 *
 * @see svp_acl_rt_malloc | svp_acl_rt_free
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_data_buffer *svp_acl_create_data_buffer(void *data, size_t size, size_t stride);

/**
 * @ingroup svp_acl
 * @brief destroy data of svp_acl_data_buffer
 *
 * @par Function
 *  Only the svp_acl_data_buffer type data is destroyed here.
 *  The memory of the data passed in when the svp_acl_data_buffer interface
 *  is called to create svp_acl_data_buffer type data must be released by the user
 *
 * @param  data_buffer [IN]   pointer to the svp_acl_data_buffer
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see svp_acl_create_data_buffer
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_destroy_data_buffer(const svp_acl_data_buffer *data_buffer);

/**
 * @ingroup svp_acl
 * @brief get data address from svp_acl_data_buffer
 *
 * @param data_buffer [IN]    pointer to the data of svp_acl_data_buffer
 *
 * @retval data address
 */
SVP_ACL_FUNC_VISIBILITY void *svp_acl_get_data_buffer_addr(const svp_acl_data_buffer *data_buffer);

/**
 * @ingroup svp_acl
 * @brief get data size of svp_acl_data_buffer
 *
 * @param  data_buffer [IN]    pointer to the data of svp_acl_data_buffer
 *
 * @retval data size
 */
SVP_ACL_FUNC_VISIBILITY size_t svp_acl_get_data_buffer_size(const svp_acl_data_buffer *data_buffer);

/**
 * @ingroup svp_acl
 * @brief get stride of svp_acl_data_buffer
 *
 * @param  data_buffer [IN]    pointer to the data of svp_acl_data_buffer
 * @retval stride
 */
SVP_ACL_FUNC_VISIBILITY size_t svp_acl_get_data_buffer_stride(const svp_acl_data_buffer *data_buffer);

/**
 * @ingroup svp_acl
 * @brief update svp_acl_data_buffer
 *
 * @param  data_buffer [IN]    pointer to the data of svp_acl_data_buffer
 * @param  data [IN]    pointer to data
 * @li Need to be managed by the user,
 *  call svp_acl_rt_malloc interface to apply for memory,
 *  call svp_acl_rt_free interface to release memory
 * @param size [IN]      size of data in bytes
 * @param stride [IN]    stride of data in bytes
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_update_data_buffer(svp_acl_data_buffer *data_buffer, void *data,
    size_t size, size_t stride);

/**
 * @ingroup svp_acl
 * @brief get size of svp_acl_data_type
 *
 * @param  data_type [IN]    svp_acl_data_type data the size to get
 * @retval bit num of the svp_acl_data_type
 */
SVP_ACL_FUNC_VISIBILITY size_t svp_acl_data_type_size(svp_acl_data_type data_type);

#ifdef __cplusplus
}
#endif

#endif /* SVP_ACL_BASE_H */

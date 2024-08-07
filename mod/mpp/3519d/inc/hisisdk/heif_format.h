/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#ifndef HEIF_FORMAT_H
#define HEIF_FORMAT_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef void *heif_handle;

/* general error code */
typedef enum {
    /* Everything ok, no error occurred. */
    HEIF_NO_ERR = 0,
    HEIF_ERR_NULL_PTR,
    HEIF_ERR_INVALID_HANDLE,
    HEIF_ERR_INVALID_ARG,

    HEIF_ERR_OPEN_FILE,
    HEIF_ERR_CLOSE_FILE,
    HEIF_ERR_READ_FILE,
    HEIF_ERR_WRITE_FILE,
    HEIF_ERR_SEEK_FILE,

    HEIF_ERR_CREATE,
    HEIF_ERR_DESTROY,
    HEIF_ERR_CREATE_STREAM,
    HEIF_ERR_DESTROY_STREAM,

    HEIF_ERR_WRITE_ATOM,

    HEIF_ERR_NO_SPS_PPS,
    HEIF_ERR_NO_MEMORY,
    HEIF_ERR_NO_IPCO,
    HEIF_ERR_UNKNOWN = -1
} heif_error_code;

#define HEIF_MAX_MANUFACTURER_NAME_LEN 256 /* manufacturer name max length 255,one for '/0' */
#define HEIF_TRACK_NUM_MAX 6
#define HEIF_MAX_URL_PATH_LEN 512 /* max length of url path */

typedef enum {
    HEIF_CONFIG_MUXER = 1, /* muxer type */
    HEIF_CONFIG_DEMUXER,   /* demuxer type */
    HEIF_CONFIG_BUTT       /* invalid config type */
} heif_config_type;

typedef enum {
    HEIF_PROFILE_HEIC = 0, /* hevc image */
    HEIF_PROFILE_AVCI,     /* avc image */
    HEIF_PROFILE_HEVC,     /* hevc sequence */
    HEIF_PROFILE_AVCS,     /* avc sequence */
    HEIF_PROFILE_BUTT
} heif_format_profile;

typedef enum {
    HEIF_TRACK_TYPE_VIDEO = 1,
    HEIF_TRACK_TYPE_DATA,
    HEIF_TRACK_TYPE_THUMBNAL,
    HEIF_TRACK_TYPE_BUTT
} heif_track_type;

typedef enum {
    HEIF_CODEC_ID_AVC = 1,
    HEIF_CODEC_ID_HEVC,
    HEIF_CODEC_ID_AV1,
    HEIF_CODEC_ID_JPEG,
    HEIF_CODEC_ID_BUTT
} heif_codec_id;

typedef enum {
    HEIF_ROTATION_TYPE_0 = 0,
    HEIF_ROTATION_TYPE_90 = 90,
    HEIF_ROTATION_TYPE_180 = 180,
    HEIF_ROTATION_TYPE_270 = 270,
    HEIF_ROTATION_TYPE_BUTT = 0xFFFFFFFF,
} heif_rotation_type;

typedef enum {
    HEIF_MIRROR_TYPE_NONE = 0,
    HEIF_MIRROR_TYPE_H,  /* horizontal mirroring */
    HEIF_MIRROR_TYPE_V,  /* vertical mirroring */
} heif_mirror_type;

typedef enum {
    HEIF_IMAGE_TYPE_MASTER = 0,
    HEIF_IMAGE_TYPE_THUMBNAIL,
    HEIF_IMAGE_TYPE_DEPTH,
} heif_image_type;

typedef struct {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} heif_crop_info;

typedef struct {
    heif_image_type image_type;   /* iamge type */
    uint32_t width;               /* image width */
    uint32_t height;              /* image height */
    uint32_t row_image_num;       /* the number of rows of images, only valid for grid mode */
    uint32_t column_image_num;    /* the number of images per row, only valid for grid mode */
    heif_codec_id codec_id;       /* codec type */
    heif_mirror_type mirror_type; /* the type of mirroring operation, only support HEIF_MIRROR_TYPE_NONE now */
    heif_rotation_type rotation;  /* the angle of rotation, only support HEIF_ROTATION_TYPE_0 now */
    heif_crop_info crop;          /* the crop info for image, not support and ignored */
} heif_image_info;

typedef struct {
    heif_track_type type;               /* the type of track, only support HEIF_TRACK_TYPE_VIDEO currently */
    char manufacturer_name[HEIF_MAX_MANUFACTURER_NAME_LEN]; /* manufacturer name */
    heif_image_info image;              /* imageInfo info */
} heif_track_config;

typedef struct {
    int32_t item_id;   /* the index of image item */
    int64_t timestamp; /* frame timestamp, unit us */
    uint8_t *data;     /* frame data buffer */
    uint32_t length;   /* frame data len */
    bool key_frame;    /* key frame flag */
} heif_image_item;

typedef struct {
    bool is_grid;                       /* true:grid, false:normal. not support grid mode currently */
    uint32_t row_image_num;             /* the number of rows of images, only valid for grid mode */
    uint32_t column_image_num;          /* the number of images per row, only valid for grid mode */
    heif_format_profile format_profile; /* heic and avci */
} heif_muxer_config;

typedef enum {
    HEIF_FILE_TYPE_URL = 0,
    HEIF_FILE_TYPE_FD,
} heif_file_type;

typedef struct {
    heif_file_type file_type;         /* file type, uri or fd */
    union {
        char url[HEIF_MAX_URL_PATH_LEN]; /* file uri */
        int32_t fd;                   /* file descriptions id */
    } input;
} heif_file_desc;

typedef struct {
    heif_file_desc file_desc;
    heif_config_type config_type;   /* the type of config, muxer or demuxer */
    heif_muxer_config muxer_config; /* only used for HEIF_CONFIG_MUXER  */
} heif_config;

typedef enum {
    HEIF_EXTEND_TYPE_MIRROR = 0,
    HEIF_EXTEND_TYPE_CROP,
    HEIF_EXTEND_TYPE_ROTATION,
    HEIF_EXTEND_TYPE_THUMBNAIL,
    HEIF_EXTEND_TYPE_HEVC_EXT,
    HEIF_EXTEND_TYPE_AVC_EXT,
    HEIF_EXTEND_TYPE_BUTT,
} heif_extend_type;

typedef struct {
    uint32_t width;         /* image width */
    uint32_t height;        /* image height */
    heif_codec_id codec_id; /* codec type */
    uint8_t *data;          /* the address of image data */
    uint32_t len;           /* the length of image data */
} heif_thumbnail_info;

typedef struct {
    heif_extend_type type; /* the type of extend info */
    void *data;            /* the address of extend info */
    uint32_t len;          /* the length of extend info */
} heif_extend_info;

typedef struct {
    uint32_t track_id;                  /* the index of track */
    heif_track_type type;               /* the type of track */
    char manufacturer_name[HEIF_MAX_MANUFACTURER_NAME_LEN]; /* manufacturer name */
    uint32_t image_count;               /* the count of iamges */
    heif_image_info *image;             /* image meta info, it may be the infomation of master image and thumbnail */
} heif_track_info;

typedef struct {
    int64_t duration;           /* unit us */
    heif_format_profile profile;
    uint32_t track_count;       /* the count of trak in file, it should be 1, if single picture scence */
    heif_track_info *track;     /* track info */
} heif_file_info;

/**
 * @brief create heif instance.
 * @param[in/out] handle, Indicates the handle of heif
 * @param[in] config, Indicates the config informaation that needed by creating operation
 * @return Returns <b> 0 <b> if success
 *  Returns others if failure.
 */
int32_t heif_create(heif_handle *handle, const heif_config *config);

/**
 * @brief destroy heif instance.
 * @param[in] handle, Indicates the handle of heif
 * @return Returns <b> 0 <b> if success
 *  Returns others if failure.
 */
int32_t heif_destroy(heif_handle handle);

/**
 * @brief create heif track. only used for video sequence scence.
 * @param[in] handle, Indicates the handle of heif
 * @param[in] config, Indicates the track information of a new stream
 * @param[out] track_id, Indicates the index of track
 * @return Returns <b> 0 <b> if success
 *  Returns others if failure.
 */
int32_t heif_create_track(heif_handle handle, const heif_track_config *config, uint32_t *track_id);

/**
 * @brief set extend info for heif; such as mirror, rotation, crop, thumnail.
 * @param[in] handle, Indicates the handle of heif
 * @param[in] track_id, Indicates the index of track. it should be get after  call  heif_create_track.
 * @param[in] info, Indicates the extend info that will be setted to heif
 * @return Returns <b> 0 <b> if success
 *  Returns others if failure.
 */
int32_t heif_set_extend_info(heif_handle handle, uint32_t track_id, const heif_extend_info *info);

/**
 * @brief destroy all track.
 * @param[in] handle, Indicates the handle of heif
 * @return Returns <b> 0 <b> if success
 *  Returns others if failure.
 */
int32_t heif_destroy_all_tracks(heif_handle handle);

/**
 * @brief write image data
 * @param[in] handle, Indicates the handle of heif
 * @param[in] track_id, Indicates the index of track. it should be get after calling heif_create_track for the scence
 * of video sequeues, using <b>-1<b> if single picture scence
 * @param[in] item, Indicates the array of iamge items info;
 * @param[in] item_count, Indicates the counts of image items
 * @return   0 success
 * @return  err num  failure
 */
int32_t heif_write_master_image(heif_handle handle, uint32_t track_id, const heif_image_item *item,
    uint32_t item_count);

/**
 * @brief get file information.
 * @param[in] handle, Indicates the handle of heif
 * @param[in/out] file_info, Indicates the information the input file.
 * @return Returns <b> 0 <b> if success
 *  Returns others if failure.
 */
int32_t heif_get_file_info(heif_handle handle, heif_file_info *file_info);

/**
 * @brief get extend info for heif; such as mirror, rotation, crop, thumnail.
 * @param[in] handle, Indicates the handle of heif
 * @param[in] track_id, Indicates the track index that getted from calling heif_get_file_info
 * @param[in] image_index, Indicates the image index that getted from calling heif_get_file_info
 * @param[in/out] info, Indicates the extend info that will be setted to heif
 * @return Returns <b> 0 <b> if success
 *  Returns others if failure.
 */
int32_t heif_get_extend_info(heif_handle handle,  uint32_t track_id,  uint32_t image_index, heif_extend_info *info);

/**
 * @brief get master iamage.
 * @param[in] handle, Indicates the handle of heif
 * @param[in] track_id, Indicates the track index that getted from calling heif_get_file_info
 * @param[in/out] item, Indicates the array of iamge items info;
 * @param[in/out] item_count uint32_t* :the array count of item, and output the actually used count. and itemCnt should
 * not small than the num calcurate by file info
 * @return Returns <b> 0 <b> if success
 *  Returns others if failure.
 */
int32_t heif_get_master_image(heif_handle handle, uint32_t track_id, heif_image_item *item, uint32_t *item_count);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif

/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#include "sample_common_ive.h"

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

#include "hi_common.h"
#include "hi_common_video.h"
#include "hi_common_sys.h"
#include "hi_common_vgs.h"
#include "hi_common_vi.h"
#include "hi_common_vo.h"
#include "hi_mpi_sys.h"

#define HI_SAMPLE_IVE_QUERY_SLEEP   100
#define HI_SAMPLE_IVE_MAX_WIDTH     4096
#define HI_SAMPLE_IVE_MAX_HEIGHT    4096

static hi_bool g_is_mpi_init = HI_FALSE;

/*
 * function :mpi_init
 */
static hi_s32 sample_common_ive_mpi_init(hi_void)
{
    hi_s32 ret;

    hi_mpi_sys_exit();

    ret = hi_mpi_sys_init();
    if (ret != HI_SUCCESS) {
        sample_svp_trace_err("hi_mpi_sys_init fail,Error(%#x)\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

/*
 * function : Mpi check init
 */
hi_s32 sample_common_ive_check_mpi_init(hi_void)
{
    if (g_is_mpi_init == HI_FALSE) {
        if (sample_common_ive_mpi_init() != HI_SUCCESS) {
            sample_svp_trace_err("Ive mpi init failed!\n");
            return HI_FALSE;
        }
        g_is_mpi_init = HI_TRUE;
    }
    return HI_TRUE;
}

/*
 * function : Mpi exit
 */
hi_void sample_common_ive_mpi_exit(hi_void)
{
    if (g_is_mpi_init == HI_TRUE) {
        if (hi_mpi_sys_exit() != HI_SUCCESS) {
            sample_svp_trace_err("Sys exit failed!\n");
            return;
        }
    }
    g_is_mpi_init = HI_FALSE;
    sample_svp_trace_info("ive mpi exit ok!\n");
}

static hi_void sample_comm_ive_get_loop_info(const hi_svp_img *img, hi_sample_rw_image_loop_info *loop_info)
{
    loop_info->ele_size = 1;
    loop_info->loop_c = 1;
    loop_info->loop_h[0] = img->height;
    switch (img->type) {
        case HI_SVP_IMG_TYPE_U8C1:
        case HI_SVP_IMG_TYPE_S8C1:
            break;
        case HI_SVP_IMG_TYPE_YUV420SP:
            loop_info->ele_size = 1;
            loop_info->loop_c = HI_SAMPLE_IVE_IMAGE_CHN_TWO;
            loop_info->loop_h[1] = img->height / HI_SAMPLE_IVE_DIV_TWO;
            break;
        case HI_SVP_IMG_TYPE_YUV422SP:
            loop_info->loop_c = HI_SAMPLE_IVE_IMAGE_CHN_TWO;
            loop_info->loop_h[1] = img->height;
            break;
        case HI_SVP_IMG_TYPE_U8C3_PACKAGE:
            loop_info->ele_size = (hi_u32)(sizeof(hi_u8) + sizeof(hi_u16));
            break;
        case HI_SVP_IMG_TYPE_U8C3_PLANAR:
            loop_info->loop_c = HI_SAMPLE_IVE_IMAGE_CHN_THREE;
            loop_info->loop_h[1] = img->height;
            loop_info->loop_h[HI_SAMPLE_IVE_IMAGE_CHN_TWO] = img->height;
            break;
        case HI_SVP_IMG_TYPE_S16C1:
        case HI_SVP_IMG_TYPE_U16C1:
            loop_info->ele_size = (hi_u32)sizeof(hi_u16);
            break;
        case HI_SVP_IMG_TYPE_U32C1:
        case HI_SVP_IMG_TYPE_S32C1:
            loop_info->ele_size = (hi_u32)sizeof(hi_u32);
            break;
        case HI_SVP_IMG_TYPE_S64C1:
        case HI_SVP_IMG_TYPE_U64C1:
            loop_info->ele_size = (hi_u32)sizeof(hi_u64);
            break;
        default:
            break;
    }
}

/*
 * function :Read file
 */
hi_s32 sample_common_ive_read_file(hi_svp_img *img, FILE *fp)
{
    hi_u8 *ptr_tmp = HI_NULL;
    hi_u16 c, h;
    hi_s32 ret = HI_ERR_IVE_NULL_PTR;
    hi_sample_rw_image_loop_info loop_info = {0};

    sample_svp_check_exps_return(img == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "img can't be null\n");
    sample_svp_check_exps_return(fp == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "fp can't be null\n");

    ret = fgetc(fp);
    sample_svp_check_exps_return(ret == EOF, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "fgetc fp failed!\n");
    if (feof(fp)) {
        sample_svp_trace_err("end of file!\n");
        ret = fseek(fp, 0, SEEK_SET);
        if (ret != 0) {
            sample_svp_trace_err("fseek failed!\n");
            return ret;
        }
    } else {
        ret = fseek(fp, -1, SEEK_CUR);
        if (ret != 0) {
            sample_svp_trace_err("fseek failed!\n");
            return ret;
        }
    }
    sample_comm_ive_get_loop_info(img, &loop_info);
    for (c = 0; (c < loop_info.loop_c) && (c < HI_SVP_IMG_STRIDE_NUM) && (c < HI_SVP_IMG_ADDR_NUM); c++) {
        ptr_tmp = sample_svp_convert_addr_to_ptr(hi_u8, img->virt_addr[c]);
        sample_svp_check_exps_return(ptr_tmp == 0, HI_ERR_IVE_ILLEGAL_PARAM,
            SAMPLE_SVP_ERR_LEVEL_ERROR, "ptr_tmp can't be 0\n");
        for (h = 0; h < loop_info.loop_h[c]; h++) {
            if (fread(ptr_tmp, img->width * loop_info.ele_size, 1, fp) != 1) {
                sample_svp_trace_err("Read file fail\n");
                return HI_ERR_IVE_ILLEGAL_PARAM;
            }
            ptr_tmp += img->stride[c] * loop_info.ele_size;
        }
    }

    return HI_SUCCESS;
}

/*
 * function :Write file
 */
hi_s32 sample_common_ive_write_file(hi_svp_img *img, FILE *fp)
{
    hi_u16 c, h;
    hi_u8 *ptr_tmp = HI_NULL;
    hi_s32 ret = HI_ERR_IVE_NULL_PTR;
    hi_sample_rw_image_loop_info loop_info = {0};

    sample_svp_check_exps_return(img == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "img can't be null\n");
    sample_svp_check_exps_return(fp == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "fp can't be null\n");
    ret = HI_ERR_IVE_ILLEGAL_PARAM;
    sample_svp_check_exps_return(img->phys_addr == 0, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "img->phys_addr can't be 0\n");
    sample_svp_check_exps_return(img->virt_addr == 0, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "img->phys_addr can't be 0\n");

    sample_comm_ive_get_loop_info(img, &loop_info);
    for (c = 0; (c < loop_info.loop_c) && (c < HI_SVP_IMG_STRIDE_NUM) && (c < HI_SVP_IMG_ADDR_NUM); c++) {
        ptr_tmp = sample_svp_convert_addr_to_ptr(hi_u8, img->virt_addr[c]);
        sample_svp_check_exps_return(ptr_tmp == 0, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "ptr_tmp can't be 0\n");
        for (h = 0; h < loop_info.loop_h[c]; h++) {
            if (fwrite(ptr_tmp, img->width * loop_info.ele_size, 1, fp) != 1) {
                sample_svp_trace_err("Write file fail\n");
                return ret;
            }
            ptr_tmp += img->stride[c] * loop_info.ele_size;
        }
    }
    return HI_SUCCESS;
}

/*
 * function :Calc stride
 */
hi_u32 sample_common_ive_calc_stride(hi_u32 width, hi_u8 align)
{
    sample_svp_check_exps_return(align == 0, HI_ERR_IVE_ILLEGAL_PARAM,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "align can't be 0\n");
    sample_svp_check_exps_return((width > HI_SAMPLE_IVE_MAX_WIDTH) || (width < 1), HI_ERR_IVE_ILLEGAL_PARAM,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "width(%u) must be in [1, %u]", width, HI_SAMPLE_IVE_MAX_WIDTH);
    return (width + (align - width % align) % align);
}

static hi_void sample_comm_ive_get_thresh(hi_ive_ccblob *blob, hi_u16 area_thr_step, hi_u16 rect_max_num,
    hi_u32 *thresh)
{
    hi_u32 i;
    hi_u16 num;
    hi_u32 thr = blob->info.bits.cur_area_threshold;
    do {
        num = 0;
        thr += area_thr_step;
        for (i = 0; i < blob->info.bits.rgn_num; i++) {
            if (blob->rgn[i].area > thr) {
                num++;
            }
        }
    } while (num > rect_max_num);

    *thresh = thr;
}

/*
 * function : judge if rect is valid
 */
static hi_void sample_common_ive_is_rect_valid(hi_sample_svp_rect_info *rect, hi_u32 num, hi_bool *valid)
{
    hi_u32 j, k;
    for (j = 0; j < (HI_POINT_NUM - 1); j++) {
        for (k = j + 1; k < HI_POINT_NUM; k++) {
            if ((rect->rect[num].point[j].x == rect->rect[num].point[k].x) &&
                (rect->rect[num].point[j].y == rect->rect[num].point[k].y)) {
                *valid = HI_FALSE;
                break;
            }
        }
    }
}

static hi_s32 sample_common_ive_check_src_dst(hi_sample_src_dst_size *src_dst_size)
{
    sample_svp_check_exps_return((hi_float)src_dst_size->src.width == 0, HI_ERR_IVE_ILLEGAL_PARAM,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "src width can't be 0\n");
    sample_svp_check_exps_return((hi_float)src_dst_size->src.height == 0, HI_ERR_IVE_ILLEGAL_PARAM,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "src height can't be 0\n");
    sample_svp_check_exps_return((hi_float)src_dst_size->dst.width == 0, HI_ERR_IVE_ILLEGAL_PARAM,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "dst width can't be 0\n");
    sample_svp_check_exps_return((hi_float)src_dst_size->dst.height == 0, HI_ERR_IVE_ILLEGAL_PARAM,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "dst height can't be 0\n");
    return HI_SUCCESS;
}
/*
 * function : Copy blob to rect
 */
hi_s32 sample_common_ive_blob_to_rect(hi_ive_ccblob *blob, hi_sample_svp_rect_info *rect,
    hi_u16 rect_max_num, hi_u16 area_thr_step, hi_sample_src_dst_size *src_dst_size)
{
    hi_u16 num, i;
    hi_s32 ret;
    hi_u32 thr = 0;
    hi_bool valid;
    sample_svp_check_exps_return(blob == HI_NULL, HI_ERR_IVE_NULL_PTR,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "blob can't be null\n");
    sample_svp_check_exps_return(rect == HI_NULL, HI_ERR_IVE_NULL_PTR,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "rect can't be null\n");
    ret = sample_common_ive_check_src_dst(src_dst_size);
    sample_svp_check_failed_return(ret, HI_ERR_IVE_ILLEGAL_PARAM, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "sample_common_ive_check_src_dst failed\n");
    if (blob->info.bits.rgn_num > rect_max_num) {
        sample_comm_ive_get_thresh(blob, area_thr_step, rect_max_num, &thr);
    }

    num = 0;
    for (i = 0; i < blob->info.bits.rgn_num; i++) {
        if (blob->rgn[i].area <= thr) {
            continue;
        }
        sample_svp_check_exps_return(num > (HI_SVP_RECT_NUM - 1), HI_FAILURE,
            SAMPLE_SVP_ERR_LEVEL_ERROR, "num is larger than %u\n", HI_SVP_RECT_NUM - 1);
        rect->rect[num].point[HI_SAMPLE_POINT_IDX_ZERO].x = (hi_u32)((hi_float)blob->rgn[i].left /
            (hi_float)src_dst_size->src.width * (hi_float)src_dst_size->dst.width) & (~1);
        rect->rect[num].point[HI_SAMPLE_POINT_IDX_ZERO].y = (hi_u32)((hi_float)blob->rgn[i].top /
            (hi_float)src_dst_size->src.height * (hi_float)src_dst_size->dst.height) & (~1);

        rect->rect[num].point[HI_SAMPLE_POINT_IDX_ONE].x = (hi_u32)((hi_float)blob->rgn[i].right /
            (hi_float)src_dst_size->src.width * (hi_float)src_dst_size->dst.width) & (~1);
        rect->rect[num].point[HI_SAMPLE_POINT_IDX_ONE].y = (hi_u32)((hi_float)blob->rgn[i].top /
            (hi_float)src_dst_size->src.height * (hi_float)src_dst_size->dst.height) & (~1);

        rect->rect[num].point[HI_SAMPLE_POINT_IDX_TWO].x = (hi_u32)((hi_float)blob->rgn[i].right /
            (hi_float)src_dst_size->src.width * (hi_float)src_dst_size->dst.width) & (~1);
        rect->rect[num].point[HI_SAMPLE_POINT_IDX_TWO].y = (hi_u32)((hi_float)blob->rgn[i].bottom /
            (hi_float)src_dst_size->src.height * (hi_float)src_dst_size->dst.height) & (~1);

        rect->rect[num].point[HI_SAMPLE_POINT_IDX_THREE].x = (hi_u32)((hi_float)blob->rgn[i].left /
            (hi_float)src_dst_size->src.width * (hi_float)src_dst_size->dst.width) & (~1);
        rect->rect[num].point[HI_SAMPLE_POINT_IDX_THREE].y = (hi_u32)((hi_float)blob->rgn[i].bottom /
            (hi_float)src_dst_size->src.height * (hi_float)src_dst_size->dst.height) & (~1);

        valid = HI_TRUE;
        sample_common_ive_is_rect_valid(rect, num, &valid);
        if (valid == HI_TRUE) {
            num++;
        }
    }

    rect->num = num;
    return HI_SUCCESS;
}


static hi_s32 sample_comm_ive_set_image_addr(hi_svp_img *img, const hi_sample_rw_image_loop_info *loop_info,
    hi_bool is_mmz_cached)
{
    hi_u32 c;
    hi_u32 size = 0;
    hi_s32 ret;
    hi_void *virt_addr = HI_NULL;
    for (c = 0; (c < loop_info->loop_c) && (c < HI_MAX_LOOP_IMG_H) && (c < HI_SVP_IMG_STRIDE_NUM); c++) {
        size += img->stride[0] * loop_info->loop_h[c] * loop_info->ele_size;
        img->stride[c] = img->stride[0];
    }

    if (is_mmz_cached == HI_FALSE) {
        ret = hi_mpi_sys_mmz_alloc((hi_phys_addr_t *)&img->phys_addr[0], (hi_void **)&virt_addr,
            HI_NULL, HI_NULL, size);
    } else {
        ret = hi_mpi_sys_mmz_alloc_cached((hi_phys_addr_t *)&img->phys_addr[0], (hi_void **)&virt_addr,
            HI_NULL, HI_NULL, size);
    }
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "mmz malloc fail\n");
    img->virt_addr[HI_SAMPLE_ADDR_IDX_ZERO] = sample_svp_convert_ptr_to_addr(hi_u64, virt_addr);

    if (img->type != HI_SVP_IMG_TYPE_U8C3_PACKAGE) {
        for (c = 1; (c < loop_info->loop_c) && (c < HI_MAX_LOOP_IMG_H) && (c < HI_SVP_IMG_STRIDE_NUM); c++) {
            img->phys_addr[c] = img->phys_addr[c - 1] + img->stride[c - 1] * img->height;
            img->virt_addr[c] = img->virt_addr[c - 1] + img->stride[c - 1] * img->height;
        }
    } else {
        img->virt_addr[HI_SAMPLE_ADDR_IDX_ONE] = img->virt_addr[HI_SAMPLE_ADDR_IDX_ZERO] + 1;
        img->virt_addr[HI_SAMPLE_ADDR_IDX_TWO] = img->virt_addr[HI_SAMPLE_ADDR_IDX_ONE] + 1;
        img->phys_addr[HI_SAMPLE_ADDR_IDX_ONE] = img->phys_addr[HI_SAMPLE_ADDR_IDX_ZERO] + 1;
        img->phys_addr[HI_SAMPLE_ADDR_IDX_TWO] = img->phys_addr[HI_SAMPLE_ADDR_IDX_ONE] + 1;
    }
    return HI_SUCCESS;
}

/*
 * function : Create ive image
 */
static hi_s32 sample_common_ive_create_image_flag(hi_svp_img *img, hi_svp_img_type type,
    hi_u32 width, hi_u32 height, hi_bool is_mmz_cached)
{
    hi_s32 ret = HI_ERR_IVE_ILLEGAL_PARAM;
    hi_sample_rw_image_loop_info loop_info = {0};
    sample_svp_check_exps_return(img == HI_NULL, HI_ERR_IVE_NULL_PTR,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "img can't be null\n");
    sample_svp_check_exps_return((type < 0) || (type >= HI_SVP_IMG_TYPE_BUTT), ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "type(%u) must be in [0, %u)!\n", type, HI_SVP_IMG_TYPE_BUTT);
    sample_svp_check_exps_return(width > HI_SAMPLE_IVE_MAX_WIDTH, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "width(%u) must be in [1, %u]!\n", width, HI_SAMPLE_IVE_MAX_WIDTH);
    sample_svp_check_exps_return(height > HI_SAMPLE_IVE_MAX_HEIGHT, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "height(%u) must be in [1, %u]!\n", height, HI_SAMPLE_IVE_MAX_HEIGHT);

    img->type = type;
    img->width = width;
    img->height = height;
    img->stride[0] = sample_common_ive_calc_stride(img->width, HI_IVE_ALIGN);

    switch (type) {
        case HI_SVP_IMG_TYPE_U8C1:
        case HI_SVP_IMG_TYPE_S8C1:
        case HI_SVP_IMG_TYPE_YUV420SP:
        case HI_SVP_IMG_TYPE_YUV422SP:
        case HI_SVP_IMG_TYPE_S16C1:
        case HI_SVP_IMG_TYPE_U16C1:
        case HI_SVP_IMG_TYPE_U8C3_PACKAGE:
        case HI_SVP_IMG_TYPE_S32C1:
        case HI_SVP_IMG_TYPE_U32C1:
        case HI_SVP_IMG_TYPE_S64C1:
        case HI_SVP_IMG_TYPE_U64C1: {
            sample_comm_ive_get_loop_info(img, &loop_info);
            ret = sample_comm_ive_set_image_addr(img, &loop_info, is_mmz_cached);
            sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
                "Err:%#x,sample_comm_ive_set_image_addr failed\n", ret);
            break;
            }
        case HI_SVP_IMG_TYPE_YUV420P:
            break;
        case HI_SVP_IMG_TYPE_YUV422P:
            break;
        case HI_SVP_IMG_TYPE_S8C2_PACKAGE:
            break;
        case HI_SVP_IMG_TYPE_S8C2_PLANAR:
            break;
        case HI_SVP_IMG_TYPE_U8C3_PLANAR:
            break;
        default:
            break;
    }

    return HI_SUCCESS;
}

hi_s32 sample_common_ive_create_image(hi_svp_img *img, hi_svp_img_type type,
    hi_u32 width, hi_u32 height)
{
    return sample_common_ive_create_image_flag(img, type, width, height, HI_FALSE);
}

/*
 * function : Create memory info
 */
hi_s32 sample_common_ive_create_mem_info(hi_svp_mem_info *mem_info, hi_u32 size)
{
    hi_s32 ret = HI_ERR_IVE_NULL_PTR;
    hi_void *virt_addr = HI_NULL;

    sample_svp_check_exps_return(mem_info == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "mem_info can't be null\n");

    mem_info->size = size;
    ret = hi_mpi_sys_mmz_alloc((hi_phys_addr_t *)&mem_info->phys_addr, (hi_void **)&virt_addr, HI_NULL, HI_NULL, size);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Mmz Alloc fail,Error(%#x)\n", ret);
    mem_info->virt_addr = sample_svp_convert_ptr_to_addr(hi_u64, virt_addr);

    return HI_SUCCESS;
}
/*
 * function : Create ive image by cached
 */
hi_s32 sample_common_ive_create_image_by_cached(hi_svp_img *img, hi_svp_img_type type,
    hi_u32 width, hi_u32 height)
{
    return sample_common_ive_create_image_flag(img, type, width, height, HI_TRUE);
}

/*
 * function : Dma frame info to ive image
 */
hi_s32 sample_common_ive_dma_image(hi_video_frame_info *frame_info, hi_svp_dst_img *dst,
    hi_bool is_instant)
{
    hi_s32 ret = HI_ERR_IVE_NULL_PTR;
    hi_ive_handle handle;
    hi_svp_src_data src_data;
    hi_svp_dst_data dst_data;
    hi_ive_dma_ctrl ctrl = { HI_IVE_DMA_MODE_DIRECT_COPY, 0, 0, 0, 0 };
    hi_bool is_finish = HI_FALSE;
    hi_bool is_block = HI_TRUE;

    sample_svp_check_exps_return(frame_info == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "frame_info can't be null\n");
    sample_svp_check_exps_return(dst == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "dst can't be null\n");
    sample_svp_check_exps_return(frame_info->video_frame.virt_addr == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "frame_info->video_frame.virt_addr can't be null\n");
    ret = HI_ERR_IVE_ILLEGAL_PARAM;
    sample_svp_check_exps_return(frame_info->video_frame.phys_addr == 0, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "frame_info->video_frame.virt_addr can't be 0\n");
    sample_svp_check_exps_return(dst->virt_addr == 0, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "dst->virt_addr can't be 0\n");
    sample_svp_check_exps_return(dst->phys_addr == 0, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "dst->phys_addr can't be 0\n");

    /* fill src */
    src_data.virt_addr = sample_svp_convert_ptr_to_addr(hi_u64, frame_info->video_frame.virt_addr[0]);
    src_data.phys_addr = frame_info->video_frame.phys_addr[0];
    src_data.width = frame_info->video_frame.width;
    src_data.height = frame_info->video_frame.height;
    src_data.stride = frame_info->video_frame.stride[0];

    /* fill dst */
    dst_data.virt_addr = dst->virt_addr[0];
    dst_data.phys_addr = dst->phys_addr[0];
    dst_data.width = dst->width;
    dst_data.height = dst->height;
    dst_data.stride = dst->stride[0];

    ret = hi_mpi_ive_dma(&handle, &src_data, &dst_data, &ctrl, is_instant);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),hi_mpi_ive_dma failed!\n", ret);

    if (is_instant == HI_TRUE) {
        ret = hi_mpi_ive_query(handle, &is_finish, is_block);
        while (ret == HI_ERR_IVE_QUERY_TIMEOUT) {
            usleep(HI_SAMPLE_IVE_QUERY_SLEEP);
            ret = hi_mpi_ive_query(handle, &is_finish, is_block);
        }
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_ive_query failed!\n", ret);
    }

    return HI_SUCCESS;
}

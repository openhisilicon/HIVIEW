/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "scene_setparam_inner.h"
#include <unistd.h>
#include <string.h>
#include "securec.h"
#include "hi_mpi_ae.h"
#include "hi_mpi_venc.h"
#include "hi_common_rc.h"
#include "hi_mpi_vpss.h"
#include "ot_scenecomm.h"
#include "hi_mpi_isp.h"

#ifdef __cplusplus
extern "C" {
#endif


hi_bool g_isp_pause = HI_FALSE;
ot_scene_pipe_param g_scene_pipe_param[HI_SCENE_PIPETYPE_NUM];

ot_scene_pipe_param *get_pipe_params(hi_void)
{
    return g_scene_pipe_param;
}

hi_bool *get_isp_pause(hi_void)
{
    return &g_isp_pause;
}

hi_u32 get_level_ltoh(hi_u32 value, hi_u32 level, hi_u32 count, const hi_u32 *thresh)
{
    for (level = 0; level < count; level++) {
        if (value <= thresh[level]) {
            break;
        }
    }

    level = (level == count) ? (count - 1) : level;
    return level;
}

hi_u32 scene_get_level_ltoh(hi_u64 value, hi_u32 count, const hi_u64 *thresh)
{
    hi_u32 level;

    for (level = 0; level < count; level++) {
        if (value <= thresh[level]) {
            break;
        }
    }

    if (level == count) {
        level = count - 1;
    }
    return level;
}

hi_u32 scene_get_level_ltoh_u32(hi_u32 value, hi_u32 count, const hi_u32 *thresh)
{
    hi_u32 level;

    for (level = 0; level < count; level++) {
        if (value <= thresh[level]) {
            break;
        }
    }

    if (level == count) {
        level = count - 1;
    }
    return level;
}

hi_u32 scene_interpulate(hi_u64 middle, hi_u64 left, hi_u64 left_value, hi_u64 right, hi_u64 right_value)
{
    hi_u32 value_tmp, k;

    if (middle <= left) {
        value_tmp = (hi_u32)left_value;
        return value_tmp;
    }
    if (middle >= right) {
        value_tmp = (hi_u32)right_value;
        return value_tmp;
    }

    k = (hi_u32)(right - left);
    value_tmp = (hi_u32)(((right - middle) * left_value + (middle - left) * right_value + (k >> 1)) / k);
    return value_tmp;
}

hi_u32 scene_time_filter(hi_u32 param0, hi_u32 param1, hi_u32 time_cnt, hi_u32 index)
{
    hi_u64 temp;
    hi_u32 value;

    if (param0 > param1) {
        temp = (hi_u64)(param0 - param1) << 8; /* 8 is param to time_cnt */
        temp = ((temp * (index + 1)) / scene_div_0to1(time_cnt)) >> 8; /* 8 is time_cnt to param */
        value = (param0 > ((hi_u32)temp + 1)) ? (param0 - (hi_u32)temp - 1) : 0;
    } else if (param0 < param1) {
        temp = (hi_u64)(param1 - param0) << 8; /* 8 is param to time_cnt */
        temp = ((temp * (index + 1)) / scene_div_0to1(time_cnt)) >> 8; /* 8 is time_cnt to param */
        value = (param0 + (hi_u32)temp + 1) > param1 ? param1 : (param0 + (hi_u32)temp + 1);
    } else {
        value = param0;
    }
    return value;
}

static void print_para_n(const hi_vpss_nrx_v2 *p_x, const hi_vpss_nrx_v2_iey *pi, const hi_vpss_nrx_v2_sfy *ps,
    const hi_vpss_nrx_v2_tfy *pt)
{
    printf("\n\n                             sizeof( X ) = %ld ", sizeof(*p_x));
    printf("\n**************N0************|***********N1*******|*******N2**********|**************N3*******************"
        "****");
    printf("\n -en                 %3d    |               %3d  |              %3d  |               %3d                 "
        "    ", p_x->nry0_en, p_x->nry1_en, p_x->nry2_en, p_x->nry3_en);
    printf("\n -nXsf1      %3d:    %3d    |       %3d:    %3d  |      %3d:    %3d  |            %3d:    %3d            "
        "    ", _tmprt2_4(ps, sfs1, sbr1));
    printf("\n -nXsf2      %3d:%3d:%3d    |       %3d:%3d:%3d  |      %3d:%3d:%3d  |            %3d:%3d:%3d            "
        "    ", _tmprt3_(ps, sfs2, sft2, sbr2));
    printf("\n -nXsf4      %3d:%3d:%3d    |       %3d:%3d:%3d  |      %3d:%3d:%3d  |            %3d:%3d:%3d            "
        "    ", _tmprt3_(ps, sfs4, sft4, sbr4));
    printf("\n -sfs5               %3d    |               %3d  |              %3d  |                    %3d            "
        "    ", _tmprt1_4(ps, sf5_md));
    printf("\n                            |                    |                   |  -sfs6     %3d:%3d:%3d            "
        "    ", ps[4].sfs4, ps[4].sft4, ps[4].sbr4); /* 4 is index */
    printf("\n -nXsf6  %3d:%3d:%3d:%3d    |   %3d:%3d:%3d:%3d  |  %3d:%3d:%3d:%3d  |  %3d:%3d:%3d:%3d  |  "
        "%3d:%3d:%3d:%3d  ", _tmprt4_5(pi, ies0, ies1, ies2, ies3));
    printf("\n -nXsf7  %3d:%3d:%3d:%3d    |   %3d:%3d:%3d:%3d  |  %3d:%3d:%3d:%3d  |  %3d:%3d:%3d:%3d  |  "
        "%3d:%3d:%3d:%3d  ", _tmprt4_5(ps, spn, sbn, pbr, j_mode)); /* pc->sf5_gama_u */
    printf("\n -nXsfr7 %3d:%3d:%3d:%3d    |   %3d:%3d:%3d:%3d  |  %3d:%3d:%3d:%3d  |  %3d:%3d:%3d:%3d  |  "
        "%3d:%3d:%3d:%3d  ", _tmprt4_5(ps, sfr7[0], sfr7[1], sfr7[2], sfr7[3])); /* pc->sf5_gama_v ,2, 3 is index */
    printf("\n -nXsbr7         %3d:%3d    |           %3d:%3d  |          %3d:%3d  |          %3d:%3d  |          "
        "%3d:%3d  ", _tmprt2_5(ps, sbr7[0], sbr7[1]));
    printf("\n -nXsf8                     |                    |                   |  %3d:%3d:%3d:%3d  |  "
        "%3d:%3d:%3d:%3d  ", /* 3, 4 is index */
        ps[3].sf8_idx0, ps[3].sf8_idx1, ps[3].sf8_thrd, ps[3].sf8_tfr, ps[4].sf8_idx0, ps[4].sf8_idx1, ps[4].sf8_thrd,
        ps[4].sf8_tfr); /*  4 is index pc->SPN1_9, pc->SBN1_9, pc->PBR1_9 */
    printf("\n -nXsfn  %3d:%3d:%3d:%3d    |   %3d:%3d:%3d:%3d  |  %3d:%3d:%3d:%3d  |  %3d:%3d:%3d:%3d  |  "
        "%3d:%3d:%3d:%3d  ", _tmprt4_5(ps, sfn0_0, sfn1_0, sfn2_0, sfn3_0));
    printf("\n -nXsth      %3d:%3d:%3d    |       %3d:%3d:%3d  |      %3d:%3d:%3d  |      %3d:%3d:%3d  |      "
        "%3d:%3d:%3d  ",
        _tmprt3_5(ps, sth1_0, sth2_0, sth3_0)); /* pHue[0x04], pHue[0x05], pHue[0x06], pHue[0x07]); pc->sfr1_9[0], */
                                                /* pc->sfr1_9[1], pc->sfr1_9[2], pc->sfr1_9[3], */
    printf("\n -sfr     (0)    %3d        |        %3d         |        %3d        |        %3d        |        %3d    "
        "    ", _tmprt1_5(ps, sfr)); /* pHue[0x08], pHue[0x09], pHue[0x0A], pHue[0x0B] */
    printf("\n -nX2sfn %3d:%3d:%3d:%3d    |   %3d:%3d:%3d:%3d  |  %3d:%3d:%3d:%3d  |                                   "
        "    ", _tmprt4_3(ps, sfn0_1, sfn1_1, sfn2_1, sfn3_1));
    printf("\n -nX2sth     %3d:%3d:%3d    |       %3d:%3d:%3d  |      %3d:%3d:%3d  |                                   "
        "    ", _tmprt3y(ps, sth1_1, sth2_1, sth3_1));
    printf("\n -nXbIdx                    |                    |      %3d:%3d      |                %3d:%3d            "
        "      ", _tmprt2z(ps, 2, 3, bri_idx0, bri_idx1)); /* 2, 3 is index */
}

static void print_nc0(const hi_vpss_nrx_v2 *p_x, const hi_vpss_nrx_v2_tfy *pt, const hi_vpss_nrx_v2_nrc1 *pc,
    const hi_vpss_nrx_v2_mdy *pm)
{
    printf("\n -ref            %3d        |        %3d         |                   "
        "|********************nr_c0****************", pt[0].ref_en, pt[1].ref_en);
    printf("\n                            |                    |                   |  -nC0en             %3d           "
        "    ", p_x->nrc0_en);
    printf("\n -nXstr   (1)    %3d        |        %3d:%3d     |        %3d        |  -nCmath            %3d           "
        "    ", pt[0].str0, pt[1].str0, pt[1].str1, pt[2].str0, p_x->nrc0.auto_math); /* 2 is index */
    printf("\n -nXsdz   (1)    %3d        |        %3d:%3d     |        %3d        |  -sfc               %3d           "
        "    ", pt[0].sdz0, pt[1].sdz0, pt[1].sdz1, pt[2].sdz0, p_x->nrc0.sfc); /* 2 is index */
    printf("\n -nXtss          %3d        |        %3d:%3d     |        %3d        |  -sfcExt          %3d:%3d         "
        "    ", pt[0].tss0, pt[1].tss0, pt[1].tss1, pt[2].tss0, p_x->nrc0.sfc_enhance, p_x->nrc0.sfc_ext); /* 2 index */
    printf("\n                            |                    |                   |  -tfc               %3d           "
        "    ", p_x->nrc0.tfc); /* -C1Hue4 %3d:%3d:%3d:%3d */
    printf("\n -nXtfs   (3)    %3d        |        %3d:%3d     |        %3d        |  -trc               %3d           "
        "      ", pt[0].tfs0, pt[1].tfs0, pt[1].tfs1, pt[2].tfs0, p_x->nrc0.trc); /* -C1Hue8 %3d:%3d:%3d:%3d, 2 index */
}

static void print_nc1(const hi_vpss_nrx_v2 *p_x, const hi_vpss_nrx_v2_tfy *pt, const hi_vpss_nrx_v2_nrc1 *pc,
    const hi_vpss_nrx_v2_mdy *pm)
{
    printf("\n -nXtdz          %3d        |        %3d:%3d     |        %3d        "
        "|********************nr_c1****************",
        pt[0].tdz0, pt[1].tdz0, pt[1].tdz1, pt[2].tdz0); /* -C1Hue12%3d:%3d:%3d:%3d, 2 index */
    printf("\n                            |                    |                   | -nC1en              %3d           "
        "    ", p_x->nrc1_en);
    printf("\n                            |                    |                   | -nCsf1          %3d    :%3d       "
        "    ", pc->sfs1, pc->sbr1);
    printf("\n -nXtfr0  (2) %3d:%3d:%3d   |     %3d:%3d:%3d    |    %3d:%3d:%3d    | -nCsf2          %3d:%3d:%3d       "
        "    ", /* 2 index */
        _t3_(pt, 0, tfr0[0], tfr0[1], tfr0[2]), _tmprt3z(pt, 1, 2, tfr0[0], tfr0[1], tfr0[2]), pc->sfs2, pc->sft2,
        pc->sbr2);
    printf("\n              %3d:%3d:%3d   |     %3d:%3d:%3d    |    %3d:%3d:%3d    | -nCsf4          %3d:%3d:%3d       "
        "    ",
        _t3_(pt, 0, tfr0[3], tfr0[4], tfr0[5]), _t3_(pt, 1, tfr0[3], tfr0[4], tfr0[5]), /* 3, 4, 5 index */
        _t3_(pt, 2, tfr0[3], tfr0[4], tfr0[5]), pc->sfs4, pc->sft4, pc->sbr4); /* 2, 3, 4, 5 index */
    printf("\n -nXtfr1                    |     %3d:%3d:%3d    |                   | -nCsf5u             %3d           "
        "      ", _t3_(pt, 1, tfr1[0], tfr1[1], tfr1[2]), pc->sf5_str_u); /* 2 as index */
    printf("\n                            |     %3d:%3d:%3d    |                   | -nCsf5v             %3d           "
        "    ", _t3_(pt, 1, tfr1[3], tfr1[4], tfr1[5]), pc->sf5_str_v); /* 3, 4, 5 index */
    printf("\n                            |                    |                   | -nCsfc6           %3d:%3d         "
        "    ", pc->sfc6, pc->sfc_ext6);
    printf("\n                            |                    |                   | -nCsf6uv          %3d:%3d         "
        "    ", pc->sfr6_u, pc->sfr6_v);
    printf("\n                            |-mXid   %3d:%3d     |      %3d:%3d      | -nCXsf7   %3d:%3d:%3d |  "
        "%3d:%3d:%3d    ",
        _tmprt2z(pm, 0, 1, mai00, mai02), pc->sbn0, pc->spn0, pc->pbr0, pc->sbn1, pc->spn1, pc->pbr1);
    printf("\n                            |-advMath %3d        |                   | -nCXsf8       %3d:%3d |      "
        "%3d:%3d  ", pm[0].adv_math, pc->sat0_l_sfn8, pc->sat0_h_sfn8, pc->sat1_l_sfn8, pc->sat1_h_sfn8);
    printf("\n -mathMode   %3d            |-advTh   %3d        |                   | -nCXsf9       %3d:%3d |      "
        "%3d:%3d  ", pt[0].math_mode, pm[0].adv_th, pc->hue0_l_sfn9, pc->hue0_h_sfn9, pc->hue1_l_sfn9, pc->hue1_h_sfn9);
    printf("\n -autoMath   %3d            |-mXmath %3d:%3d     |        %3d        | -nCXsf10      %3d:%3d |      "
        "%3d:%3d  ",
        pt[0].auto_math, pm[0].math0, pm[0].math1, pm[1].math0, pc->bri0_l_sfn10, pc->bri0_h_sfn10, pc->bri1_l_sfn10,
        pc->bri1_h_sfn10);
    printf("\n                            |-mXmate %3d:%3d     |        %3d        | -nCsfn            %3d |          "
        "%3d  ", pm[0].mate0, pm[0].mate1, pm[1].mate0, pc->sfn0, pc->sfn1);
    printf("\n                            |-mXmabw %3d:%3d     |        %3d        | -postSFC           %3d            "
        "    ", pm[0].mabw0, pm[0].mabw1, pm[1].mabw0, pc->post_sfc);
}

static void print_nrx(const hi_vpss_nrx_v2 *p_x)
{
    const hi_vpss_nrx_v2_iey *pi = p_x->iey;
    const hi_vpss_nrx_v2_sfy *ps = p_x->sfy;
    const hi_vpss_nrx_v2_mdy *pm = p_x->mdy;
    const hi_vpss_nrx_v2_tfy *pt = p_x->tfy;
    const hi_vpss_nrx_v2_nrc1 *pc = &p_x->nrc1;

    print_para_n(p_x, pi, ps, pt);

    print_nc0(p_x, pt, pc, pm);

    print_nc1(p_x, pt, pc, pm);

    printf("\n*********************************************************************************************************"
        "******");
    printf("\n -c0sat0   %3d:%3d:%3d:%3d  |-c0sat4  %3d:%3d:%3d:%3d |-c0sat8  %3d:%3d:%3d:%3d |-c0sat12  "
        "%3d:%3d:%3d:%3d:%3d ",
        _t4_0_(pc, for_grd_sat), _t4_4_(pc, for_grd_sat), _t4_8_(pc, for_grd_sat), _t4_12_(pc, for_grd_sat),
        pc->for_grd_sat[0x10]); /* 0x10 index */
    printf("\n -c0hue0   %3d:%3d:%3d:%3d  |-c0hue4  %3d:%3d:%3d:%3d |-c0hue8  %3d:%3d:%3d:%3d |-c0hue12  "
        "%3d:%3d:%3d:%3d:%3d ",
        _t4_0_(pc, for_grd_hue), _t4_4_(pc, for_grd_hue), _t4_8_(pc, for_grd_hue), _t4_12_(pc, for_grd_hue),
        pc->for_grd_hue[0x10]); /* 0x10 index */
    printf("\n -c0bri0   %3d:%3d:%3d:%3d  |-c0bri4  %3d:%3d:%3d:%3d |-c0bri8  %3d:%3d:%3d:%3d |-c0bri12  "
        "%3d:%3d:%3d:%3d:%3d ",
        _t4_0_(pc, for_grd_bri), _t4_4_(pc, for_grd_bri), _t4_8_(pc, for_grd_bri), _t4_12_(pc, for_grd_bri),
        pc->for_grd_bri[0x10]); /* 0x10 index */
    printf("\n -c1sat0   %3d:%3d:%3d:%3d  |-c1sat4  %3d:%3d:%3d:%3d |-c1sat8  %3d:%3d:%3d:%3d |-c1sat12  "
        "%3d:%3d:%3d:%3d:%3d ",
        _t4_0_(pc, bak_grd_sat), _t4_4_(pc, bak_grd_sat), _t4_8_(pc, bak_grd_sat), _t4_12_(pc, bak_grd_sat),
        pc->bak_grd_sat[0x10]); /* 0x10 index */
    printf("\n -c1hue0   %3d:%3d:%3d:%3d  |-c1hue4  %3d:%3d:%3d:%3d |-c1hue8  %3d:%3d:%3d:%3d |-c1hue12  "
        "%3d:%3d:%3d:%3d:%3d ",
        _t4_0_(pc, bak_grd_hue), _t4_4_(pc, bak_grd_hue), _t4_8_(pc, bak_grd_hue), _t4_12_(pc, bak_grd_hue),
        pc->bak_grd_hue[0x10]); /* 0x10 index */
    printf("\n -c1bri0   %3d:%3d:%3d:%3d  |-c1bri4  %3d:%3d:%3d:%3d |-c1bri8  %3d:%3d:%3d:%3d |-c1bri12  "
        "%3d:%3d:%3d:%3d:%3d ",
        _t4_0_(pc, bak_grd_bri), _t4_4_(pc, bak_grd_bri), _t4_8_(pc, bak_grd_bri), _t4_12_(pc, bak_grd_bri),
        pc->bak_grd_bri[0x10]); /* 0x10 index */
    printf("\n -n2bri0   %3d:%3d:%3d:%3d  |-n2bri4  %3d:%3d:%3d:%3d |-n2bri8  %3d:%3d:%3d:%3d |-n2bri12  "
        "%3d:%3d:%3d:%3d:%3d ", /* 2, index */
        _t4a_0_(ps, 2, bri_str), _t4a_4_(ps, 2, bri_str), _t4a_8_(ps, 2, bri_str), _t4a_12_(ps, 2, bri_str),
        ps[2].bri_str[0x10]); /* 2, 0x10 index */
    printf("\n -n3bri0   %3d:%3d:%3d:%3d  |-n3bri4  %3d:%3d:%3d:%3d |-n3bri8  %3d:%3d:%3d:%3d |-n3bri12  "
        "%3d:%3d:%3d:%3d:%3d ", /* 3, index */
        _t4a_0_(ps, 3, bri_str), _t4a_4_(ps, 3, bri_str), _t4a_8_(ps, 3, bri_str), _t4a_12_(ps, 3, bri_str),
        ps[3].bri_str[0x10]); /* 3, 0x10 index */
    printf("\n*********************************************************************************************************"
        "******\n");
}

hi_void scene_set_static_h265_avbr(hi_venc_rc_param *rc_param, hi_u8 index)
{
    rc_param->h265_avbr_param.chg_pos = g_scene_pipe_param[index].static_venc_attr.venc_param_h265_avbr.change_pos;
    rc_param->h265_avbr_param.min_i_proportion =
        g_scene_pipe_param[index].static_venc_attr.venc_param_h265_avbr.min_iprop;
    rc_param->h265_avbr_param.max_i_proportion =
        g_scene_pipe_param[index].static_venc_attr.venc_param_h265_avbr.max_iprop;
    rc_param->h265_avbr_param.max_reencode_times =
        g_scene_pipe_param[index].static_venc_attr.venc_param_h265_avbr.max_reencode_times;
    rc_param->h265_avbr_param.min_still_percent =
        g_scene_pipe_param[index].static_venc_attr.venc_param_h265_avbr.min_still_percent;
    rc_param->h265_avbr_param.max_still_qp =
        g_scene_pipe_param[index].static_venc_attr.venc_param_h265_avbr.max_still_qp;
    rc_param->h265_avbr_param.min_still_psnr =
        g_scene_pipe_param[index].static_venc_attr.venc_param_h265_avbr.min_still_psnr;
    rc_param->h265_avbr_param.max_qp = g_scene_pipe_param[index].static_venc_attr.venc_param_h265_avbr.max_qp;
    rc_param->h265_avbr_param.min_qp = g_scene_pipe_param[index].static_venc_attr.venc_param_h265_avbr.min_qp;
    rc_param->h265_avbr_param.max_i_qp = g_scene_pipe_param[index].static_venc_attr.venc_param_h265_avbr.max_iqp;
    rc_param->h265_avbr_param.min_i_qp = g_scene_pipe_param[index].static_venc_attr.venc_param_h265_avbr.min_iqp;
    rc_param->h265_avbr_param.min_qp_delta =
        g_scene_pipe_param[index].static_venc_attr.venc_param_h265_avbr.min_qp_delta;
    rc_param->h265_avbr_param.motion_sensitivity =
        g_scene_pipe_param[index].static_venc_attr.venc_param_h265_avbr.motion_sensitivity;
    rc_param->h265_avbr_param.qpmap_en = g_scene_pipe_param[index].static_venc_attr.venc_param_h265_avbr.qp_map_en;
    rc_param->h265_avbr_param.qpmap_mode = g_scene_pipe_param[index].static_venc_attr.venc_param_h265_avbr.qp_map_mode;
}

hi_void scene_set_static_h265_cvbr(hi_venc_rc_param *rc_param, hi_u8 index)
{
    rc_param->h264_cbr_param.min_i_proportion =
        g_scene_pipe_param[index].static_venc_attr.venc_param_h265_cvbr.min_iprop;
    rc_param->h265_cvbr_param.max_i_proportion =
        g_scene_pipe_param[index].static_venc_attr.venc_param_h265_cvbr.max_iprop;
    rc_param->h265_cvbr_param.max_reencode_times =
        g_scene_pipe_param[index].static_venc_attr.venc_param_h265_cvbr.max_reencode_times;
    rc_param->h265_cvbr_param.max_qp = g_scene_pipe_param[index].static_venc_attr.venc_param_h265_cvbr.max_qp;
    rc_param->h265_cvbr_param.min_qp = g_scene_pipe_param[index].static_venc_attr.venc_param_h265_cvbr.min_qp;
    rc_param->h265_cvbr_param.max_i_qp = g_scene_pipe_param[index].static_venc_attr.venc_param_h265_cvbr.max_iqp;
    rc_param->h265_cvbr_param.min_i_qp = g_scene_pipe_param[index].static_venc_attr.venc_param_h265_cvbr.min_iqp;
    rc_param->h265_cvbr_param.min_qp_delta =
        g_scene_pipe_param[index].static_venc_attr.venc_param_h265_cvbr.min_qp_delta;
    rc_param->h265_cvbr_param.qpmap_en = g_scene_pipe_param[index].static_venc_attr.venc_param_h265_cvbr.qp_map_en;
    rc_param->h265_cvbr_param.qpmap_mode = g_scene_pipe_param[index].static_venc_attr.venc_param_h265_cvbr.qp_map_mode;
    rc_param->h265_cvbr_param.max_qp_delta =
        g_scene_pipe_param[index].static_venc_attr.venc_param_h265_cvbr.max_qp_delta;
    rc_param->h265_cvbr_param.extra_bit_percent =
        g_scene_pipe_param[index].static_venc_attr.venc_param_h265_cvbr.extra_bit_percent;
    rc_param->h265_cvbr_param.long_term_stats_time_unit =
        g_scene_pipe_param[index].static_venc_attr.venc_param_h265_cvbr.long_term_static_time_unit;
}

hi_void scene_set_isp_attr(set_isp_attr_param param, const hi_u32 *ratio_level_thresh,
    const hi_u32 *iso_level_thresh, hi_isp_drc_attr *isp_drc_attr)
{
    hi_u8 index = param.index;
    hi_u32 iso_level = param.iso_level;
    hi_u32 iso_count = param.iso_count;
    hi_u32 ratio_level = param.ratio_level;
    hi_u32 ratio_count = param.ratio_count;
    hi_u32 wdr_ratio = param.wdr_ratio;
    hi_u32 iso = param.iso;
    hi_s32 i;
    for (i = 0; i < HI_ISP_DRC_LMIX_NODE_NUM; i++) {
        scene_set_dynamic_drc_return(isp_drc_attr->local_mixing_bright[i], local_mixing_bright[i], 1);
        scene_set_dynamic_drc_return(isp_drc_attr->local_mixing_dark[i], local_mixing_dark[i], 1);
        scene_set_dynamic_drc_return(isp_drc_attr->local_mixing_bright_x[i], local_mixing_bright_x[i], 1);
        scene_set_dynamic_drc_return(isp_drc_attr->local_mixing_dark_x[i], local_mixing_dark_x[i], 1);
    }
    for (i = 0; i < HI_ISP_DRC_FLTX_NODE_NUM; i++) {
        scene_set_dynamic_drc_return(isp_drc_attr->filter_coef_x[i], filter_coef_x[i], 0);
        scene_set_dynamic_drc_return(isp_drc_attr->filter_low_threshold_x[i], filter_low_threshold_x[i], 0);
        scene_set_dynamic_drc_return(isp_drc_attr->filter_high_threshold_x[i], filter_high_threshold_x[i], 0);
    }
    scene_set_dynamic_drc_return(isp_drc_attr->spatial_filter_coef, spatial_filter_coef, 1);
    scene_set_dynamic_drc_return(isp_drc_attr->range_filter_coef, range_filter_coef, 1);
    scene_set_dynamic_drc_return(isp_drc_attr->detail_adjust_coef, detail_adjust_coef, 1);
    scene_set_dynamic_drc_return(isp_drc_attr->rim_reduction_strength, rim_reduction_strength, 1);
    scene_set_dynamic_drc_return(isp_drc_attr->rim_reduction_threshold, rim_reduction_threshold, 1);
    scene_set_dynamic_drc_return(isp_drc_attr->bright_gain_limit, bright_gain_limit, 1);
    scene_set_dynamic_drc_return(isp_drc_attr->bright_gain_limit_step, bright_gain_limit_step, 1);
    scene_set_dynamic_drc_return(isp_drc_attr->dark_gain_limit_luma, dark_gain_limit_luma, 1);
    scene_set_dynamic_drc_return(isp_drc_attr->dark_gain_limit_chroma, dark_gain_limit_chroma, 1);
    scene_set_dynamic_drc_return(isp_drc_attr->blend_luma_max, blend_luma_max, 0);
    scene_set_dynamic_drc_return(isp_drc_attr->detail_adjust_coef_x, detail_adjust_coef_x, 0);
    scene_set_dynamic_drc_return(isp_drc_attr->detail_adjust_coef_blend, detail_adjust_coef_blend, 0);
    scene_set_dynamic_drc_return(isp_drc_attr->low_saturation_color_ctrl, low_saturation_color_ctrl, 0);
    scene_set_dynamic_drc_return(isp_drc_attr->high_saturation_color_ctrl, high_saturation_color_ctrl, 0);
    if (isp_drc_attr->op_type == 0) {
        scene_set_dynamic_drc_return(isp_drc_attr->auto_attr.strength, manual_str, 1);
    } else {
        scene_set_dynamic_drc_return(isp_drc_attr->manual_attr.strength, manual_str, 1);
    }
}

hi_s32 scene_blend_tone_mapping(hi_vi_pipe vi_pipe, hi_u8 index,
    ot_scene_dynamic_drc_coef *drc_coef, hi_isp_drc_attr *isp_drc_attr)
{
    hi_u32 i, j;
    hi_u16 tonemapping_value[HI_ISP_DRC_TM_NODE_NUM] = {0};
    hi_isp_exp_info ae_exp_info;
    hi_s32 ret;
    hi_u16 blend_alpha;

    check_scene_return_if_pause();
    ret = hi_mpi_isp_query_exposure_info(vi_pipe, &ae_exp_info);
    check_scene_ret(ret);

    blend_alpha = (ae_exp_info.wdr_exp_coef < 0x200) ? 0 : ae_exp_info.wdr_exp_coef - 0x200; /* clip 0x200 */

    if (drc_coef->ref_exp_ratio >= (g_scene_pipe_param[index].static_wdr_exposure.exp_ratio_max - 16)) { /* sub16 */
        for (i = 0; i < HI_ISP_DRC_TM_NODE_NUM; i++) {
            tonemapping_value[i] =
            ((hi_u32)blend_alpha * g_scene_pipe_param[index].dynamic_drc.tm_val_higher[i] +
             /* 0x200 blend_alpha, precison 9bit */
            (hi_u32)(0x200 - blend_alpha) * g_scene_pipe_param[index].dynamic_drc.tm_val_highest[i]) >> 9; /* shift9 */
        }
    } else if (drc_coef->ref_exp_ratio <= g_scene_pipe_param[index].dynamic_drc.tm_ratio_threshold) {
        for (i = 0; i < HI_ISP_DRC_TM_NODE_NUM; i++) {
            tonemapping_value[i] =
            ((hi_u32)drc_coef->ref_ratio_alpha * g_scene_pipe_param[index].dynamic_drc.tm_value_low[i] +
             /* 0x100 ref_exp_ratio_alpha, precison 8bit */
            (hi_u32)(0x100 - drc_coef->ref_ratio_alpha) *
            g_scene_pipe_param[index].dynamic_drc.tm_value_high[i]) >> 8; /* shift8 */
        }
    } else {
        for (i = 0; i < HI_ISP_DRC_TM_NODE_NUM; i++) {
            tonemapping_value[i] =
            /* 0x100 ref_exp_ratio_alpha, precison 8bit */
            ((hi_u32)(0x100 - drc_coef->ref_ratio_alpha) * g_scene_pipe_param[index].dynamic_drc.tm_value_high[i] +
            (hi_u32)drc_coef->ref_ratio_alpha * g_scene_pipe_param[index].dynamic_drc.tm_val_higher[i]) >> 8; /* sf8 */
        }
    }

    for (j = 0; j < HI_ISP_DRC_TM_NODE_NUM; j++) {
        isp_drc_attr->tone_mapping_value[j] = scene_time_filter(isp_drc_attr->tone_mapping_value[j],
            tonemapping_value[j], g_scene_pipe_param[index].dynamic_drc.interval, drc_coef->k);
    }

    return HI_SUCCESS;
}

hi_s32 scene_set_tone_mapping_value(hi_vi_pipe vi_pipe, hi_u8 index, hi_u32 k,
    hi_isp_inner_state_info inner_state_info, hi_isp_drc_attr *isp_drc_attr)
{
    static hi_u32 ref_exp_ratio[HI_SCENE_PIPETYPE_NUM][HI_SCENE_REF_EXP_RATIO_FRM] = {0};
    hi_u32 cur_ref_exp_ratio = 0;
    hi_u32 ref_ratio_level;
    hi_u32 ref_exp_ratio_alpha;
    hi_u32 i;
    ot_scene_dynamic_drc_coef dynamic_drc_coef = {0};
    hi_s32 ret;

    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    isp_drc_attr->enable = g_scene_pipe_param[index].dynamic_drc.enable;
    ot_scenecomm_expr_true_return(vi_pipe >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    for (i = 0; i < HI_SCENE_REF_EXP_RATIO_FRM - 1; i++) {
        ref_exp_ratio[vi_pipe][i] = ref_exp_ratio[vi_pipe][i + 1];
    }
    ref_exp_ratio[vi_pipe][i] = inner_state_info.wdr_exp_ratio_actual[0];

    for (i = 0; i < HI_SCENE_REF_EXP_RATIO_FRM; i++) {
        cur_ref_exp_ratio += ref_exp_ratio[vi_pipe][i];
    }

    cur_ref_exp_ratio = cur_ref_exp_ratio / HI_SCENE_REF_EXP_RATIO_FRM;
    ref_ratio_level = scene_get_level_ltoh_u32(cur_ref_exp_ratio, g_scene_pipe_param[index].dynamic_drc.ref_ratio_count,
        g_scene_pipe_param[index].dynamic_drc.ref_ratio_ltoh);
    ot_scenecomm_expr_true_return(ref_ratio_level >= HI_SCENE_DRC_REF_RATIO_MAX_COUNT, HI_FAILURE);
    if ((ref_ratio_level == 0) || (ref_ratio_level == (g_scene_pipe_param[index].dynamic_drc.ref_ratio_count - 1))) {
        ref_exp_ratio_alpha = g_scene_pipe_param[index].dynamic_drc.ref_ratio_alpha[ref_ratio_level];
    } else {
        ref_exp_ratio_alpha = scene_interpulate(cur_ref_exp_ratio,
            g_scene_pipe_param[index].dynamic_drc.ref_ratio_ltoh[ref_ratio_level - 1],
            g_scene_pipe_param[index].dynamic_drc.ref_ratio_alpha[ref_ratio_level - 1],
            g_scene_pipe_param[index].dynamic_drc.ref_ratio_ltoh[ref_ratio_level],
            g_scene_pipe_param[index].dynamic_drc.ref_ratio_alpha[ref_ratio_level]);
    }

    dynamic_drc_coef.ref_exp_ratio = cur_ref_exp_ratio;
    dynamic_drc_coef.ref_ratio_alpha = ref_exp_ratio_alpha;
    dynamic_drc_coef.k = k;
    ret = scene_blend_tone_mapping(vi_pipe, index, &dynamic_drc_coef, isp_drc_attr);
    check_scene_ret(ret);

    return HI_SUCCESS;
}

hi_s32 scene_set_isp_drc_attr(hi_u8 index, hi_u64 iso, hi_isp_drc_attr *isp_drc_attr)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    hi_u32 iso_count = g_scene_pipe_param[index].dynamic_linear_drc.iso_count;
    hi_u32 *iso_level_thresh = g_scene_pipe_param[index].dynamic_linear_drc.iso_level;
    isp_drc_attr->enable = g_scene_pipe_param[index].dynamic_linear_drc.enable;
    hi_u32 iso_level = (hi_u32)scene_get_level_ltoh_u32((hi_u32)iso, iso_count, iso_level_thresh);
    ot_scenecomm_expr_true_return(iso_level >= HI_SCENE_DRC_ISO_MAX_COUNT, HI_FAILURE);

    for (hi_u32 i = 0; i < HI_ISP_DRC_LMIX_NODE_NUM; i++) {
        scene_set_dynamic_linear_drc(isp_drc_attr->local_mixing_bright[i], local_mixing_bright[i]);
        scene_set_dynamic_linear_drc(isp_drc_attr->local_mixing_dark[i], local_mixing_dark[i]);
        scene_set_dynamic_linear_drc(isp_drc_attr->local_mixing_bright_x[i], local_mixing_bright_x[i]);
        scene_set_dynamic_linear_drc(isp_drc_attr->local_mixing_dark_x[i], local_mixing_dark_x[i]);
    }
    for (hi_u32 i = 0; i < HI_ISP_DRC_FLTX_NODE_NUM; i++) {
        scene_set_dynamic_linear_drc(isp_drc_attr->filter_coef_x[i], filter_coef_x[i]);
        scene_set_dynamic_linear_drc(isp_drc_attr->filter_low_threshold_x[i], filter_low_threshold_x[i]);
        scene_set_dynamic_linear_drc(isp_drc_attr->filter_high_threshold_x[i], filter_high_threshold_x[i]);
    }

    scene_set_dynamic_linear_drc(isp_drc_attr->spatial_filter_coef, spatial_filter_coef);
    scene_set_dynamic_linear_drc(isp_drc_attr->range_filter_coef, range_filter_coef);
    scene_set_dynamic_linear_drc(isp_drc_attr->detail_adjust_coef, detail_adjust_coef);
    scene_set_dynamic_linear_drc(isp_drc_attr->rim_reduction_strength, rim_reduction_strength);
    scene_set_dynamic_linear_drc(isp_drc_attr->rim_reduction_threshold, rim_reduction_threshold);

    scene_set_dynamic_linear_drc(isp_drc_attr->bright_gain_limit, bright_gain_limit);
    scene_set_dynamic_linear_drc(isp_drc_attr->bright_gain_limit_step, bright_gain_limit_step);
    scene_set_dynamic_linear_drc(isp_drc_attr->dark_gain_limit_luma, dark_gain_limit_luma);
    scene_set_dynamic_linear_drc(isp_drc_attr->dark_gain_limit_chroma, dark_gain_limit_chroma);

    scene_set_dynamic_linear_drc(isp_drc_attr->detail_adjust_coef_x, detail_adjust_coef_x);
    scene_set_dynamic_linear_drc(isp_drc_attr->detail_adjust_coef_blend, detail_adjust_coef_blend);

    scene_set_dynamic_linear_drc(isp_drc_attr->low_saturation_color_ctrl, low_saturation_color_ctrl);
    scene_set_dynamic_linear_drc(isp_drc_attr->high_saturation_color_ctrl, high_saturation_color_ctrl);

    scene_set_dynamic_linear_drc(isp_drc_attr->asymmetry_curve.asymmetry, asymmetry);
    scene_set_dynamic_linear_drc(isp_drc_attr->asymmetry_curve.second_pole, second_pole);
    scene_set_dynamic_linear_drc(isp_drc_attr->asymmetry_curve.compress, compress);
    scene_set_dynamic_linear_drc(isp_drc_attr->asymmetry_curve.stretch, stretch);

    if (isp_drc_attr->op_type == 0) {
        scene_set_dynamic_linear_drc(isp_drc_attr->auto_attr.strength, strength);
    } else {
        scene_set_dynamic_linear_drc(isp_drc_attr->manual_attr.strength, strength);
    }

    return HI_SUCCESS;
}

static hi_void scene_set_wdr_cfg(hi_u8 index, hi_u32 iso_level, hi_isp_nr_attr *nr_attr,
    const ot_scene_nr_para *nr_para)
{
    hi_s32 i;

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        nr_attr->wdr_cfg.snr_sfm0_wdr_strength[i] =
            g_scene_pipe_param[index].dynamic_nr.snr_sfm0_wdr_frame_str[i][iso_level] +
            nr_para->sfm0_wdr_incr[i];
        nr_attr->wdr_cfg.snr_sfm0_fusion_strength[i] =
            g_scene_pipe_param[index].dynamic_nr.snr_sfm0_fusion_frame_str[i][iso_level] +
            nr_para->sfm0_fusion_incr[i];

        nr_attr->wdr_cfg.md_wdr_strength[i] = g_scene_pipe_param[index].dynamic_nr.md_wdr_frame_str[i][iso_level] +
            nr_para->md_wdr_incr[i];
        nr_attr->wdr_cfg.md_fusion_strength[i] =
            g_scene_pipe_param[index].dynamic_nr.md_fusion_frame_str[i][iso_level] +
            nr_para->md_fusion_incr[i];
    }

    nr_attr->wdr_cfg.snr_sfm1_fusion_strength =
        g_scene_pipe_param[index].dynamic_nr.snr_sfm1_fusion_frame_str_s[iso_level];
    nr_attr->wdr_cfg.snr_sfm1_wdr_strength = g_scene_pipe_param[index].dynamic_nr.snr_sfm1_wdr_frame_str_s[iso_level];
}

hi_s32 scene_set_nr_attr_para(hi_u8 index, hi_u32 iso, hi_isp_nr_attr *nr_attr, const ot_scene_nr_para *nr_para)
{
    hi_s32 i;
    hi_u32 count = g_scene_pipe_param[index].dynamic_nr.coring_ratio_count;
    hi_u32 *pthresh = (hi_u32 *)g_scene_pipe_param[index].dynamic_nr.coring_ratio_iso;
    hi_u32 iso_level = scene_get_level_ltoh_u32(iso, count, pthresh);
    ot_scenecomm_expr_true_return(iso_level >= HI_SCENE_ISO_STRENGTH_NUM, HI_FAILURE);

    if ((iso_level == (count - 1)) || (iso_level == 0)) {
        for (i = 0; i < HI_ISP_BAYERNR_LUT_LENGTH; i++) {
            nr_attr->coring_ratio[i] = g_scene_pipe_param[index].dynamic_nr.coring_ratio[i][iso_level];
        }
        scene_set_wdr_cfg(index, iso_level, nr_attr, nr_para);
    } else {
        hi_u32 mid_iso = (hi_u32)iso;
        hi_u32 left_iso = pthresh[iso_level - 1];
        hi_u32 right_iso = pthresh[iso_level];

        for (i = 0; i < HI_ISP_BAYERNR_LUT_LENGTH; i++) {
            nr_attr->coring_ratio[i] = scene_interpulate(mid_iso, left_iso,
                g_scene_pipe_param[index].dynamic_nr.coring_ratio[i][iso_level - 1], right_iso,
                g_scene_pipe_param[index].dynamic_nr.coring_ratio[i][iso_level]);
        }

        for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
            nr_attr->wdr_cfg.snr_sfm0_wdr_strength[i] = scene_interpulate(mid_iso, left_iso,
                g_scene_pipe_param[index].dynamic_nr.snr_sfm0_wdr_frame_str[i][iso_level - 1], right_iso,
                g_scene_pipe_param[index].dynamic_nr.snr_sfm0_wdr_frame_str[i][iso_level]) +
                nr_para->sfm0_wdr_incr[i];
            nr_attr->wdr_cfg.snr_sfm0_fusion_strength[i] = scene_interpulate(mid_iso, left_iso,
                g_scene_pipe_param[index].dynamic_nr.snr_sfm0_fusion_frame_str[i][iso_level - 1], right_iso,
                g_scene_pipe_param[index].dynamic_nr.snr_sfm0_fusion_frame_str[i][iso_level]) +
                nr_para->sfm0_fusion_incr[i];

            nr_attr->wdr_cfg.md_wdr_strength[i] = scene_interpulate(mid_iso, left_iso,
                g_scene_pipe_param[index].dynamic_nr.md_wdr_frame_str[i][iso_level - 1], right_iso,
                g_scene_pipe_param[index].dynamic_nr.md_wdr_frame_str[i][iso_level]) +
                nr_para->md_wdr_incr[i];
            nr_attr->wdr_cfg.md_fusion_strength[i] = scene_interpulate(mid_iso, left_iso,
                g_scene_pipe_param[index].dynamic_nr.md_fusion_frame_str[i][iso_level - 1], right_iso,
                g_scene_pipe_param[index].dynamic_nr.md_fusion_frame_str[i][iso_level]) +
                nr_para->md_fusion_incr[i];
        }

        nr_attr->wdr_cfg.snr_sfm1_fusion_strength = scene_interpulate(mid_iso, left_iso,
            g_scene_pipe_param[index].dynamic_nr.snr_sfm1_fusion_frame_str_s[iso_level - 1], right_iso,
            g_scene_pipe_param[index].dynamic_nr.snr_sfm1_fusion_frame_str_s[iso_level]);
        nr_attr->wdr_cfg.snr_sfm1_wdr_strength = scene_interpulate(mid_iso, left_iso,
            g_scene_pipe_param[index].dynamic_nr.snr_sfm1_wdr_frame_str_s[iso_level - 1], right_iso,
            g_scene_pipe_param[index].dynamic_nr.snr_sfm1_wdr_frame_str_s[iso_level]);
    }
    return HI_SUCCESS;
}

#define MAXRATIO 16384
hi_s32 scene_set_nr_wdr_ratio_para(hi_vi_pipe vi_pipe, hi_u8 index, hi_u32 wdr_ratio, hi_isp_nr_attr *nr_attr)
{
    hi_u32 i;
    hi_s32 ret;
    hi_isp_pub_attr pub_attr;
    hi_u32 coring_count = get_pipe_params()[index].dynamic_nr.coring_ratio_count;

    ret = hi_mpi_isp_get_pub_attr(vi_pipe, &pub_attr);
    check_scene_ret(ret);

    if (pub_attr.wdr_mode == HI_WDR_MODE_NONE) {
        return HI_SUCCESS;
    }

    if (wdr_ratio > get_pipe_params()[index].dynamic_nr.wdr_ratio_threshold) {
        for (i = 0; i < HI_ISP_AUTO_ISO_NUM; i++) {
            nr_attr->tnr_cfg.tnr_auto.tss[i] = get_pipe_params()[index].dynamic_nr.tnr_tss_h[i];
            nr_attr->snr_cfg.snr_auto.coring_wgt[i] = get_pipe_params()[index].dynamic_nr.coring_wgt_h[i];
        }

        for (i = 0; i < HI_ISP_BAYERNR_LUT_LENGTH; i++) {
            nr_attr->coring_ratio[i] = scene_interpulate(wdr_ratio,
                get_pipe_params()[index].dynamic_nr.wdr_ratio_threshold,
                get_pipe_params()[index].dynamic_nr.coring_ratio[i][coring_count - 2], /* 2 */
                MAXRATIO,
                get_pipe_params()[index].dynamic_nr.coring_ratio[i][coring_count - 1]);
        }
    } else {
        for (i = 0; i < HI_ISP_AUTO_ISO_NUM; i++) {
            nr_attr->tnr_cfg.tnr_auto.tss[i] = get_pipe_params()[index].dynamic_nr.tnr_tss_l[i];
            nr_attr->snr_cfg.snr_auto.coring_wgt[i] = get_pipe_params()[index].dynamic_nr.coring_wgt_l[i];
        }
    }

    return HI_SUCCESS;
}

static hi_void scene_set_nry_en(hi_vpss_grp_param *nrx_attr, const ot_scene_3dnr *_3dnr)
{
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nry0_en = _3dnr->nry0_en;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nry1_en = _3dnr->nry1_en;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nry2_en = _3dnr->nry2_en;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nry3_en = _3dnr->nry3_en;
}

static hi_void scene_set_nry_iey(hi_vpss_grp_param *nrx_attr, const ot_scene_3dnr *_3dnr)
{
    /* ie_y */
    for (hi_u32 i = 0; i < ARRAY_SIZE(_3dnr->iey); i++) {
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.iey[i].ies0 = _3dnr->iey[i].ies0;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.iey[i].ies1 = _3dnr->iey[i].ies1;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.iey[i].ies2 = _3dnr->iey[i].ies2;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.iey[i].ies3 = _3dnr->iey[i].ies3;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.iey[i].iedz = _3dnr->iey[i].iedz;
    }
}

static hi_void scene_set_nry_sfy_1to4(hi_vpss_grp_param *nrx_attr, const ot_scene_3dnr *_3dnr)
{
    /* sf_y */
    for (hi_u32 i = 0; i < 4; i++) { /* array sfy have 4 valid element */
        /* sfs1 */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sfs1 = _3dnr->sfy[i].sfs1;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sbr1 = _3dnr->sfy[i].sbr1;

        /* sfs2 */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sfs2 = _3dnr->sfy[i].sfs2;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sft2 = _3dnr->sfy[i].sft2;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sbr2 = _3dnr->sfy[i].sbr2;

        /* sfs4 */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sfs4 = _3dnr->sfy[i].sfs4;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sft4 = _3dnr->sfy[i].sft4;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sbr4 = _3dnr->sfy[i].sbr4;

        /* sf5_md */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sf5_md = _3dnr->sfy[i].sf5_md;
    }

    /* sfs4 */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[4].sfs4 = _3dnr->sfy[4].sfs4; /* 4 index */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[4].sft4 = _3dnr->sfy[4].sft4; /* 4 index */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[4].sbr4 = _3dnr->sfy[4].sbr4; /* 4 index */
}

static hi_void scene_set_nry_sfy_6to8(hi_vpss_grp_param *nrx_attr, const ot_scene_3dnr *_3dnr)
{
    for (hi_u32 i = 0; i < ARRAY_SIZE(_3dnr->sfy); i++) {
        /* sfs7 */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].spn = _3dnr->sfy[i].spn;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sbn = _3dnr->sfy[i].sbn;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].pbr = _3dnr->sfy[i].pbr;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].j_mode = _3dnr->sfy[i].j_mode;

        /* sfr7 */
        for (hi_u32 j = 0; j < ARRAY_SIZE(_3dnr->sfy[i].sfr7); j++) {
            nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sfr7[j] = _3dnr->sfy[i].sfr7[j];
        }

        /* sbr7 */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sbr7[0] = _3dnr->sfy[i].sbr7[0];
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sbr7[1] = _3dnr->sfy[i].sbr7[1];

        /* sfn */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sfn0_0 = _3dnr->sfy[i].sfn0_0;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sfn1_0 = _3dnr->sfy[i].sfn1_0;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sfn2_0 = _3dnr->sfy[i].sfn2_0;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sfn3_0 = _3dnr->sfy[i].sfn3_0;

        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sfn0_1 = _3dnr->sfy[i].sfn0_1;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sfn1_1 = _3dnr->sfy[i].sfn1_1;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sfn2_1 = _3dnr->sfy[i].sfn2_1;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sfn3_1 = _3dnr->sfy[i].sfn3_1;

        /* sth */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sth1_0 = _3dnr->sfy[i].sth1_0;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sth2_0 = _3dnr->sfy[i].sth2_0;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sth3_0 = _3dnr->sfy[i].sth3_0;

        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sth1_1 = _3dnr->sfy[i].sth1_1;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sth2_1 = _3dnr->sfy[i].sth1_2;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sth3_1 = _3dnr->sfy[i].sth1_3;

        /* sfr */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sfr = _3dnr->sfy[i].sfr;
    }

    /* sf8 */
    for (hi_u32 i = 3; i <= 4; i++) { /* sfy array index 3 4 */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sf8_idx0 = _3dnr->sfy[i].sf8_idx0;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sf8_idx1 = _3dnr->sfy[i].sf8_idx1;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sf8_tfr = _3dnr->sfy[i].sf8_thr;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].sf8_thrd = _3dnr->sfy[i].sf8_thrd;
    }

    /* bri */
    for (hi_u32 i = 2; i <= 3; i++) { /* sfy array index 2 3 */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].bri_idx0 = _3dnr->sfy[i].bri_idx0;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[i].bri_idx1 = _3dnr->sfy[i].bri_idx1;
    }
}

static hi_void scene_set_nry_tfy(hi_vpss_grp_param *nrx_attr, const ot_scene_3dnr *_3dnr)
{
    /* ref */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.tfy[0].ref_en = _3dnr->tfy[0].ref_en;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.tfy[1].ref_en = _3dnr->tfy[1].ref_en;

    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.tfy[0].math_mode = _3dnr->tfy[0].math_mode;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.tfy[0].auto_math = _3dnr->tfy[0].auto_math;

    /* tf_y */
    for (hi_u32 i = 0; i < ARRAY_SIZE(_3dnr->tfy); i++) {
        /* str */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.tfy[i].str0 = _3dnr->tfy[i].str0;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.tfy[1].str1 = _3dnr->tfy[1].str1;

        /* sdz */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.tfy[i].sdz0 = _3dnr->tfy[i].sdz0;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.tfy[1].sdz1 = _3dnr->tfy[1].sdz1;

        /* tss */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.tfy[i].tss0 = _3dnr->tfy[i].tss0;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.tfy[1].tss1 = _3dnr->tfy[1].tss1;

        /* tfs */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.tfy[i].tfs0 = _3dnr->tfy[i].tfs0;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.tfy[1].tfs1 = _3dnr->tfy[1].tfs1;

        /* tdz */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.tfy[i].tdz0 = _3dnr->tfy[i].tdz0;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.tfy[1].tdz1 = _3dnr->tfy[1].tdz1;

        /* tfr0 */
        for (hi_u32 j = 0; j < ARRAY_SIZE(_3dnr->tfy[i].tfr0); j++) {
            nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.tfy[i].tfr0[j] = _3dnr->tfy[i].tfr0[j];
        }

        /* tfr1 */
        for (hi_u32 j = 0; j < ARRAY_SIZE(_3dnr->tfy[1].tfr1); j++) {
            nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.tfy[1].tfr1[j] = _3dnr->tfy[1].tfr1[j];
        }
    }
}

static hi_void scene_set_nry_mdy(hi_vpss_grp_param *nrx_attr, const ot_scene_3dnr *_3dnr)
{
    /* md_y */
    for (hi_u32 i = 0; i < ARRAY_SIZE(_3dnr->mdy); i++) {
        /* id0 */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.mdy[i].mai00 = _3dnr->mdy[i].mai00;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.mdy[i].mai02 = _3dnr->mdy[i].mai02;

        /* math */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.mdy[i].math0 = _3dnr->mdy[i].math0;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.mdy[0].math1 = _3dnr->mdy[0].math1;

        /* mate */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.mdy[i].mate0 = _3dnr->mdy[i].mate0;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.mdy[0].mate1 = _3dnr->mdy[0].mate1;

        /* mabw */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.mdy[i].mabw0 = _3dnr->mdy[i].mabw0;
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.mdy[0].mabw1 = _3dnr->mdy[0].mabw1;

        /* adv_math */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.mdy[0].adv_math = _3dnr->mdy[0].adv_math;

        /* adv_th */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.mdy[0].adv_th = _3dnr->mdy[0].adv_th;
    }
}

static hi_void scene_set_nry_nc0(hi_vpss_grp_param *nrx_attr, const ot_scene_3dnr *_3dnr)
{
    /* nrc0_en */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc0_en = _3dnr->nrc0_en;

    /* auto_math */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc0.auto_math = _3dnr->nrc0.auto_math;

    /* sfc */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc0.sfc = _3dnr->nrc0.sfc;

    /* sfc_enhance */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc0.sfc_enhance = _3dnr->nrc0.sfc_enhance;

    /* sfc_ext */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc0.sfc_ext = _3dnr->nrc0.sfc_ext;

    /* tfc */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc0.tfc = _3dnr->nrc0.tfc;

    /* trc */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc0.trc = _3dnr->nrc0.trc;
}

static hi_void scene_set_nry_nc1(hi_vpss_grp_param *nrx_attr, const ot_scene_3dnr *_3dnr)
{
    /* nrc1_en */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1_en = _3dnr->nrc1_en;

    /* sfs1 */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.sfs1 = _3dnr->nrc1.sfs1;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.sbr1 = _3dnr->nrc1.sbr1;

    /* sfs2 */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.sfs2 = _3dnr->nrc1.sfs2;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.sft2 = _3dnr->nrc1.sft2;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.sbr2 = _3dnr->nrc1.sbr2;

    /* sfs4 */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.sfs4 = _3dnr->nrc1.sfs4;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.sft4 = _3dnr->nrc1.sft4;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.sbr4 = _3dnr->nrc1.sbr4;

    /* sf5_str_u */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.sf5_str_u = _3dnr->nrc1.sf5_str_u;

    /* sf5_str_v */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.sf5_str_v = _3dnr->nrc1.sf5_str_v;

    /* sf6 */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.sfc6 = _3dnr->nrc1.sfc6;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.sfc_ext6 = _3dnr->nrc1.sfc_ext6;

    /* sfr6_uv */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.sfr6_u = _3dnr->nrc1.sfr6_u;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.sfr6_v = _3dnr->nrc1.sfr6_v;

    /* sf7 */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.spn0 = _3dnr->nrc1.spn0;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.sbn0 = _3dnr->nrc1.sbn0;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.pbr0 = _3dnr->nrc1.pbr0;

    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.spn1 = _3dnr->nrc1.spn1;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.sbn1 = _3dnr->nrc1.sbn1;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.pbr1 = _3dnr->nrc1.pbr1;

    /* sf8 */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.sat0_l_sfn8 = _3dnr->nrc1.sat0_l_sfn8;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.sat0_h_sfn8 = _3dnr->nrc1.sat0_h_sfn8;

    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.sat1_l_sfn8 = _3dnr->nrc1.sat1_l_sfn8;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.sat1_h_sfn8 = _3dnr->nrc1.sat1_h_sfn8;

    /* sf9 */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.hue0_l_sfn9 = _3dnr->nrc1.hue0_l_sfn9;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.hue0_h_sfn9 = _3dnr->nrc1.hue0_h_sfn9;

    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.hue1_l_sfn9 = _3dnr->nrc1.hue1_l_sfn9;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.hue1_h_sfn9 = _3dnr->nrc1.hue1_h_sfn9;

    /* sf10 */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.bri0_l_sfn10 = _3dnr->nrc1.bri0_l_sfn10;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.bri0_h_sfn10 = _3dnr->nrc1.bri0_h_sfn10;

    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.bri1_l_sfn10 = _3dnr->nrc1.bri1_l_sfn10;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.bri1_h_sfn10 = _3dnr->nrc1.bri1_h_sfn10;

    /* sfn */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.sfn0 = _3dnr->nrc1.sfn0;
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.sfn1 = _3dnr->nrc1.sfn1;

    /* post_sfc */
    nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.post_sfc = _3dnr->nrc1.post_sfc;

    for (hi_u32 i = 0; i < HI_SCENE_3DNR_IDX_LEN; i++) { /* 2 3 sfy index */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.bak_grd_sat[i] = _3dnr->nrc1.bak_grd_sat[i];
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.bak_grd_hue[i] = _3dnr->nrc1.bak_grd_hue[i];
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.bak_grd_bri[i] = _3dnr->nrc1.bak_grd_bri[i];
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.for_grd_sat[i] = _3dnr->nrc1.for_grd_sat[i];
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.for_grd_hue[i] = _3dnr->nrc1.for_grd_hue[i];
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.nrc1.for_grd_bri[i] = _3dnr->nrc1.for_grd_bri[i];
        /* 2 index */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[2].bri_str[i] = _3dnr->sfy[2].bri_str[i];
        /* 3 index */
        nrx_attr->nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param.sfy[3].bri_str[i] = _3dnr->sfy[3].bri_str[i];
    }
}

hi_s32 scene_set_3dnr(hi_vi_pipe vi_pipe, const ot_scene_3dnr *_3dnr, hi_u8 index)
{
    hi_s32 ret;

    hi_vpss_grp_param nrx_attr, nrx_get_attr;
    nrx_attr.nrx_param.nr_version = HI_VPSS_NR_V2;
    nrx_attr.nrx_param.nrx_norm_param_v2.op_mode = HI_OP_MODE_MANUAL;
    ret = hi_mpi_vpss_get_grp_param(vi_pipe, &nrx_attr);
    check_scene_ret(ret);

    scene_set_nry_en(&nrx_attr, _3dnr);

    scene_set_nry_iey(&nrx_attr, _3dnr);

    scene_set_nry_sfy_1to4(&nrx_attr, _3dnr);
    scene_set_nry_sfy_6to8(&nrx_attr, _3dnr);

    scene_set_nry_tfy(&nrx_attr, _3dnr);

    scene_set_nry_mdy(&nrx_attr, _3dnr);

    scene_set_nry_nc0(&nrx_attr, _3dnr);

    scene_set_nry_nc1(&nrx_attr, _3dnr);

    nrx_attr.nrx_param.nr_version = HI_VPSS_NR_V2;
    nrx_attr.nrx_param.nrx_norm_param_v2.op_mode = HI_OP_MODE_MANUAL;

    ret = hi_mpi_vpss_set_grp_param(vi_pipe, &nrx_attr);
    check_scene_ret(ret);

    nrx_get_attr.nrx_param.nr_version = HI_VPSS_NR_V2;
    nrx_get_attr.nrx_param.nrx_norm_param_v2.op_mode = HI_OP_MODE_MANUAL;
    ret = hi_mpi_vpss_get_grp_param(vi_pipe, &nrx_get_attr);
    check_scene_ret(ret);

    static hi_u32 count[HI_SCENE_PIPETYPE_NUM] = {0};
    if ((g_scene_pipe_param[index].module_state.debug == 1) && ((count[vi_pipe] % 15) == 0)) { /* 15 max count */
        printf("2======================nrx_get_attr ================================");
        print_nrx(&nrx_get_attr.nrx_param.nrx_norm_param_v2.nrx_manual.nrx_param);
    }
    count[vi_pipe]++;
    return HI_SUCCESS;
}

hi_void scene_set_3dnr_nrx_nry(_3dnr_nrx_param param)
{
    ot_scene_3dnr *nrx_attr = param.nrx_attr;
    const ot_scene_static_3dnr *nrx_param = param.nrx_param;
    hi_u32 iso_level = param.iso_level;
    hi_u32 mid = param.mid;
    hi_u32 left = param.left;
    hi_u32 right = param.right;

    scene_set_nrx_attr_interpulate(nry0_en);
    scene_set_nrx_attr_interpulate(nry1_en);
    scene_set_nrx_attr_interpulate(nry2_en);
    scene_set_nrx_attr_interpulate(nry3_en);
}

hi_void scene_set_3dnr_nrx_iey(_3dnr_nrx_param param)
{
    ot_scene_3dnr *nrx_attr = param.nrx_attr;
    const ot_scene_static_3dnr *nrx_param = param.nrx_param;
    hi_u32 iso_level = param.iso_level;
    hi_u32 mid = param.mid;
    hi_u32 left = param.left;
    hi_u32 right = param.right;

    /* ie_y */
    for (hi_u32 i = 0; i < 5; i++) { /* 5 iey array size */
        scene_set_nrx_attr_interpulate(iey[i].ies0);
        scene_set_nrx_attr_interpulate(iey[i].ies1);
        scene_set_nrx_attr_interpulate(iey[i].ies2);
        scene_set_nrx_attr_interpulate(iey[i].ies3);
        scene_set_nrx_attr_interpulate(iey[i].iedz);
    }
}

static hi_void scene_set_nrx_sfy(_3dnr_nrx_param param)
{
    ot_scene_3dnr *nrx_attr = param.nrx_attr;
    const ot_scene_static_3dnr *nrx_param = param.nrx_param;
    hi_u32 iso_level = param.iso_level;
    hi_u32 mid = param.mid;
    hi_u32 left = param.left;
    hi_u32 right = param.right;

    /* sf_y */
    for (hi_u32 i = 0; i < 4; i++) { /* 4 sfy index */
        /* sfs1 */
        scene_set_nrx_attr_interpulate(sfy[i].sfs1);
        scene_set_nrx_attr_interpulate(sfy[i].sbr1);

        /* sfs2 */
        scene_set_nrx_attr_interpulate(sfy[i].sfs2);
        scene_set_nrx_attr_interpulate(sfy[i].sft2);
        scene_set_nrx_attr_interpulate(sfy[i].sbr2);

        /* sfs4 */
        scene_set_nrx_attr_interpulate(sfy[i].sfs4);
        scene_set_nrx_attr_interpulate(sfy[i].sft4);
        scene_set_nrx_attr_interpulate(sfy[i].sbr4);

        /* sf5_md */
        scene_set_nrx_attr_interpulate(sfy[i].sf5_md);
    }
}

static hi_void scene_set_nrx_sfy4(_3dnr_nrx_param param)
{
    ot_scene_3dnr *nrx_attr = param.nrx_attr;
    const ot_scene_static_3dnr *nrx_param = param.nrx_param;
    hi_u32 iso_level = param.iso_level;
    hi_u32 mid = param.mid;
    hi_u32 left = param.left;
    hi_u32 right = param.right;

    /* 4 sfy index */
    scene_set_nrx_attr_interpulate(sfy[4].sfs4); /* 4 sfy index */
    scene_set_nrx_attr_interpulate(sfy[4].sft4); /* 4 sfy index */
    scene_set_nrx_attr_interpulate(sfy[4].sbr4); /* 4 sfy index */
}

static hi_void set_sfs7_param(_3dnr_nrx_param param)
{
    ot_scene_3dnr *nrx_attr = param.nrx_attr;
    const ot_scene_static_3dnr *nrx_param = param.nrx_param;
    hi_u32 iso_level = param.iso_level;
    hi_u32 mid = param.mid;
    hi_u32 left = param.left;
    hi_u32 right = param.right;

    for (hi_u32 i = 0; i < ARRAY_SIZE(nrx_attr->sfy); i++) {
        /* sfs7 */
        scene_set_nrx_attr_interpulate(sfy[i].spn);
        scene_set_nrx_attr_interpulate(sfy[i].sbn);
        scene_set_nrx_attr_interpulate(sfy[i].pbr);
        scene_set_nrx_attr_interpulate(sfy[i].j_mode);

        /* sfr7 */
        for (hi_u32 j = 0; j < ARRAY_SIZE(nrx_attr->sfy[i].sfr7); j++) {
            scene_set_nrx_attr_interpulate(sfy[i].sfr7[j]);
        }

        /* sbr7 */
        scene_set_nrx_attr_interpulate(sfy[i].sbr7[0]);
        scene_set_nrx_attr_interpulate(sfy[i].sbr7[1]);

        /* sfn */
        nrx_attr->sfy[i].sfn0_0 = nrx_param->threednr_value[iso_level - 1].sfy[i].sfn0_0;
        nrx_attr->sfy[i].sfn1_0 = nrx_param->threednr_value[iso_level - 1].sfy[i].sfn1_0;
        nrx_attr->sfy[i].sfn2_0 = nrx_param->threednr_value[iso_level - 1].sfy[i].sfn2_0;
        nrx_attr->sfy[i].sfn3_0 = nrx_param->threednr_value[iso_level - 1].sfy[i].sfn3_0;

        nrx_attr->sfy[i].sfn0_1 = nrx_param->threednr_value[iso_level - 1].sfy[i].sfn0_1;
        nrx_attr->sfy[i].sfn1_1 = nrx_param->threednr_value[iso_level - 1].sfy[i].sfn1_1;
        nrx_attr->sfy[i].sfn2_1 = nrx_param->threednr_value[iso_level - 1].sfy[i].sfn2_1;
        nrx_attr->sfy[i].sfn3_1 = nrx_param->threednr_value[iso_level - 1].sfy[i].sfn3_1;

        /* sth */
        scene_set_nrx_attr_interpulate(sfy[i].sth1_0);
        scene_set_nrx_attr_interpulate(sfy[i].sth2_0);
        scene_set_nrx_attr_interpulate(sfy[i].sth3_0);

        scene_set_nrx_attr_interpulate(sfy[i].sth1_1);
        scene_set_nrx_attr_interpulate(sfy[i].sth1_2);
        scene_set_nrx_attr_interpulate(sfy[i].sth1_3);

        /* sfr */
        scene_set_nrx_attr_interpulate(sfy[i].sfr);
    }
}

static hi_void scene_set_nrx_sfy8(_3dnr_nrx_param param)
{
    ot_scene_3dnr *nrx_attr = param.nrx_attr;
    const ot_scene_static_3dnr *nrx_param = param.nrx_param;
    hi_u32 iso_level = param.iso_level;
    hi_u32 mid = param.mid;
    hi_u32 left = param.left;
    hi_u32 right = param.right;

    for (hi_u32 i = 3; i <= 4; i++) { /* sfy array index 3 4 */
        scene_set_nrx_attr_interpulate(sfy[i].sf8_idx0);
        scene_set_nrx_attr_interpulate(sfy[i].sf8_idx1);

        scene_set_nrx_attr_interpulate(sfy[i].sf8_thr);
        scene_set_nrx_attr_interpulate(sfy[i].sf8_thrd);
    }
}

static hi_void scene_set_nrx_sfy_bri(_3dnr_nrx_param param)
{
    ot_scene_3dnr *nrx_attr = param.nrx_attr;
    const ot_scene_static_3dnr *nrx_param = param.nrx_param;
    hi_u32 iso_level = param.iso_level;
    hi_u32 mid = param.mid;
    hi_u32 left = param.left;
    hi_u32 right = param.right;

    for (hi_u32 i = 2; i <= 3; i++) { /* sfy array index 2 3 */
        scene_set_nrx_attr_interpulate(sfy[i].bri_idx0);
        scene_set_nrx_attr_interpulate(sfy[i].bri_idx1);
    }
}

hi_void scene_set_3dnr_nrx_sfy(_3dnr_nrx_param param)
{
    /* sf_y */
    scene_set_nrx_sfy(param);

    /* sfs4 */
    scene_set_nrx_sfy4(param);

    set_sfs7_param(param);

    /* sf8 */
    scene_set_nrx_sfy8(param);

    /* bri */
    scene_set_nrx_sfy_bri(param);
}

hi_void scene_set_3dnr_nrx_tfy(_3dnr_nrx_param param)
{
    ot_scene_3dnr *nrx_attr = param.nrx_attr;
    const ot_scene_static_3dnr *nrx_param = param.nrx_param;
    hi_u32 iso_level = param.iso_level;
    hi_u32 mid = param.mid;
    hi_u32 left = param.left;
    hi_u32 right = param.right;

    /* ref */
    scene_set_nrx_attr_interpulate(tfy[0].ref_en);
    scene_set_nrx_attr_interpulate(tfy[1].ref_en);

    scene_set_nrx_attr_interpulate(tfy[0].math_mode);
    scene_set_nrx_attr_interpulate(tfy[0].auto_math);

    /* tf_y */
    for (hi_u32 i = 0; i < ARRAY_SIZE(nrx_attr->tfy); i++) {
        /* str */
        scene_set_nrx_attr_interpulate(tfy[i].str0);
        scene_set_nrx_attr_interpulate(tfy[1].str1);

        /* sdz */
        scene_set_nrx_attr_interpulate(tfy[i].sdz0);
        scene_set_nrx_attr_interpulate(tfy[1].sdz1);

        /* tss */
        scene_set_nrx_attr_interpulate(tfy[i].tss0);
        scene_set_nrx_attr_interpulate(tfy[1].tss1);

        /* tfs */
        scene_set_nrx_attr_interpulate(tfy[i].tfs0);
        scene_set_nrx_attr_interpulate(tfy[1].tfs1);

        /* tdz */
        scene_set_nrx_attr_interpulate(tfy[i].tdz0);
        scene_set_nrx_attr_interpulate(tfy[1].tdz1);

        /* tfr0 */
        for (hi_u32 j = 0; j < ARRAY_SIZE(nrx_attr->tfy[i].tfr0); j++) {
            scene_set_nrx_attr_interpulate(tfy[i].tfr0[j]);
        }

        /* tfr1 */
        for (hi_u32 j = 0; j < ARRAY_SIZE(nrx_attr->tfy[1].tfr1); j++) { /* tfr1 array size 6 */
            scene_set_nrx_attr_interpulate(tfy[1].tfr1[j]);
        }
    }
}

hi_void scene_set_3dnr_nrx_mdy(_3dnr_nrx_param param)
{
    ot_scene_3dnr *nrx_attr = param.nrx_attr;
    const ot_scene_static_3dnr *nrx_param = param.nrx_param;
    hi_u32 iso_level = param.iso_level;
    hi_u32 mid = param.mid;
    hi_u32 left = param.left;
    hi_u32 right = param.right;

    /* md_y */
    for (hi_u32 i = 0; i < ARRAY_SIZE(nrx_attr->mdy); i++) {
        /* id0 */
        scene_set_nrx_attr_interpulate(mdy[i].mai00);
        scene_set_nrx_attr_interpulate(mdy[i].mai02);

        /* math */
        scene_set_nrx_attr_interpulate(mdy[i].math0);
        scene_set_nrx_attr_interpulate(mdy[0].math1);

        /* mate */
        scene_set_nrx_attr_interpulate(mdy[i].mate0);
        scene_set_nrx_attr_interpulate(mdy[0].mate1);

        /* mabw */
        scene_set_nrx_attr_interpulate(mdy[i].mabw0);
        scene_set_nrx_attr_interpulate(mdy[0].mabw1);

        /* adv_math */
        scene_set_nrx_attr_interpulate(mdy[0].adv_math);

        /* adv_th */
        scene_set_nrx_attr_interpulate(mdy[0].adv_th);
    }
}

hi_void scene_set_3dnr_nrx_nrc0(_3dnr_nrx_param param)
{
    ot_scene_3dnr *nrx_attr = param.nrx_attr;
    const ot_scene_static_3dnr *nrx_param = param.nrx_param;
    hi_u32 iso_level = param.iso_level;
    hi_u32 mid = param.mid;
    hi_u32 left = param.left;
    hi_u32 right = param.right;

    /* nrc0_en */
    scene_set_nrx_attr_interpulate(nrc0_en);

    /* auto_math */
    scene_set_nrx_attr_interpulate(nrc0.auto_math);

    /* sfc */
    scene_set_nrx_attr_interpulate(nrc0.sfc);

    /* sfc */
    scene_set_nrx_attr_interpulate(nrc0.sfc_enhance);

    /* sfc_ext */
    scene_set_nrx_attr_interpulate(nrc0.sfc_ext);

    /* tfc */
    scene_set_nrx_attr_interpulate(nrc0.tfc);

    /* trc */
    scene_set_nrx_attr_interpulate(nrc0.trc);
}

static hi_void set_nrc_sfs_attr(_3dnr_nrx_param param)
{
    ot_scene_3dnr *nrx_attr = param.nrx_attr;
    const ot_scene_static_3dnr *nrx_param = param.nrx_param;
    hi_u32 iso_level = param.iso_level;
    hi_u32 mid = param.mid;
    hi_u32 left = param.left;
    hi_u32 right = param.right;

    /* sfs1 */
    scene_set_nrx_attr_interpulate(nrc1.sfs1);
    scene_set_nrx_attr_interpulate(nrc1.sbr1);

    /* sfs2 */
    scene_set_nrx_attr_interpulate(nrc1.sfs2);
    scene_set_nrx_attr_interpulate(nrc1.sft2);
    scene_set_nrx_attr_interpulate(nrc1.sbr2);

    /* sfs4 */
    scene_set_nrx_attr_interpulate(nrc1.sfs4);
    scene_set_nrx_attr_interpulate(nrc1.sft4);
    scene_set_nrx_attr_interpulate(nrc1.sbr4);

    /* sf5_str_u */
    scene_set_nrx_attr_interpulate(nrc1.sf5_str_u);

    /* sf5_str_u */
    scene_set_nrx_attr_interpulate(nrc1.sf5_str_v);

    /* sf6 */
    scene_set_nrx_attr_interpulate(nrc1.sfc6);
    scene_set_nrx_attr_interpulate(nrc1.sfc_ext6);

    /* sfr6_uv */
    scene_set_nrx_attr_interpulate(nrc1.sfr6_u);
    scene_set_nrx_attr_interpulate(nrc1.sfr6_v);
    /* sf7 */
    scene_set_nrx_attr_interpulate(nrc1.spn0);
    scene_set_nrx_attr_interpulate(nrc1.sbn0);
    scene_set_nrx_attr_interpulate(nrc1.pbr0);
    scene_set_nrx_attr_interpulate(nrc1.spn1);
    scene_set_nrx_attr_interpulate(nrc1.sbn1);
    scene_set_nrx_attr_interpulate(nrc1.pbr1);

    /* sf8 */
    scene_set_nrx_attr_interpulate(nrc1.sat0_l_sfn8);
    scene_set_nrx_attr_interpulate(nrc1.sat0_h_sfn8);
    scene_set_nrx_attr_interpulate(nrc1.sat1_l_sfn8);
    scene_set_nrx_attr_interpulate(nrc1.sat1_h_sfn8);

    /* sf9 */
    scene_set_nrx_attr_interpulate(nrc1.hue0_l_sfn9);
    scene_set_nrx_attr_interpulate(nrc1.hue0_h_sfn9);
    scene_set_nrx_attr_interpulate(nrc1.hue1_l_sfn9);
    scene_set_nrx_attr_interpulate(nrc1.hue1_h_sfn9);

    /* sf10 */
    scene_set_nrx_attr_interpulate(nrc1.bri0_l_sfn10);
    scene_set_nrx_attr_interpulate(nrc1.bri0_h_sfn10);
    scene_set_nrx_attr_interpulate(nrc1.bri1_l_sfn10);
    scene_set_nrx_attr_interpulate(nrc1.bri1_h_sfn10);
}

hi_void scene_set_3dnr_nrx_nrc1(_3dnr_nrx_param param)
{
    ot_scene_3dnr *nrx_attr = param.nrx_attr;
    const ot_scene_static_3dnr *nrx_param = param.nrx_param;
    hi_u32 iso_level = param.iso_level;
    hi_u32 mid = param.mid;
    hi_u32 left = param.left;
    hi_u32 right = param.right;

    /* nrc1_en */
    scene_set_nrx_attr_interpulate(nrc1_en);

    /* sf1 - sf10 */
    set_nrc_sfs_attr(param);

    /* sfn */
    scene_set_nrx_attr_interpulate(nrc1.sfn0);
    scene_set_nrx_attr_interpulate(nrc1.sfn1);

    /* post_sfc */
    scene_set_nrx_attr_interpulate(nrc1.post_sfc);

    for (hi_u32 i = 0; i < HI_SCENE_3DNR_IDX_LEN; i++) {
        scene_set_nrx_attr_interpulate(nrc1.bak_grd_sat[i]);
        scene_set_nrx_attr_interpulate(nrc1.bak_grd_hue[i]);
        scene_set_nrx_attr_interpulate(nrc1.bak_grd_bri[i]);
        scene_set_nrx_attr_interpulate(nrc1.for_grd_sat[i]);
        scene_set_nrx_attr_interpulate(nrc1.for_grd_hue[i]);
        scene_set_nrx_attr_interpulate(nrc1.for_grd_bri[i]);
        scene_set_nrx_attr_interpulate(sfy[2].bri_str[i]); /* 2 index */
        scene_set_nrx_attr_interpulate(sfy[3].bri_str[i]); /* 3 index */
    }
}

static hi_void scene_set_qp_h265_avbr_param(hi_venc_rc_param *rc_param, hi_u32 max_bitrate)
{
    if (max_bitrate <= 768) {                                   /* low than 768 */
        rc_param->h265_avbr_param.min_i_qp = 30;                /* min_i_qp is 30 */
        rc_param->h265_avbr_param.min_qp = 32;                  /* min_qp is 32 */
        rc_param->h265_avbr_param.max_i_qp = 48;                /* max_i_qp is 48 */
        rc_param->h265_avbr_param.max_qp = 48;                  /* max_qp is 48 */
    } else if ((max_bitrate > 768) && (max_bitrate <= 1536)) {  /* 768 to 1536 */
        rc_param->h265_avbr_param.min_i_qp = 29;                /* min_i_qp is 29 */
        rc_param->h265_avbr_param.min_qp = 31;                  /* min_qp is 31 */
        rc_param->h265_avbr_param.max_i_qp = 44;                /* x is 44 */
        rc_param->h265_avbr_param.max_qp = 44;                  /* max_qp is 44 */
    } else if ((max_bitrate > 1536) && (max_bitrate <= 2560)) { /* 1536 to 2560 */
        rc_param->h265_avbr_param.min_i_qp = 26;                /* min_i_qp is 26 */
        rc_param->h265_avbr_param.min_qp = 28;                  /* min_qp is 28 */
        rc_param->h265_avbr_param.max_i_qp = 42;                /* max_i_qp is 42 */
        rc_param->h265_avbr_param.max_qp = 42;                  /* max_qp is 42 */
    } else if ((max_bitrate > 2560) && (max_bitrate <= 3584)) { /* 2560 to 3584 */
        rc_param->h265_avbr_param.min_i_qp = 25;                /* min_i_qp is 25 */
        rc_param->h265_avbr_param.min_qp = 27;                  /* min_qp is 27 */
        rc_param->h265_avbr_param.max_i_qp = 42;                /* max_i_qp is 42 */
        rc_param->h265_avbr_param.max_qp = 42;                  /* max_qp is 42 */
    } else if ((max_bitrate > 3584) && (max_bitrate <= 4096)) { /* 3584 to 4096 */
        rc_param->h265_avbr_param.min_i_qp = 24;                /* min_i_qp is 24 */
        rc_param->h265_avbr_param.min_qp = 26;                  /* min_qp is 26 */
        rc_param->h265_avbr_param.max_i_qp = 42;                /* max_i_qp is 42 */
        rc_param->h265_avbr_param.max_qp = 42;                  /* max_qp is 42 */
    } else if (max_bitrate > 4096) {                            /* bigger than 4096 */
        rc_param->h265_avbr_param.min_i_qp = 23;                /* min_i_qp is 23 */
        rc_param->h265_avbr_param.min_qp = 25;                  /* min_qp is 25 */
        rc_param->h265_avbr_param.max_i_qp = 42;                /* max_i_qp is 42 */
        rc_param->h265_avbr_param.max_qp = 42;                  /* max_qp is 42 */
    }
    rc_param->h265_avbr_param.min_i_proportion = 1;
    rc_param->h265_avbr_param.max_i_proportion = 100; /* max_i_proportion is 100 */
    rc_param->h265_avbr_param.max_reencode_times = 2; /* max_reencode_times is 2 */

    rc_param->h265_avbr_param.min_still_percent = 25; /* min_still_percent is 25 */
    rc_param->h265_avbr_param.max_still_qp = 35;      /* max_still_qp is 35 */
    rc_param->h265_avbr_param.min_still_psnr = 0;
}

static hi_void scene_set_qp_h264_avbr_param(hi_venc_rc_param *rc_param, hi_u32 max_bitrate)
{
    if (max_bitrate <= 768) {                                   /* low than 768 */
        rc_param->h264_avbr_param.min_i_qp = 30;                /* min_i_qp is 30 */
        rc_param->h264_avbr_param.min_qp = 32;                  /* min_qp is 32 */
        rc_param->h264_avbr_param.max_i_qp = 48;                /* max_i_qp is 48 */
        rc_param->h264_avbr_param.max_qp = 48;                  /* max_qp is 48 */
    } else if ((max_bitrate > 768) && (max_bitrate <= 1536)) {  /* 768 to 1536 */
        rc_param->h264_avbr_param.min_i_qp = 29;                /* min_i_qp is 29 */
        rc_param->h264_avbr_param.min_qp = 31;                  /* min_qp is 31 */
        rc_param->h264_avbr_param.max_i_qp = 44;                /* max_i_qp is 44 */
        rc_param->h264_avbr_param.max_qp = 44;                  /* max_qp is 44 */
    } else if ((max_bitrate > 1536) && (max_bitrate <= 3072)) { /* 1536 to 3072 */
        rc_param->h264_avbr_param.min_i_qp = 27;                /* min_i_qp is 27 */
        rc_param->h264_avbr_param.min_qp = 29;                  /* min_qp is 29 */
        rc_param->h264_avbr_param.max_i_qp = 42;                /* max_i_qp is 42 */
        rc_param->h264_avbr_param.max_qp = 42;                  /* max_qp is 42 */
    } else if ((max_bitrate > 3072) && (max_bitrate <= 5120)) { /* 3072 to 5120 */
        rc_param->h264_avbr_param.min_i_qp = 26;                /* min_i_qp is 26 */
        rc_param->h264_avbr_param.min_qp = 28;                  /* min_qp is 28 */
        rc_param->h264_avbr_param.max_i_qp = 42;                /* max_i_qp is 42 */
        rc_param->h264_avbr_param.max_qp = 42;                  /* max_qp is 42 */
    } else if ((max_bitrate > 5120) && (max_bitrate <= 7168)) { /* 5120 to 7168 */
        rc_param->h264_avbr_param.min_i_qp = 25;                /* min_i_qp is 25 */
        rc_param->h264_avbr_param.min_qp = 27;                  /* min_qp is 27 */
        rc_param->h264_avbr_param.max_i_qp = 42;                /* max_i_qp is 42 */
        rc_param->h264_avbr_param.max_qp = 42;                  /* max_qp is 42 */
    } else if (max_bitrate > 7168) {                            /* bigger than 7168 */
        rc_param->h264_avbr_param.min_i_qp = 23;                /* min_i_qp is 23 */
        rc_param->h264_avbr_param.min_qp = 25;                  /* min_qp is 25 */
        rc_param->h264_avbr_param.max_i_qp = 42;                /* max_i_qp is 42 */
        rc_param->h264_avbr_param.max_qp = 42;                  /* max_qp is 42 */
    }
    rc_param->h264_avbr_param.min_i_proportion = 1;
    rc_param->h264_avbr_param.max_i_proportion = 100; /* max_qp is 100 */
    rc_param->h264_avbr_param.max_reencode_times = 2; /* max_qp is 2 */

    rc_param->h264_avbr_param.min_still_percent = 25; /* max_qp is 25 */
    rc_param->h264_avbr_param.max_still_qp = 35;      /* max_qp is 35 */
    rc_param->h264_avbr_param.min_still_psnr = 0;
}

hi_s32 scene_set_qp(hi_u32 pic_width, hi_u32 pic_height, hi_u32 max_bitrate, hi_payload_type type,
    hi_venc_rc_param *rc_param)
{
    ot_scenecomm_check_pointer_return(rc_param, HI_FAILURE);
    hi_u32 align_pic_height;
    hi_u32 align_pic_width;
    hi_u32 align_pic_height_1080p;
    hi_u32 align_pic_width_1080p;

    align_pic_height = ((pic_height + MB_ALIGN_H264 - 1) / MB_ALIGN_H264);
    align_pic_width = ((pic_width + MB_ALIGN_H264 - 1) / MB_ALIGN_H264);
    align_pic_height_1080p = ((PIC_HEIGHT_1080P + MB_ALIGN_H264 - 1) / MB_ALIGN_H264);
    align_pic_width_1080p = ((PIC_WIDTH_1080P + MB_ALIGN_H264 - 1) / MB_ALIGN_H264);
    max_bitrate = (align_pic_height_1080p * align_pic_width_1080p * max_bitrate) / (align_pic_height * align_pic_width);

    if (type == HI_PT_H265) {
        scene_set_qp_h265_avbr_param(rc_param, max_bitrate);
    }
    if (type == HI_PT_H264) {
        scene_set_qp_h264_avbr_param(rc_param, max_bitrate);
    }
    return HI_SUCCESS;
}

hi_s32 get_iso_mean_qp_chn_attr(hi_vi_pipe vi_pipe, hi_u32 *iso, hi_u32 *mean_qp, hi_venc_chn_attr *venc_chn_attr)
{
    hi_s32 ret;
    hi_isp_exp_info isp_exp_info;
    hi_venc_chn_status status;

    ret = hi_mpi_venc_get_chn_attr(vi_pipe, venc_chn_attr);
    check_scene_ret(ret);

    ret = hi_mpi_isp_query_exposure_info(vi_pipe, &isp_exp_info);
    check_scene_ret(ret);

    *iso = isp_exp_info.iso;

    ret = hi_mpi_venc_query_status(vi_pipe, &status);
    check_scene_ret(ret);

    *mean_qp = status.stream_info.mean_qp;

    return HI_SUCCESS;
}


hi_void calculate_manual_percent_index(hi_u8 pipe_param_index, hi_u32 sum_iso, hi_u32 *out_index)
{
    hi_u32 i;
    for (i = 0; i < g_scene_pipe_param[pipe_param_index].dynamic_venc_bitrate.iso_thresh_cnt; i++) {
        if (sum_iso > g_scene_pipe_param[pipe_param_index].dynamic_venc_bitrate.iso_thresh_ltoh[i]) {
            (*out_index)++;
        }
    }
}

hi_void set_initial_percent(hi_payload_type type, hi_u32 index, hi_venc_rc_param *rc_param,
    const ot_scene_pipe_param *param, hi_s32 *percent)
{
    if (type == HI_PT_H264) {
        rc_param->h264_avbr_param.chg_pos = param->dynamic_venc_bitrate.manual_percent[index - 1];
        *percent = rc_param->h264_avbr_param.chg_pos;
    }
    if (type == HI_PT_H265) {
        rc_param->h265_avbr_param.chg_pos = param->dynamic_venc_bitrate.manual_percent[index - 1];
        *percent = rc_param->h265_avbr_param.chg_pos;
    }
}

hi_void set_min_qp_delta_when_iso_larger(hi_payload_type type, hi_u32 sum_mean_qp, hi_s32 percent,
    hi_venc_rc_param *rc_param)
{
    if (sum_mean_qp <= 33 && sum_mean_qp >= 31) {        /* 31 to 33 */
        percent = 19 * percent / 20;                     /* 19 multiplier, 20 is divider */
    } else if (sum_mean_qp <= 30 && sum_mean_qp >= 29) { /* 29 to 30 */
        percent = 9 * percent / 10;                      /* 9 multiplier, 10 is divider */
    } else if (sum_mean_qp < 29) {                       /* low than 29 */
        percent = 8 * percent / 10;                      /* 8 multiplier, 10 is divider */
    }

    if (type == HI_PT_H264) {
        rc_param->h264_avbr_param.chg_pos = percent;
        rc_param->h264_avbr_param.min_qp_delta = 2; /* min_qp_delta is 2 */
    }
    if (type == HI_PT_H265) {
        rc_param->h265_avbr_param.chg_pos = percent;
        rc_param->h265_avbr_param.min_qp_delta = 2; /* min_qp_delta is 2 */
    }
}

hi_void set_min_qp_delta_when_iso_less(hi_payload_type type, hi_u32 sum_mean_qp, hi_venc_rc_param *rc_param,
    hi_venc_chn_attr *venc_chn_attr)
{
    if (venc_chn_attr->gop_attr.gop_mode == HI_VENC_GOP_MODE_SMART_P) {
        if (type == HI_PT_H264) {
            rc_param->h264_avbr_param.min_qp_delta = 3; /* min_qp_delta is 3 */
        }
        if (type == HI_PT_H265) {
            rc_param->h265_avbr_param.min_qp_delta = 3; /* min_qp_delta is 3 */
        }
    } else {
        if (type == HI_PT_H264) {
            rc_param->h264_avbr_param.min_qp_delta = 3; /* min_qp_delta is 3 */
        }
        if (type == HI_PT_H265) {
            rc_param->h265_avbr_param.min_qp_delta = 3; /* min_qp_delta is 3 */
        }
    }
}

#ifdef __cplusplus
}
#endif

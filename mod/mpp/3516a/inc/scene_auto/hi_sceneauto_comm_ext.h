#ifndef __HI_SCENEAUTO_COMM_EXT_H__
#define __HI_SCENEAUTO_COMM_EXT_H__

#include "hi_type.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

HI_S32 CommSceneautoGetDemosaic(HI_S32 s32IspDev, ADPT_SCENEAUTO_DEMOSAIC_S* pstAdptDemosaic);
HI_S32 CommSceneautoSetDemosaic(HI_S32 s32IspDev, const ADPT_SCENEAUTO_DEMOSAIC_S* pstAdptDemosaic);
HI_S32 CommSceneautoGetSharpen(HI_S32 s32IspDev, ADPT_SCENEAUTO_SHARPEN_S* pstAdptSharpen);
HI_S32 CommSceneautoSetSharpen(HI_S32 s32IspDev, const ADPT_SCENEAUTO_SHARPEN_S* pstAdptSharpen);
HI_S32 CommSceneautoGetDP(HI_S32 s32IspDev, ADPT_SCENEAUTO_DP_S* pstAdptDP);
HI_S32 CommSceneautoSetDP(HI_S32 s32IspDev, const ADPT_SCENEAUTO_DP_S* pstAdptDP);
HI_S32 CommSceneautoGetPubAttr(HI_S32 s32IspDev, ADPT_SCENEAUTO_PUBATTR_S* pstAdptPubAttr);
HI_S32 CommSceneautoGetGamma(HI_S32 s32IspDev, ADPT_SCENEAUTO_GAMMA_S* pstAdptGamma);
HI_S32 CommSceneautoSetGamma(HI_S32 s32IspDev, const ADPT_SCENEAUTO_GAMMA_S* pstAdptGamma);
HI_S32 CommSceneautoGetH264Trans(HI_S32 s32VencChn, ADPT_SCENEAUTO_H264TRANS_S* pstAdptH264Trans);
HI_S32 CommSceneautoSetH264Trans(HI_S32 s32VencChn, const ADPT_SCENEAUTO_H264TRANS_S* pstAdptH264Trans);
HI_S32 CommSceneautoGetH24Deblock(HI_S32 s32VencChn, ADPT_SCENEAUTO_H264_DEBLOCK_S* pstAdptH264Deblock);
HI_S32 CommSceneautoSetH24Deblock(HI_S32 s32VencChn, const ADPT_SCENEAUTO_H264_DEBLOCK_S* pstAdptH264Deblock);
HI_S32 CommSceneautoGetH264RcParam(HI_S32 s32VencChn, ADPT_SCENEAUTO_H264_RCPARAM_S* pstAdptRCParam);
HI_S32 CommSceneautoSetH264RcParam(HI_S32 s32VencChn, const ADPT_SCENEAUTO_H264_RCPARAM_S* pstAdptRCParam);
HI_S32 CommSceneautoGetAERoute(HI_S32 s32IspDev, ADPT_SCENEAUTO_AEROUTE_S* pstAdptAERoute);
HI_S32 CommSceneautoSetAERoute(HI_S32 s32IspDev, const ADPT_SCENEAUTO_AEROUTE_S* pstAdptAERoute);
HI_S32 CommSceneautoGetDCIParam(HI_S32 s32ViDev, ADPT_SCENEAUTO_DCIPARAM_S* pstAdptDCIPara);
HI_S32 CommSceneautoSetDCIParam(HI_S32 s32ViDev, const ADPT_SCENEAUTO_DCIPARAM_S* pstAdptDCIPara);
HI_S32 CommSceneautoGetDRCAttr(HI_S32 s32IspDev, ADPT_SCENEAUTO_DRCATTR_S* pstAdptDRCAttr);
HI_S32 CommSceneautoSetDRCAttr(HI_S32 s32IspDev, const ADPT_SCENEAUTO_DRCATTR_S* pstAdptDRCAttr);
HI_S32 CommSceneautoGetSaturation(HI_S32 s32IspDev, ADPT_SCENEAUTO_SATURATION_S* pstAdptSaturation);
HI_S32 CommSceneautoSetSaturation(HI_S32 s32IspDev, const ADPT_SCENEAUTO_SATURATION_S* pstAdptSaturation);
HI_S32 CommSceneautoGetDISAttr(HI_S32 s32IspDev, ADPT_SCENEAUTO_DIS_ATTR_S* pstAdptDisAttr);
HI_S32 CommSceneautoSetDISAttr(HI_S32 s32IspDev, const ADPT_SCENEAUTO_DIS_ATTR_S* pstAdptDisAttr);
HI_S32 CommSceneautoGetAEAttr(HI_S32 s32IspDev, ADPT_SCENEAUTO_AEATTR_S* pstAdptAEAttr);
HI_S32 CommSceneautoSetAEAttr(HI_S32 s32IspDev, const ADPT_SCENEAUTO_AEATTR_S* pstAdptAEAttr);
HI_S32 CommSceneautoGetExposureInfo(HI_S32 s32IspDev, ADPT_SCENEAUTO_EXPOSUREINFO_S* pstAdptExposureInfo);
HI_S32 CommSceneautoGetWDRAttr(HI_S32 s32IspDev, ADPT_SCENEAUTO_WDRATTR_S* pstAdptWDRAttr);
HI_S32 CommSceneautoGetQueryInnerStateInfo(HI_S32 s32IspDev, ADPT_SCENEAUTO_STATEINFO_S* pstAdptStatInfo);
HI_S32 CommSceneautoGetBitrate(HI_S32 s32VencChn, HI_U32* pu32Bitrate);
HI_S32 CommSceneautoGetDefogAttr(HI_S32 s32IspDev, ADPT_SCENEAUTO_DEFOG_ATTR_S* pstAdptDefogAttr);
HI_S32 CommSceneautoSet3DNR(HI_S32 s32VpssGrp, ADPT_SCENEAUTO_3DNR *pstAdptSpecial3dnr);
HI_S32 CommSceneautoSetFPNAttr(HI_S32 s32IspDev, const ADPT_SCENEAUTO_FPN_ATTR_S* pstAdptFpnAttr);
HI_S32 CommSceneautoGetVencAttr(HI_S32 s32VencChn, ADPT_SCENEAUTO_VENC_ATTR_S* pstAdptVencAttr);
HI_S32 CommSceneautoGetH265FaceCfg(HI_S32 s32VencChn, ADPT_SCENEAUTO_H265_FACE_CFG_S* pstAdptH265FaceCfg);
HI_S32 CommSceneautoSetH265FaceCfg(HI_S32 s32VencChn, const ADPT_SCENEAUTO_H265_FACE_CFG_S* pstAdptH265FaceCfg);
HI_S32 CommSceneautoGetH265RcParam(HI_S32 s32VencChn, ADPT_SCENEAUTO_H265_RCPARAM_S* pstAdptH265RCParam);
HI_S32 CommSceneautoSetH265RcParam(HI_S32 s32VencChn, const ADPT_SCENEAUTO_H265_RCPARAM_S* pstAdptH265RCParam);
HI_S32 CommSceneautoGetCcmAttr(HI_S32 s32IspDev, ADPT_SCENEAUTO_CCM_ATTR_S* pstAdptCcmAttr);
HI_S32 CommSceneautoSetCcmAttr(HI_S32 s32IspDev, const ADPT_SCENEAUTO_CCM_ATTR_S* pstAdptCcmAttr);
HI_S32 CommSceneautoGetAcmAttr(HI_S32 s32IspDev, ADPT_SCENEAUTO_ACM_ATTR_S* pstAdptAcmAttr);
HI_S32 CommSceneautoSetAcmAttr(HI_S32 s32IspDev, const ADPT_SCENEAUTO_ACM_ATTR_S* pstAdptAcmAttr);
HI_S32 CommSceneautoSetRegister(HI_S32 s32IspDev, HI_U32 u32Addr, HI_U32 u32Vaule);
HI_S32 CommSceneautoIVEStart(HI_S32 s32VpssGrp, HI_S32 s32VpssChn);
HI_S32 CommSceneautoIVEStop();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
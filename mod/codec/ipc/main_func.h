#ifndef __main_func_h__
#define __main_func_h__

extern int dzoom_plus;

int vo_res_get(gsf_resolu_t *res);
int vo_ly_get(gsf_layout_t *ly);
int vo_ly_set(int ly);
int venc_fixed_sdp(int ch, int i, gsf_sdp_t *sdp);
int venc_start(int start);

int main_start(gsf_bsp_def_t *bsp_def);
int main_loop(void);


#endif //__main_func_h__

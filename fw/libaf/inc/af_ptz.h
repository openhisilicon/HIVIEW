
#ifndef __AF_PTZ_H__
#define __AF_PTZ_H__

typedef enum
{
	AF_PTZ_CMD_ZOOM_WIDE       = 0x01,
	AF_PTZ_CMD_ZOOM_TELE       = 0x02,
	AF_PTZ_CMD_ZOOM_STOP       = 0x03,
	AF_PTZ_CMD_FOCUS_FAR       = 0x04,
	AF_PTZ_CMD_FOCUS_NEAR      = 0x05,
	AF_PTZ_CMD_FOCUS_STOP      = 0x06,  
	AF_PTZ_CMD_ONEFOCUS        = 0x07,
	AF_PTZ_CMD_LENS_INIT       = 0x08,
	AF_PTZ_CMD_LENS_CORRECT    = 0x09,
	
  AF_PTZ_CMD_STOP            = 0x0A,
  AF_PTZ_CMD_MOVE            = 0x0B,
  
	AF_PTZ_CMD_END,
} AF_PTZ_CMD_E;

typedef enum
{
	AF_PTZ_ERR_OK         = 0,	
	AF_PTZ_ERR_ZOOMLIMIT  = 1,
	AF_PTZ_ERR_FOCUSLIMIT = 2,
} AF_PTZ_ERR_E;

typedef struct {
  int (*get_vd)(void);
  int (*get_value)(unsigned int fv[2]);
}af_ptz_fv_t;


typedef struct {
  int (*uart_write)(char *buf, int size);
  int (*zoom_value)(int z);
}af_ptz_cb_t;

int af_ptz_init(af_ptz_fv_t *fv_cb, af_ptz_cb_t *ptz_cb);
int af_ptz_uninit(void);

int af_ptz_ctl(char *buf, int size);

#endif /* __AF_PTZ_H__ */


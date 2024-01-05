#include "mod/codec/inc/codec.h"

//<PTZCmd>A50F4D1000001021</PTZCmd>

int ptzcmd_str2hex(char *ptz_str, unsigned char ptz_cmd[8])
{
	int i = 0;

	if (ptz_str == NULL || ptz_cmd == NULL)
		return -1;

	for (i = 0; i < 16; i++)
	{
		if (ptz_str[i] >= '0' && ptz_str[i] <= '9')
			ptz_cmd[i/2] = (ptz_cmd[i/2] * 0x10) + (ptz_str[i] - '0');
		else if (ptz_str[i] >= 'A' && ptz_str[i] <= 'F')
			ptz_cmd[i/2] = (ptz_cmd[i/2] * 0x10) + (ptz_str[i] - 'A' + 10);
		else if (ptz_str[i] >= 'a' && ptz_str[i] <= 'f')
			ptz_cmd[i/2] = (ptz_cmd[i/2] * 0x10) + (ptz_str[i] - 'a' + 10);
	}
	return 0;
}

int ptzcmd_ctl_ptz(int ch, unsigned char ptz_cmd[8])
{
  int ret = 0;

  GSF_MSG_DEF(gsf_lens_t, lens, 2*1024);

	// stop
	if (ptz_cmd[3] == 0x00)
	{
		// PTZ_CMD_STOP;
    lens->cmd = GSF_LENS_STOP;
    GSF_MSG_SENDTO(GSF_ID_CODEC_LENS, 0, GET, 0
                      , sizeof(gsf_lens_t)
                      , GSF_IPC_CODEC, 2000);
		return ret;
	}
	else
	{
	  // zoom
		switch(ptz_cmd[3] & 0x30)
		{
			case 0x10:
				//PTZ_CMD_ZOOM_TELE;
				//ptz_cmd[6]
        lens->cmd = GSF_LENS_ZOOM;
        lens->arg1 = 1; //++
        lens->arg2 = 0;
        GSF_MSG_SENDTO(GSF_ID_CODEC_LENS, 0, GET, 0
                          , sizeof(gsf_lens_t)
                          , GSF_IPC_CODEC, 2000);
				break;
			case 0x20:
				//PTZ_CMD_ZOOM_WIDE;
				//ptz_cmd[6]
        lens->cmd = GSF_LENS_ZOOM;
        lens->arg1 = 0; //--
        lens->arg2 = 0;
        GSF_MSG_SENDTO(GSF_ID_CODEC_LENS, 0, GET, 0
                          , sizeof(gsf_lens_t)
                          , GSF_IPC_CODEC, 2000);
				break;
			default:
				break;
		}
		// move
		switch(ptz_cmd[3] & 0x0F)
		{
			case 0x1:             //右
				//PTZ_CMD_RIGHT;
        //ptz_cmd[4]
				break;
			case 0x2:             //左
				//PTZ_CMD_LEFT;
				//ptz_cmd[4]
				break;
			case 0x08:						//上
				//PTZ_CMD_UP;
				//ptz_cmd[5]
				break;
			case 0x04:						//下
				//PTZ_CMD_DOWN;
				//ptz_cmd[5]
				break;
			case 0x05:						//右下
				//PTZ_CMD_RIGHT_DOWN;
				//ptz_cmd[4]
				//ptz_cmd[5]
				break;
			case 0x06:						//左下
				//PTZ_CMD_LEFT_DOWN;
        //ptz_cmd[4]
				//ptz_cmd[5]
				break;
			case 0x09:						//右上
				//PTZ_CMD_RIGHT_UP;
        //ptz_cmd[4]
				//ptz_cmd[5]
				break;
			case 0x0A:						//左上
				//PTZ_CMD_LEFT_UP;
        //ptz_cmd[4]
				//ptz_cmd[5]
				break;
			default:
				break;
				
		}
	}
	
	return ret;
}

int ptzcmd_ctl_fi(int ch, unsigned char ptz_cmd[8])
{
  int ret = 0;

  GSF_MSG_DEF(gsf_lens_t, lens, 2*1024);

	if (ptz_cmd[3] == 0x40)
	{
		//PTZ_CMD_STOP;
    lens->cmd = GSF_LENS_STOP;
    GSF_MSG_SENDTO(GSF_ID_CODEC_LENS, 0, GET, 0
                      , sizeof(gsf_lens_t)
                      , GSF_IPC_CODEC, 2000);
		return ret;
	}
	
	/* 镜头聚焦 */
	if (ptz_cmd[3]&0x01)
	{
		//PTZ_CMD_FOCUS_FAR;
		//ptz_cmd[4]
 	} 
	else if (ptz_cmd[3]&0x02)
	{
		//PTZ_CMD_FOCUS_NEAR;
		//ptz_cmd[4]
	}

	/* 光圈缩放 */
	if (ptz_cmd[3]&0x04)
	{
		//PTZ_CMD_IRIS_OPEN;
		//ptz_cmd[5]
	}
	else if (ptz_cmd[3]&0x08)
	{
		//PTZ_CMD_IRIS_CLOSE;
		//ptz_cmd[5]
	}
	
  return ret;
}

int ptzcmd_ctl_preset(int ch, unsigned char ptz_cmd[8])
{
	int ret = 0;
	
	switch(ptz_cmd[3])
	{
		/* 设置预置点 */
		case 0x81:
			//PTZ_CMD_SET_PRESET;
			//ptz_cmd[5];
			break;

		/* 调用预置点 */
		case 0x82:
			//PTZ_CMD_GOTO_PRESET;
			//ptz_cmd[5];
			break;

		/* 删除预置点 */
		case 0x83:
			//PTZ_CMD_CLR_PRESET;
			//ptz_cmd[5];
			break;

		/* 自动扫描 */
		case 0x89:
			//PTZ_CMD_AUTOSCAN;
			break;
			
		default:
			return -1;
	}
	
	//sendmsg;
	return ret;
}



int ptzcmd_ctl(int ch, char *ptz_str)
{
  int ret = 0;
  
  unsigned char	ptz_cmd[8] = {0};
  if(ptzcmd_str2hex(ptz_str, ptz_cmd) < 0)
  {
    return ret;
  }
  
  switch(ptz_cmd[3]&0xC0)
	{
		case 0x00:
			ret = ptzcmd_ctl_ptz(ch, ptz_cmd);
			break;
		case 0x40:
			ptzcmd_ctl_fi(ch, ptz_cmd);
			break;
		case 0x80:
			ptzcmd_ctl_preset(ch, ptz_cmd);
	}
	return ret;
}

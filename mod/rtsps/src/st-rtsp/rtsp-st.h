#ifndef _rtsp_st_h_
#define _rtsp_st_h_

struct rtsp_st_ini_t
{
  unsigned short server_port;
};
int rtsp_st_init();

enum {
  // server;
  RTSP_CTL_S_START = 0, // start ip, port;
  RTSP_CTL_S_STOP,      // stop;
  RTSP_CTL_S_LIST,      // list connect;
  // client;
  RTSP_CTL_C_OPEN,      // open url;
  RTSP_CTL_C_CLOSE,     // close url;
  RTSP_CTL_C_LIST,      // list connect;
  // push;
  RTSP_CTL_P_OPEN,      // open url;
  RTSP_CTL_P_CLOSE,     // close url;
  RTSP_CTL_P_LIST,      // list connect;
};

struct rtsp_st_ctl_t
{
  int id; //RTSP_CTL_ 
  int size;
  unsigned char data[0];
};

int rtsp_st_ctl(struct rtsp_st_ctl_t *ctl);


#endif //!_rtsp_st_h_
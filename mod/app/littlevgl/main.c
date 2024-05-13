#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/mouse_hid.h"
#include "lv_examples/lv_apps/demo/demo.h"

#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/select.h>
#include <errno.h>

#include "mod/svp/inc/svp.h"
#include "mod/app/inc/app.h"
#include "mod/codec/inc/codec.h"
#include "mod/rec/inc/rec.h"

#define __UI_LINES__  0
#define __UI_IOU__    0

#define __UI_LOGO__   1

#define __UI_MOUSE__  0
#define __UI_ZOOM__   0
#define __UI_STAT__   0




#define DISP_BUF_SIZE (80*LV_HOR_RES_MAX/*1280*/)

static pthread_t tid;
static int lv_running = 0, lv_w = 1280, lv_h = 1024, lt = 1;
static void* lvgl_main(void* p);
static int msq = -1;
static int lv_push_osd = 0;

int lvgl_stop(void)
{
  lv_running = 0;
  return pthread_join(tid, NULL);
}

extern char home_path[128];

int lvgl_start(int w, int h, int push_osd)
{
  lv_w = w;
  lv_h = h;
  lv_push_osd = push_osd;
  lv_running = 1;
  return pthread_create(&tid, NULL, lvgl_main, NULL);
}

int lvgl_ctl(int cmd, void* args)
{
  lt = (int)args;
  return 0;
}


typedef struct msgbuf
{
    long mtype;    /* type of received/sent message */
    char mtext[0]; /* text of the message */
}msgbuf;

static int sub_recv(char *msg, int size, int err)
{
  gsf_msg_t *pmsg = (gsf_msg_t*)msg;
  if(pmsg->id == GSF_EV_SVP_YOLO)
  {
    int len = pmsg->size;
    msgbuf *mbuf = (msgbuf*)(pmsg->data - sizeof(msgbuf));
    mbuf->mtype = 1 + pmsg->ch;
    msgsnd(msq, mbuf, len, IPC_NOWAIT);
  }
  return 0;
}

//send act to GSF_IPC_OSD;
static void fb_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p)
{
  static int osd_push = 0;
  static char* osd_buf = NULL;
  static struct fb_var_info var = {0};
  
  fbdev_var(&var);
  
  if(sizeof(lv_color_t) != 2
     || var.xres > 1920 || var.bits_per_pixel != 16)
  {
    //warn("lv_color_t:%d, bits_per_pixel:%d, xres:%d\n", sizeof(lv_color_t), var.xres, var.bits_per_pixel);
    return fbdev_flush(drv, area, color_p);
  }  

  
  if(!osd_push)
  {
    osd_push = nm_push_conn(GSF_IPC_OSD);
    if(!osd_buf)
    {
      osd_buf = malloc(sizeof(gsf_osd_act_t) + DISP_BUF_SIZE*2);
    }
    usleep(100*1000);
    //printf("nm_push_conn osd_push:%d\n", osd_push);
  }
  if(osd_push && osd_buf)
  {
    gsf_osd_act_t *act = (gsf_osd_act_t*)osd_buf;
    
    act->pixel = 16;
    act->x = act->y = 0;
    act->w = var.xres;
    act->h = var.yres;
    act->x1 = area->x1 < 0 ? 0 : area->x1;
    act->y1 = area->y1 < 0 ? 0 : area->y1;
    act->x2 = area->x2 > ((int32_t)var.xres - 1) ? (int32_t)var.xres - 1 : area->x2;
    act->y2 = area->y2 > ((int32_t)var.yres - 1) ? (int32_t)var.yres - 1 : area->y2;
      
    int act_size = (act->x2-act->x1+1)*(act->y2-act->y1+1)*2;
    memcpy(act->data, color_p, act_size);
    nm_push_send_wait(osd_push, (char*)act, sizeof(gsf_osd_act_t)+act_size);
    //printf("nm_push_send_wait act_size:%d\n", act_size);
  }
  return fbdev_flush(drv, area, color_p);
}


static lv_style_t note_s;
static lv_obj_t *note_l = NULL; // ref by snap_event_cb

#if __UI_STAT__

static int stat_draw(lv_coord_t hres, lv_coord_t vres)
{
    static lv_obj_t *wifi_l = NULL;
    static lv_obj_t *eth_l = NULL;

    static struct timeval last_time;
    
    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    
    if(tv_now.tv_sec - last_time.tv_sec < 3)
      return -1;
    last_time = tv_now;
     
    if(!wifi_l)
    {
      wifi_l = lv_label_create(lv_scr_act(), NULL);
      lv_obj_set_x(wifi_l, hres-230);
      lv_obj_set_y(wifi_l, vres-60);
    }
    if(!eth_l)
    {
      eth_l = lv_label_create(lv_scr_act(), NULL);
      lv_obj_set_x(eth_l, hres-230);
      lv_obj_set_y(eth_l, vres-30);
      
    }
    if(!note_l)
    {
      lv_style_copy(&note_s, &lv_style_plain);
      note_s.text.color = LV_COLOR_RED;
      note_s.text.font  = &lv_font_roboto_28;

      note_l = lv_label_create(lv_scr_act(), NULL);
      lv_obj_set_style(note_l, &note_s);
      lv_obj_set_x(note_l, 320);
      lv_obj_set_y(note_l, vres-60);
    }
    lv_label_set_text(note_l, "");

    char lable_str[256];
    int wifi_en = 0, wifi_ap = 0;
    {
      GSF_MSG_DEF(gsf_wifi_t, wifi, 4*1024);
      GSF_MSG_SENDTO(GSF_ID_BSP_WIFI, 0, GET, 0
                        , sizeof(gsf_wifi_t)
                        , GSF_IPC_BSP, 2000);                
      
      wifi_en = wifi->en;
      wifi_ap = wifi->ap;
    }
    
    {
      GSF_MSG_DEF(gsf_eth_t, eth, 4*1024);  
      GSF_MSG_SENDTO(GSF_ID_BSP_ETH, 0, GET, 0
                        , sizeof(gsf_eth_t)
                        , GSF_IPC_BSP, 2000);
                        
      sprintf(lable_str, LV_SYMBOL_WIFI " [%s][%s][%s]", (!wifi_en)?"CLOSE":(wifi_ap)?"AP":"STA"
                                   , (eth->dhcp)?"AUTO":"MANU"
                                   , (!wifi_en)?"":(wifi_ap)?"192.168.1.2":eth->ipaddr);
      lv_label_set_text(wifi_l, lable_str);

      sprintf(lable_str, LV_SYMBOL_SHUFFLE " [%s][%s][%s]", "ETH"
                                   , (eth->dhcp)?"AUTO":"MANU"
                                   , (wifi_en && !wifi_ap)?"192.168.1.2":eth->ipaddr);
      lv_label_set_text(eth_l, lable_str);
    }
}
#endif

#if __UI_IOU__

static int lines_iou(lv_coord_t hres, lv_coord_t vres)
{
  int i = 0, j = 0;
  
  extern gsf_app_nvr_t app_nvr;
  static gsf_polygons_t lines = {0};
    
  if(!memcmp(&lines, &app_nvr.lines, sizeof(gsf_polygons_t)))
  {  
    return -1;
  }
  lines = app_nvr.lines;
  
  #define IOU_LINES_NUM 1
  
  //init;
  static lv_obj_t* lines_hdl[IOU_LINES_NUM] = {NULL};
  static lv_style_t style_line[IOU_LINES_NUM];
  static lv_point_t line_points[IOU_LINES_NUM][16] = {0};
  if(lines_hdl[0] == NULL)
  {
    for(i = 0; i < (IOU_LINES_NUM); i++)
    {
      lines_hdl[i] = lv_line_create(lv_scr_act(), NULL);
      lv_style_copy(&style_line[i], &lv_style_plain);
      style_line[i].line.color = LV_COLOR_CYAN;
      style_line[i].line.width = 4;
      style_line[i].line.rounded = 0;
    }
  }
  
  //clear;
  for(i = 0; i < (IOU_LINES_NUM); i++)
  {
    lv_obj_set_style(lines_hdl[i], &style_line[i]);
    lv_line_set_points(lines_hdl[i], line_points[i], 0);
  }
  
  //draw;
  for(i = 0; i < lines.polygon_num && i < IOU_LINES_NUM; i++)
  {
    if(lines.polygons[i].point_num == 0)
      continue;

    style_line[i].line.color = LV_COLOR_LIME;
      
    for(j = 0; j < lines.polygons[i].point_num; j++)
    {
      line_points[i][j].x = lines.polygons[i].points[j].x*hres;
      line_points[i][j].y = lines.polygons[i].points[j].y*vres; 
    }
    line_points[i][j].x = lines.polygons[i].points[0].x*hres;
    line_points[i][j].y = lines.polygons[i].points[0].y*vres; 
    lv_obj_set_style(lines_hdl[i], &style_line[i]);
    lv_line_set_points(lines_hdl[i], line_points[i], lines.polygons[i].point_num + 1);
  }
  
  return 0;
}
#endif



#if __UI_ZOOM__

static lv_obj_t *btn_zoomplus, *btn_zoomminus, *btn_snap, *slider_ae;

static void zoom_hid_cb(struct input_event* in)
{
   //printf("%s => type:%d, code:%d, value:%d\n", __func__, in->type, in->code, in->value);
   
   switch(in->code)
   {
    case 103:
      {
        int ev = (in->value)?LV_EVENT_PRESSED:LV_EVENT_RELEASED;
        //printf("%s => btn_zoomplus ev:%s\n", __func__, (ev==LV_EVENT_PRESSED)?"LV_EVENT_PRESSED":"LV_EVENT_RELEASED");
        lv_btn_set_state(btn_zoomplus, (ev==LV_EVENT_PRESSED)?LV_BTN_STYLE_PR:LV_BTN_STYLE_REL);
        lv_event_send(btn_zoomplus, ev, NULL);
      }
      break;
    case 108:
      {
        int ev = (in->value)?LV_EVENT_PRESSED:LV_EVENT_RELEASED;
        //printf("%s => btn_zoomminus ev:%s\n", __func__, (ev==LV_EVENT_PRESSED)?"LV_EVENT_PRESSED":"LV_EVENT_RELEASED");
        lv_btn_set_state(btn_zoomminus, (ev==LV_EVENT_PRESSED)?LV_BTN_STYLE_PR:LV_BTN_STYLE_REL);
        lv_event_send(btn_zoomminus, ev, NULL);
      }
      break;
    case 105:
    case 106:
      if(in->value)
      { 
        int val = lv_slider_get_value(slider_ae)+((in->code==105)?-1:1);
        //printf("%s => lv_slider_set_value val:%d\n", __func__, val);
        lv_slider_set_value(slider_ae, val, true);
        lv_event_send(slider_ae,LV_EVENT_VALUE_CHANGED, NULL);
      }
      break;
    case 582:
      if(in->value)
      {
        lv_btn_set_state(btn_snap, LV_BTN_STYLE_PR);
      }
      else 
      {
        lv_btn_set_state(btn_snap, LV_BTN_STYLE_REL);
        lv_event_send(btn_snap, LV_EVENT_RELEASED, NULL);
      }
   }
}

static void zoomplus_event_cb(lv_obj_t * btn, lv_event_t event)
{
    (void) btn; /*Unused*/
    GSF_MSG_DEF(gsf_lens_t, lens, 2*1024);
    if(event == LV_EVENT_PRESSED)
    {
      printf("%s => LV_EVENT_PRESSED\n", __func__);
      printf("%s => LV_EVENT_PRESSED\n", __func__);
      lens->cmd = GSF_LENS_ZOOM;
      lens->arg1 = 1; //++
      lens->arg2 = 0;
      GSF_MSG_SENDTO(GSF_ID_CODEC_LENS, 0, GET, 0
                        , sizeof(gsf_lens_t)
                        , GSF_IPC_CODEC, 2000);
    }
    else if(event == LV_EVENT_RELEASED)
    {
      printf("%s => LV_EVENT_RELEASED\n", __func__);
      lens->cmd = GSF_LENS_STOP;
      GSF_MSG_SENDTO(GSF_ID_CODEC_LENS, 0, GET, 0
                        , sizeof(gsf_lens_t)
                        , GSF_IPC_CODEC, 2000);
    }
}

static void zoomminus_event_cb(lv_obj_t * btn, lv_event_t event)
{
    (void) btn; /*Unused*/
    GSF_MSG_DEF(gsf_lens_t, lens, 2*1024);
    if(event == LV_EVENT_PRESSED)
    {
      printf("%s => LV_EVENT_RELEASED\n", __func__);
      lens->cmd = GSF_LENS_ZOOM;
      lens->arg1 = 0; //--
      lens->arg2 = 0;
      GSF_MSG_SENDTO(GSF_ID_CODEC_LENS, 0, GET, 0
                        , sizeof(gsf_lens_t)
                        , GSF_IPC_CODEC, 2000);
    }
    else if(event == LV_EVENT_RELEASED)
    {
      printf("%s => LV_EVENT_RELEASED\n", __func__);
      lens->cmd = GSF_LENS_STOP;
      GSF_MSG_SENDTO(GSF_ID_CODEC_LENS, 0, GET, 0
                        , sizeof(gsf_lens_t)
                        , GSF_IPC_CODEC, 2000);
    }
}

static void sceneae_event_handler(lv_obj_t * slider, lv_event_t event)
{
    GSF_MSG_DEF(gsf_scene_ae_t, ae, 2*1024);
    if(event == LV_EVENT_VALUE_CHANGED) {
        int16_t v = lv_slider_get_value(slider);
        printf("%s => LV_EVENT_VALUE_CHANGED v:%d\n", __func__, v);
        ae->compensation_mul = v*0.1;
        GSF_MSG_SENDTO(GSF_ID_CODEC_SCENEAE, 0, SET, 0
                          , sizeof(gsf_scene_ae_t)
                          , GSF_IPC_CODEC, 2000);
    }
}

static void snap_event_cb(lv_obj_t * btn, lv_event_t event)
{
    (void) btn; /*Unused*/
    
    if(event == LV_EVENT_RELEASED)
    {
      printf("%s => LV_EVENT_RELEASED\n", __func__);

      GSF_MSG_DEF(char, filename, 2*1024);
      int ret = GSF_MSG_SENDTO(GSF_ID_CODEC_SNAP, 0, GET, 0
                        , 0
                        , GSF_IPC_CODEC, 2000);
      
      if(ret == 0 && __pmsg->size > 0)
      {
        GSF_MSG_DEF(char, image, 2*1024);
        strcpy(image, filename);
        ret = GSF_MSG_SENDTO(GSF_ID_REC_IMAGE, 0, SET, 0
                          , strlen(filename)+1
                          , GSF_IPC_REC, 2000);
        printf("ret:%d, err:%d\n", ret, __pmsg->err);
        if(note_l)
        {  
          lv_label_set_text(note_l, (ret || __pmsg->err)?"snap fail":"snap success");
        }    
      }
    }
}

#endif

static void* lvgl_main(void* p)
{
    /*LittlevGL init*/
    lv_init();

    /*Linux frame buffer device init*/
    fbdev_init();

    /*Initialize and register a display driver*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    
    /*Set the resolution of the display*/
    disp_drv.hor_res = lv_w; //1280
    disp_drv.ver_res = lv_h; //1024
    
    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];
    /*Initialize a descriptor for the buffer*/
    static lv_disp_buf_t disp_buf;
    lv_disp_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);
    
    disp_drv.buffer = &disp_buf;
    disp_drv.flush_cb = lv_push_osd?fb_flush:fbdev_flush;
    lv_disp_drv_register(&disp_drv);
    
    #if __UI_MOUSE__
    mouse_hid_init();
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);          /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = mouse_hid_read;     /*This function will be called periodically (by the library) to get the mouse position and state*/
    lv_indev_t * mouse_indev = lv_indev_drv_register(&indev_drv);

    LV_IMG_DECLARE(mouse_ico);
    lv_obj_t * cursor_obj =  lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(cursor_obj, &mouse_ico);
    //lv_img_set_src(cursor_obj, LV_SYMBOL_CALL);
    lv_indev_set_cursor(mouse_indev, cursor_obj);
    #endif
    
    lv_coord_t hres = lv_disp_get_hor_res(NULL);
    lv_coord_t vres = lv_disp_get_ver_res(NULL);

    static lv_style_t style_tv_body_bg;
    lv_style_copy(&style_tv_body_bg, &lv_style_plain);
    style_tv_body_bg.body.main_color = lv_color_hex(0x487fb7);
    LV_COLOR_SET_A(style_tv_body_bg.body.main_color, 0x00);
    style_tv_body_bg.body.grad_color = lv_color_hex(0x487fb7);
    LV_COLOR_SET_A(style_tv_body_bg.body.grad_color, 0x00);
    style_tv_body_bg.body.padding.top = 0;
    
    static lv_style_t style_tv_body_fg;
    lv_style_copy(&style_tv_body_fg, &lv_style_plain);
    style_tv_body_fg.body.main_color = LV_COLOR_RED;
    LV_COLOR_SET_A(style_tv_body_fg.body.main_color, sizeof(lv_color_t)==4?30:0);
    style_tv_body_fg.body.grad_color = LV_COLOR_RED;
    LV_COLOR_SET_A(style_tv_body_fg.body.grad_color, sizeof(lv_color_t)==4?30:0);
    style_tv_body_fg.body.padding.top = 0;
    style_tv_body_fg.body.border.color = LV_COLOR_RED;
    style_tv_body_fg.body.border.width = 4;
    
    static lv_style_t style_label; // lv_style_plain_color
    lv_style_copy(&style_label, &lv_style_plain);
    style_label.text.color = LV_COLOR_WHITE;
    style_label.text.font  = &lv_font_roboto_28;

    static lv_style_t style_label_ol; // lv_style_plain_color
    lv_style_copy(&style_label_ol, &lv_style_plain);
    style_label_ol.text.color = LV_COLOR_WHITE;
    style_label_ol.text.font  = &lv_font_roboto_28;

    #ifdef EXTRA_FREETYPE
      static char font_path[256];
      sprintf(font_path, "%s/cfg/arial.ttf", home_path);

      /*Create a font*/
      static lv_ft_info_t info;
      info.name = font_path;
      info.weight = 48;
      info.style = FT_FONT_STYLE_NORMAL;
      info.mem = NULL;
      if(!lv_ft_font_init(&info)) {
          LV_LOG_ERROR("create failed.");
      }
      style_label.text.color = LV_COLOR_WHITE;
      style_label.text.font  = info.font;
      
      /*Create a font*/
      static lv_ft_info_t info2;
      info2.name = font_path;
      info2.weight = 48;
      info2.style = FT_FONT_STYLE_OUTLINE;
      info2.mem = NULL;
      if(!lv_ft_font_init(&info2)) {
          LV_LOG_ERROR("create failed.");
      }
      style_label_ol.text.color = LV_COLOR_RED;//LV_COLOR_GRAY;
      style_label_ol.text.font  = info2.font;
    #endif

    lv_obj_t *win = lv_obj_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_size(win, hres, vres);
    lv_obj_set_style(win, &style_tv_body_bg);
    
    #if __UI_LOGO__ // HDMI/LCD OUT LOGO;
      lv_obj_t *obj_note = lv_obj_create(win, NULL);
      lv_obj_set_style(obj_note, &style_tv_body_fg);
      lv_obj_set_size(obj_note, hres/1, vres/4/4);
      lv_obj_set_x(obj_note, 0);
      lv_obj_set_y(obj_note, vres/1.5);
      
      #ifdef EXTRA_FREETYPE
        static lv_color_t* cbuf = NULL;
        cbuf = (lv_color_t*)malloc(LV_CANVAS_BUF_SIZE_TRUE_COLOR((hres/1-16), (vres/4/4-8)));
        lv_obj_t * canvas = lv_canvas_create(obj_note, NULL);
        lv_canvas_set_buffer(canvas, cbuf, hres/1-16, vres/4/4-8, LV_IMG_CF_TRUE_COLOR);
        lv_obj_align(canvas, NULL, LV_ALIGN_IN_LEFT_MID, 8, 0);
        lv_color_t bgcolor = LV_COLOR_RED;//LV_COLOR_SILVER;
        LV_COLOR_SET_A(bgcolor, 0);
        lv_canvas_fill_bg(canvas, bgcolor);
        lv_canvas_draw_text(canvas, 0, 0, hres/1-16, &style_label_ol, "LittlevGL && FreeType && https://github.com/openhisilicon/HIVIEW", LV_LABEL_ALIGN_LEFT);
        lv_canvas_draw_text(canvas, 0, 0, hres/1-16, &style_label, "LittlevGL && FreeType && https://github.com/openhisilicon/HIVIEW", LV_LABEL_ALIGN_LEFT);
      #else
        lv_obj_t *label_note = lv_label_create(obj_note, NULL);
        lv_obj_set_style(label_note, &style_label);
        lv_obj_align(label_note, NULL, LV_ALIGN_IN_LEFT_MID, 8, 0);
        lv_label_set_text(label_note, "LittlevGL && https://github.com/openhisilicon/HIVIEW");
      #endif
    #endif
    
    #if __UI_ZOOM__ //button;
    
    static lv_style_t zoom_s; // lv_style_plain_color
    lv_style_copy(&zoom_s, &lv_style_plain);
    zoom_s.text.color = LV_COLOR_WHITE;
    zoom_s.text.font  = &lv_font_roboto_16;
    
    btn_zoomplus = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_fit(btn_zoomplus, LV_FIT_TIGHT);
    lv_obj_set_event_cb(btn_zoomplus, zoomplus_event_cb);
    //lv_obj_set_size(btn_zoomplus, 60, 40);
    lv_obj_set_x(btn_zoomplus, 25);
    lv_obj_set_y(btn_zoomplus, vres-100);
    lv_obj_t * btn_l = lv_label_create(btn_zoomplus, NULL);
    lv_obj_set_style(btn_l, &zoom_s);
    lv_label_set_text(btn_l, LV_SYMBOL_EYE_OPEN " " LV_SYMBOL_PLUS);
    
    btn_zoomminus = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_fit(btn_zoomminus, LV_FIT_TIGHT);
    lv_obj_set_event_cb(btn_zoomminus, zoomminus_event_cb);
    //lv_obj_set_size(btn_zoomminus, 60, 40);
    lv_obj_set_x(btn_zoomminus, 125);
    lv_obj_set_y(btn_zoomminus, vres-100);
    btn_l = lv_label_create(btn_zoomminus, btn_l);
    lv_label_set_text(btn_l, LV_SYMBOL_EYE_OPEN " " LV_SYMBOL_MINUS);
    
    btn_snap = lv_btn_create(lv_scr_act(), NULL);
    lv_btn_set_fit(btn_zoomminus, LV_FIT_TIGHT);
    lv_obj_set_event_cb(btn_snap, snap_event_cb);
    lv_obj_set_size(btn_snap, 65, 70);
    lv_obj_set_x(btn_snap, 230);
    lv_obj_set_y(btn_snap, vres-92);
    btn_l = lv_label_create(btn_snap, btn_l);
    lv_obj_set_style(btn_l, &style_label);
    lv_label_set_text(btn_l, LV_SYMBOL_IMAGE);    
    
    slider_ae = lv_slider_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(slider_ae, sceneae_event_handler);
    lv_slider_set_range(slider_ae, 2, 18);
    lv_slider_set_value(slider_ae, 10, false);
    lv_obj_set_size(slider_ae, 200, 25);
    lv_obj_set_x(slider_ae, 25);
    lv_obj_set_y(slider_ae, vres-40);
    
    mouse_hid_cb_set(zoom_hid_cb);
    #endif

    #define OBJ_MAX_CHN  9
    #define OBJ_MAX_CNT  10
    int i = 0, cnt[OBJ_MAX_CHN] = {0,};
    lv_obj_t *obj[OBJ_MAX_CNT*OBJ_MAX_CHN];
    lv_obj_t *label[OBJ_MAX_CNT*OBJ_MAX_CHN];
    
    for(i = 0; i < OBJ_MAX_CNT*OBJ_MAX_CHN; i++)
    {
      obj[i] = lv_obj_create(win, NULL);
      label[i] = lv_label_create(obj[i], NULL);
      //lv_obj_set_style(label[i], &lv_style_plain_color);
      lv_obj_set_style(label[i], &style_label);
      lv_obj_set_style(obj[i], &style_tv_body_fg);
      lv_obj_set_size(obj[i], 0, 0);
    }

    /*Create a Demo*/
    #if 0
    lv_obj_t* tv = demo_create();
    #endif
    
    // GSF_PUB_SVP
    //msq;
    msq = msgget(111, IPC_CREAT | 0666);
    struct msqid_ds queue_info;
    msgctl(msq, IPC_STAT, &queue_info);
    printf("msg_qbytes: %d\n", queue_info.msg_qbytes);
    queue_info.msg_qbytes = 160*1024;
    msgctl(msq, IPC_SET, &queue_info);
    
    // GSF_PUB_SVP
    void* sub = nm_sub_conn(GSF_PUB_SVP, sub_recv);
    
    /*Handle LitlevGL tasks (tickless mode)*/
    while(lv_running) {
      
        int ret = 0;
        char _buf[16*1024];
        msgbuf *mbuf = (msgbuf*)_buf;
        
        lv_task_handler();

        #if __UI_LINES__
        lines_draw(hres, vres);
        #endif
        
        #if __UI_IOU__
        lines_iou(hres, vres);
        #endif
        
        #if __UI_STAT__
        stat_draw(hres, vres);
        #endif
        
        if((ret = msgrcv(msq, _buf, sizeof(_buf), 0, IPC_NOWAIT)) > 0)
        do  
        {
          gsf_svp_yolos_t *yolos = (gsf_svp_yolos_t*)mbuf->mtext;
          int chn = mbuf->mtype - 1;
          if(chn >= OBJ_MAX_CHN)
          {  
            break;
          }
          
          float xr = 0, yr = 0, wr = 0, hr = 0;
          
          if(vres > hres) //for vertical screen
          {
            xr = 0; yr = chn*(vres/2);
            wr = hres; wr/= yolos->w;
            hr = vres/2; hr/= yolos->h;
          }
          else if(lt >= 8) //for 8ch/9ch
          {
            xr = 0 + (chn%3)*(hres/3); yr = (chn/3)*(vres/3);
            wr = hres/3; wr/= yolos->w;
            hr = vres/3; hr/= yolos->h;
          }
          else if(lt == 4) //for 4ch
          {
            xr = 0 + (chn%2)*(hres/2); yr = (chn/2)*(vres/2);
            wr = hres/2; wr/= yolos->w;
            hr = vres/2; hr/= yolos->h;
          }
          else if(lt == 2) //for 2ch
          {
            xr = 0 + chn*(hres/2); yr = vres/4;
            wr = hres/2; wr/= yolos->w;
            hr = vres/2; hr/= yolos->h;
          }
          else  // for 1ch;
          {
            wr = hres; wr/= yolos->w;
            hr = vres; hr/= yolos->h;
          }
          
          yolos->cnt = (yolos->cnt > OBJ_MAX_CNT)?OBJ_MAX_CNT:yolos->cnt;
          
          for(i = yolos->cnt; i < cnt[chn]; i++)
          {
            lv_obj_set_size(obj[i + chn*OBJ_MAX_CNT], 0, 0);
          }
          
          for(i = 0; i < yolos->cnt; i++)
          {
            lv_label_set_text(label[i + chn*OBJ_MAX_CNT], yolos->box[i].label);
            lv_obj_set_size(obj[i+ chn*OBJ_MAX_CNT], yolos->box[i].rect[2] * wr, yolos->box[i].rect[3] * hr);
            lv_obj_set_x(obj[i+ chn*OBJ_MAX_CNT], xr + yolos->box[i].rect[0] *wr);
            lv_obj_set_y(obj[i+ chn*OBJ_MAX_CNT], yr + yolos->box[i].rect[1] *hr);
            #if 0
            printf("chn:%d, cnt:%d, i:%d, boj(x:%.2f, y:%.2f, w:%.2f, h:%.2f)\n", chn, yolos->cnt
                    , i, xr + yolos->box[i].rect[0] *wr, yr + yolos->box[i].rect[1] *hr, yolos->box[i].rect[2] * wr, yolos->box[i].rect[3] * hr);
            #endif
          }
          cnt[chn] = yolos->cnt;
        }while(0);
        else
        {
          usleep(5*1000);
        }
    }
    
    return NULL;
}

/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }
    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}

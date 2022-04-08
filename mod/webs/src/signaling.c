#include "webs.h"

#include "mongoose.h"
#include "webrtc.h"
#include "http.h"

static pthread_t _pid;
static struct mg_mgr *_mgr;

typedef struct {
  int stat;
  int timeout;
  int lastsec;
  char url[256];
  struct mg_connection *nc;
  struct list_head clients;
}sg_conn_t;

typedef struct {
  rtc_sess_t *rtc_sess; //fisrt field;
  struct list_head list;
}sg_client_t;


static sg_conn_t _conn;


static void* _mg_task(void* parm);

int signaling_init(char *url)
{
    // protocol://host/uuid      
    //"wss://openhisilicon.glitch.me/hiview-camera"
    
    if(!strlen(url))
      return -1;
  
    if(!_mgr)
    {
      strncpy(_conn.url, url, sizeof(_conn.url)-1);
      _conn.timeout = 5;
      INIT_LIST_HEAD(&_conn.clients);
      _mgr = (struct mg_mgr *)malloc(sizeof(struct mg_mgr));
      mg_mgr_init(_mgr, NULL);
      pthread_create(&_pid, NULL, _mg_task, NULL);
    }
    return 0;
}


static void _ev_handler_mg(struct mg_connection *nc, int ev, void *ev_data)
{   
    sg_conn_t *conn = (sg_conn_t*)(nc->user_data);
    if(!conn) return;
      
    switch (ev)
    {
      case MG_EV_CONNECT: {
        int status = *((int *)ev_data);
        if (status != 0) {
          //printf("err MG_EV_CONNECT(nc:%p) status:%d.\n", nc, status);
        }
        break;
      }
      case MG_EV_WEBSOCKET_HANDSHAKE_DONE: {
        printf("MG_EV_WEBSOCKET_HANDSHAKE_DONE(nc:%p).\n", nc);
        conn->stat = 1;
        break;
      }
      case MG_EV_POLL:
      {
          //check timeout;
          if((int)mg_time() - conn->lastsec > conn->timeout)
          {
            //send heartbeat;
            //char *heartbeat = "{\"type\":\"heartbeat\"}";
            //mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, heartbeat, strlen(heartbeat));
            
            if(conn->lastsec && conn->stat == 0)
            {
              printf("MG_EV_POLL(nc:%p): timeout.\n", nc);  
              nc->flags |= MG_F_CLOSE_IMMEDIATELY;
            }
            conn->lastsec = (int)mg_time();
          }
          break;
      }
      case MG_EV_WEBSOCKET_FRAME: 
      {
          struct websocket_message *wm = (struct websocket_message *) ev_data;
          //printf("MG_EV_WEBSOCKET_FRAME(nc:%p): [%s:%d]\n", nc, wm->data, wm->size);
          
          char msg[4096] = {0};
          char dst[128] = {0};
          memcpy(msg, wm->data, wm->size);
          char *pdst = strstr(msg, ",\"src\":\"");
          sscanf(pdst, ",\"src\":\"%127[^\"]", dst);

          sg_client_t *n = NULL, *cli = NULL;
          list_for_each_entry(n, &conn->clients, list)
          {
            if(!strcmp(n->rtc_sess->dst, dst))
            {
              cli = n;
              warn("FINDOUT dst:[%s], rtc_sess:%p\n", dst, cli->rtc_sess);
              break;
            }
          }
          
          rtc_sess_t *new_sess = NULL;
          int ret = webrtc_proc(nc, (cli)?(&cli->rtc_sess):(&new_sess), wm);
          if(ret == WEBRTC_NEW)
          {
            sg_client_t *new_cli = (sg_client_t*)calloc(1, sizeof(sg_client_t));
            new_cli->rtc_sess = new_sess;
            warn("WEBRTC_NEW dst:[%s], rtc_sess:%p\n", dst, new_cli->rtc_sess);
            list_add(&new_cli->list, &conn->clients);
          }
          else if(ret == WEBRTC_DEL)
          {
            if(cli)
            {
              warn("WEBRTC_DEL dst:[%s], rtc_sess:%p\n", dst, cli->rtc_sess);
              list_del(&cli->list);
              if(n->rtc_sess)
                rtc_sess_free(n->rtc_sess);
            }
          }
          break;
      }
      case MG_EV_CLOSE:
      {
          printf("MG_EV_CLOSE(nc:%p).\n", nc);
          //free list;
          sg_client_t *n, *nn;
          list_for_each_entry_safe(n, nn, &conn->clients, list)
          {
              list_del(&n->list);
              if(n->rtc_sess)
                rtc_sess_free(n->rtc_sess);
              free(n);
          }
          break;
      }
    }
}


static void* _mg_task(void* parm)
{
  while(1)
  {
    
    assert(list_empty(&_conn.clients));
    _conn.nc = mg_connect_ws(_mgr, _ev_handler_mg, _conn.url, NULL, NULL);
    if(_conn.nc)
    {
      _conn.stat = 0;
      _conn.nc->user_data = &_conn;
      _conn.lastsec = (int)mg_time();
      printf("mg_connect_ws _conn.nc:%p, lastsec:%d\n", _conn.nc, _conn.lastsec);
    }

    do {
      mg_mgr_poll(_mgr, 6000);
    }while(_mgr->active_connections);

  }
  return NULL;
}

